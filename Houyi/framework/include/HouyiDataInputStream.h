//
//  HouyiDataInputStream.h
//  Houyi
//
//  Created by Lance Mao on 23/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiDataInputStream_h
#define Houyi_HouyiDataInputStream_h

//
//  HouyiDataInputStream.h
//  houyi
//
//  Created by Lance Mao on 13/08/2011.
//  Copyright 2011 Dong wen. All rights reserved.
//

namespace Houyi
{
    class DataInputStream
    {
    public:
        enum
        {
            ELittleEndian,
            EBigEndian
        };
        
        DataInputStream(const char* data, int size);
        virtual ~DataInputStream();
        
        int available();
        int getPos();
        void skip(int count);
        int read(void* target, int length);
        bool readBool();
        int readByte();
        int readUnsignedByte();
        char readASCIIChar();
        void readFourCC(char target[4]);
        char* readLine();
        string readUTFString();
        
        virtual int readShort(int endian = ELittleEndian);
        virtual int readInt();
        virtual float readFloat();
        
    protected:
        const char* mData;
        int mSize;
        int mPos;
    };
}

#endif
