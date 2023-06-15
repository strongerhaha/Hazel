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
		entt::registry m_Registry;//���� ID
		friend class Entity;//entity����ֱ�ӷ���������˽�����ԡ�
	};


}