#include "HouyiSettings.h"
#include "HouyiRoot.h"
#include "HouyiRenderer.h"
#include "HouyiMeshOptimizer.h"

namespace Houyi
{
    Settings* Settings::mIns = 0;

    Settings::Settings() : mUsePlatformGesture(false), mUseCameraFromFile(false),
    mShadingMode(ESM_Default), mUseHardwareBone(true), mLightingEnabled(true)
    {
        // 29, 54, 79 | 0x1D364F
        mDefClearColor = Color(0.1137, 0.2117, 0.3019, 1);
        mClearColor = mDefClearColor;
    }
    
    Settings* Settings::getInstance()
    {
        if (!mIns)
        {
            mIns = HouyiNew Settings();
        }
        return mIns;
    }

    Settings::~Settings()
    {
        mIns = 0;
    }

    void Settings::setShadingMode(Root* root, int mode)
    {
        Renderer* renderer = root->getRenderer();
        World* world = root->getWorld();
        if (!renderer || !world)
        {
            mShadingMode = mode;
            return;
        }
        Scene* scene = world->getFocusScene();
        if (!scene)
        {
            mShadingMode = mode;
            return;
        }
        
        if (mShadingMode == ESM_Hologram)
        {
            for (int i = 0;i < scene->getMeshCount();++i)
            {
                Mesh* mesh = scene->getMesh(i);
                mesh->compact();
                for (int i = 0;i < mesh->getSurfaceCount();++i)
                {
                    SurfacePtr surface = mesh->getSurface(i);
                    surface->setBufferValid(false);
                }
            }
        }
        else if (mode == ESM_Hologram)
        {
            for (int i = 0;i < scene->getMeshCount();++i)
            {
                Mesh* mesh = scene->getMesh(i);
                mesh->expand();
                MeshOptimizer::generateScifiTangent(mesh->getVertexBuffer());
                for (int i = 0;i < mesh->getSurfaceCount();++i)
                {
                    SurfacePtr surface = mesh->getSurface(i);
                    surface->setBufferValid(false);
                }
            }
        }
        else if (mode == ESM_Wireframe)
        {
            for (int i = 0;i < scene->getMeshCount();++i)
            {
                Mesh* mesh = scene->getMesh(i);
                for (int i = 0;i < mesh->getSurfaceCount();++i)
                {
                    SurfacePtr surface = mesh->getSurface(i);
                    if (!surface->getWireFrameIndexBuffer())
                    {
                        MeshOptimizer::generateWireFrame(surface);
                    }
                }
            }
        }
        mShadingMode = mode;
        
        if (mode == ESM_Hologram)
        {
            mUseHardwareBone = false;
//            mClearColor = Color(0, 0, 0, 1);
        }
        else
        {
            mUseHardwareBone = true;
//            mClearColor = mDefClearColor;
        }

        for (int i = 0;i < world->getSceneCount();++i)
        {
            renderer->getShaderManager()->updateShader(world->getScene(i));
        }
    }
}
