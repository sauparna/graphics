#include "world_pch.h"
#include "game.h"

extern void ExitGame();

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;
using namespace DX;

Game::Game(DirectX::XMUINT2 backBufSize) noexcept(false) : m_devrsrc{ make_unique<DeviceResources>(XMUINT2{backBufSize}) }
{
	m_devrsrc->RegisterDeviceNotify(this);
}

Game::~Game()
{
#ifdef DXTK_AUDIO
	if (m_audioEngine)
		m_audioEngine->Suspend();
#endif
}

void Game::Initialize(HWND hWnd)
{
	m_gamepad = make_unique<GamePad>();
	m_keyboard = make_unique<Keyboard>();
	m_mouse = make_unique<Mouse>();
	m_mouse->SetWindow(hWnd);
	m_devrsrc->SetWindow(hWnd);
	m_devrsrc->cddr();
	m_devrsrc->CreateSwapChain();
	cddr();
	m_devrsrc->cwsdr();
	cwsdr();

#ifdef DXTK_AUDIO
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif
	m_audioEngine = make_unique<AudioEngine>(eflags);
	m_audioEvent = 0;
	m_audioTimerAcc = 10.0f;
	m_retryDefault = false;
	m_wavebank = = make_unique<WaveBank>(m_audioEngine.get(), L"adpcmdroid.xwb");
	m_soundEffect = make_unique<SoundEffect>(m_audioEngine.get(), L"MusicMono_adpcm.wav");
	m_soundFxInst1 = m_soundEffect->CreateInstance();
	m_soundFxInst2 = m_wavebank->CreateInstance(10);
	m_soundFxInst1->Play(true);
	m_soundFxInst2->Play();
#endif
}

void Game::Tick()
{
	m_timer.Tick( [&](){ Update(m_timer); } );
#ifdef DXTK_AUDIO
	// Update audio engine once per frame and set up a retry in 1 second.
	if (!m_audioEngine->IsCriticalError() && m_audioEngine->Update())
	{
		m_audioTimerAcc = 1.0f;
		m_retryDefault = true;
	}
#endif
	Render();
}

void Game::Update(StepTimer const& timer)
{
	m_world = Matrix::CreateRotationY(cosf(static_cast<float>(timer.AgeInSeconds())));

#ifdef DXTK_AUDIO
	m_audioTimerAcc -= (float)timer.FrameDeltaInSeconds();
	if (m_audioTimerAcc < 0)
	{
		if (m_retryDefault)
		{
			m_retryDefault = false;
			if (m_audioEngine->Reset())
				m_soundFxInst1->Play(true);
		}
		else
		{
			m_audioTimerAcc = 4.0f;
			m_wavebank->Play(m_audioEvent++);
			if (m_audioEvent >= 11)
				m_audioEvent = 0;
		}
	}
#endif

	auto gamepad = m_gamepad->GetState(0);
	if (gamepad.IsConnected())
	{
		if (gamepad.IsViewPressed())
			ExitGame();
	}

	auto kb = m_keyboard->GetState();
	if (kb.Escape)
		ExitGame();
}

void Game::Render()
{
	if (m_timer.FrameCount() == 0)
		return;
	
	Clear();
	
	m_devrsrc->PIXBeginEvent(L"Render");
	
	auto context = m_devrsrc->GetD3DContext();

	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	context->RSSetState(m_rasterState.Get());
	context->IASetInputLayout(m_inputLayout.Get());

	m_effect->Apply(context);
	m_effect->SetWorld(m_world);

	m_batch->Begin();

	// Triangle.
	VertexPositionColor v1(Vector3( 0, 0, 0), Colors::Red);
	VertexPositionColor v2(Vector3( 1, 1, 0), Colors::Green);
	VertexPositionColor v3(Vector3(-1, 1, 0), Colors::Blue);
	m_batch->DrawTriangle(v1, v2, v3);

	// Grid on the xz plane, like a floor.
	{
		Vector3 x(2, 0, 0);
		Vector3 z(0, 0, 2);
		Vector3 o = Vector3::Zero;
		float sz = 10.f;
		for (float i = -sz; i <= sz; ++i)
		{
			float s = i / sz;
			Vector3 dx = o + x * s;
			Vector3 dz = o + z * s;
			VertexPositionColor vz1(dx - z, Colors::White);
			VertexPositionColor vz2(dx + z, Colors::White);
			VertexPositionColor vx1(dz - x, Colors::White);
			VertexPositionColor vx2(dz + x, Colors::White);
			m_batch->DrawLine(vz1, vz2);
			m_batch->DrawLine(vx1, vx2);
		}
	}

	m_batch->End();

	m_devrsrc->PIXEndEvent(); // "Render"

	m_devrsrc->Present();
}

