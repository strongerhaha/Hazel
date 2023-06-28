#include"SceneHierarchyPanel.h"
#include"imgui/imgui.h"
#include"Hazel/Scene/Components.h"
namespace Hazel {
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}
	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		m_Context->m_Registry.each([&](auto entityID)
		{	
			Entity entity{ entityID,m_Context.get() };
			DrawEntityNode(entity);

			
		
		});
		ImGui::End();
	}
	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;//判断选择的是不是entity？
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())//是否呗点
		{
			m_SelectionContext = entity;
		}
		if (opened)//分支是否打开，是否有更多的分支
		{
			ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_OpenOnArrow;//判断选择的是不是entity？
			bool opened = ImGui::TreeNodeEx((void*)9841147, flags, tag.c_str());
			if(opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
	}
}