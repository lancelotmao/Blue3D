//
//  Planet.h
//  Houyi
//
//  Created by Lance Mao on 13/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_Planet_h
#define Houyi_Planet_h

#include "HouyiHeavenlyObject.h"

namespace Houyi
{
    class Planet : public HeavenlyObject
    {
    public:
        Planet(string name = "");

        void addRing(float innerR, float outterR, string texName);
        
        virtual bool onUpdate() override;
        
        // distance from sun. in AU
        double mDistance;

        // distance in our simulation. i.e. length to origin in 3D space
        double mDistanceSim;

        double mAngle;

        // rad per second
        double mAngularSpeed;

        double mAngularSpeedSim;
        
        // earth has size 1
        double mSize;
        double mSizeSim;

    protected:
        SceneNode* mOrbit;
        SceneNode* mNodeName;
        SceneNode* mRing;
    };
}

#endif
