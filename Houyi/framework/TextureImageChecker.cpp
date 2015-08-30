//
//  TextureImageChecker.cpp
//  Houyi
//
//  Created by Lance Mao on 3/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "TextureImageChecker.h"
#include "HouyiWorld.h"

//using namespace collada;

namespace Houyi
{
	Scene* TextureImageChecker::loadFromMemory(const char* data, int size)
	{
		xmlDoc* doc = xmlParseMemory(data, size);
		if (!doc)
		{
			return NULL;
		}

		xmlNode* root = xmlDocGetRootElement(doc);
		if (!root)
		{
			xmlFreeDoc(doc);
			return NULL;
		}

		Scene* scene = HouyiNew Scene();
		xmlNode *cur_node = root->children;

		while (cur_node)
		{
			if (cur_node->type == XML_ELEMENT_NODE)
			{
				if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_IMAGES))
				{
					loadLibraryImages(cur_node);
				}
				if (!xmlStrcmp(cur_node->name, TAG_LIBRARY_ANIMATIONS))
				{
					// add dummy to tell caller we have animation data
					SkeletonPtr s = new Skeleton();
					scene->addSkeleton(s);
				}
			}

			cur_node = cur_node->next;
		}

		xmlFreeDoc(doc);

		// return dummy world so that caller know if this is valid collada at all
		return scene;
	}

	Scene* TextureImageChecker::convert(collada::Collada& collada)
	{
		return NULL;
	}
}
