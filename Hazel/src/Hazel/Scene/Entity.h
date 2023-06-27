#pragma once
#include"Scene.h"
#include"entt.hpp"
namespace Hazel {
	class Entity //�ṩ����ĺ�����entt
	{
	public:
		Entity() = default;
		Entity(entt::entity handle,Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T,typename... Args>
		T& AddComponent(Args&&... args)
		{
			HZ_CORE_ASSERT(!HasComponet<T>(), "Entity already has componet!");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			HZ_CORE_ASSERT(HasComponet<T>(), "Entity does not have componet!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponet()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);//��ģ��T���ж���û��һ��������T�������κΣ�m_EntityHandle��registry�����һ��entity
		}

		template<typename T>
		void RemoveComponet()
		{
			HZ_CORE_ASSERT(!HasComponet<T>(), "Entity does not have componet!");
			return m_Scene->m_Registry.remove<T>(m_EntityHandle);//��ģ��T���ж���û��һ��������T�������κΣ�m_EntityHandle��registry�����һ��entity
		}
		operator bool() const { return m_EntityHandle != entt::null; }

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene=nullptr;//Ϊɶ����weak ref������SharePoint������scene role pointer  12bit  ֻ�Ǹ��ӿ�û�����ݿ��������������/Scene���뱻ɾ�����ڴ��У�����ֻ��ָ��һ����ַ������weak��Ϊ֮��Ҫת��Ϊintrusive reference countingsystem

	};


}