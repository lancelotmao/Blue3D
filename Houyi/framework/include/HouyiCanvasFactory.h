//
//  HouyiCanvasFactory.h
//  Houyi
//
//  Created by Lance Mao on 19/11/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiCanvasFactory_h
#define Houyi_HouyiCanvasFactory_h

#include "HouyiCanvas.h"
#include "Platform.h"

namespace Houyi
{
    class CanvasFactory
    {
    public:
        static CanvasPtr getCanvas(ImagePtr image);
        
    private:
        CanvasFactory(){}
    };
}

#endif
