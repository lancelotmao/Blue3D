#ifndef Houyi_AndroidAudioTrack_h
#define Houyi_AndroidAudioTrack_h

#include "HouyiAudioTrack.h"
#include "SmartPtr.h"
#include "RefCount.h"

namespace Houyi
{
	class AndroidAudioTrack : public AudioTrack, public RefCountBase
    {
    public:
	    AndroidAudioTrack(const string fileName);

	    virtual void prepare();
        virtual void play(int loop = 1);
        virtual void pause();
        virtual void stop();
        virtual void setVolume(float volume);
        virtual int getCurrentPosition();
        virtual float getDuration();

    private:
        jobject mJAudioTrack;
    };

    typedef SmartPtr<AndroidAudioTrack> AudioTrackPtr;
}
#endif
