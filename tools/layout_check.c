#include "types.h"
#include "entity.h"
#include "scene.h"
#include "map.h"
#include "save.h"
#include "combatant.h"
#include "m4a.h"

typedef char Entity_size[sizeof(Entity) == 0x70 ? 1 : -1];
typedef char MapSpawn_size[sizeof(MapSpawn) == 0x18 ? 1 : -1];
typedef char MapObject_size[sizeof(MapObject) == 0x0C ? 1 : -1];

int probe(Entity *e) { return e->drawKind + e->facing + (int)e->updateHook; }
