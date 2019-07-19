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
	void GetDefaultSize(int& w, int& h) const;

private:
	void Update(DX::StepTimer const& timer);
	void Render();
	void Clear();

	void cdir();
	void cddr();
	void cwsdr();

	void XM_CALLCONV DrawGrid(DirectX::FXMVECTOR xAxis, DirectX::FXMVECTOR yAxis, DirectX::FXMVECTOR origin, size_t xdivs, size_t ydivs, DirectX::GXMVECTOR color);

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	std::unique_ptr<DX::DeviceResources> m_devrsrc;
	std::unique_ptr<DirectX::GamePad> m_gamepad;
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse>	m_mouse;
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::EffectFactory>	m_fxFactory;
	std::unique_ptr<DirectX::GeometricPrimitive> m_shape;
	DX::StepTimer m_timer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	DirectX::SimpleMath::Matrix	m_world;
	DirectX::SimpleMath::Matrix	m_view;
	DirectX::SimpleMath::Matrix	m_proj;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterState;
#ifdef DXTK_AUDIO
	std::unique_ptr<DirectX::AudioEngine>				m_audioEngine;
	std::unique_ptr<DirectX::WaveBank>					m_wavebank;
	std::unique_ptr<DirectX::SoundEffect>				m_soundeEffect;
	std::unique_ptr<DirectX::SoundEffectInstance>		m_soundFxInst1;
	std::unique_ptr<DirectX::SoundEffectInstance>		m_soundFxInst2;
	uint32_t											m_audioEvent;
	float												m_audioTimerAcc;
	bool												m_retryDefault;
#endif
};
