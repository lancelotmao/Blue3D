//
//  HouyiPlanetarySystem.cpp
//  Houyi
//
//  Created by Lance Mao on 13/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiPlanetarySystem.h"

namespace Houyi
{
    PlanetarySystem::PlanetarySystem()
    {
        
    }
    
    PlanetarySystem::~PlanetarySystem()
    {
        
    }

    bool PlanetarySystem::onUpdate()
    {
        return false;
    }
    
    void PlanetarySystem::addStar(Star* star)
    {
        mStars.push_back(star);
        addChild(star);
    }
    
    void PlanetarySystem::addPlanet(Planet* planet)
    {
        mPlanets.push_back(planet);
        addChild(planet);
    }
}