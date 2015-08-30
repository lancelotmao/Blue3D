#include "HouyiCylinder.h"

namespace Houyi
{
    Cylinder::Cylinder(float r, float h, int dtheta) : mR(r), mH(h), mDTheta(dtheta)
    {
        generate();
    }
    
    bool Cylinder::generate(int vertexMode, int vertexAttribute, int indexFlag)
    {
        float r = mR;
        float h = mH;
        float dtheta = mDTheta;
        
        vector<Vector3> vbtm;
        for (int theta = 0;theta < 360;theta+=dtheta)
        {
            float x = r * cos(Math::DegreesToRadians(theta));
            float z = r * sin(Math::DegreesToRadians(theta));
            vbtm.push_back(Vector3(x, 0, z));
        }
        
        vector<Vector3> vertice;
        int triangleCntBtm = vbtm.size();
        for (int i = 0;i < triangleCntBtm;i++)
        {
            vertice.push_back(vbtm.at(i));
            vertice.push_back(Vector3(0, 0, 0));
            if (i < triangleCntBtm - 1)
            {
                vertice.push_back(vbtm.at(i + 1));
            }
            else
            {
                vertice.push_back(vbtm.at(0));
            }
        }
        
        for (int i = 0;i < triangleCntBtm;i++)
        {
            vertice.push_back(Vector3(0, h, 0));
            vertice.push_back(vbtm.at(i) + Vector3(0, h, 0));
            if (i < triangleCntBtm - 1)
            {
                vertice.push_back(vbtm.at(i + 1) + Vector3(0, h, 0));
            }
            else
            {
                vertice.push_back(vbtm.at(0) + Vector3(0, h, 0));
            }
        }
        
        for (int i = 0;i < triangleCntBtm;i++)
        {
            Vector3 v1 = vbtm.at(i);
            Vector3 v2;
            if (i < triangleCntBtm - 1)
            {
                v2 = vbtm.at(i + 1);
            }
            else
            {
                v2 = vbtm.at(0);
            }
            
            Vector3 v1p = v1 + Vector3(0, h, 0);
            Vector3 v2p = v2 + Vector3(0, h, 0);
            
            vertice.push_back(v1);
            vertice.push_back(v2);
            vertice.push_back(v1p);
            
            vertice.push_back(v2);
            vertice.push_back(v2p);
            vertice.push_back(v1p);
        }
        
        int vcnt = vertice.size();
        float* pos = new float[vcnt * 3];
        float* normal = new float[vcnt * 3];
        for (int i = 0;i < vcnt;i++)
        {
            Vector3 v = vertice.at(i);
            pos[i * 3] = v.x;
            pos[i * 3 + 1] = v.y;
            pos[i * 3 + 2] = v.z;
            
            if (i < triangleCntBtm * 3)
            {
                normal[i * 3] = 0;
                normal[i * 3 + 1] = -1;
                normal[i * 3 + 2] = 0;
            }
            else if (i < triangleCntBtm * 3 * 2)
            {
                normal[i * 3] = 0;
                normal[i * 3 + 1] = 1;
                normal[i * 3 + 2] = 0;
            }
            else
            {
                Vector3 n = vertice.at(i);
                if (n.y > 0)
                {
                    n.y -= h;
                }
                n.normalize();
                normal[i * 3] = n.x;
                normal[i * 3 + 1] = n.y;
                normal[i * 3 + 2] = n.z;
            }
        }
        
        VertexBufferPtr vb = new VertexBuffer();
        vb->setPosition(pos, vcnt * 3);
        vb->setNormal(normal, vcnt * 3);
        
        SurfacePtr surface = new Surface(vb);
        addSurface(surface);
        return true;
    }
}
