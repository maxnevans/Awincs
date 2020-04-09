#pragma once

#include "ComponentEventHandler.h"

namespace Awincs
{

	/*	
		IMPORTANT:	Do not modify there anything inside handleEvent methods! 
					const_cast<WinAPIWindowDefaultEventHandler&> in WinAPIWindow.cpp is performed!
	*/
	class WinAPIWindowDefaultEventHandler
		:
		public ComponentEvent::ComponentEventHandler
	{
	};
}