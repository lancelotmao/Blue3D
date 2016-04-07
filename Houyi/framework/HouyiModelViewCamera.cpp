#include "HouyiModelViewCamera.h"
#include "HouyiScene.h"

namespace Houyi
{
    void ModelViewCamera::prepare(Scene* scene)
    {
        mType = Const::EModelView;

        scene->getRootSceneNode()->update();
        
        float maxx = -FLT_MAX;
        float maxy = -FLT_MAX;
        float maxz = -FLT_MAX;
        float minx = FLT_MAX;
        float miny = FLT_MAX;
        float minz = FLT_MAX;
        for (int i = 0; i < scene->getRenderableCount(); i++)
        {
            const RenderEntity& entity = scene->getRenderable(i);

            SceneNode* node = entity.mNode;
            SurfacePtr surface = entity.mSurface;

            VertexBufferPtr vb = surface->getVertexBuffer();
            if (!vb)
            {
                continue;
            }
            float* pos = vb->getPosition();
            for (int k = 0; k < vb->getPositionLength() / 3; k++)
            {
                Vector3 v = Vector3(pos[k * 3], pos[k * 3 + 1], pos[k * 3 + 2]);
                Vector3 vw = node->getLocal2World() * v;
                float x = vw.x;
                float y = vw.y;
                float z = vw.z;
                if (x > maxx)
                {
                    maxx = x;
                }
                if (x < minx)
                {
                    minx = x;
                }

                if (y > maxy)
                {
                    maxy = y;
                }
                if (y < miny)
                {
                    miny = y;
                }

                if (z > maxz)
                {
                    maxz = z;
                }
                if (z < minz)
                {
                    minz = z;
                }
            }
        }

        float cx = (maxx + minx) / 2;
        float cy = (maxy + miny) / 2;
        float cz = (maxz + minz) / 2;
        LOGD("ModelViewCamera::init(), cx = %f, cy = %f, cz = %f\n", cx, cy, cz);
        setLookAt(cx, cy, cz);

        float width = maxx - minx;
        float height = maxy - miny;
        float depth = maxz - minz;
        float maxLength = Math::max(width, height, depth);
        float z = cz + maxLength / 2 + maxLength / (tan(Camera::DEF_FOV * Math::PI / 360));
        setPosition(cx, cy, z);
        LOGD("ModelViewCamera::init() Estimated z = %f\n", z);

        // TODO our boundary is based on static position
        // however during animation vertices can go even further
        float scale = 2;
        float absMaxZ = z + maxz - minz;
        if (absMaxZ * scale > getFarRange())
        {
            setFarRange(absMaxZ * scale);
            setNearRange(z / 4);
            LOGW("model is out of default camera frustum. set new Far Range = %f\n", absMaxZ * scale);
        }
    }

    void ModelViewCamera::rotate(const float x, const float y, const float z, const float radian)
    {
        mPosition -= mLookAt;
        mPosition.rotate(Vector3(x, y, z), radian);
        mPosition += mLookAt;
        mUp.rotate(Vector3(x, y, z), radian);
    }
}
