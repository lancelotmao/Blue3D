//
//  HouyiArray.h
//  Houyi
//
//  Created by Lance Mao on 9/6/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiArray_h
#define Houyi_HouyiArray_h

#include "SmartPtr.h"
#include "RefCount.h"

namespace Houyi
{
    template< typename T > class Array : public RefCountBase
    {
    public:
        Array() : mSize(0), mData(0)
        {
            
        }
        
        Array (int s)
        {
            mSize = s;
            mData = new T[mSize];
        }
        
        ~Array()
        {
            delete[] mData;
        }
        
        inline int size()
        {
            return mSize;
        }
        
        inline T* getData()
        {
            return mData;
        }
        
        inline T operator [] ( int index )
        {
            return mData[index];
        }
        
        inline const T operator [] ( int index ) const
        {
            return mData[index];
        }
        
    private:
        int mSize;
        T *mData;
    };
    
    template<typename T>
    using ArrayPtr = SmartPtr<Array <T> >;
}

#endif
