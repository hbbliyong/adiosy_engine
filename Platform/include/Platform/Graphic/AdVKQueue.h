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
	private:
		uint32_t m_FamilyIndex;
		uint32_t m_Index;
		VkQueue m_Queue;
		bool m_CanPresent;
	};
} // namespace ade
