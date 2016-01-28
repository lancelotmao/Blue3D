#include "HouyiMap.h"
#include "HouyiRoot.h"
#include "HouyiRenderer.h"
#include "HouyiWorld.h"
#include "HouyiModelViewCamera.h"
#include "HouyiVector2.h"
#include "HouyiMaterialManager.h"
#include "HouyiTextureManager.h"

namespace Houyi
{
    Map::Map() : mXCount(0), mYCount(0), mNode(0), mWin2Space(1), mScroll(Vector2::ZERO),
    mStepX(0), mStepY(0)
    {

    }

    Map::~Map()
    {
        HouyiDelete(mMaterial);
       for (int i = 0;i < mTileSet.size();++i)
       {
           HouyiDelete(mTileSet[i]);
       }

       for (int i = 0;i < mLayers.size();++i)
       {
           HouyiDelete(mLayers[i]);
       }
    }

    void Map::onPostInit(World* world)
    {
        Renderer* renderer = world->getRoot()->getRenderer();
        mWidth = (int)renderer->getWidth();
        mHeight = (int)renderer->getHeight();

        mWin2Space = 2.0 / mHeight;

        int fullVisibleCountX = mWidth / mTileWidth;
        int fullVisibleCountY = mHeight / mTileHeight;
        mPlaneXCount = mXCount <= fullVisibleCountX ? mXCount
                : (mWidth % mTileWidth == 0 ? mWidth / mTileWidth + 1 : mWidth / mTileWidth + 2);
        mPlaneYCount = mYCount <= fullVisibleCountY ? mYCount
                : (mHeight % mTileHeight == 0 ? mHeight / mTileHeight + 1 : mHeight / mTileHeight + 2);
        float yunit = 2.0 * mTileHeight / mHeight;
        float xunit = 2.0 * mTileWidth / mHeight;
        mPlaneWidth = mPlaneXCount * xunit;
        mPlaneHeight = mPlaneYCount * yunit;
        GeoPlane* plane = HouyiNew GeoPlane(mPlaneXCount, mPlaneYCount, xunit, yunit, false, false);
        plane->generate(VertexBuffer::EExpandAsTriangles,
                        VertexBuffer::Attribute_Texcoord | VertexBuffer::Attribute_Normal, 0);
        mVertexBuffer = plane->getSurface(0)->getVertexBuffer();
        mVertexBuffer->setUseVBO(false); // need update texcoord
        mNode = HouyiNew SceneNode();
        mNode->setMesh(plane);
        world->getFocusScene()->addMesh(plane);

        // add material
        mMaterial = HouyiNew Material("gamemapdefaultmat");
        ImagePtr img = mTileSet[0]->getImage();
        if (img)
        {
//            mMaterial->addTexture(TextureManager::getInstance()->createTexture(img));
//            mMaterial->setTextureMode(RenderState::EMinusTransparent);
        }
        else
        {
            mMaterial->setDiffuseColor(0xFFAECBF5);
        }
        plane->setMaterial(mMaterial);

        // ready to add to scene
//        world->getScene()->addSceneNode(mNode);
//        world->addRenderable(mNode);
//
//        // add a default camera
//        Camera* camera = HouyiNew Camera();
//        float fov = camera->getFOV();
//        camera->setPosition(0, 0, 1 / tan(fov * Math::PI / 360));
//        world->getScene()->addCamera(camera);
//        world->getScene()->setCurrentCamera(camera);
//        camera->setPeekConstraint(3);
//        camera->disablePeekAnim();
//
//        // add default light
//        world->getScene()->addDefaultLight();
//        world->getScene()->getLight(0)->setPosFixed(false);

        mScrollRange = Vector2(mXCount * mTileWidth - mWidth, mYCount * mTileHeight - mHeight);
        setScroll(Vector2::ZERO);
        update();
    }

    void Map::setPosition(int x, int y)
    {
        float spacex = (x - mWidth / 2) * mWin2Space + mPlaneWidth / 2;
        float spacey = (mHeight / 2 - y) * mWin2Space - mPlaneHeight / 2;
        mNode->setIdentity();
        mNode->translate(spacex, spacey, 0);
    }
    
    void Map::setScroll(Vector2 s)
    {
        mScroll = s;
        mScroll.x = Math::Clamp<float>(mScroll.x, 0, mScrollRange.x);
        mScroll.y = Math::Clamp<float>(mScroll.y, 0, mScrollRange.y);
        mStepX = (int)(mScroll.x / mTileWidth);
        mStepY = (int)(mScroll.y / mTileHeight);
        Vector2 extra = mScroll - Vector2(mStepX * mTileWidth, mStepY * mTileHeight);
        setPosition(-extra.x, -extra.y);
        
        update();
    }

    void Map::scroll(Vector2 s)
    {
        mScroll += -s;
        setScroll(mScroll);
    }
    
    void Map::update()
    {
        if (mTileSet.size() == 0 || mLayers.size() == 0 || !mTileSet[0]->getImage())
        {
            return;
        }
        
        ImagePtr img = mTileSet[0]->getImage();
        int imageW = img->getWidth();
        int imageH = img->getHeight();
        int ucnt = imageW / mTileWidth;
        int vcnt = imageH / mTileHeight;
        float uStep = 1.0 / ucnt;
        float vStep = 1.0 / vcnt;
        float* texcoord = mVertexBuffer->getTexcoord();
        int tidx = 0;
        for (int y = mPlaneYCount - 1;y >= 0;--y)
        {
            for (int x = 0;x < mPlaneXCount;++x)
            {
                int xidx = mStepX + x;
                int yidx = mStepY + y;
                int dataIdx = yidx * mXCount + xidx;
                int dataValue = mLayers[0]->getData()[dataIdx] - 1;
                if (dataValue < 0)
                {
                    for (int i = 0;i < 12;++i)
                    {
                        texcoord[tidx++] = -1;
                    }
                }
                else
                {
                    int uidx = dataValue % ucnt;
                    int vidx = dataValue / ucnt;
                    float u = uidx * uStep;
                    float v = 1 - (vidx + 1) * vStep;
                    
                    texcoord[tidx++] = u;
                    texcoord[tidx++] = v;
                    texcoord[tidx++] = u + uStep;
                    texcoord[tidx++] = v;
                    texcoord[tidx++] = u;
                    texcoord[tidx++] = v + vStep;
                    
                    texcoord[tidx++] = u + uStep;
                    texcoord[tidx++] = v;
                    texcoord[tidx++] = u + uStep;
                    texcoord[tidx++] = v + vStep;
                    texcoord[tidx++] = u;
                    texcoord[tidx++] = v + vStep;
                }
            }
        }
    }
}
