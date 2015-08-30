//
//  PlanetarySystem.h
//  Houyi
//
//  Created by Lance Mao on 13/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_PlanetarySystem_h
#define Houyi_PlanetarySystem_h

#include "HouyiSceneNode.h"
#include "HouyiStar.h"
#include "HouyiPlanet.h"

namespace Houyi
{
    class PlanetarySystem : public SceneNode
    {
    public:
        PlanetarySystem();
        virtual ~PlanetarySystem();
        
        virtual bool onUpdate() override;
        
        void addStar(Star* star);
        void addPlanet(Planet* planet);
        
    protected:
        vector<Star*> mStars;
        vector<Planet*> mPlanets;
    };
}

#endif
