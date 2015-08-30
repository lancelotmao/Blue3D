//
//  HouyiMeshOptimizer.h
//  Houyi
//
//  Created by Lance Mao on 18/5/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiMeshOptimizer_h
#define Houyi_HouyiMeshOptimizer_h

#include "HouyiMesh.h"

namespace Houyi
{
    class Mesh;
    
    class MeshOptimizer : Object
    {
    public:
        static void optimizeShadow(Mesh* mesh);
        
        static void generateWireFrame(Mesh* mesh);
        static void generateWireFrame(SurfacePtr surface);
        
        static void generateScifiTangent(VertexBufferPtr vertexBuffer);
        
        static void generateNormal(Mesh* mesh);
        static void generateSmoothNormal(Mesh* mesh);
        static void generateFaceNormal(Mesh* mesh);
        static void generateTangent(Mesh* mesh);
    };
}

#endif
