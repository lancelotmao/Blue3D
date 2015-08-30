#ifndef Houyi_HouyiGrid_h
#define Houyi_HouyiGrid_h

#include "HouyiSceneNode.h"
#include "HouyiGeoPlane.h"

namespace Houyi
{
    class Scene;

    class Grid : public SceneNode
    {
    public:
        Grid();
        void attachToScene(Scene* scene);
    };
}

#endif
