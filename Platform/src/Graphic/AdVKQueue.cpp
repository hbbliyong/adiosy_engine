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
}