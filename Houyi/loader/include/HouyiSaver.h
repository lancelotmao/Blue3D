//
//  HouyiSaver.h
//  Houyi
//
//  Created by Lance Mao on 5/6/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiSaver_h
#define Houyi_HouyiSaver_h

#include <fstream>
#include "HouyiObject.h"
#include "HouyiDef.h"

namespace Houyi
{
    class Scene;
    class Saver : public Object
    {
    public:
        virtual ~Saver();
        
        // returns default app path where we can save data. e.g.
        // on android it will be "/"
        // on iOS it will be "documents directory"
        static string getDefaultAppWritablePath(const string& filePath);
        
        // returns a proper saver based on filePath. e.g.
        // if path ends with .obj it should return ObjSaver
        static Saver* getSaver(const string& filePath);
        
        bool save(Scene* scene);
        
    protected:
        Saver();
        virtual void saveScene(Scene* scene) = 0;
        
    protected:
        string mFilePath;
        ofstream mFileStream;
    };
}

#endif
