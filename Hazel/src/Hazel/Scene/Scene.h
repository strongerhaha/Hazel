#pragma once
#include"entt.hpp"
#include "Hazel/Core/UUID.h"
#include"Hazel/Renderer/Editorcamera.h"
#include"Hazel/Core/Timestep.h"
class b2World;
namespace Hazel {
	class Entity;
	class Scene 
	{
	public:
		Scene();
		~Scene();
		static Ref<Scene> Copy(Ref<Scene> other);
		Entity CreateEntity(const std::string& name=std::string());
		Entity CreateLightEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());//用uuid创建entity

		void DestroyEntity(Entity entity);
		//entt::registry& Reg() { return m_Registry; }
		//2D 物理
		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateRuntime(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity entity);//

		Entity GetPrimaryCameraEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		uint32_t m_ViewportWidth = 0, m_ViewportHeight=0;
		entt::registry m_Registry;//容器 ID
		b2World* m_PhysicsWorld = nullptr;
		friend class Entity;//entity可以直接访问这个类的私有属性。
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};


}