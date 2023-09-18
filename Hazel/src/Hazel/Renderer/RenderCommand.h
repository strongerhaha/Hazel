#pragma once
#include"RendererAPI.h"

namespace Hazel {

	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}
		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray,uint32_t count=0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, count);//并不是自己调用自己。RendererAPI根据api 调用
		}
		inline static void DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawArrays(vertexArray, count);//并不是自己调用自己。RendererAPI根据api 调用
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