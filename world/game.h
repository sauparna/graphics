#pragma once

#include "devrsrc.h"
#include "steptimer.h"

class Game final : public DX::IDeviceNotify
{
public:
	Game(DirectX::XMUINT2 backBufSize = DirectX::XMUINT2{ 400, 300 }) noexcept(false);
	~Game();
	void Initialize(HWND window);
	void Tick();
	virtual void OnDeviceLoss() override;
	virtual void OnDeviceRestore() override;
	void OnActivate();
	void OnDeactivate();
	void OnSuspend();
	void OnResume();
	void OnWindowMove();
	void OnWindowResize(size_t w, size_t h);
#ifdef DXTK_AUDIO
	void NewAudioDevice(void);
#endif
	DirectX::XMUINT2 device_back_buffer_size() const;

private:
	void cdir();
	void cddr();
	void cwsdr();
	void update(DX::StepTimer const& timer);
	void render();
	void clear();
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> primitive_batch_;
	std::unique_ptr<DX::DeviceResources> devrsrc_;
	std::unique_ptr<DirectX::GamePad> gamepad_;
	std::unique_ptr<DirectX::Keyboard> keyboard_;
	std::unique_ptr<DirectX::Mouse>	mouse_;
	std::unique_ptr<DirectX::CommonStates> states_;
	std::unique_ptr<DirectX::BasicEffect> effect_;
	std::unique_ptr<DirectX::IEffectFactory> fxfactory_;
	std::unique_ptr<DirectX::GeometricPrimitive> geometric_primitive_;
	std::unique_ptr<DirectX::Model> model_;
	DX::StepTimer step_timer_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_;
	DirectX::SimpleMath::Matrix	world_matrix_, view_matrix_, projection_matrix_;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state_;
#ifdef DXTK_AUDIO
	std::unique_ptr<DirectX::AudioEngine>				audio_engine_;
	std::unique_ptr<DirectX::WaveBank>					wave_bank_;
	std::unique_ptr<DirectX::SoundEffect>				sound_effect_;
	std::unique_ptr<DirectX::SoundEffectInstance>		sound_effect_instance1_;
	std::unique_ptr<DirectX::SoundEffectInstance>		sound_effect_instance2_;
	uint32_t											audio_event_;
	float												audio_timer_acc_;
	bool												retry_default_;
#endif
};
