//
//  TextureImageChecker.h
//  Houyi
//
//  Created by Lance Mao on 3/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef __Houyi__TextureImageChecker__
#define __Houyi__TextureImageChecker__

#include "HouyiAbsColladaLoader.h"

namespace Houyi
{
	class TextureImageChecker : public collada::AbsColladaLoader
	{
	public:
		virtual Scene* loadFromMemory(const char* data, int size);
		virtual Scene* convert(collada::Collada& collada);
	};
}

#endif /* defined(__Houyi__TextureImageChecker__) */
