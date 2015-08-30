//
//  HouyiAudioManager.h
//  Houyi
//
//  Created by Lance Mao on 22/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiAudioManager_h
#define Houyi_HouyiAudioManager_h

#include <vector>
#include "HouyiAudioTrack.h"
#include "Platform.h"
#include "HouyiObject.h"

using namespace std;

namespace Houyi
{
    class AudioManager : public Object
    {
    public:
        ~AudioManager();
        static AudioManager* getInstance();
        AudioTrackPtr createAudioTrack(const string fileName);
        
    private:
        AudioManager(){}
        static AudioManager* mIns;

    private:
        vector<AudioTrackPtr> mAudioTracks;
    };
}

#endif
