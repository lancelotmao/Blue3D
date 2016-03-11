#ifndef Houyi_HouyiSettings_h
#define Houyi_HouyiSettings_h

#include "HouyiObject.h"
#include "Platform.h"
#include "HouyiCapability.h"
#include "HouyiColor.h"

namespace Houyi
{
    class Root;
    
    class Settings : public Object
    {
    public:
        enum
        {
            ESM_Default = 0,
            ESM_PPL,
            ESM_Solid,
            ESM_Wireframe,
            ESM_Hologram
        };
        
        static Settings* getInstance();

        virtual ~Settings();

        inline const Color getClearColor() const
        {
            return mClearColor;
        }
        
        inline void setClearColor(Color color)
        {
            mClearColor = color;
        }
        
        inline void setDefClearColor()
        {
            mClearColor = mDefClearColor;
        }
        
        inline bool usePlatformGesture()
        {
            if (currentPlatform() == PLATFORM_ANDROID)
            {
                mUsePlatformGesture = false;
            }
            return mUsePlatformGesture;
        }
        
        inline void setUsePlatformGesture(bool flag)
        {
            mUsePlatformGesture = flag;
        }
        
        inline bool useCameraFromFile()
        {
            return mUseCameraFromFile;
        }

        inline void setUseCameraFromFile(bool flag)
        {
            mUseCameraFromFile = flag;
        }
        
        inline int getShadingMode()
        {
            return mShadingMode;
        }
        
        inline void setHardwareBone(bool flag)
        {
            mUseHardwareBone = flag;
        }
        
        inline bool useHardwareBone()
        {
            return mUseHardwareBone && Capability::useHardwareBone();
        }
        
        void setShadingMode(Root* root, int mode);

        inline bool isLightingEnabled()
        {
            return mLightingEnabled;
        }

        inline void setLightingEnabled(bool enable)
        {
            mLightingEnabled = enable;
        }

    private:
        static Settings* mIns;
        Settings();

        Color mDefClearColor;
        Color mClearColor;
        bool mUsePlatformGesture;
        bool mUseCameraFromFile;
        int mShadingMode;
        bool mUseHardwareBone;
        bool mLightingEnabled;
    };
}

#endif
