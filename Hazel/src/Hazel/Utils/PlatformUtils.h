#pragma once
#include<string>
namespace Hazel {//接口类给不同platform提供接口
	class FileDialogs
	{
	public:
		//返回空strings如果失败了
		static std::string OpenFile(const char* filter);//filter是那些类型，可以打开的类型类似txt
		static std::string SaveFile(const char* filter);
	};
}