/* UI panels: release a panel's OBJ tiles.
 *
 * Split out of asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_a_a.s; the neighbours are
 * the _a and _c files beside it, listed around this one in stage1.ld so the
 * ROM layout is unchanged.
 */
#include "gba/types.h"

/* Only the two fields this function establishes are declared. The panel block
 * itself starts at +0x30C of whatever iwram_3001e98 points at.
 */
struct Panel {
    u8 pad_00[0x0a];
    u16 handle;   /* zero means nothing is allocated                        */
    u16 tiles;    /* the OBJ tile allocation handed back to Func_8003f3c    */
};

extern u8 *iwram_3001e98;
extern void Func_8003f3c(s32 tiles);

/* Frees the panel's tiles and clears its handle. Takes no arguments -- our
 * annotation for this address claims it takes the panel in r0, which is wrong;
 * r0 is loaded from the block before its only use.
 */
void Func_801c21c(void)
{
    struct Panel *panel = (struct Panel *)(iwram_3001e98 + 0x30c);

    if (panel->handle != 0) {
        Func_8003f3c(panel->tiles);
        panel->handle = 0;
    }
}
