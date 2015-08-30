//
//  HouyiConst.h
//  Houyi
//
//  Created by Lance Mao on 20/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiConst_h
#define Houyi_HouyiConst_h

namespace Houyi
{
    class Const
    {
    public:
        const static int ERenderModeTriangle = 1;
        const static int ERenderModeLine = 1 << 1;
        const static int ERenderModePoint = 1 << 2;
        const static int ERenderModeWireFrame = 1 << 3;

        enum // camera type
        {
            ENormal = 0,
            EModelView,
            EFirstPerson
        };
        
        // loading options
        const static int LO_MERGE_MESH = 1;
        const static int LO_USE_FP = 1 << 1; // whether to use first person camera
        const static int LO_Z_UP = 1 << 2;
        const static int LO_AABB = 1 << 3;
    };
}

#endif
