#include "HouyiGrid.h"
#include "HouyiScene.h"
#include "HouyiRoot.h"
#include "HouyiRenderer.h"

namespace Houyi
{
    Grid::Grid()
    {
        mSelectable = false;
    }

    void Grid::attachToScene(Scene* scene)
    {
        // add first to prevent memory leak
        scene->addSceneNode(this);
        
        if (!scene->getCurrentCamera())
        {
            LOGW("Grid::attachToWorld. Scene has no camera\n");
            return;
        }
        
        GeoPlane* plane = HouyiNew GeoPlane(8, 8, 1, 1);
        plane->getSurface(0)->setRenderMode(Const::ERenderModeLine);
        setMesh(plane);
        scene->addMesh(plane);
        setColor(scene, Color(0xFFAECBF5));
        getMaterial(0)->setPassConfigurable(false);
        scene->addRenderable(this);

        Root* root = getScene()->getWorld()->getRoot();
        const Vector3& pos = scene->getCurrentCamera()->getPosition();
        const Vector3& lookat = scene->getCurrentCamera()->getLookAt();
        float fov = scene->getCurrentCamera()->getFOV();
        Vector3 forward = pos - lookat;
        float sw = root->getRenderer()->getWidth();
        float sh = root->getRenderer()->getHeight();
        float spaceh = tan(Math::DegreesToRadians(fov/2)) * forward.length();
        float spacew = spaceh * sw / sh;
        float s = min(spacew, spaceh) / 4;
        scale(s, s, s);

        if (scene->isZUp())
        {
        	translate(lookat.x, lookat.y, 0);
        }
        else
        {
            rotate(1, 0, 0, Math::PI / 2);
        	translate(lookat.x, 0, lookat.z);
        }
    }
}
