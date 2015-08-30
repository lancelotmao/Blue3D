//
//  SolarSystem.h
//  Houyi
//
//  Created by Lance Mao on 13/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_SolarSystem_h
#define Houyi_SolarSystem_h

#include "HouyiPlanetarySystem.h"

namespace Houyi
{
    class SolarSystem : public PlanetarySystem
    {
    public:
        static const float ONE_LOOP_ONE_SEC_RATIO;

        static const int ESun = 1;
        static const int EMercury = 1 << 1;
        static const int EVenus = 1 << 2;
        static const int EEarth = 1 << 3;
        static const int EMars = 1 << 4;
        static const int EAsteroidBelt = 1 << 5;
        static const int EJupiter = 1 << 6;
        static const int ESaturn = 1 << 7;
        static const int EUranus = 1 << 8;
        static const int ENeptune = 1 << 9;

        SolarSystem(int flag = 0xffffffff);

        inline void setDistanceRatio(Vector3 ratio)
        {
            mDisRatio = ratio;
            updateParma();
        }

        inline void setAngleRatio(double ratio)
        {
            mAngleRatio = ratio;
            updateParma();
        }
        
        inline void setSizeRatio(Vector2 ratio)
        {
            mSizeRatio = ratio;
            updateParma();
        }

        void updateParma();
        void randomizeAngle();

    private:
        Vector3 mDisRatio;
        double mAngleRatio;
        Vector2 mSizeRatio;
    };
}

#endif
