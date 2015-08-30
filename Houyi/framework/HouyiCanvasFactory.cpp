//
//  HouyiCanvasFactory.cpp
//  Houyi
//
//  Created by Lance Mao on 19/11/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#include "HouyiCanvasFactory.h"

namespace Houyi
{
    CanvasPtr CanvasFactory::getCanvas(ImagePtr image)
    {
#ifdef ANDROID
        return new AndroidCanvas(image);
#elif __APPLE__
        return new iOSCanvas(image);
#endif
    }
}
