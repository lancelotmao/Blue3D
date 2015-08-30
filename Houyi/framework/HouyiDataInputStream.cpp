//
//  HouyiDataInputStream.cpp
//  Houyi
//
//  Created by Lance Mao on 23/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//
#include <iostream>
#include "HouyiMath.h"
#include "HouyiDataInputStream.h"

namespace Houyi
{
    DataInputStream::DataInputStream(const char* data, int size)
    {
        mData = data;
        mSize = size;
        mPos = 0;
    }
    
    DataInputStream::~DataInputStream()
    {
    }
    
    int DataInputStream::available()
    {
        return mSize - mPos;
    }
    
    int DataInputStream::getPos()
    {
        return mPos;
    }
    
    void DataInputStream::skip(int count)
    {
        mPos += count;
    }
    
    int DataInputStream::read(void* target, int length)
    {
        int amount = length;
        if (mPos + length > mSize)
            amount = mSize - mPos;
        
        if (amount <= 0)
            return 0;
        
        memcpy(target, mData + mPos, amount);
        
        mPos += amount;
        
        return amount;
    }
    
    bool DataInputStream::readBool()
    {
        return mData[mPos++] != 0;
    }
    
    int DataInputStream::readByte()
    {
        return mData[mPos++];
    }
    
    int DataInputStream::readUnsignedByte()
    {
        return (mData[mPos++] & 0xff);
    }
    
    char DataInputStream::readASCIIChar()
    {
        return (char)readByte();
    }
    
    void DataInputStream::readFourCC(char target[4])
    {
        for (int i = 0;i < 4;++i)
        {
            target[i] = readASCIIChar();
        }
    }
    
    char* DataInputStream::readLine()
    {
        if (mPos >= mSize)
        {
            return NULL;
        }
        
        int pos = mPos;
        char c = (char)mData[pos];
        while (c != '\n') {
            pos++;
            c = (char)mData[pos];
        }
        
        char* res = NULL;
        if (pos == mPos)
        {
            res = new char[1];
            res[0] = '\0';
            mPos++;
        }
        else
        {
            res = new char[pos - mPos + 1];
            for (int i = mPos;i < pos;i++)
            {
                res[i - mPos] = (char)mData[i];
            }
            res[pos - mPos] = '\0';
            
            mPos = pos + 1;
        }
        
        return res;
    }
    
    string DataInputStream::readUTFString()
    {
        int len = readInt();
        char* s = new char[len];
        read(s, len);
        string str(s);
        delete [] s;
        return str;
    }
    
    int DataInputStream::readShort(int endian)
    {
        int ret = 0;
        if (endian == ELittleEndian)
        {
            ret = (readByte() & 0xff) | ((readByte() & 0xff) << 8);
        }
        else
        {
            ret = ((readByte() & 0xff) << 8) | (readByte() & 0xff);
        }
        return ret;
    }
    
    int DataInputStream::readInt()
    {
        int res = 0;
        int a = readUnsignedByte();
        int b = readUnsignedByte() << 8;
        int c = readUnsignedByte() << 16;
        int d = readUnsignedByte() << 24;
        res = a | b | c | d;
        return res;
    }
    
    float DataInputStream::readFloat()
    {
        int bits = readInt();
        int s = ((bits >> 31) == 0) ? 1 : -1;
        int e = ((bits >> 23) & 0xff);
        int m = (e == 0) ? (bits & 0x7fffff) << 1 : (bits & 0x7fffff) | 0x800000;
        return (float)(s * m * Math::Pow(2, e - 150));
    }
}
