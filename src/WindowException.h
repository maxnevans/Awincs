#pragma once

#include <Exception.h>

namespace Awincs
{
    class WindowException
        :
        public Exception
    {
    public:
        using Exception::Exception;
    };
}