//
//  HouyiDef.h
//  Houyi
//
//  Created by Lance Mao on 26/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiDef_h
#define Houyi_HouyiDef_h

#if defined(__i386__) \
|| defined(i386)     \
|| defined(_M_IX86)  \
|| defined(_X86_)    \
|| defined(__THW_INTEL)
#define WTF_CPU_X86 1
#endif

#include <string>

namespace Houyi
{
    
    typedef float Real;

    typedef unsigned char uchar;
    typedef unsigned short ushort;
    typedef unsigned int uint;
    typedef unsigned long ulong;

    class AxisAlignedBox;
    class Degree;
    class Matrix3;
    class Matrix4;
    class Radian;
    class Ray;
    class Plane;
    class Quaternion;
    class Sphere;
    class Vector2;
    class Vector3;
    class Vector4;
}

#endif
