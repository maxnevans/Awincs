#pragma once

#include "Hierarchal.h"

namespace Awincs
{
	template<typename GNode>
	class HierarchalIterator
	{
	private:
		struct HierarchalStackFrame
		{
			const std::vector<std::shared_ptr<Hierarchal>>& data;
			std::shared_ptr<HierarchalStackFrame> parent = nullptr;
			size_t index = 0;
		};

	public:
		HierarchalIterator(const std::shared_ptr<Hierarchal>& root)
			:
			root(root)
		{}
		bool next()
		{
			if (init())
				return true;

			if (down())
				return true;

			if (right())
				return true;

			if (up())
				return true;

			return false;
		}
		inline std::shared_ptr<Hierarchal> get()
		{
			return stack->data[stack->index];
		}

	private:
		bool init()
		{
			if (stack->data.empty())
			{
				stack = std::make_shared<TraverseStackFrame>(std::vector<std::shared_ptr<Hierarchal>>{root}, nullptr, 0);
				return true;
			}

			return false;
		}
		bool down()
		{
			const auto& children = stack->data[stack->index]->getChildren();

			if (children.empty())
				return false;

			auto newStack = std::make_shared<TraverseStackFrame>(children, stack, 0);
			stack = newStack;

			return true;
		}
		bool right()
		{
			if (stack->index + 1 >= stack->data.size())
				return false;

			stack->index++;

			return true;
		}
		bool up()
		{
			if (!stack->parent)
				return false;

			stack = stack->parent;

			return true;
		}

	private:
		std::shared_ptr<HierarchalStackFrame> stack;
		const std::shared_ptr<Hierarchal>& root;
	};
}