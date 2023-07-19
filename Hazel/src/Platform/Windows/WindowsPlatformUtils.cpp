#include"hzpch.h"
#include"Hazel/Utils/PlatformUtils.h"
#include"Hazel/Core/Application.h"
#include<commdlg.h>
#include<GLFW/glfw3.h>//�ô���֪�����Ǹ��򴰿ڴ��ļ���
#define GLFW_EXPOSE_NATIVE_WIN32
#include<GLFW/glfw3native.h>
namespace Hazel {

	 std::string FileDialogs::OpenFile(const char* filter)//filter����Щ���ͣ����Դ򿪵���������txt
	 {
		 OPENFILENAME ofn;
		 CHAR szFile[260] = { 0 };
		 CHAR currentDir[256] = { 0 };
		 ZeroMemory(&ofn, sizeof(OPENFILENAME));
		 ofn.lStructSize = sizeof(OPENFILENAME);
		 ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());//��ofn֪����glfw��window
		 ofn.lpstrFile = (LPWSTR)szFile;
		 ofn.nMaxFile = sizeof(szFile);
		 if (GetCurrentDirectoryA(256, currentDir))
			 ofn.lpstrInitialDir = (LPWSTR)currentDir;
		 ofn.lpstrFilter = (LPWSTR)filter;
		 ofn.nFilterIndex = 1;
		 ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		 if (GetOpenFileNameA(&ofn) == TRUE)
		 {
		 return ofn.lpstrFile;
		 }
		 return std::string();
	 }
	 std::string FileDialogs::SaveFile(const char* filter)
	 {
		 OPENFILENAME ofn;
		 CHAR szFile[260] = { 0 };
		 CHAR currentDir[256] = { 0 };
		 ZeroMemory(&ofn, sizeof(OPENFILENAME));
		 ofn.lStructSize = sizeof(OPENFILENAME);
		 ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());//��ofn֪����glfw��window
		 ofn.lpstrFile = (LPWSTR)szFile;
		 ofn.nMaxFile = sizeof(szFile);
		 if (GetCurrentDirectoryA(256, currentDir))
			 ofn.lpstrInitialDir = (LPWSTR)currentDir;
		 ofn.lpstrFilter = (LPWSTR)filter;
		 ofn.nFilterIndex = 1;

		 ofn.lpstrDefExt = (LPWSTR)strchr(filter, '\0') + 1;//Added default extension string when saving a scene 

		 ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		 if (GetSaveFileNameA(&ofn) == TRUE)
		{
			 return ofn.lpstrFile;
		 }
		 return std::string();
	 }
}