#pragma once
#include"Hazel.h"

#include"ParticleSystem.h"
class Sandbox2D:public Hazel::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	void OnUpdate(Hazel::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Hazel::Event& e) override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;

private:
	Hazel::OrthographicCameraController m_CameraController;
	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::Texture2D> m_SheetTexture;
	Hazel::Ref<Hazel::Texture2D> m_Texture, m_HazelTexture;
	glm::vec4 m_SquareColor = { 1.5f,1.5f,0.8f,1 };
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	ParticleSystem m_ParticleSystem;

	ParticleProps m_Particle;
};