#pragma once

#include "EventDispatcher.h"
#include "KeyboardEventHandler.h"
#include "../Events.h"

namespace Awincs
{
	namespace Event
	{
		template<typename GHandler>
		struct KeyboardEventDispatcherContext
		{
			std::shared_ptr<KeyboardEventHandler<GHandler>> focused;
		};

		template<typename GHandler>
		class KeyboardEventDispatcher
			:
			public EventDispatcher<KeyboardEventHandler<GHandler>>
		{
		public:
			void setFocus(std::shared_ptr<KeyboardEventHandler<GHandler>> handler)
			{
				focused = handler;
			}
			void setupContext(KeyboardEventDispatcherContext<GHandler> ctx)
			{
				focused = ctx.focused;
			}

		protected:
			virtual bool checkEventType(const CoreEvent& e) override
			{
				const auto ke = dynamic_cast<const Keyboard::Event*>(&e);
				if (ke == nullptr)
					return false;

				keyboardEvent = *ke;

				return true;
			}
			virtual std::shared_ptr<KeyboardEventHandler<GHandler>> getNextEventHandler(const CoreEvent& e) override
			{
				current = nullptr;
				return focused;
			}
			virtual void beforeHandling(const CoreEvent& e) override
			{
				current = focused;
			}

		public:
			static inline typename EventDispatcher<GHandler>::DispatcherTypeHash typeId = 0;

		private:
			std::optional<Keyboard::Event> keyboardEvent = std::nullopt;
			std::shared_ptr<KeyboardEventHandler<GHandler>> current = nullptr;
			std::shared_ptr<KeyboardEventHandler<GHandler>> focused = nullptr;
		};
	}
}