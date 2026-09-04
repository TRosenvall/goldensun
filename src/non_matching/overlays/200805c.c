/* OvlFunc_919_200805c -- 0x0200805c,
 * asm/overlays/rom_7a67d8/ovl_30_a_c_c_c_a_a.s
 *
 * Nudges two actors by the same delta: the one the map's current-actor slot
 * names, and the one parked at iwram_3001ebc + 0x1e0. Each gets its two
 * coordinate words advanced by (dx << 20) and (dy << 20), then its facing
 * recomputed from the byte at +0x22 and written to both +0xc and +0x14.
 *
 * SIX OF 56, and the six are ONE REGISTER. Instruction for instruction the two
 * bodies are the ROM's; the only disagreement is that the ROM holds the first
 * loaded coordinate in r1 and we hold it in r3:
 *
 *     rom   ldr r1, [r5, #8]   ldr r2, [r5, #0x10]   add r1, r6   add r2, r7
 *     ours  ldr r3, [r5, #8]   ldr r2, [r5, #0x10]   add r3, r6   add r2, r7
 *
 * and the same again in the second body, which is why it is six and not three.
 *
 * BLOCKER: ALLOCATION ORDER. REG_ALLOC_ORDER starts {3, 2, 1, 0, ...}, so gcc
 * hands out r3 first and r2 second -- which is exactly what we get. The ROM
 * took r1 and r2, skipping r3, which means r3 was unavailable at that point in
 * ITS build. Nothing in the function's own text accounts for that: r3 is next
 * used well after the stores, to form `a + 0x22`. This is the same disagreement
 * recorded across the register-allocation parks, and it is an unusually clean
 * instance -- no other difference of any kind survives, so if the
 * REG_ALLOC_ORDER hypothesis in HANDOFF.md is ever tested, this function is a
 * good probe: it should go from six to zero and nothing else can move.
 *
 * TRIED -- SIX spellings tie at EXACTLY six, which is the notebook's own signal
 * that the lever is not in the spelling:
 *   - u/v declared in either order, and before or after the other locals (4 of
 *     the 6; declaring v first is WORSE at 12)
 *   - the two loads issued in either order (loading v first is worse, 10)
 *   - the +0x22 byte read into its own local AFTER the stores (6, unchanged);
 *     reading it BEFORE the loads is much worse (18)
 *
 * WHAT WAS WON, and it is most of the function -- 51 of 56 down to 6, in three
 * measured steps:
 *
 *   1. THE gState BASE MUST BE A NAMED LOCAL (51 -> 12). Written inline, gcc
 *      folds symbol and offset into one pool word `=gState+500`; the ROM loads
 *      the bare base and builds 0x1f4 with mov+lsl. Same lever as Func_808b320
 *      in batch 189, and the third function to need it -- treat any gState
 *      access at an offset past 255 as wanting a named base by default.
 *
 *   2. BOTH COORDINATE LOADS ARE NAMED LOCALS (12 -> 10). The ROM issues both
 *      `ldr`s before either `add`; written as `a->f8 += dx; a->f10 += dy;` gcc
 *      runs each field load-add-store to completion in turn. This is the
 *      eager-issue face of the named-local rule with no call involved.
 *
 *   3. THE ADDS ARE COMPOUND ASSIGNMENTS ONTO THOSE LOCALS (10 -> 6). The ROM's
 *      `add r1, r6` is TWO-address, so the destination is the loaded value
 *      itself: `u += dx; ... a->f8 = u;`. Writing `a->f8 = u + dx;` gives the
 *      three-address `add r3, r2, r7` instead. That is the recorded
 *      two-address/three-address rule, and here it is worth four instructions
 *      because the shape repeats in both bodies.
 *
 * The two bodies are written out in full rather than shared. They are
 * identical and gcc does NOT cross-jump them, because each is guarded by its
 * own null test against a different pointer -- the duplicated-ROM-code rule
 * applies, and a static helper would not be equivalent.
 */

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x22 - 0x18];
    unsigned char f22;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern int __Func_8011f54(int a);

void OvlFunc_919_200805c(int dx, int dy)
{
    unsigned char *blk;
    unsigned char *g;
    struct Actor *a;
    int r;
    int u;
    int v;

    blk = iwram_3001ebc;
    g = gState;
    a = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    dx <<= 20;
    dy <<= 20;
    if (a != 0) {
        u = a->f8;
        v = a->f10;
        u += dx;
        v += dy;
        a->f8 = u;
        a->f10 = v;
        r = __Func_8011f54(a->f22);
        a->fc = r;
        a->f14 = r;
    }
    a = *(struct Actor **)(blk + (0xf0 << 1));
    if (a != 0) {
        u = a->f8;
        v = a->f10;
        u += dx;
        v += dy;
        a->f8 = u;
        a->f10 = v;
        r = __Func_8011f54(a->f22);
        a->fc = r;
        a->f14 = r;
    }
}
