#pragma once
#include"RendererAPI.h"

namespace Hazel {

	class RenderCommand
	{
	public:
		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);//并不是自己调用自己。RendererAPI根据api 调用
		}
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}