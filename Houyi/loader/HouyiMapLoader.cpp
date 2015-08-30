#include "HouyiMapLoader.h"
#include "HouyiGameWorld.h"
#include "HouyiImageFactory.h"
#include "HouyiStringUtils.h"

namespace Houyi
{
    const xmlChar* MapLoader::ATTR_FIRST_GRID = (const xmlChar*) "firstgid";
    const xmlChar* MapLoader::ATTR_NAME = (const xmlChar*) "name";
    const xmlChar* MapLoader::ATTR_TILE_WIDTH = (const xmlChar*) "tilewidth";
    const xmlChar* MapLoader::ATTR_TILE_HEIGHT = (const xmlChar*) "tileheight";
    const xmlChar* MapLoader::ATTR_SOURCE = (const xmlChar*) "source";
    const xmlChar* MapLoader::ATTR_WIDTH = (const xmlChar*) "width";
    const xmlChar* MapLoader::ATTR_HEIGHT = (const xmlChar*) "height";

    const xmlChar* MapLoader::TAG_MAP = (const xmlChar*)"map";
    const xmlChar* MapLoader::TAG_TILESET = (const xmlChar*)"tileset";
    const xmlChar* MapLoader::TAG_IMAGE = (const xmlChar*)"image";
    const xmlChar* MapLoader::TAG_LAYER = (const xmlChar*)"layer";
    const xmlChar* MapLoader::TAG_DATA = (const xmlChar*)"data";

    Scene* MapLoader::loadFromMemory(const char* data, int size)
    {
        mScene = HouyiNew Scene();

        xmlDoc* doc = xmlParseMemory(data, size);
        if (!doc)
        {
            LOGD("Map file not parsed correctly. Abort...");
            return NULL;
        }

        xmlNode* root = xmlDocGetRootElement(doc);
        if (!root)
        {
            LOGD("Map file is empty. Abort...");
            xmlFreeDoc(doc);
            return NULL;
        }

//        Map* map = mScene->getMap();
//        if (!map)
//        {
//            map = mScene->createMap();
//            mScene->setMap(map);
//        }
//        mMap = map;

        xmlNode* cur = root;

        double startTime = currentTimeMillis();
        LOGD("start loading map file\n");

        loadMap(cur);

        LOGD("load map file finished. used %f second(s)\n", ((currentTimeMillis() - startTime) / 1000));

        return mScene;
    }

    void MapLoader::loadMap(xmlNode* node)
    {
        xmlNode *cur_node = node->children;

        while (cur_node)
        {
            if (cur_node->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(cur_node->name, TAG_TILESET))
                {
                    mMap->addMapTileSet(loadTileSet(cur_node));
                }
                else if (!xmlStrcmp(cur_node->name, TAG_LAYER))
                {
                    mMap->addMapLayer(loadLayer(cur_node));
                }
            }
            cur_node = cur_node->next;
        }
    }

    MapTileSet* MapLoader::loadTileSet(xmlNode* node)
    {
        xmlChar* width = xmlGetProp(node, ATTR_TILE_WIDTH);
        int tileWidth = atoi((const char*) width);
        xmlFree(width);

        xmlChar* height = xmlGetProp(node, ATTR_TILE_HEIGHT);
        int tileHeight = atoi((const char*) height);
        xmlFree(height);

        ImagePtr img;

        xmlNode *child = node->children;
        while (child)
        {
            if (child->type == XML_ELEMENT_NODE)
            {
                if (!xmlStrcmp(child->name, TAG_IMAGE))
                {
                    img = loadImage(child);
                }
            }
            child = child->next;
        }

        MapTileSet* tileset = HouyiNew MapTileSet(tileWidth, tileHeight, img);
        return tileset;
    }

    ImagePtr MapLoader::loadImage(xmlNode* node)
    {
        xmlChar* source = xmlGetProp(node, ATTR_SOURCE);
        string src = xmlChar2string(source);
        xmlFree(source);

        ImagePtr ret = ImageFactory::createImage(StringUtils::getLastPathComponent(src));
        if (!ret)
        {
            ret = ImageFactory::createImage(src);
        }
        return ret;
    }

    string MapLoader::xmlChar2string(xmlChar* a_xmlChar)
    {
        if (!a_xmlChar)
        {
            return "";
        }
        return (char*) a_xmlChar;
    }

    MapLayer* MapLoader::loadLayer(xmlNode* node)
    {
        xmlChar* w = xmlGetProp(node, ATTR_WIDTH);
        int width = atoi((const char*) w);
        xmlFree(w);

        xmlChar* h = xmlGetProp(node, ATTR_HEIGHT);
        int height = atoi((const char*) h);
        xmlFree(h);

        int* data = 0;

        xmlNode *child = node->children;
        while (child)
        {
           if (child->type == XML_ELEMENT_NODE)
           {
               if (!xmlStrcmp(child->name, TAG_DATA))
               {
                   data = loadData(child, width, height);
               }
           }
           child = child->next;
        }

        MapLayer* layer = HouyiNew MapLayer(width, height, data);
        return layer;
    }

    int* MapLoader::loadData(xmlNode* node, int w, int h)
    {
        int* data = new int[w * h];

        xmlChar* xmlRaw = node->children->content;
        char* raw = (char*) xmlRaw;
        char* p = strtok(raw, ",");
        int i = 0;
        while (p)
        {
            data[i] = atof(p);
            p = strtok(NULL, ",");
            i++;
        }

        return data;
    }
}
