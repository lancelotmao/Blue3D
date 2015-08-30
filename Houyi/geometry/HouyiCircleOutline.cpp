//
//  HouyiCircleOutline.cpp
//  Houyi
//
//  Created by Lance Mao on 19/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiCircleOutline.h"

namespace Houyi
{
    CircleOutline::CircleOutline(float r, int dtheta, bool generateNow) : mR(r), mDTheta(dtheta)
    {
        if (generateNow)
        {
            generate();
        }
    }
    
    bool CircleOutline::generate(int vertexMode, int vertexAttribute, int indexFlag)
    {
        float r = mR;
        float dtheta = mDTheta;
        
        vector<Vector3> vCircle;
        for (int theta = 0;theta < 360;theta+=dtheta)
        {
            float x = r * cos(Math::DegreesToRadians(theta));
            float y = r * sin(Math::DegreesToRadians(theta));
            vCircle.push_back(Vector3(x, y, 0));
        }
        
        VertexBufferPtr vb = new VertexBuffer(VertexBuffer::EExpandAsLines, 0);
        int vcnt = vCircle.size() * 2;
        float* pos = new float[vcnt * 3];
        
        for (int i = 0;i < vCircle.size();i++)
        {
            Vector3& v1 = vCircle.at(i);
            pos[i * 6] = v1.x;
            pos[i * 6 + 1] = v1.y;
            pos[i * 6 + 2] = v1.z;
            
            Vector3 v3;
            if (i < vCircle.size() - 1)
            {
                v3 = vCircle.at(i + 1);
            }
            else
            {
                v3 = vCircle.at(0);
            }
            pos[i * 6 + 3] = v3.x;
            pos[i * 6 + 4] = v3.y;
            pos[i * 6 + 5] = v3.z;
        }
        
        vb->setPosition(pos, vcnt * 3);
        
        SurfacePtr surface = new Surface(vb);
        surface->setRenderMode(Const::ERenderModeLine);
        addSurface(surface);
        return true;
        
    }
}
