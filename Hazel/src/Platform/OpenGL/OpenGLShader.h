#pragma once
#include"Hazel/Renderer/Shader.h"
#include"glm/glm.hpp"


typedef unsigned int GLenum;
namespace Hazel {
	class OpenGLShader:public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		virtual const std::string& GetName()const override { return m_Name; };

		void UploadUniformInt(const std::string name, const int& value);
		void UploadUniformMat4(const std::string name, const glm::mat4& matrix);
		void UploadUniformFloat(const std::string name, const float& value);
		void UploadUniformFloat2(const std::string name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string name, const glm::vec4& value);
		void UploadUniformMat3(const std::string name, const glm::mat3& matrix);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map < GLenum, std::string > PreProcess(const std::string& source);
		void Compile(const std::unordered_map < GLenum, std::string >& shaderSources);
	};
}