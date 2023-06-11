#pragma once
#include"Hazel/Core/Core.h"
namespace Hazel {
	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;//是不是直接画出来，不用frame，
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind()=0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t Width, uint32_t Height) = 0;
		
		virtual const FramebufferSpecification& GetSpecification()const = 0;
		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
		virtual const uint32_t& GetColorAttachment()const=0;
	};
}