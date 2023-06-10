#pragma once
#include"Hazel/Core/Core.h"
namespace Hazel {
	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;//�ǲ���ֱ�ӻ�����������frame��
	};

	class Frambuffer
	{
	public:
		virtual void Bind()=0;
		virtual void Unbind() = 0;
		virtual const FramebufferSpecification& GetSpecification()const = 0;
		static Ref<Frambuffer> Create(const FramebufferSpecification& spec);
		virtual const uint32_t& GetColorAttachment()const=0;
	};
}