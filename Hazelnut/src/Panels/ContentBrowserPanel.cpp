#include"hzpch.h"
#include"ContentBrowserPanel.h"
#include<imgui/imgui.h>

namespace Hazel {

	extern const std::filesystem::path g_AssetPath = "asset";//�̶���
	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(g_AssetPath)
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}
	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");
		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))//ֻ�ܻص�asset��
		{
			if (ImGui::Button("<-"))
				m_CurrentDirectory = m_CurrentDirectory.parent_path();//������һ��
		}

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))//�������������Ŀ¼
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			std::string filenameString = relativePath.filename().string();
			ImGui::PushID(filenameString.c_str());//���ÿ�β�ͬ���ļ�
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;// directoryEntry.is_directory() �ǲ����ļ��У�
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));//����͸����
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });//�������ĸ�texture��Ϊ��ť
			if (ImGui::BeginDragDropSource())//��������scene
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))//˫���������IsItemHovered
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}
		
		/*
		if (directoryEntry.is_directory())//�ǲ��������Ŀ¼����λ���Ƿ����
			{
				if (ImGui::Button(filenameString.c_str()))//������ť�����ж��Ƿ��µ��
				{
					m_CurrentDirectory /= path.filename();// "/="��ǰ���һ����/��
				}
			}
		*/
			
			ImGui::TextWrapped(filenameString.c_str());//��ʾ����

			ImGui::NextColumn();//��һ��
			ImGui::PopID();
		}
			ImGui::Columns(1);

			ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
			ImGui::SliderFloat("Padding", &padding, 0, 32);

		ImGui::End();
	}
}