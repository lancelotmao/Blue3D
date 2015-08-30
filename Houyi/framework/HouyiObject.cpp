//
//  HouyiObject.cpp
//  houyi
//
//  Created by Lance Mao on 12/08/2011.
//  Copyright 2011 Dong wen. All rights reserved.
//

#include "HouyiObject.h"

namespace Houyi
{
    Object::Object() : mId(-1)
    {
    }

    Object::~Object()
    {
    }

    const string& Object::getName() const
    {
        return mName;
    }

    void Object::setName(string name)
    {
        mName = name;
    }

    int Object::getId() const
    {
        return mId;
    }

    void Object::setId(int aId)
    {
        mId = aId;
    }
}