void Game::Clear()
{
	m_devrsrc->PIXBeginEvent(L"Clear");
	auto context = m_devrsrc->GetD3DContext();
	auto renderTargetView = m_devrsrc->GetRenderTargetView();
	auto depthStencilView = m_devrsrc->GetDepthStencilView();
	context->ClearRenderTargetView(renderTargetView, Colors::Black);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	auto viewport = m_devrsrc->GetScreenViewPort();
	context->RSSetViewports(1, &viewport);
	m_devrsrc->PIXEndEvent();
}

void XM_CALLCONV Game::DrawGrid(FXMVECTOR u, FXMVECTOR v, FXMVECTOR o, size_t dx, size_t dy, GXMVECTOR color)
{
	m_devrsrc->PIXBeginEvent(L"Draw grid");

	auto context = m_devrsrc->GetD3DContext();
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	context->RSSetState(m_states->CullCounterClockwise());

	m_effect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	m_batch->Begin();
	dx = max<size_t>(1, dx);
	dy = max<size_t>(1, dy);
	for (size_t i = 0; i <= dx; ++i)
	{
		XMVECTOR x = XMVectorScale(u, float(i) / float(dx) * 2.0f - 1.0f);
		x = XMVectorAdd(o, x);
		VertexPositionColor v1(XMVectorSubtract(x, v), color);
		VertexPositionColor v2(XMVectorAdd(x, v), color);
		m_batch->DrawLine(v1, v2);
	}
	for (size_t i = 0; i <= dy; ++i)
	{
		XMVECTOR y = XMVectorScale(v, float(i) / float(dy) * 2.0f - 1.0f);
		y = XMVectorAdd(o, y);
		VertexPositionColor v1(XMVectorSubtract(y, u), color);
		VertexPositionColor v2(XMVectorAdd(y, u), color);
		m_batch->DrawLine(v1, v2);
	}
	m_batch->End();

	m_devrsrc->PIXEndEvent(); // "Draw grid"
}

void Game::OnActivate() {}
void Game::OnDeactivate() {}
void Game::OnSuspend()
{
#ifdef DXTK_AUDIO
	m_audioEngine->Suspend();
#endif
}
void Game::OnResume()
{
	m_timer.Reset();
#ifdef DXTK_AUDIO
	m_audioEngine->Resume();
#endif
}
void Game::OnWindowMove()
{
	auto sz = m_devrsrc->GetBackBufSize();
	m_devrsrc->HandleResize(sz.x, sz.y);
}

void Game::OnWindowResize(size_t w, size_t h)
{
	m_devrsrc->HandleResize(w, h);
	cwsdr();
}
#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
	if (m_audioEngine && !m_audioEngine->IsAudioDevicePresent())
	{
		m_audioTimerAcc = 1.0f;
		m_retryDefault = true;
	}
}
#endif
void Game::GetDefaultSize(int& w, int& h) const
{
	w = 800;
	h = 600;
}

void Game::cdir()
{

}

void Game::cddr()
{
	auto context = m_devrsrc->GetD3DContext();
	auto device = m_devrsrc->GetD3DDevice();
	m_batch = make_unique<PrimitiveBatch<VertexPositionColor>>(context);
	m_states = make_unique<CommonStates>(device);
	m_fxFactory = make_unique<EffectFactory>(device);
	m_effect = make_unique<BasicEffect>(device);
	m_effect->SetVertexColorEnabled(true);

	{
		void const* shaderByteCode;
		size_t byteCodeLength;
		m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
		ThrowIfFailed(device->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength, m_inputLayout.ReleaseAndGetAddressOf()));
	}

	m_world = Matrix::Identity;
	CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE, D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP, D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
	ThrowIfFailed(device->CreateRasterizerState(&rastDesc, m_rasterState.ReleaseAndGetAddressOf()));
}

void Game::cwsdr()
{
	auto sz = m_devrsrc->GetBackBufSize();
	float aspectRatio = static_cast<float>(sz.x) / static_cast<float>(sz.y);
	float fovAngleY = XM_PI / 4.f;
	if (aspectRatio < 1.0f)
		fovAngleY *= 2.0f;
	m_view = Matrix::CreateLookAt(Vector3(0.f, 1.f, 5.f), Vector3::Zero, Vector3::UnitY);
	m_proj = Matrix::CreatePerspectiveFieldOfView(fovAngleY, aspectRatio, 0.1f, 10.0f);
	// Scale proportionally, as the buffer width and height.
	//m_proj *= Matrix::CreateScale(1000.f / float(bbuf_w), 1000.f / float(bbuf_h), 1.f);
	//m_proj = Matrix::CreateOrthographicOffCenter(0.f, float(bbuf_w), float(bbuf_h), 0.f, -1.f, 1.f);
	m_effect->SetView(m_view);
	m_effect->SetProjection(m_proj);
}

void Game::OnDeviceLoss()
{
	m_states.reset();
	m_fxFactory.reset();
	m_batch.reset();
	m_effect.reset();
	m_inputLayout.Reset();
	m_rasterState.Reset();
}

void Game::OnDeviceRestore()
{
	cddr();
	cwsdr();
}
