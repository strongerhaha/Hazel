#include <Hazel.h>
#include"Platform/OpenGL/OpenGLShader.h"
#include"imgui/imgui.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.0f, 1.0f, -1.0f, 1.0f), m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(Hazel::VertexArray::Create());

		float vertices[3 * 7] = {
				-0.5f,-0.5f,0.0f,1.0f,0.0f,1.0f,1.0f,
				 0.5f,-0.5f,0.0f,1.0f,0.0f,1.0f,1.0f,
				 0.0f, 0.5f,0.0f,1.0f,0.0f,1.0f,1.0f
		};

		Hazel::Ref<Hazel::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));

		Hazel::BufferLayout layout = {
			{Hazel::ShaderDataType::Float3,"a_Position"},
			{Hazel::ShaderDataType::Float4,"a_Color"}
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer); //������������OpenGLVertexBuffer��

		uint32_t indices[3] = { 0,1,2 };
		Hazel::Ref<Hazel::IndexBuffer> IndexBuffer;
		IndexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(IndexBuffer);



		m_SquareVA.reset(Hazel::VertexArray::Create());

		float squareVertices[5 * 4] = {
			-0.5f,-0.5f,0.0f,0.0f,0.0f,
			 0.5f,-0.5f,0.0f,1.0f,0.0f,
			 0.5f, 0.5f,0.0f,1.0f,1.0f,
			 -0.5f,0.5f,0.0f,0.0f,1.0f
		};

		Hazel::Ref<Hazel::VertexBuffer> squareVB;
		squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		squareVB->SetLayout({
			{Hazel::ShaderDataType::Float3,"a_Position"},
			{Hazel::ShaderDataType::Float2,"a_TexCoord"}
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0,1,2,2,3,0 };
		Hazel::Ref<Hazel::IndexBuffer>squareIB;
		squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
		#version 330 core
		layout (location =0 )in vec3 a_Position;
		layout (location =1 )in vec4 a_Color;
		
		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;
		out vec3 v_Position;
		out vec4 v_Color;
		void main()
		{
			v_Position=a_Position;
			v_Color=a_Color;
			gl_Position=u_ViewProjection*u_Transform*vec4(a_Position,1.0f);
		}
		)";

		std::string fragmentSrc = R"(
		#version 330 core
		layout (location =0 )out vec4 color;
		in vec3 v_Position;
		in vec4 v_Color;
		void main()
		{
			color=vec4(v_Position+0.5f,1.0f);//rgba
			color=v_Color;
		}
		)";

		m_Shader = Hazel::Shader::Create("VertexPosColor",vertexSrc, fragmentSrc);


		std::string flatColorShaderVertexSrc = R"(
		#version 330 core
		layout (location =0 )in vec3 a_Position;
		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;
		out vec3 v_Position;
		void main()
		{
			v_Position=a_Position;
			gl_Position=u_ViewProjection * u_Transform*vec4(a_Position,1.0f);
		}
		)";

		std::string  flatColorShaderFragmentSrc = R"(
		#version 330 core
		layout (location =0 )out vec4 color;
		in vec3 v_Position;

		uniform vec3 u_Color;
		void main()
		{
			color=vec4(u_Color,1.0);//rgba
		}
		)";

		m_FlatColorShader=Hazel::Shader::Create("FlatColor",flatColorShaderVertexSrc, flatColorShaderFragmentSrc);
	

		//auto textureShader=m_ShaderLibrary.Load("asset/shaders/Texture.glsl");
		m_TextureShader=Hazel::Shader::Create("asset/shaders/Texture.glsl");

		m_Texture=(Hazel::Texture2D::Create("asset/textures/awesomeface.png"));
		m_HazelTexture = (Hazel::Texture2D::Create("asset/textures/container.jpg"));
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TextureShader)->Bind();

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
	
}

	void OnUpdate(Hazel::Timestep ts) override 
	{
		HZ_TRACE("Delta time :{0}s,({1})ms", ts.GetSeconds(),ts.GetMilliseconds());

		if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed*ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		if (Hazel::Input::IsKeyPressed(HZ_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts ;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
			m_CameraRotation -= m_CameraRotationSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_CameraRotation += m_CameraRotationSpeed * ts;

	

		Hazel::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1 });
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
		Hazel::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
			
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

	

		for (int y = 0; y < 20; y++)
		{
			for (int  x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0);
		glm::mat4 tansform = glm::translate(glm::mat4(1.0f), pos)* scale;
		Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, tansform);
			}
		}

		//auto textureShader = m_ShaderLibrary.Get("Texture");

		m_HazelTexture->Bind(0);
		Hazel::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_Texture->Bind(0);
		Hazel::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));


		//Hazel::Renderer::Submit(m_Shader, m_VertexArray);
		Hazel::Renderer::EndScene();

	}
	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color",glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Hazel::Event& event) override
	{
	}
private:
	Hazel::ShaderLibrary m_ShaderLibrary;
	Hazel::Ref<Hazel::Shader> m_Shader;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;		
	Hazel::Ref<Hazel::Shader> m_TextureShader;
	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;

	Hazel::Ref<Hazel::Texture2D> m_Texture, m_HazelTexture;

	Hazel::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraRotation = 0.0f;
	float m_CameraMoveSpeed = 0.1f;
	float m_CameraRotationSpeed = 2.0f;
	glm::vec3 m_SquareColor = {0.0f,0.2f,0.8f};
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}