#include "gba/types.h"
#include "actor.h"

struct Sprite {
    u8  pad_00[8];
    u16 attr;           /* low ten bits are the part copied */
    u8  pad_0a[0x12];
    u8  kind;           /* 0x1C */
};

extern Actor *MapActor_GetActor(s32 slot);

void Func_8092b54(s32 dst, s32 src)
{
    struct Sprite *sp;
    struct Sprite *dp;
    u8 v;
    s32 bits;
    s32 merged;

    sp = MapActor_GetActor(src)->sprite;
    v = sp->kind;
    bits = sp->attr;
    dp = MapActor_GetActor(dst)->sprite;
    merged = (dp->attr & ~0x3ff) | (bits & 0x3ff);
    dp->kind = v;
    dp->attr = merged;
}
