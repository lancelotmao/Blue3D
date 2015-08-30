#ifndef __Houyi__HouyiMapLayer__
#define __Houyi__HouyiMapLayer__

#include "HouyiObject.h"
#include "Platform.h"
#include "HouyiLog.h"

namespace Houyi
{
    class MapTileSet : public Object
    {
    public:
        MapTileSet(int w, int h, ImagePtr img) : mTileWidth(w), mTileHeight(h), mImage(img)
        {
            LOGI("new MapTileSet. w = %d, h = %d\n", w, h);
        }

        inline void setImage(ImagePtr img)
        {
            mImage = img;
        }

        inline ImagePtr getImage()
        {
            return mImage;
        }

        inline int getTileWidth()
        {
            return mTileWidth;
        }

        inline int getTileHeight()
        {
            return mTileHeight;
        }

    private:
        int mTileWidth;
        int mTileHeight;
        ImagePtr mImage;
    };

    class MapLayer : public Object
    {
    public:
        MapLayer(int xcnt, int ycnt, int* data) : mXCount(xcnt), mYCount(ycnt), mData(data)
        {
            LOGI("new MapLayer. xcount = %d, ycount = %d\n", xcnt, ycnt);
        }

        virtual ~MapLayer()
        {
            delete[] mData;
        }

        inline int getXCount()
        {
            return mXCount;
        }

        inline int getYCount()
        {
            return mYCount;
        }
        
        inline int* getData()
        {
            return mData;
        }

    protected:
        int mXCount;
        int mYCount;
        int* mData;
    };
}
#endif
