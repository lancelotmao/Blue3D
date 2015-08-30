//
//  HouyiiOSAudioTrack.h
//  Houyi
//
//  Created by Lance Mao on 22/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiiOSAudioTrack_h
#define Houyi_HouyiiOSAudioTrack_h

#include <AVFoundation/AVFoundation.h>

#include "SmartPtr.h"
#include "RefCount.h"
#include "HouyiAudioTrack.h"

namespace Houyi
{
    class iOSAudioTrack : public AudioTrack, public RefCountBase
    {
    public:
        iOSAudioTrack(const string fileName);
        virtual void play(int loop = 1);
        virtual void prepare();
        virtual void pause();
        virtual void stop();
        virtual void setVolume(float volume);
        virtual int getCurrentPosition();
        virtual float getDuration();
        
    private:
        AVAudioPlayer* mAVAudioPlayer;
    };
    
    typedef SmartPtr<iOSAudioTrack> AudioTrackPtr;
}

#endif
