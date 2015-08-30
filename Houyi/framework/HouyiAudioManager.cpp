//
//  HouyiAudioManager.cpp
//  Houyi
//
//  Created by Lance Mao on 22/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiAudioManager.h"

namespace Houyi
{
    AudioManager* AudioManager::mIns = 0;

    AudioManager::~AudioManager()
    {
        mIns = 0;
    }
    
    AudioManager* AudioManager::getInstance()
    {
        if (!mIns)
        {
            mIns = HouyiNew AudioManager();
        }
        return mIns;
    }

    AudioTrackPtr AudioManager::createAudioTrack(const string fileName)
    {
        AudioTrackPtr track;
#ifdef ANDROID
        track = new AndroidAudioTrack(fileName);
#elif __APPLE__
        track = new iOSAudioTrack(fileName);
#endif
        mAudioTracks.push_back(track);
        return track;
    }
}
