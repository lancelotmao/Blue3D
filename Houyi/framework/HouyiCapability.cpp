#include "HouyiCapability.h"
#include "Platform.h"
#include "HouyiLog.h"

namespace Houyi
{
    bool Capability::useVBO()
    {
        // TODO dynamically find out
        return true;
    }
    
    bool Capability::useHardwareBone()
    {
        return true;
    }

    bool Capability::useNeon()
    {
        return true;
    }
}
