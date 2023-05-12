#pragma once
#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();
int main(int argc,char** argv)
{
	Hazel::Log::Iint();
	HZ_CORE_ERROR("log!");
	int a = 22;
	HZ_INFO("hello Var={0}",a);
	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif