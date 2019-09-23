#include "pch.h"
#include "PrimitiveBatch.h"
#include "dx_helpers.h"
#include "helpers.h"

using namespace DirectX;
using namespace DirectX::Internal;
using Microsoft::WRL::ComPtr;

// internal PrimitiveBatch implementation
class PrimitiveBatchBase::Impl
{
public:
	Impl(_In_ ID3D11DeviceContext* device_context, size_t max_indices, size_t max_vertices, size_t vertex_size);
	void Begin();
	void End();
	void Draw(D3D11_PRIMITIVE_TOPOLOGY topology, bool is_indexed, _In_opt_count_(index_count) uint16_t const* indices, size_t index_count, size_t vertex_count, _Out_ void** mapped_vertices);
private:
	void FlushBatch();

	ComPtr<ID3D11DeviceContext> device_context_;
	ComPtr<ID3D11Buffer> index_buffer_;
	ComPtr<ID3D11Buffer> vertex_buffer_;

	size_t max_indices_;
	size_t max_vertices_;
	size_t vertex_size_;

	D3D11_PRIMITIVE_TOPOLOGY current_topology_;

	bool in_begin_end_pair_;
	bool currently_indexed_;

	size_t current_index_;
	size_t current_vertex_;
	size_t base_index_;
	size_t base_vertex_;

	D3D11_MAPPED_SUBRESOURCE mapped_indices_;
	D3D11_MAPPED_SUBRESOURCE mapped_vertices_;
};

namespace
{
	// helper for creating D3D vertex or index buffer
	void CreateBuffer(_In_ ID3D11Device* device, size_t buffer_size, D3D11_BIND_FLAG bind_flag, _Out_ ID3D11Buffer** buffer)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = static_cast<unsigned int>(buffer_size);
		desc.BindFlags = bind_flag;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ThrowIfFailed(
			device->CreateBuffer(&desc, nullptr, buffer)
		);
		_Analysis_assume_(*buffer != 0);
		SetDebugObjectName(*buffer, "dxlib:PrimitiveBatch");
	}
}

// constructor
PrimitiveBatchBase::Impl::Impl(_In_ ID3D11DeviceContext* device_context, size_t max_indices, size_t max_vertices, size_t vertex_size)
	:	max_indices_{max_indices},
		max_vertices_{max_vertices},
		vertex_size_{vertex_size},
		current_topology_{D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED},
		in_begin_end_pair_{false},
		currently_indexed_{false},
		current_index_{0},
		current_vertex_{0},
		base_index_{0},
		base_vertex_{0},
		mapped_indices_{},
		mapped_vertices_{}
{
	ComPtr<ID3D11Device> device;
	device_context->GetDevice(&device);
	if (!max_vertices)
		throw std::exception("max_vertices must be greater the 0");
	if (vertex_size > D3D11_REQ_MULTI_ELEMENT_STRUCTURE_SIZE_IN_BYTES)
		throw std::exception("Vertex size is too large for DirectX 11");
	if ((uint64_t(max_indices) * sizeof(uint16_t)) > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
		throw std::exception("IB too large for DirectX 11");
	if ((uint64_t(max_vertices) * uint64_t(vertex_size)) > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
		throw std::exception("VB too large for DirectX 11");
	device_context_ = device_context;
	if (max_indices > 0) // specifying max_indices = 0 skips index buffer creation when drawing non-indexed geometry
	{
		CreateBuffer(device.Get(), max_indices * sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER, &index_buffer_);
	}
	CreateBuffer(device.Get(), max_vertices * vertex_size, D3D11_BIND_INDEX_BUFFER, &vertex_buffer_);
}

void PrimitiveBatchBase::Impl::Begin()
{
	if (in_begin_end_pair_)
		throw std::exception("Cannot nest Begin calls");
	if (max_indices_ > 0)
		device_context_->IASetIndexBuffer(index_buffer_.Get(), DXGI_FORMAT_R16_UINT, 0);
	auto vertex_buffer = vertex_buffer_.Get();
	unsigned int vertex_stride = static_cast<unsigned int>(vertex_size_);
	unsigned int vertex_offset = 0;
	device_context_->IASetVertexBuffers(0, 1, &vertex_buffer, &vertex_stride, &vertex_offset);
	if (device_context_->GetType() == D3D11_DEVICE_CONTEXT_DEFERRED)
	{
		current_index_ = 0;
		current_vertex_ = 0;
	}
	in_begin_end_pair_ = true;
}

void PrimitiveBatchBase::Impl::End()
{
	if (!in_begin_end_pair_)
		throw std::exception("Begin must be called before End");
	FlushBatch();
	in_begin_end_pair_ = false;
}

namespace
{
	bool CanBatchPrimitives(D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		switch (topology)
		{
		case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			return true; // lists can easilty be merged
		default:
			return false; // strings cannot
		}
		// note: indexed strips can be merged by inserting degenerates, but that is not always a perf win, so we keep it simple here
	}

#if !defined(_XBOX_ONE) || !defined(_TITLE)
	// helper for locking a vertex or index buffer
	void LockBuffer(_In_ ID3D11DeviceContext* device_context, _In_ ID3D11Buffer* buffer, size_t current_position, _Out_ size_t* base_position, _Out_ D3D11_MAPPED_SUBRESOURCE* mapped_resource)
	{
		D3D11_MAP map_type = (current_position == 0) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
		ThrowIfFailed(
			device_context->Map(buffer, 0, map_type, 0, mapped_resource)
		);
		*base_position = current_position;
	}
#endif
}

