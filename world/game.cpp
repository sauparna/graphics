#include "world_pch.h"
#include "game.h"

extern void ExitGame();

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;
using namespace DX;

Game::Game(DirectX::XMUINT2 backBufSize) noexcept(false) : devrsrc_{ make_unique<DeviceResources>(XMUINT2{backBufSize}) }
{
	devrsrc_->RegisterDeviceNotify(this);
}

Game::~Game()
{
#ifdef DXTK_AUDIO
	if (audio_engine_)
		audio_engine_->Suspend();
#endif
}

XMUINT2 Game::device_back_buffer_size() const
{ 
	if (devrsrc_)
		return devrsrc_->back_buffer_size();
	else
		return XMUINT2{ 400, 300 };
}

void Game::Initialize(HWND hWnd)
{
	gamepad_ = make_unique<GamePad>();
	keyboard_ = make_unique<Keyboard>();
	mouse_ = make_unique<Mouse>();
	mouse_->SetWindow(hWnd);
	devrsrc_->set_window(hWnd);
	devrsrc_->cddr();
	devrsrc_->create_swap_chain();
	cddr();
	devrsrc_->cwsdr();
	cwsdr();

#ifdef DXTK_AUDIO
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif
	audio_engine_ = make_unique<AudioEngine>(eflags);
	audio_event_ = 0;
	audio_timer_acc_ = 10.0f;
	retry_default_ = false;
	wave_bank_ = = make_unique<WaveBank>(audio_engine_.get(), L"adpcmdroid.xwb");
	m_soundEffect = make_unique<SoundEffect>(audio_engine_.get(), L"MusicMono_adpcm.wav");
	sound_effect_instance1_ = m_soundEffect->CreateInstance();
	sound_effect_instance2_ = wave_bank_->CreateInstance(10);
	sound_effect_instance1_->Play(true);
	sound_effect_instance2_->Play();
#endif
}

void Game::Tick()
{
	step_timer_.Tick( [&](){ update(step_timer_); } );
#ifdef DXTK_AUDIO
	// update audio engine once per frame and set up a retry in 1 second.
	if (!audio_engine_->IsCriticalError() && audio_engine_->Update())
	{
		audio_timer_acc_ = 1.0f;
		retry_default_ = true;
	}
#endif
	render();
}

void Game::update(StepTimer const& timer)
{
	//static float angle = 0.1f;
	//world_matrix_ = Matrix::CreateRotationY(cosf(static_cast<float>(timer.AgeInSeconds())));
	//world_matrix_ = Matrix::CreateRotationY(angle);
	//angle += 0.01f;

#ifdef DXTK_AUDIO
	audio_timer_acc_ -= (float)timer.FrameDeltaInSeconds();
	if (audio_timer_acc_ < 0)
	{
		if (retry_default_)
		{
			retry_default_ = false;
			if (audio_engine_->Reset())
				sound_effect_instance1_->Play(true);
		}
		else
		{
			audio_timer_acc_ = 4.0f;
			wave_bank_->Play(audio_event_++);
			if (audio_event_ >= 11)
				audio_event_ = 0;
		}
	}
#endif

	auto gamepad = gamepad_->GetState(0);
	if (gamepad.IsConnected())
	{
		if (gamepad.IsViewPressed())
			ExitGame();
	}

	auto kb = keyboard_->GetState();
	if (kb.Escape)
		ExitGame();
}

void Game::render()
{
	if (step_timer_.FrameCount() == 0)
		return;
	
	clear();
	
	devrsrc_->PIXBeginEvent(L"render");
	
	auto context = devrsrc_->d3d_context();

	context->OMSetBlendState(states_->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states_->DepthNone(), 0);
	context->RSSetState(rasterizer_state_.Get());
	context->IASetInputLayout(input_layout_.Get());

	effect_->Apply(context);
	effect_->SetWorld(world_matrix_);

	primitive_batch_->Begin();

	// Triangle.
	VertexPositionColor v1(Vector3( 0, 0, 0), Colors::Red);
	VertexPositionColor v2(Vector3( 1, 1, 0), Colors::Green);
	VertexPositionColor v3(Vector3(-1, 1, 0), Colors::Blue);
	primitive_batch_->DrawTriangle(v1, v2, v3);

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
			primitive_batch_->DrawLine(vz1, vz2);
			primitive_batch_->DrawLine(vx1, vx2);
		}
	}

	// 3D model
	model_->Draw(context, *states_, world_matrix_, view_matrix_, projection_matrix_);

	primitive_batch_->End();

	devrsrc_->PIXEndEvent(); // "render"

	devrsrc_->present();
}

