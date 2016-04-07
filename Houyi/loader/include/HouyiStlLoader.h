//
//  HouyiStlLoader.h
//  Houyi
//
//  Created by Ross on 14-6-6.
//  Copyright (c) 2014年 Lance Mao. All rights reserved.
//

#ifndef __Houyi__HouyiStlLoader__
#define __Houyi__HouyiStlLoader__

#include "HouyiLoader.h"
#include "HouyiVector.h"
#include "HouyiScene.h"
#include "HouyiMesh.h"

namespace Houyi
{
    class StlLoader : public Loader
    {
    public:
        virtual Scene* loadFromMemory(const char* data, int size);
    protected:
        vector<float> vertextArray;
    	vector<float> vertextNormalArray;
        float* vertexPosition;
        float* vertexNormal;
    };

}
#endif /* defined(__Houyi__HouyiStlLoader__) */
