/* OvlFunc_956_20093c0 -- 0x020093c0
 *
 * Every 180th tick, on three specific counts, place one of three follower
 * actors at the leader's position, give it its offsets and timers, and hand it
 * a behaviour script.
 *
 * ONE POINTER VARIABLE SPANS THE WHOLE FUNCTION, AND THAT IS WHAT BUYS r5.
 * This was the entire residue. The ROM's `mov r5, r0` after the first
 * __MapActor_GetActor is not scheduling -- it is global-alloc giving one pseudo
 * a callee-saved register because THAT PSEUDO'S live range crosses calls, even
 * though its first sub-range dies immediately. Writing the slot-0 read and the
 * slot-n target as the SAME variable, with the middle slot-0 re-read as a
 * separate short-lived one, produces both the r5 copy in the first block and no
 * copy in the second.
 *
 * Assigning them the other way round costs the copy in the first block (74
 * differing, and a line SHORT); naming a counter temp to force it only
 * relocates the copy into the second block (49). gcc-2.96 has no live-range
 * splitting, so WHICH C VARIABLE HOLDS WHICH VALUE is directly observable in
 * the register class -- the variables are the allocation.
 *
 * A PIN IS WRONG HERE: pinning that pointer to r5 forced r10 into the frame and
 * blew up the prologue, 84 differing of 86.
 *
 * The slot constant must be assigned BEFORE the two field reads. Three
 * placements are exact and equivalent, so the real constraint is only "before
 * the second read" -- which is the statement whose reload copy the ROM emits.
 * One instruction's transposition separated that from exact.
 *
 * CALL _modsi3_RAM DIRECTLY rather than writing `%`. This overlay's linker
 * script aliases `__modsi3 = _modsi3_RAM`, so `%` would have matched the
 * instructions and left a `__modsi3` relocation -- a phantom that objcmp would
 * flag and that the direct call avoids. All ten relocation names match.
 *
 * The three narrow stores are typed struct fields, per the standing lever; no
 * pin, cast or named local was needed for any of them.
 *
 * Verified with tools/objcmp.py: 180 bytes, 79 encodings and 10 relocations
 * identical. That check mattered -- tryc says OK but its reference holds four
 * functions, so its size check was skipped.
 */
struct E {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[4];
    int a;
    int b;
    unsigned char pad20[0x3c - 0x20];
    int f3c;
    unsigned char pad40[0x55 - 0x40];
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    short f64;
    short f66;
};

extern int L5b80 __asm__(".L5b80");
extern unsigned char gScript_956__0200d96c[];

extern struct E *__MapActor_GetActor(int slot);
extern int _modsi3_RAM(int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Actor_SetSpriteFlags(struct E *e, int f);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);

void OvlFunc_956_20093c0(void)
{
    struct E *p;
    struct E *e;
    int n;
    int px, py;

    n = 0x29;
    e = __MapActor_GetActor(0);
    px = e->x;
    py = e->y;
    switch (_modsi3_RAM(++L5b80, 0xb4)) {
    case 0x14:
        n = 0x2a;
        break;
    case 0x1e:
        n = 0x2b;
        break;
    case 0xa:
        break;
    default:
        return;
    }
    e = __MapActor_GetActor(n);
    if (e == 0)
        return;
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(n, p->x, p->z);
    __Actor_SetSpriteFlags(__MapActor_GetActor(n), 0);
    e->f55 = 0;
    e->a = 0x6666;
    e->b = 0x6666;
    e->x = px + (0x80 << 11);
    e->y = py + (0x80 << 11);
    e->f3c = py + (0x80 << 11);
    e->f64 = 0x19;
    e->f66 = 0x80;
    __MapActor_SetBehavior(n, gScript_956__0200d96c);
}
