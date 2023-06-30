#include"SceneHierarchyPanel.h"
#include"imgui/imgui.h"
#include"imgui/imgui_internal.h"
#include<glm/gtc/type_ptr.hpp>
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
	static void DrawVec3Control(const std::string& label,glm::vec3& values,float resetValue=0.0f,float columnWidth=100.0f)//
	{
		ImGui::PushID(label.c_str());//根据不同的label创建不同的按钮之类，不然“rotation”,"transform"都由一个控制

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f,lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f,0.1f,0.15f,1.0f });//原本颜色
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f,0.2f,0.2f,1.0f });//停在上面
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f,0.2f,0.2f,1.0f });//摁一下
		if (ImGui::Button("X", buttonSize))//如果点击按钮就重置
			values.x = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f,0.0f,0.0f,"%.2f");//fomat指的是格式
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))//如果点击按钮就重置
			values.y = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize))//如果点击按钮就重置
			values.z = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		//ImGui::SameLine();


		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}
	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		m_Context->m_Registry.each([&](auto entityID)//每个都找出来
		{	
			Entity entity{ entityID,m_Context.get() };
			DrawEntityNode(entity);//单个entity在列表上
		});
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())//当鼠标左键点击时，并且鼠标在页面内，0是左键1是右键2是中键 只在"Scene Hierarchy"生效。
			m_SelectionContext = {};
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->CreateEntity("Empty Entity");
			ImGui::EndPopup();
		}
		ImGui::End();

		ImGui::Begin("Properties");
		if(m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);//画ui，把属性画出来在ui
			if (ImGui::Button("Add Component"))//按钮
				ImGui::OpenPopup("AddComponent");//摁了之后冒出来一个AddComponent
			if (ImGui::BeginPopup("AddComponent"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_SelectionContext.AddComponent<CameraComponent>();//给选中的entity加摄像机component
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("SpriteRenderer"))
				{
					m_SelectionContext.AddComponent<SpriteRendererComponent>();//给选中的entity加摄像机component
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

		}
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
		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())//右键
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;//最后在删掉，免得后面的运行不了
			ImGui::EndPopup();
		}

		if (opened)//分支是否打开，是否有更多的分支
		{
			ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_OpenOnArrow;//判断选择的是不是entity？
			bool opened = ImGui::TreeNodeEx((void*)9841147, flags, tag.c_str());
			if(opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			m_SelectionContext = {};
		}
	}
	void SceneHierarchyPanel::DrawComponents(Entity entity)//画ui
	{
		if (entity.HasComponet<TagComponent>())//如果有tagcom
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			
			char buffer[256];//暂时用来保存更改的名字
			memset(buffer, 0, sizeof(buffer));//重置
			strcpy_s(buffer, sizeof(buffer), tag.c_str());//将这个写入
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))//如果他改变了
			{
				tag = std::string(buffer);
			}

		}
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

		if (entity.HasComponet<TransformComponent>())
		{
			bool open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform");
			
			if (open)
			{
			auto& tc = entity.GetComponent<TransformComponent>();
			DrawVec3Control("Translation", tc.Translation);//自己写的控制X,Y,Z
			glm::vec3 rotation = glm::degrees(tc.Rotation);//变回角度
			DrawVec3Control("Rotation",rotation);//改变
			tc.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", tc.Scale,1.0f);//把resetvalue设置为1.0f
			//ImGui::DragFloat3("Position", glm::value_ptr(tc.Translation), 0.5f);//speed=0.5f,返回的是bool所以可以用if，也会调用里面的东西,transform[3]这个是mat4的translation
			ImGui::TreePop();//添加transform分支，在选择的entity里面
			}
			
			
		}
		if (entity.HasComponet<CameraComponent>())//如果有CameraComponent
		{
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{//Camera的分支
				auto& cameraComponent = entity.GetComponent<CameraComponent>();//返回的是cameraComponent
				auto& camera = cameraComponent.Camera;
				ImGui::Checkbox("Primary", &cameraComponent.Primary);//引用会改变。
				const char* projectionTypeStrings[] = { "Perspective","Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];//获得Camera里面的Type来选择类型P or O
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))//选择
				{
					for (int i = 0; i < 2; i++)//二选一
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))//选了啥
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float verticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Size", &verticalFOV))//如果她变了
						camera.SetPerspectiveVerticalFOV(glm::radians(verticalFOV));

					float orthoFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetPerspectiveFarClip(orthoFar);

					float orthoNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetPerspectiveNearClip(orthoNear);

				}
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize= camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))//如果她变了
						camera.SetOrthographicSize(orthoSize);

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);
					if (ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio))//Fixed Aspect Ratio按钮
					{

					}
				}

				ImGui::TreePop();//添加transform分支，在选择的entity里面
			}

		}

		if (entity.HasComponet<SpriteRendererComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });//格式设置
			bool open = ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer");
			ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
			if (ImGui::Button("+", ImVec2{20, 20}))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			ImGui::PopStyleVar();
			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;
				ImGui::EndPopup();
			}

			if (open)
			{
				auto& src = entity.GetComponent<SpriteRendererComponent>();
				ImGui::ColorEdit4("Color", glm::value_ptr(src.Color));
				ImGui::TreePop();
			}
			if (removeComponent)
				entity.RemoveComponet<SpriteRendererComponent>();
		}
	}
}