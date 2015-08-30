//
//  HouyiRenderState.h
//  Houyi
//
//  Created by Lance Mao on 27/4/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiRenderState_h
#define Houyi_HouyiRenderState_h

#include "HouyiColor.h"

namespace Houyi
{
    struct RenderState
    {
    public:
        enum
        {
            ENoTex = 0,
            ECombine,
            EReplace,
            EModulate,
            EMinusTransparent,
            EUnspecified
        };
        
        RenderState() : mIsLightingEnabled(true), mIsDepthTestEnabled(true),
        mIsAlphaBlendEnabled(true), mIsAttenuationEnabled(false),
        mIsAmbientShadingEnabled(true), mIsSpecularShadingEnabled(true),
        mIsShadowEnabled(false), mIsDoubleSided(false),
        mOverreideGlobalSetting(false), mClearMask(~0), mTexEnv(ECombine)
        {
            mColor.mR = 1;
            mColor.mG = 1;
            mColor.mB = 1;
            mColor.mA = 1;
        }
        
        inline void setOverrideGlobalSetting(bool overrideGlobal)
        {
            mOverreideGlobalSetting = overrideGlobal;
        }
        
        inline bool isOverridingGlobalSetting() const
        {
            return mOverreideGlobalSetting;
        }
        
        inline void enableLighting(bool enabled = true)
        {
            mIsLightingEnabled = enabled;
        }
        
        inline void disableLighting()
        {
            mIsLightingEnabled = false;
        }
        
        inline bool isLightingEnabled() const
        {
            return mIsLightingEnabled;
        }
        
        inline void enableDepthTest(bool enable = true)
        {
            mIsDepthTestEnabled = enable;
        }

        inline void disableDepthTest()
        {
            mIsDepthTestEnabled = false;
        }

        inline bool isDepthTestEnabled() const
        {
            return mIsDepthTestEnabled;
        }
        
        inline void enableAlphaBlend(bool enable = true)
        {
            mIsAlphaBlendEnabled = enable;
        }
        
        inline void disableAlphaBlend()
        {
            mIsAlphaBlendEnabled = false;
        }

        inline bool isAlphaBlendEnabled() const
        {
            return mIsAlphaBlendEnabled;
        }
        
        inline void enableAttenuation(bool enable = true)
        {
            mIsAttenuationEnabled = enable;
        }
        
        inline void disableAttenuation()
        {
            mIsAttenuationEnabled = false;
        }
        
        inline bool isAttenuationEnabled() const
        {
            return mIsAttenuationEnabled;
        }
        
        inline void enableAmbientShading(bool enable = true)
        {
            mIsAmbientShadingEnabled = enable;
        }
        
        inline void disableAmbientShading()
        {
            mIsAmbientShadingEnabled = false;
        }
        
        inline bool isAmbientShadingEnabled() const
        {
            return mIsAmbientShadingEnabled;
        }
        
        inline void enableSpecularShading(bool enable = true)
        {
            mIsSpecularShadingEnabled = enable;
        }

        inline void disableSpecularShading()
        {
            mIsSpecularShadingEnabled = false;
        }

        inline bool isSpecularShadingEnabled() const
        {
            return mIsSpecularShadingEnabled;
        }

        inline void enableShadow(bool enable = true)
        {
            mIsShadowEnabled = enable;
        }

        inline void disableShadow()
        {
            mIsShadowEnabled = false;
        }

        inline bool isShadowEnabled() const
        {
            return mIsShadowEnabled;
        }
        
        inline void setDoubleSided(bool doubleSided)
        {
            mIsDoubleSided = doubleSided;
        }
        
        inline bool isDoubleSided() const
        {
            return mIsDoubleSided;
        }

        inline void setColor(Color color)
        {
            mColor = color;
        }
        
        inline Color& getColor()
        {
            return mColor;
        }
        
        inline int getClearMask()
        {
            return mClearMask;
        }

        inline void setClearMask(int clearMask)
        {
            mClearMask = clearMask;
        }

        inline void clearMaskBit(int bit)
        {
            mClearMask = mClearMask & (~bit);
        }
        
        inline int getTexEnv() const
        {
            return mTexEnv;
        }
        
        inline void setTexEnv(int texEnv)
        {
            mTexEnv = texEnv;
        }

    private:
        bool mOverreideGlobalSetting;
        bool mIsLightingEnabled;
        bool mIsDepthTestEnabled;
        bool mIsAlphaBlendEnabled;
        bool mIsAttenuationEnabled;
        bool mIsAmbientShadingEnabled;
        bool mIsSpecularShadingEnabled;
        bool mIsShadowEnabled;
        bool mIsDoubleSided;
        Color mColor;
        int mClearMask;
        int mTexEnv;
    };
}

#endif
