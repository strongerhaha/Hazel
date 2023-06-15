#pragma once
#include"entt.hpp"

#include"Hazel/Core/Timestep.h"
namespace Hazel {
	class Entity;
	class Scene 
	{
	public:
		Scene();
		~Scene();
		Entity CreateEntity(const std::string& name=std::string());

		//entt::registry& Reg() { return m_Registry; }
		void OnUpdata(Timestep ts);
	private:
		entt::registry m_Registry;//容器 ID
		friend class Entity;//entity可以直接访问这个类的私有属性。
	};


}