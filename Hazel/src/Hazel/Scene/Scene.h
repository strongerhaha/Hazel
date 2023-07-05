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
		void DestroyEntity(Entity entity);
		//entt::registry& Reg() { return m_Registry; }
		void OnUpdata(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		uint32_t m_ViewportWidth = 0, m_ViewportHeight=0;
		entt::registry m_Registry;//容器 ID
		friend class Entity;//entity可以直接访问这个类的私有属性。
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};


}