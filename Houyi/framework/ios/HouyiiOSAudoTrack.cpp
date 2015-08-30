//
//  HouyiiOSSoundPlayer.cpp
//  Houyi
//
//  Created by Lance Mao on 22/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "HouyiUtils.h"
#include "Platform.h"

namespace Houyi
{
    iOSAudioTrack::iOSAudioTrack(const string fileName) : AudioTrack(fileName)
    {
        NSString* path = [NSString stringWithCString:fileName.c_str() encoding:NSUTF8StringEncoding];
        NSURL *url = [NSURL fileURLWithPath:[NSString stringWithFormat:@"%@/%@", [[NSBundle mainBundle] resourcePath], path]];
        
        NSError *error;
        mAVAudioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
        mAVAudioPlayer.numberOfLoops = -1;
        
        if (mAVAudioPlayer)
        {
            prepare();
        }
        else
        {
            NSLog(@"Cannot create sound. file name = %@", path);
        }
    }
    
    void iOSAudioTrack::play(int loop)
    {
        mLoop = loop;
        if (mAVAudioPlayer)
        {
            mAVAudioPlayer.volume = mVolume;
            mAVAudioPlayer.numberOfLoops = mLoop - 1;
            [mAVAudioPlayer play];
        }
    }
    
    void iOSAudioTrack::prepare()
    {
        NSString* path = [NSString stringWithCString:mFileName.c_str() encoding:NSUTF8StringEncoding];
        NSData* data = [HouyiUtils getDataFromFile:path];
        
        // call common decode function
        AudioTrack::decode((char*)[data bytes], [data length]);
    }
    
    void iOSAudioTrack::pause()
    {
        
    }
    
    void iOSAudioTrack::stop()
    {
        if (mAVAudioPlayer)
        {
            [mAVAudioPlayer stop];
        }
    }
    
    void iOSAudioTrack::setVolume(float volume)
    {
        mVolume = volume;
        if (mAVAudioPlayer)
        {
            mAVAudioPlayer.volume = mVolume;
        }
    }

    int iOSAudioTrack::getCurrentPosition()
    {
        return mAVAudioPlayer.currentTime * 1000;
    }
    
    float iOSAudioTrack::getDuration()
    {
        return mAVAudioPlayer.duration * 1000;
    }
}
