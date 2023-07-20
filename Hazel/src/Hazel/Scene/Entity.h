#pragma once
#include "Hazel/Core/UUID.h"
#include"Scene.h"
#include "Components.h"//Ƕ����
#include"entt.hpp"
namespace Hazel {
	class Entity //�ṩ����ĺ�����entt
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
			m_Scene->OnComponentAdded<T>(*this,component);//���ʱ�����⺯��
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
			return m_Scene->m_Registry.has<T>(m_EntityHandle);//��ģ��T���ж���û��һ��������T�������κΣ�m_EntityHandle��registry�����һ��entity
		}

		template<typename T>
		void RemoveComponet()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have componet!");
			return m_Scene->m_Registry.remove<T>(m_EntityHandle);//��ģ��T���ж���û��һ��������T�������κΣ�m_EntityHandle��registry�����һ��entity
		}
		operator bool() const { return m_EntityHandle != entt::null; }
		operator uint32_t() const { return (uint32_t)(m_EntityHandle); }//����uint32_t ����������m_EntityHandle
		operator entt::entity() const { return m_EntityHandle; }//entt::entity()ת���͵�ʱ�����

		

		bool operator==(const Entity& other)const//����==���������������entity
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}
		bool operator!=(const Entity& other)const//����==���������������entity
		{
			return !(*this == other);
			//return !operator==(other);���ַ�ʽ
		} 
		
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene=nullptr;//Ϊɶ����weak ref������SharePoint������scene role pointer  12bit  ֻ�Ǹ��ӿ�û�����ݿ��������������/Scene���뱻ɾ�����ڴ��У�����ֻ��ָ��һ����ַ������weak��Ϊ֮��Ҫת��Ϊintrusive reference countingsystem

	};


}