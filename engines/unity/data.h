#ifndef _UNITY_DATA_H
#define _UNITY_DATA_H

#include "common/stream.h"
#include "common/archive.h"
#include "common/rect.h"

#include "object.h"
#include "origdata.h"

namespace Unity {

class Graphics;
class Sound;
class SpritePlayer;
class Object;
class Trigger;

struct Triangle {
	Common::Point points[3];
	uint16 distances[3];
};

struct ScreenPolygon {
	unsigned int id;
	byte type;

	Common::Array<Common::Point> points;
	Common::Array<uint16> distances;

	Common::Array<Triangle> triangles;

	bool insideTriangle(unsigned int x, unsigned int y, unsigned int &triangle);
};

struct Screen {
	unsigned int world, screen;

	Common::Array<Common::Array<Common::Point> > entrypoints;

	Common::Array<ScreenPolygon> polygons;
	Common::Array<Object *> objects;
};

class UnityData {
protected:
	class UnityEngine *_vm;

public:
	UnityData(UnityEngine *p) : _vm(p) { }
	~UnityData();

	// data file access
	Common::Archive *data, *instdata;
	Common::SeekableReadStream *openFile(Common::String filename);

	// current away team screen
	Screen current_screen;
	void loadScreenPolys(Common::String filename);

	// triggers
	Common::Array<Trigger *> triggers;
	void loadTriggers();
	Trigger *getTrigger(uint32 id);

	// all objects
	Common::HashMap<uint32, Object *> objects;
	Object *getObject(objectID id);

	// sprite filenames
	Common::Array<Common::String> sprite_filenames;
	void loadSpriteFilenames();
	Common::String getSpriteFilename(unsigned int id);

	// sector names
	Common::Array<Common::String> sector_names;
	void loadSectorNames();
	Common::String getSectorName(unsigned int x, unsigned int y, unsigned int z);

	// icon sprites
	Common::HashMap<uint32, Common::String> icon_sprites;
	void loadIconSprites();
	Common::String getIconSprite(objectID id);

	// movie info
	Common::HashMap<unsigned int, Common::String> movie_filenames;
	Common::HashMap<unsigned int, Common::String> movie_descriptions;
	void loadMovieInfo();

	// hardcoded bridge data
	Common::Array<BridgeItem> bridge_items;
	Common::Array<BridgeObject> bridge_objects;
	Common::Array<BridgeScreenEntry> bridge_screen_entries;
	void loadBridgeData();

	Common::HashMap<unsigned int, Common::HashMap<unsigned int, Conversation *>*> conversations;
	Conversation *getConversation(unsigned int world, unsigned int id);
};

} // Unity

#endif

