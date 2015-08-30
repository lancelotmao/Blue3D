#ifndef Houyi_HouyiWaveformView_h
#define Houyi_HouyiWaveformView_h

#include "HouyiImageView.h"

namespace Houyi
{
    class WaveformView : public ImageView
    {
    public:
        WaveformView(AudioTrackPtr data);
        virtual bool onLayout(const HRect& bound, World* world);
        virtual bool onUpdate() override;
        
        inline void setIndicatorWidth(int width)
        {
            mIndicatorWidth = width;
        }
        
        inline void setWaveColor(Color c)
        {
            mWaveColor = c;
        }
        
        inline void setIndicatorColor(Color c)
        {
            mIndicatorColor = c;
        }

    protected:
        AudioTrackPtr mData;
        ImageView* mIndicator;
        int mIndicatorWidth;
        Color mWaveColor;
        Color mIndicatorColor;
    };
}
#endif
