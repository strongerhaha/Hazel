#pragma once
#include "Hazel/Core/UUID.h"
#include"Scene.h"
#include "Components.h"//嵌套了
#include"entt.hpp"
namespace Hazel {
	class Entity //提供方便的函数给entt
	{
	public:
		Entity() = default;
		Entity(entt::entity handle,Scene* scene);
		Entity(const Entity& other) = default;

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }
		template<typename T,typename... Args>
		T& AddComponent(Args&&... args)
		{
			HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has componet!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this,component);//添加时调用这函数
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have componet!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);//用模板T，判断有没有一个函数，T可以是任何，m_EntityHandle是registry里面的一个entity
		}

		template<typename T>
		void RemoveComponet()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have componet!");
			return m_Scene->m_Registry.remove<T>(m_EntityHandle);//用模板T，判断有没有一个函数，T可以是任何，m_EntityHandle是registry里面的一个entity
		}
		operator bool() const { return m_EntityHandle != entt::null; }
		operator uint32_t() const { return (uint32_t)(m_EntityHandle); }//重载uint32_t ，让他返回m_EntityHandle
		operator entt::entity() const { return m_EntityHandle; }//entt::entity()转类型的时候调用

		

		bool operator==(const Entity& other)const//重载==，如果传进来的是entity
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}
		bool operator!=(const Entity& other)const//重载==，如果传进来的是entity
		{
			return !(*this == other);
			//return !operator==(other);两种方式
		} 
		
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene=nullptr;//为啥不用weak ref，或者SharePoint，而用scene role pointer  12bit  只是个接口没有内容可以用这个？可能/Scene不想被删掉在内存中，仅仅只想指向一个地址，不用weak因为之后要转变为intrusive reference countingsystem

	};


}