#pragma once
#include"Scene.h"
namespace Hazel {
	class SceneSerializer 
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);//整个Scene搞进来不用每个函数都搞一个
		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string filepath);
	private:
		Ref<Scene> m_Scene;
	};
}