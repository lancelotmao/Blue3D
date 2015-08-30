#ifndef __Houyi__HouyiCapability__
#define __Houyi__HouyiCapability__

#include <string>

using namespace std;

namespace Houyi
{
    class Capability
    {
    public:
    	static string GLSL_VERSION;
    	static int GLSL_VERSION_INT;

    	static void getGPUInfo();

        static bool useVBO();
        static bool useHardwareBone();
        static bool useNeon();
    };
}

#endif
