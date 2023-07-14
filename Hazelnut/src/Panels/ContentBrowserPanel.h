#pragma once
#include<filesystem>
#include "Hazel/Renderer/Texture.h"
namespace Hazel {
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;//调用filesystem的path class，其实就是个string
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};

}