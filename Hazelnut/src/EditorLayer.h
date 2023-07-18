#pragma once
#include"Hazel.h"
#include"Panels/SceneHierarchyPanel.h"
#include"Panels/ContentBrowserPanel.h"
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
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		
		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();

		void OnScenePlay();
		void OnSceneStop();

		// UI Panels
		void UI_Toolbar();
	private:

		OrthographicCameraController m_CameraController;
		Ref<Shader> m_FlatColorShader;
		Ref<SubTexture2D> m_SheetTestTexture;
		Ref<Texture2D> m_SheetTexture;
		Ref<Texture2D> m_Texture, m_HazelTexture;
		Ref<VertexArray> m_VertexArray;
		Ref<VertexArray> m_SquareVA;
		glm::vec2 m_ViewportSize{0.0f,0.0f};
		glm::vec2 m_ViewportBounds[2];
		Ref<Scene>m_ActiveScene;
		Ref<Framebuffer>m_Framebuffer;
		EditorCamera m_EditorCamera;
		int m_GizmoType = -1;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;
		Entity m_HoveredEntity;//当前是哪个entity
		bool m_PrimaryCamera = true;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
	
		enum class SceneState//用于开始和关闭按钮
		{
			Edit = 0, Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		// Editor resources
		Ref<Texture2D> m_IconPlay, m_IconStop;
	};
}
