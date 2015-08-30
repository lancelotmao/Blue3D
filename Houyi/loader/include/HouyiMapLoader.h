#ifndef __Houyi__HouyiMapLoader__
#define __Houyi__HouyiMapLoader__

#include "libxml/tree.h"
#include "HouyiLoader.h"
#include "HouyiMap.h"

namespace Houyi
{
    class GameWorld;
    class MapLoader : public Loader
    {
    public:
        // attributes
        static const xmlChar* ATTR_FIRST_GRID;
        static const xmlChar* ATTR_NAME;
        static const xmlChar* ATTR_TILE_WIDTH;
        static const xmlChar* ATTR_TILE_HEIGHT;
        static const xmlChar* ATTR_SOURCE;
        static const xmlChar* ATTR_WIDTH;
        static const xmlChar* ATTR_HEIGHT;

        // tags
        static const xmlChar* TAG_MAP;
        static const xmlChar* TAG_TILESET;
        static const xmlChar* TAG_IMAGE;
        static const xmlChar* TAG_LAYER;
        static const xmlChar* TAG_DATA;

    protected:
        virtual Scene* loadFromMemory(const char* data, int size);

        void loadMap(xmlNode* node);
        MapTileSet* loadTileSet(xmlNode* node);
        ImagePtr loadImage(xmlNode* node);

        MapLayer* loadLayer(xmlNode* node);
        int* loadData(xmlNode* node, int w, int h);

        // utils
        string xmlChar2string(xmlChar* a_xmlChar);

    protected:
        Map* mMap; // point to world's map
        GameWorld* mGameWorld;
    };
}
#endif
