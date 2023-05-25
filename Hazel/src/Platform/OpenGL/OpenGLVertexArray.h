#pragma once
#include"Hazel/Renderer/VertexArray.h"
#include"Platform/OpenGL/OpenGLBuffer.h"
namespace Hazel {

	class OpenGLVertexArray :public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;
		//virtual void SetIndexBuffer(const std::shared_ptr< OpenGLVertexBuffer>& indexBuffer) override;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffer()const { return m_VertexBuffer; };
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer()const { return m_IndexBuffer; };

	private:
		std::vector < std::shared_ptr<VertexBuffer>> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		uint32_t m_RendererID;
	};
}