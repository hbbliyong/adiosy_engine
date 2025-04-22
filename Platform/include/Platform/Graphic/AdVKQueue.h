#pragma once

#include "AdVKCommon.h"

namespace ade
{
	class AdVKQueue
	{
	public:
		AdVKQueue(uint32_t familyIndex, uint32_t index, VkQueue queue, bool canPresent);
		~AdVKQueue() = default;
		void WaitIdle()const;
		void Submit(std::vector<VkCommandBuffer> cmdBuffers,const std::vector<VkSemaphore>& waitSemaphores,
								const std::vector<VkSemaphore>& signalSemaphores, VkFence fence);
		VkQueue GetHandle() const { return m_Queue; }
	private:
		uint32_t m_FamilyIndex;
		uint32_t m_Index;
		VkQueue m_Queue;
		bool m_CanPresent;
	};
} // namespace ade
