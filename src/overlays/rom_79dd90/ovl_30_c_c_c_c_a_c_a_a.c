/* Cluster OvlFunc_910_20084bc..OvlFunc_910_20084bc extracted from
 * goldensun/asm/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c_a.s.
 *
 * Total .text for this TU = 64 bytes (= 0x40).
 * Placed in the run in goldensun/overlays/rom_79dd90/overlay.ld.
 *
 * Seeds a word in the area block, resets actor 8's flag byte, sets its
 * sprite's two-bit selector to 1, and runs the area-22 extra step.
 *
 * TWO CONSTANTS ARE SHARED WITH THINGS THAT LOOK UNRELATED, and both fall out
 * of writing the obvious C rather than being forced:
 *
 *   The block field and the gState area field are BOTH at 0x1c0, and gcc keeps
 *   one register (r5) holding 0x1c0 across the call to use as the index for
 *   both -- `str r3, [r2, r5]` and later `ldrsh r2, [r3, r5]`.
 *
 *   The sprite's mask is built as `sub r3, #0xd` from the ZERO that was just
 *   stored to the actor's flag byte, not as `mov r3, #0xd / neg r3, r3`. That
 *   is the constant-derivation peephole again -- here it is what the ROM does,
 *   so the bitfield spelling is still correct and the derivation is free.
 *   Ordering matters: the `f23 = 0` store has to come before the bitfield write
 *   or there is no live zero to derive from.
 */

struct Sprite {
    unsigned char pad[9];
    unsigned char lo : 2;
    unsigned char sel : 2;
    unsigned char hi : 4;
};

struct Actor {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct Sprite *spr;
};

typedef struct {
    unsigned char pad[0x1c0];
    short area;
} GlobalState;

typedef struct {
    unsigned char pad[0x1c0];
    int f1c0;
} Blk;

extern GlobalState gState;
extern Blk *iwram_3001ebc;
extern int _AREA_22;
extern struct Actor *__MapActor_GetActor(int slot);
extern void OvlFunc_910_200850c(void);

int OvlFunc_910_20084bc(void)
{
    struct Actor *act;
    struct Sprite *s;

    iwram_3001ebc->f1c0 = 0x80 << 1;
    act = __MapActor_GetActor(8);
    act->f23 = 0;
    s = act->spr;
    s->sel = 1;
    if (gState.area == (int)(&_AREA_22))
        OvlFunc_910_200850c();
    return 0;
}
