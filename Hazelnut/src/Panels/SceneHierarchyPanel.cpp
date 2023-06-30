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
		ImGui::PushID(label.c_str());//���ݲ�ͬ��label������ͬ�İ�ť֮�࣬��Ȼ��rotation��,"transform"����һ������

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f,lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f,0.1f,0.15f,1.0f });//ԭ����ɫ
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f,0.2f,0.2f,1.0f });//ͣ������
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f,0.2f,0.2f,1.0f });//��һ��
		if (ImGui::Button("X", buttonSize))//��������ť������
			values.x = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f,0.0f,0.0f,"%.2f");//fomatָ���Ǹ�ʽ
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))//��������ť������
			values.y = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize))//��������ť������
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
		m_Context->m_Registry.each([&](auto entityID)//ÿ�����ҳ���
		{	
			Entity entity{ entityID,m_Context.get() };
			DrawEntityNode(entity);//����entity���б���
		});
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())//�����������ʱ�����������ҳ���ڣ�0�����1���Ҽ�2���м� ֻ��"Scene Hierarchy"��Ч��
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
			DrawComponents(m_SelectionContext);//��ui�������Ի�������ui
			if (ImGui::Button("Add Component"))//��ť
				ImGui::OpenPopup("AddComponent");//����֮��ð����һ��AddComponent
			if (ImGui::BeginPopup("AddComponent"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_SelectionContext.AddComponent<CameraComponent>();//��ѡ�е�entity�������component
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("SpriteRenderer"))
				{
					m_SelectionContext.AddComponent<SpriteRendererComponent>();//��ѡ�е�entity�������component
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
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;//�ж�ѡ����ǲ���entity��
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())//�Ƿ��µ�
		{
			m_SelectionContext = entity;
		}
		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())//�Ҽ�
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;//�����ɾ������ú�������в���
			ImGui::EndPopup();
		}

		if (opened)//��֧�Ƿ�򿪣��Ƿ��и���ķ�֧
		{
			ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_OpenOnArrow;//�ж�ѡ����ǲ���entity��
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
	void SceneHierarchyPanel::DrawComponents(Entity entity)//��ui
	{
		if (entity.HasComponet<TagComponent>())//�����tagcom
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			
			char buffer[256];//��ʱ����������ĵ�����
			memset(buffer, 0, sizeof(buffer));//����
			strcpy_s(buffer, sizeof(buffer), tag.c_str());//�����д��
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))//������ı���
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
			DrawVec3Control("Translation", tc.Translation);//�Լ�д�Ŀ���X,Y,Z
			glm::vec3 rotation = glm::degrees(tc.Rotation);//��ؽǶ�
			DrawVec3Control("Rotation",rotation);//�ı�
			tc.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", tc.Scale,1.0f);//��resetvalue����Ϊ1.0f
			//ImGui::DragFloat3("Position", glm::value_ptr(tc.Translation), 0.5f);//speed=0.5f,���ص���bool���Կ�����if��Ҳ���������Ķ���,transform[3]�����mat4��translation
			ImGui::TreePop();//���transform��֧����ѡ���entity����
			}
			
			
		}
		if (entity.HasComponet<CameraComponent>())//�����CameraComponent
		{
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{//Camera�ķ�֧
				auto& cameraComponent = entity.GetComponent<CameraComponent>();//���ص���cameraComponent
				auto& camera = cameraComponent.Camera;
				ImGui::Checkbox("Primary", &cameraComponent.Primary);//���û�ı䡣
				const char* projectionTypeStrings[] = { "Perspective","Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];//���Camera�����Type��ѡ������P or O
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))//ѡ��
				{
					for (int i = 0; i < 2; i++)//��ѡһ
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))//ѡ��ɶ
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
					if (ImGui::DragFloat("Size", &verticalFOV))//���������
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
					if (ImGui::DragFloat("Size", &orthoSize))//���������
						camera.SetOrthographicSize(orthoSize);

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);
					if (ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio))//Fixed Aspect Ratio��ť
					{

					}
				}

				ImGui::TreePop();//���transform��֧����ѡ���entity����
			}

		}

		if (entity.HasComponet<SpriteRendererComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });//��ʽ����
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