#include "HouyiCapability.h"
#include "Platform.h"
#include "HouyiLog.h"

namespace Houyi
{
	string Capability::GLSL_VERSION = "100";
	int Capability::GLSL_VERSION_INT = 100;

	void Capability::getGPUInfo()
	{
		GLSL_VERSION = reinterpret_cast< char const * >(glGetString(GL_SHADING_LANGUAGE_VERSION));
		LOGI("GLSL_VERSION: %s\n", GLSL_VERSION.c_str());
		if (GLSL_VERSION.find("1.00") == string::npos || currentPlatform() == PLATFORM_IOS)
		{
			GLSL_VERSION_INT = 140;
		}
	}

    bool Capability::useVBO()
    {
        // TODO dynamically find out
        return false;
    }
    
    bool Capability::useHardwareBone()
    {
        return false;
    }

    bool Capability::useNeon()
    {
        return true;
    }
}
