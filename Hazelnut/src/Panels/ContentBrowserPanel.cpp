#include"hzpch.h"
#include"ContentBrowserPanel.h"
#include<imgui/imgui.h>

namespace Hazel {

	extern const std::filesystem::path g_AssetPath = "asset";//固定的
	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(g_AssetPath)
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}
	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");
		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))//只能回到asset这
		{
			if (ImGui::Button("<-"))
				m_CurrentDirectory = m_CurrentDirectory.parent_path();//返回上一级
		}

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))//遍历这里的所有目录
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			std::string filenameString = relativePath.filename().string();
			ImGui::PushID(filenameString.c_str());//辨别每次不同的文件
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;// directoryEntry.is_directory() 是不是文件夹？
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));//背景透明化
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });//画出来哪个texture作为按钮
			if (ImGui::BeginDragDropSource())//拖拉生成scene
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))//双击左键并且IsItemHovered
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}
		
		/*
		if (directoryEntry.is_directory())//是不是在这个目录？该位置是否存在
			{
				if (ImGui::Button(filenameString.c_str()))//创建按钮，并判断是否呗点击
				{
					m_CurrentDirectory /= path.filename();// "/="在前面加一个‘/’
				}
			}
		*/
			
			ImGui::TextWrapped(filenameString.c_str());//显示文字

			ImGui::NextColumn();//下一行
			ImGui::PopID();
		}
			ImGui::Columns(1);

			ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
			ImGui::SliderFloat("Padding", &padding, 0, 32);

		ImGui::End();
	}
}