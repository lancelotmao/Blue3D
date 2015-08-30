#ifndef __Houyi__HouyiModelViewCamera__
#define __Houyi__HouyiModelViewCamera__

#include "HouyiCamera.h"

namespace Houyi
{
    class Scene;

    class ModelViewCamera: public Camera
    {
    public:
        virtual ~ModelViewCamera(){}

        virtual void prepare(Scene* scene);
        virtual void rotate(const float x, const float y, const float z, const float radian);
    };
}

#endif
