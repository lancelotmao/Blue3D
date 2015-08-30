//
//  HouyiAudioTrack.cpp
//  Houyi
//
//  Created by Lance Mao on 23/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiAudioTrack.h"
#include "HouyiDataInputStream.h"
#include "HouyiLog.h"
#include "HouyiMath.h"
#include "HouyiStringUtils.h"

namespace Houyi
{
    float AudioTrack::normalize(int value)
    {
        return (float)value / (mBitsPerSample == 1 ? 255 : (65535/2));
    }
    
    void AudioTrack::decode(char* data, int length)
    {
        if (!StringUtils::endsWith(mFileName, ".wav"))
        {
            LOGW("cannot decode sound file: %s\n", mFileName.c_str());
            return;
        }
        
        LOGD("start decoding sound file: %s\n", mFileName.c_str());

        DataInputStream dis(data, length);
        
        // "RIFF"
        char riff[4];
        dis.readFourCC(riff);
        
        // file size minus 8
        dis.readInt();
        
        // "WAVE"
        char format[4];
        dis.readFourCC(format);
        
        // "fmt "
        char fmt[4];
        dis.readFourCC(fmt);
        
        // sub chunk size should be 16 for PCM
        dis.readInt();
        
        // 1 for LPCM
        dis.readShort();
        
        // channels
        mChannels = dis.readShort();
        
        mSampleRate = dis.readInt();
        
        mByteRate = dis.readInt();
        
        mAlignBlock = dis.readShort();
        
        mBitsPerSample = dis.readShort();
        
        LOGD("Sound file info channels = %d, sample rate = %d, bits per sample = %d\n",
                mChannels, mSampleRate, mBitsPerSample);

        // "data"
        char dataC[4];
        dis.readFourCC(dataC);
        
        mPCMLength = dis.readInt();
        mPCM = new char[mPCMLength];
        
        dis.read(mPCM, mPCMLength);
    }
    
    int AudioTrack::getAmplitude(double now)
    {
        double elapsed = now / 1000;
        int index = (int)(elapsed * mByteRate);
        if (index < 0 || index + mAlignBlock >= mPCMLength)
        {
            return 0;
        }
        
        return getSampleAmplitude(index);
    }

    int AudioTrack::getSampleAmplitude(int index)
    {
        // align on data boundary
        index = (index + mAlignBlock - 1) / mAlignBlock * mAlignBlock;
        int ret = 0;
        DataInputStream dis(mPCM, mPCMLength);
        dis.skip(index);
        if (mChannels == 1)
        {
            if (mBitsPerSample == 16)
            {
                ret = dis.readShort();
            }
            else if (mBitsPerSample == 8)
            {
                ret = dis.readByte();
            }
        }
        else
        {
            int left = 0;
            int right = 0;
            if (mBitsPerSample == 16)
            {
                left = dis.readShort();
                right = dis.readShort();
            }
            else
            {
                left = dis.readByte();
                right = dis.readByte();
            }
            ret = (Math::Abs(left) + Math::Abs(right)) / 2;
        }
        return ret;
    }
}
