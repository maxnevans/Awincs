#pragma once

#include "pch.h"

namespace Awincs
{
	template<typename GNode>
	class Hierarchal
	{
	public:
		virtual const std::vector<std::shared_ptr<GNode>>& getChildren() const = 0;
		virtual const std::shared_ptr<GNode>& getParent() const = 0;
		virtual void setParent(std::shared_ptr<GNode>) = 0;
		virtual void unsetParent() = 0;
	};
}