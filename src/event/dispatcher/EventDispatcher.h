#pragma once

#include "pch.h"

#include "../event/CoreEvent.h"

namespace Awincs
{
	namespace Event
	{
		template<typename GHandler>
		class EventDispatcher
		{
		public:
			using DispatcherTypeHash = unsigned long long;

		public:
			EventDispatcher(std::shared_ptr<GHandler> root) : root(root) {}

			template<typename TEvent>
			bool dispatch(TEvent e)
			{
				if (!checkEventType(e))
					return true;

				beforeHandling(e);

				bool shouldHandleNext = true;

				while (auto handler = getNextEventHandler(e))
					shouldHandleNext = handler->handleEvent(e);

				afterHandling(e);

				return shouldHandleNext;
			}

		protected:
			virtual bool checkEventType(const CoreEvent& e) = 0;
			virtual std::shared_ptr<GHandler> getNextEventHandler(const CoreEvent& e) = 0;
			virtual void beforeHandling(const CoreEvent& e) {}
			virtual void afterHandling(const CoreEvent& e) {}

		protected:
			std::shared_ptr<GHandler> root;
		};
	}
}

