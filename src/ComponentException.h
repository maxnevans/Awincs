#pragma once

#include "Exception.h"

namespace Awincs
{
    class ComponentException
        :
        public Exception
    {
    public:
        using Exception::Exception;
    };
}