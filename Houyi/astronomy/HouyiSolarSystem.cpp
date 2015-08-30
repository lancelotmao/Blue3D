//
//  SolarSystem.c
//  Houyi
//
//  Created by Lance Mao on 13/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiSolarSystem.h"

namespace Houyi
{
    // for earth assuming 60 FPS
    const float SolarSystem::ONE_LOOP_ONE_SEC_RATIO = 60 * 24 * 365;
    
    SolarSystem::SolarSystem(int flag) : mDisRatio(1, 360, 0),
    mAngleRatio(ONE_LOOP_ONE_SEC_RATIO / 10), mSizeRatio(0.8, 0.01)
    {
        mName = "Solar System";
        
        if ((flag & ESun) != 0)
        {
            Star* sun = HouyiNew Star();
            sun->setName("Sun");
            addStar(sun);
        }
        
        if ((flag & EMercury) != 0)
        {
            Planet* mercury = HouyiNew Planet("Mercury");
            mercury->setTextureImage("mercury.jpg");
            mercury->mDistance = 0.38;
            mercury->mAngularSpeed = 0.00000125;
            mercury->mSize = 0.3829;
            addPlanet(mercury);
        }

        if ((flag & EVenus) != 0)
        {
            Planet* venus = HouyiNew Planet("Venus");
            venus->setTextureImage("venus.jpg");
            venus->mDistance = 0.7;
            venus->mAngularSpeed = 0.000000124;
            venus->mSize = 0.9499;
            addPlanet(venus);
        }
        
        if ((flag & EEarth) != 0)
        {
            Planet* p = HouyiNew Planet("Earth");
            p->setTextureImage("earth.jpg");
            p->mDistance = 1;
            p->mAngularSpeed = 0.0000002;
            p->mSize = 1;
            addPlanet(p);
        }
        
        if ((flag & EMars) != 0)
        {
            Planet* p = HouyiNew Planet("Mars");
            p->setTextureImage("mars.jpg");
            p->mDistance = 1.523679;
            p->mAngularSpeed = 0.00000006912933;
            p->mSize = 0.533;
            addPlanet(p);
        }

        if ((flag & EJupiter) != 0)
        {
            Planet* p = HouyiNew Planet("Jupiter");
            p->setTextureImage("jupiter.jpg");
            p->mDistance = 5;
            p->mAngularSpeed = 0.0000000135;
            p->mSize = 11.209;
            addPlanet(p);
        }
        
        if ((flag & ESaturn) != 0)
        {
            Planet* p = HouyiNew Planet("Saturn");
            p->setTextureImage("saturn.jpg");
            p->mDistance = 9.582;
            p->mAngularSpeed = 0.000000006759;
            p->mSize = 9.4494;
            addPlanet(p);
            p->addRing(1.2, 2, "saturn_ring.png");
        }
        
        if ((flag & EUranus) != 0)
        {
            Planet* p = HouyiNew Planet("Uranus");
            p->setTextureImage("uranus.jpg");
            p->mDistance = 19.22941195;
            p->mAngularSpeed = 0.00000000174988;
            p->mSize = 4.007;
            addPlanet(p);
        }
        
        if ((flag & ENeptune) != 0)
        {
            Planet* p = HouyiNew Planet("Neptune");
            p->setTextureImage("neptune.jpg");
            p->mDistance = 30.10366151;
            p->mAngularSpeed = 0.00000000071053557;
            p->mSize = 3.883;
            addPlanet(p);
        }

        updateParma();
    }

    void SolarSystem::updateParma()
    {
        for (int i = 0;i < mPlanets.size();++i)
        {
            Planet* p = mPlanets[i];
            p->mDistanceSim = mDisRatio.x + mDisRatio.y * p->mDistance + mDisRatio.z * Math::Sqrt(p->mDistance);
            p->mAngularSpeedSim = mAngleRatio * p->mAngularSpeed;
            p->mSizeSim = mSizeRatio.x + mSizeRatio.y * p->mSize;
        }
    }
    
    void SolarSystem::randomizeAngle()
    {
        srand (time(NULL));
        for (int i = 0;i < mPlanets.size();++i)
        {
            Planet* p = mPlanets[i];
            p->mAngle = (rand() % 360) * Math::PI / 360;
        }
    }
}
