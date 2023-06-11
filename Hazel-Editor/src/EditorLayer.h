#pragma once
#include"Hazel.h"

namespace Hazel {
	class EditorLayer :public Hazel::Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Hazel::Event& e) override;
		virtual void OnAttach() override;
		virtual void OnDetach() override;

	private:
		OrthographicCameraController m_CameraController;
		Ref<Shader> m_FlatColorShader;
		Ref<SubTexture2D> m_SheetTestTexture;
		Ref<Texture2D> m_SheetTexture;
		Ref<Texture2D> m_Texture, m_HazelTexture;
		glm::vec4 m_SquareColor = { 1.5f,1.5f,0.8f,1 };
		Ref<VertexArray> m_VertexArray;
		Ref<VertexArray> m_SquareVA;
		


		Ref<Frambuffer>m_Framebuffer;

	
	};
}
