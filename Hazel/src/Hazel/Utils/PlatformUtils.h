#pragma once
#include<string>
namespace Hazel {//�ӿ������ͬplatform�ṩ�ӿ�
	class FileDialogs
	{
	public:
		//���ؿ�strings���ʧ����
		static std::string OpenFile(const char* filter);//filter����Щ���ͣ����Դ򿪵���������txt
		static std::string SaveFile(const char* filter);
	};
}