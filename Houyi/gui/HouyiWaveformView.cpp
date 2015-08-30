#include "HouyiWaveformView.h"
#include "HouyiImageFactory.h"
#include "HouyiCanvasFactory.h"

namespace Houyi
{
    WaveformView::WaveformView(AudioTrackPtr data) : mData(data), mIndicatorWidth(4),
    mWaveColor(0xff00ff00), mIndicatorColor(0xff0000ff)
    {

    }

    bool WaveformView::onLayout(const HRect& bound, World* world)
    {
        int w = (int)bound.mWidth;
        int h = (int)bound.mHeight;
        if (!mImage)
        {
            ImagePtr img = ImageFactory::createImage((int)w, (int)h);
            setImage(img);
            CanvasPtr canvas = CanvasFactory::getCanvas(mImage);
            canvas->setColor(mWaveColor.intValue());
            int length = mData->getPCMLength();
            float dx = (float)length / w;
            int skip = 1;
            if (dx > 1)
            {
            	skip = (int)Math::Sqrt(dx);
            }
            for (int i = 0;i < w - 1;++i)
            {
                int from = (int)(i * dx);
                int to = (int)((i + 1) * dx);
                float total = 0;
                int sample = 0;
                for (int j = from;j < to;j+=skip)
                {
                    float v = mData->normalize(mData->getSampleAmplitude(j));
                    total += v;
                    sample++;
                }
                float nv = (total) / sample * h;
                canvas->drawLine(i, (h - nv) / 2, i, (h + nv) / 2);
            }
            
            ImagePtr imgIndi = ImageFactory::createImage(mIndicatorWidth, (int)h);
            mIndicator = HouyiNew ImageView();
            mIndicator->setImage(imgIndi);
            getParent()->addChild(mIndicator);
            
            CanvasPtr canvasIndi = CanvasFactory::getCanvas(mIndicator->getImage());
            canvasIndi->drawColor(mIndicatorColor.intValue());
        }
        return true;
    }
    
    bool WaveformView::onUpdate()
    {
        float d = mData->getDuration();
        float p = mData->getCurrentPosition();
        float r = p / d;
        float w = mIndicatorWidth;
        float h = getHeight();
        float x = r * getWidth();
        if (mHAlignment == EHAlignRight)
        {
            x = getWidth() - x;
        }
        else if (mHAlignment == EHAlignCenter)
        {
            x = x - getWidth() / 2;
        }
        mIndicator->setBound(HRect(x, mBound.mTop, w, h), mHAlignment, mVAlignment);
        return false;
    }
}
