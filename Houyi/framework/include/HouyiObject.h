//
//  HouyiObject.h
//  houyi
//
//  Created by Lance Mao on 12/08/2011.
//  Copyright 2011 Dong wen. All rights reserved.
//

#ifndef HOUYIOBJECT_H
#define HOUYIOBJECT_H

#define Profile 1

#include <string>
#include "HouyiAllocator.h"

using namespace std;

namespace Houyi
{
    class Object: public Allocator
    {
    protected:
        int mId;
        string mName;

    public:
        Object();
        virtual ~Object();
        virtual const string& getName() const;
        virtual void setName(string name);
        virtual int getId() const;
        virtual void setId(int aId);
    };
}

#endif
