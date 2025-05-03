#pragma once
#include "Platform/Event/AdEventDispatcher.h"

namespace ade
{
	class AdEventObserver
	{
	public:
		AdEventObserver() = default;
		~AdEventObserver()
		{
			AdEventDispatcher::GetInstance()->DestroyObserver(this);
		}

		template<typename T>
		void OnEvent(const std::function<void(const T&)>& func)
		{
			AdEventDispatcher::GetInstance()->AddObserverHandler(this, func);
		}
	};
}