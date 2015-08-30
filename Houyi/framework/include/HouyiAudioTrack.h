//
//  HouyiAudioTrack.h
//  Houyi
//
//  Created by Lance Mao on 22/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiAudioTrack_h
#define Houyi_HouyiAudioTrack_h

#include <string>

using namespace std;

namespace Houyi
{
    class AudioTrack
    {
    public:
        enum
        {
            EInvalid,
            EPrepared,
            EPlaying,
            EPaused
        };

        AudioTrack(const string fileName) : mState(EInvalid), mPCM(0), mPCMLength(0),
        mFileName(fileName), mLoop(1), mVolume(0.5), mChannels(0), mSampleRate(0),
        mBitsPerSample(16), mByteRate(0)
        {
        }
        
        virtual ~AudioTrack(){};
        
        virtual void prepare() = 0;
        virtual void play(int loop = 1) = 0;
        virtual void pause() = 0;
        virtual void stop() = 0;
        virtual int getCurrentPosition() = 0;
        virtual float getDuration() = 0; // in millisecond
        
        // from 0 - 1 where 0 is silent and 1 is full volume, default is 0.5
        virtual void setVolume(float volume) = 0;
        
        // with implementation
        inline string getFileName()
        {
            return mFileName;
        }
        
        inline int getPCMLength()
        {
            return mPCMLength;
        }

        inline char* getPCM()
        {
            return mPCM;
        }

        inline int getChannels()
        {
            return mChannels;
        }
        
        inline int getSampleRate()
        {
            return mSampleRate;
        }
        
        inline int getBitsPerSample()
        {
            return mBitsPerSample;
        }
        
        inline int getByteRate()
        {
            return mByteRate;
        }
        
        inline int getAlignBlock()
        {
            return mAlignBlock;
        }
        
        inline int getSampleCount()
        {
            return mPCMLength / mChannels / (mBitsPerSample / 8);
        }

        // [0,1]
        float normalize(int value);

        void decode(char* data, int length);
        int getAmplitude(double now);
        int getSampleAmplitude(int index);
        
    protected:
        int mState;
        char* mPCM;
        int mPCMLength;
        string mFileName;
        int mLoop;
        float mVolume;
        int mChannels;
        int mSampleRate;
        int mBitsPerSample;
        int mByteRate;
        int mAlignBlock;
    };
}

#endif
