#pragma once
#include<filesystem>
namespace Hazel {
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;//调用filesystem的path class，其实就是个string
	};

}