void Game::clear()
{
	devrsrc_->PIXBeginEvent(L"clear");
	auto context = devrsrc_->d3d_context();
	auto renderTargetView = devrsrc_->rendet_target_view();
	auto depthStencilView = devrsrc_->depth_stencil_view();
	context->ClearRenderTargetView(renderTargetView, Colors::Black);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	auto viewport = devrsrc_->screen_viewport();
	context->RSSetViewports(1, &viewport);
	devrsrc_->PIXEndEvent();
}

void Game::OnActivate() {}
void Game::OnDeactivate() {}
void Game::OnSuspend()
{
#ifdef DXTK_AUDIO
	audio_engine_->Suspend();
#endif
}
void Game::OnResume()
{
	step_timer_.Reset();
#ifdef DXTK_AUDIO
	audio_engine_->Resume();
#endif
}
void Game::OnWindowMove()
{
	auto sz = devrsrc_->back_buffer_size();
	devrsrc_->handle_resize(sz.x, sz.y);
}

void Game::OnWindowResize(size_t w, size_t h)
{
	devrsrc_->handle_resize(w, h);
	cwsdr();
}
#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
	if (audio_engine_ && !audio_engine_->IsAudioDevicePresent())
	{
		audio_timer_acc_ = 1.0f;
		retry_default_ = true;
	}
}
#endif

void Game::cdir()
{

}

void Game::cddr()
{
	auto context = devrsrc_->d3d_context();
	auto device = devrsrc_->d3d_device();
	primitive_batch_ = make_unique<PrimitiveBatch<VertexPositionColor>>(context);
	states_ = make_unique<CommonStates>(device);
	fxfactory_ = make_unique<EffectFactory>(device);
	effect_ = make_unique<BasicEffect>(device);
	effect_->SetVertexColorEnabled(true);
	model_ = Model::CreateFromCMO(device, L"..\\assets\\teapot.cmo", *fxfactory_);

	{
		void const* shaderByteCode;
		size_t byteCodeLength;
		effect_->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
		ThrowIfFailed(device->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength, input_layout_.ReleaseAndGetAddressOf()));
	}

	world_matrix_ = Matrix::Identity;
	CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE, D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP, D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
	ThrowIfFailed(device->CreateRasterizerState(&rastDesc, rasterizer_state_.ReleaseAndGetAddressOf()));
}

void Game::cwsdr()
{
	auto sz = devrsrc_->back_buffer_size();
	float aspectRatio = static_cast<float>(sz.x) / static_cast<float>(sz.y);
	float fovAngleY = XM_PI / 4.f;
	if (aspectRatio < 1.0f)
		fovAngleY *= 2.0f;
	view_matrix_ = Matrix::CreateLookAt(Vector3(0.f, 1.f, 5.f), Vector3::Zero, Vector3::UnitY);
	projection_matrix_ = Matrix::CreatePerspectiveFieldOfView(fovAngleY, aspectRatio, 0.1f, 10.0f);
	// Scale proportionally, as the buffer width and height.
	//projection_matrix_ *= Matrix::CreateScale(1000.f / float(bbuf_w), 1000.f / float(bbuf_h), 1.f);
	//projection_matrix_ = Matrix::CreateOrthographicOffCenter(0.f, float(bbuf_w), float(bbuf_h), 0.f, -1.f, 1.f);
	effect_->SetView(view_matrix_);
	effect_->SetProjection(projection_matrix_);
}

void Game::OnDeviceLoss()
{
	states_.reset();
	fxfactory_.reset();
	primitive_batch_.reset();
	effect_.reset();
	input_layout_.Reset();
	rasterizer_state_.Reset();
	model_.reset();
}

void Game::OnDeviceRestore()
{
	cddr();
	cwsdr();
}
