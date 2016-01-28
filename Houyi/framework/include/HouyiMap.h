#ifndef __Houyi__HouyiMap__
#define __Houyi__HouyiMap__

#include <vector>
#include "HouyiMapLayer.h"
#include "HouyiSceneNode.h"

namespace Houyi
{
    class Renderer;
    class World;

    class Map : public Object
    {
    public:
        Map();
        ~Map();

        void addMapTileSet(MapTileSet* tileset)
        {
            mTileSet.push_back(tileset);
            mTileWidth = tileset->getTileWidth();
            mTileHeight = tileset->getTileHeight();
        }

        MapTileSet* getMapTileSet(int index)
        {
            return mTileSet[index];
        }

        void addMapLayer(MapLayer* layer)
        {
            mLayers.push_back(layer);
            if (mXCount < layer->getXCount())
            {
                mXCount = layer->getXCount();
            }
            if (mYCount < layer->getYCount())
            {
                mYCount = layer->getYCount();
            }
        }

        inline MapLayer* getMapLayer(int index)
        {
            return mLayers[index];
        }

        inline Material* getGridMaterial()
        {
            return mMaterial;
        }

        inline Vector2& getScrollRange()
        {
            return mScrollRange;
        }

        inline int getXCount()
        {
            return mXCount;
        }

        inline int getYCount()
        {
            return mYCount;
        }

        inline int getTileWidth()
        {
            return mTileWidth;
        }

        inline int getTileHeight()
        {
            return mTileHeight;
        }

        void onPostInit(World* world);

        // s is absolute
        void setScroll(Vector2 s);

        // s is delta
        void scroll(Vector2 s);

    protected:
        void update();
        
        // screen space. origin at top left
        // this only moves our plane around within one grid
        void setPosition(int x, int y);
        
    protected:
        vector<MapTileSet*> mTileSet;
        vector<MapLayer*> mLayers;
        VertexBufferPtr mVertexBuffer; // backing plane's vertex buffer
        Material* mMaterial; // plane's material

        int mWidth;
        int mHeight;
        int mXCount; // total tile count horizontally
        int mYCount; // total tile count vertically
        int mTileWidth;
        int mTileHeight;
        float mPlaneWidth;
        float mPlaneHeight;
        int mPlaneXCount; // visible
        int mPlaneYCount; // visible
        int mStepX;
        int mStepY;

        SceneNode* mNode;
        float mWin2Space;
        Vector2 mScroll;
        Vector2 mScrollRange;
    };
}
#endif
