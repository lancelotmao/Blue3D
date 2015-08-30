//
//  HouyiVector4.cpp
//  Houyi
//
//  Created by Lance Mao on 20/3/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiVector4.h"
#include "HouyiMatrix4.h"

namespace Houyi
{
    void Vector4::transform(const float* m, const Vector4* vIn, Vector4* vOut)
    {
    #if DEBUG
        Matrix4 m4 = Matrix4(m);
        Matrix4 transpose = m4.transpose(); // not caring much about performance on simulator
        (*vOut) = transpose * (*vIn);
    #else
        asm volatile
        (
         
         "vldmia %1, {q1-q4 }     \n\t"
         "vldmia %2, {q5}         \n\t"
         
         "vmul.f32 q0, q1, d10[0] \n\t"
         "vmla.f32 q0, q2, d10[1] \n\t"
         "vmla.f32 q0, q3, d11[0] \n\t"
         "vmla.f32 q0, q4, d11[1] \n\t"
         
         "vstmia %0, {q0}"
         
         : // no output
         : "r" (vOut), "r" (m), "r" (vIn)
         : "memory", "q0", "q1", "q2", "q3", "q4", "q5"
         );
    #endif
    }
}
