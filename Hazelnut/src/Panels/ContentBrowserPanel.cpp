#include"hzpch.h"
#include"ContentBrowserPanel.h"
#include<imgui/imgui.h>

namespace Hazel {

	static const std::filesystem::path s_AssetsPath = "asset";//固定的
	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(s_AssetsPath)
	{
	}
	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");
		if (m_CurrentDirectory != std::filesystem::path(s_AssetsPath))//只能回到asset这
		{
			if (ImGui::Button("<-"))
				m_CurrentDirectory = m_CurrentDirectory.parent_path();//返回上一级
		}

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))//遍历这里的所有目录
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetsPath);
			std::string filenameString = relativePath.filename().string();
			if (directoryEntry.is_directory())//是不是在这个目录？该位置是否存在
			{
				if (ImGui::Button(filenameString.c_str()))//创建按钮，并判断是否呗点击
				{
					m_CurrentDirectory /= path.filename();// "/="在前面加一个‘/’
				}
			}
			else
			{
				if (ImGui::Button(filenameString.c_str()))//显示文件
				{

				}
			}
		}

		ImGui::End();
	}
}