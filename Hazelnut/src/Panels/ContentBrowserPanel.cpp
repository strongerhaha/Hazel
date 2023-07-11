#include"hzpch.h"
#include"ContentBrowserPanel.h"
#include<imgui/imgui.h>

namespace Hazel {

	static const std::filesystem::path s_AssetsPath = "asset";//�̶���
	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(s_AssetsPath)
	{
	}
	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");
		if (m_CurrentDirectory != std::filesystem::path(s_AssetsPath))//ֻ�ܻص�asset��
		{
			if (ImGui::Button("<-"))
				m_CurrentDirectory = m_CurrentDirectory.parent_path();//������һ��
		}

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))//�������������Ŀ¼
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetsPath);
			std::string filenameString = relativePath.filename().string();
			if (directoryEntry.is_directory())//�ǲ��������Ŀ¼����λ���Ƿ����
			{
				if (ImGui::Button(filenameString.c_str()))//������ť�����ж��Ƿ��µ��
				{
					m_CurrentDirectory /= path.filename();// "/="��ǰ���һ����/��
				}
			}
			else
			{
				if (ImGui::Button(filenameString.c_str()))//��ʾ�ļ�
				{

				}
			}
		}

		ImGui::End();
	}
}