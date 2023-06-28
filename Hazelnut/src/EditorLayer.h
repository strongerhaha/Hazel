#pragma once
#include"Hazel.h"
#include"Panels/SceneHierarchyPanel.h"
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
		Ref<VertexArray> m_VertexArray;
		Ref<VertexArray> m_SquareVA;
		glm::vec2 m_ViewportSize{0.0f,0.0f};
		Ref<Scene>m_ActiveScene;
		Ref<Framebuffer>m_Framebuffer;


		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		bool m_PrimaryCamera = true;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		//Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;

	};
}
