/* OvlFunc_924_20094cc  --  0x020094cc
 *
 * Was goldensun/asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_a_c.s, which held it
 * alone.
 *
 * A one-shot trigger, guarded by flag 0x256: if the player is standing inside a
 * small box -- x in [0x54, 0x5b] and z in [0xd4, 0xdb] -- set the flag, drop the
 * player 2 tiles, copy that height into +0x3c, redraw a tile group, play a cue
 * and run a tile animation script.
 *
 * Picked on the current criteria: 10 shared symbols and zero r8-r11 traffic.
 * The neighbour supplied the whole extern block and a struct Actor that already
 * had fa, fc and f12 at the right offsets; only +0x3c had to be added.
 *
 * NEEDS CSE_CFLAGS (-fno-rerun-cse-after-loop). Flag id 0x256 is read once
 * before the guard branch, to test it, and once after, to set it. The first use
 * DOMINATES the second, which is the guard/set shape this flag group exists
 * for. Without it gcc parks the id in callee-saved r6 and feeds the second site
 * with `mov r0, r6`, which also widens the prologue to `push {r5, r6, lr}`
 * against the ROM's `push {r5, lr}` -- the recorded wider-prologue marker.
 *
 * THE RANGE TEST IS UNSIGNED, and that is a whole instruction. The ROM does
 * `sub r5, #0x54` then `cmp r5, #7 / bhi`, and `bhi` is the unsigned branch:
 * the source wrote the subtraction and compared it as UNSIGNED, which is the
 * standard one-sided form of `x >= 0x54 && x <= 0x5b`. Written with a plain
 * `int` the compare comes out `bgt` and the low end of the range is never
 * checked. The two z comparisons immediately after are `ble` and `bgt`, both
 * SIGNED, so the function genuinely mixes the two in adjacent tests -- read
 * each condition code separately rather than assuming one signedness for the
 * whole guard.
 *
 * __MapActor_GetActor(0) IS CALLED FIVE TIMES, once per field access, and that
 * is what the ROM does rather than something to optimise away. It is the
 * "two loads of the same field are direct field reads" rule one level up: the
 * source calls the accessor again instead of holding the pointer, including
 * twice in a single statement for `f3c = fc`.
 */

struct Actor {
    unsigned char pad0[0xa];
    short fa;
    int fc;
    unsigned char pad10[2];
    short f12;
    unsigned char pad14[0x3c - 0x14];
    int f3c;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __Func_8010560(void *table, int a, int b);
extern unsigned short L6010[] __asm__(".L6010");

void OvlFunc_924_20094cc(void)
{
    int x;
    int z;

    if (__GetFlag(0x256))
        return;
    x = __MapActor_GetActor(0)->fa;
    z = __MapActor_GetActor(0)->f12;
    if ((unsigned int)(x - 0x54) > 7)
        return;
    if (z <= 0xd3)
        return;
    if (z > 0xdb)
        return;
    __CutsceneStart();
    __SetFlag(0x256);
    __CutsceneWait(5);
    __MapActor_GetActor(0)->fc += 0xfffe0000;
    __MapActor_GetActor(0)->f3c = __MapActor_GetActor(0)->fc;
    __CopyMapTiles(5, 2, 5, 0xb, 1, 1);
    __PlaySound(0xd9);
    __Func_8010560(L6010, 9, 7);
    __CutsceneEnd();
}
