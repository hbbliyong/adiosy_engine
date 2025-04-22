#include "Graphic/AdVKQueue.h"


namespace ade
{
	AdVKQueue::AdVKQueue(uint32_t familyIndex, uint32_t index, VkQueue queue, bool canPresent):
		m_FamilyIndex(familyIndex),m_Index(index),m_Queue(queue),m_CanPresent(canPresent)
	{
		LOG_T("Create a new queue: {0} - {1} - {2}, present: {3}", m_FamilyIndex, index, (void*)queue, canPresent);
	}
	void AdVKQueue::WaitIdle() const
	{
		vkQueueWaitIdle(m_Queue);
	}
	void AdVKQueue::Submit(std::vector<VkCommandBuffer> cmdBuffers, const std::vector<VkSemaphore>& waitSemaphores,
		const std::vector<VkSemaphore>& signalSemaphores,VkFence fence)
	{
		VkPipelineStageFlags waitDstStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo = {
			.sType=VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext=nullptr,
			.waitSemaphoreCount=static_cast<uint32_t>(waitSemaphores.size()),
			.pWaitSemaphores=waitSemaphores.data(),
			.pWaitDstStageMask= waitDstStageMask,
			.commandBufferCount=static_cast<uint32_t>(cmdBuffers.size()),
			.pCommandBuffers=cmdBuffers.data(),
			.signalSemaphoreCount=static_cast<uint32_t>(signalSemaphores.size()),
			.pSignalSemaphores=signalSemaphores.data()
		};
		CALL_VK(vkQueueSubmit(m_Queue, 1, &submitInfo, fence));
	}
}