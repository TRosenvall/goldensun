/* OvlFunc_903_2008dd8  --  src/overlays/rom_798dc4/ovl_314_c_c_c_a.c
 *
 * EXACT:
 *   OK OvlFunc_903_2008dd8 -- 436 bytes, 192 encodings and 22 relocations identical
 *
 * [asm/overlays/rom_798dc4/ovl_314_c_c_c_a.s, THE WHOLE FILE.  It holds
 *  EXACTLY ONE FUNCTION (`grep -c thumb_func_start` = 1) and NO DATA WHATEVER
 *  (`grep -n "\.section\|\.incbin\|\.word\|\.byte\|\.global\|\.align\|\.data\|
 *  \.rodata\|\.bss"` over the file returns NOTHING).  NO SPLIT, no new .s, no
 *  split_s.py run.
 *
 *  ONE .ld LINE NAMES THIS .o AND IT IS THE ONLY LINE IN ANY .ld IN THE TREE
 *  THAT NAMES IT ON FULL PATH:
 *      overlays/rom_798dc4/overlay.ld:39
 *          asm/overlays/rom_798dc4/ovl_314_c_c_c_a.o(.text)
 *  THAT LINE STAYS VERBATIM.  The build rule is `asm/%.o: src/%.c`, so an
 *  elevated .c still produces asm/overlays/rom_798dc4/ovl_314_c_c_c_a.o;
 *  re-aiming the line at src/ would match NOTHING and an unmatched .ld entry is
 *  silently ignored, dropping the function from the ROM behind a green build.
 *  The basename `ovl_314_c_c_c_a.o` also appears in rom_78ef88 and rom_7a5214,
 *  but never on this overlay's path, so there is no collision.  The overlay's
 *  .data list does not name this .o and there is no .bss list.
 *  LANDING IS: write this .c, delete the .s.
 *
 *  NO FLAG GROUP: tryc.makefile_flags() returns set(); the tree default
 *  -O2 -mthumb -mthumb-interwork -fcall-used-r4.  The Makefile has only
 *  explicit per-file rules for this directory (line 4633, a different file),
 *  no wildcard that could capture this path.]
 *
 * TEMPLATE.  src/overlays/rom_7e636c/ovl_cc0_c_c_a_c.c (OvlFunc_958_20091f8),
 * the top neighbour, handed over the entire skeleton: the struct Actor layout,
 * the unified `struct Actor *f50` tag, the prototype set, `unsigned int` loop
 * counters, `int v[3]` written v[0]/v[1]/v[2], the signed-division rounding
 * spelled `v[0] - v[0] / 4`, the literal 0 at every shared-zero site, and the
 * ascending PIN3 fill at each constant-argument call.  EXACT ON THE FIRST
 * SCREEN with no lever re-derived.  The stack struct is the one addition; its
 * shape came from src/overlays/rom_7a5214/ovl_17ec_c_c_b.c.
 *
 * ------------------------------------------------------ CONFIRMATIONS -------
 * THE UNIFIED STRUCT TAG IS THE ALIASING LEVER (neighbour's NEW entry, second
 * sighting).  A separate `struct Sub *f50` tag is 168 differing and 2 SHORT --
 * the `ldr r3,[r6,#0x50]` reload after `strh r3,[r2,#0x1e]` is commoned away.
 * Declaring f1e in the SAME tag f50 points at restores it at no flag cost, and
 * -fno-strict-aliasing is then byte-identical, which says the dependence lives
 * in the types rather than in the flags.
 *
 * FRAME LAYOUT FOLLOWS DECLARATION ORDER, LAST-DECLARED LOWEST (recorded at
 * docs/elevation.md:4831).  sub sp,#0x44 = 0x10 outgoing args + `s` at 0x10
 * (0x28 bytes) + `v[3]` at 0x38.  Last-declared-lowest means `int v[3];` must
 * come FIRST and `struct Cfg s;` second.  Swapping them is 2 differing.  The
 * 0x28 size is load-bearing: a 0x24 struct is 3 differing.
 *
 * THE PIN IS AN EVICTION DEVICE.  All pins dropped is 124 differing and 2
 * SHORT -- gcc commons the rebuilt constants and has nothing left for the
 * values the ROM holds.
 *
 * MEASURED WORSE (against 436 bytes / 192 encodings):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   `int v0, v1, v2;` instead of `int v[3]`                  188 (12 short)
 *   loop counters typed `int`                                179 (2 LONG)
 *   separate `struct Sub` tag for the f50 sub-object          168 (2 short)
 *   drop all five pins                                       124 (2 short)
 *   drop the __Func_8012330(-1,-1,0xe666) pin                118 (2 short)
 *   the a->f28 store moved after the second TravelTo            7
 *   PIN1 (r0 only) at all five sites                            6
 *   the struct-Cfg stores in ascending offset order            13
 *   drop the __MapActor_SetSpeed pin                            3
 *   drop the __Func_8012330(0xa0<<11,...) pin                   3
 *   struct Cfg sized 0x24 instead of 0x28                       3
 *   a named `int z;` for the shared zero                        3
 *   drop the __MapActor_TravelTo prototype                      4
 *   drop the __MapActor_SetSpeed prototype                      3
 *   drop the OvlFunc_903_2008db8 prototype                      2
 *   drop the __MapActor_SetAnim prototype                       2
 *   drop the __cos prototype                                  115
 *   `struct Cfg s;` declared before `int v[3]`                  2
 *
 *   INERT (tie at 0, so the uniform form ships):
 *     drop EITHER __MapActor_TravelTo pin -- and BOTH together (see below)
 *     PIN2 at any one of the five sites, and PIN2 at all five
 *     the first loop as `i = 0; do { ... } while (i <= 0x11);`
 *     `v[0] -= v[0] / 4;` / `v[2] -= v[2] / 2;`
 *     plain literals (0x30000, 0x50000, 0x200000, ...) for every shifted constant
 *     dropping the __WaitFrames, __PlaySound, __sin, __Func_8012330,
 *       __Func_8012350 or __MapActor_WaitMovement prototype
 *
 * FLAGS: no flag group.  -fno-gcse 167, -fno-rerun-cse-after-loop 113,
 * -fno-schedule-insns2 55.  -fno-strict-aliasing, -fno-schedule-insns,
 * -fno-cse-follow-jumps, -fno-expensive-optimizations and -fno-strength-reduce
 * are all BYTE-IDENTICAL.
 *
 * ------------------------------------------------------------------- NEW ----
 * THE PIN SET DOES NOT TRANSFER BETWEEN CONSTANT-SUBSTITUTION TWINS, AND THE
 * ALL-CHEAP RULE SAYS IN ADVANCE WHICH SITES WILL MOVE.  See the twin,
 * src/overlays/rom_79aad8/ovl_314_c_c_c.c (OvlFunc_906_20084f4): the two files
 * are the same source text with six constants and two callee names changed.
 * The minimised pin set is NOT the same, and the difference is predictable
 * from the constants alone, before any compile.  Full argument at the head of
 * the twin.  Ship the uniform five-site PIN3 fill in BOTH: it is the only form
 * verified exact on both.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0xa];
    unsigned short f1e;
    unsigned short f20;
    unsigned char f22;
    unsigned char pad23[5];
    int f28;
    unsigned char pad2c[0xc];
    int f38;
    unsigned char pad3c[0xc];
    int f48;
    unsigned char pad4c[4];
    struct Actor *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0x16];
    void (*f6c)(void);
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    int f10;
    int f14;
    unsigned short f18;
    unsigned short f1a;
    int f1c;
    int f20;
    void (*f24)(void);
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __WaitFrames(int n);
extern int __cos(int a);
extern int __sin(int a);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void OvlFunc_903_2008da8(void);
extern void OvlFunc_903_2008db8(struct Actor *p, int n);
extern void OvlFunc_common0_10c(int x, int y, int z, int a, int b, int c,
                                int d, struct Cfg *s);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_903_2008dd8(int slot)
{
    struct Actor *a;
    int v[3];
    struct Cfg s;
    unsigned int i;
    int ang;

    a = __MapActor_GetActor(slot);
    a->f55 = 0;
    for (i = 0; i <= 0x11; i++) {
        __WaitFrames(1);
        a->f50->f1e -= 0x100;
        a->x -= __cos(a->f50->f1e) / 2;
        a->f38 = 0x80 << 24;
    }
    a->f6c = OvlFunc_903_2008da8;
    { PIN3; q0 = slot; q1 = 0xc0 << 10; q2 = 0xc0 << 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = slot; q1 = 0xa0; q2 = 0xc0;
      __MapActor_TravelTo(q0, q1, q2); }
    a->f48 = 0xcccc;
    a->f55 = 3;
    a->f22 = 0;
    __MapActor_WaitMovement(slot);
    OvlFunc_903_2008db8(a, 0x80 << 14);
    __PlaySound(0xbc);
    { PIN3; q0 = 0xa0 << 11; q1 = 0xa0 << 11; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __PlaySound(0x8d);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    for (i = 0; i <= 0x10; i++) {
        ang = i << 12;
        v[0] = __cos(ang);
        v[1] = 0;
        v[2] = __sin(ang);
        v[0] = v[0] - v[0] / 4;
        v[2] = v[2] - v[2] / 2;
        OvlFunc_common0_10c(a->x, a->y, a->z, v[0], v[1], v[2], 0, 0);
    }
    a->f28 = 0xa0 << 11;
    { PIN3; q0 = slot; q1 = 0x8b; q2 = 0xc4;
      __MapActor_TravelTo(q0, q1, q2); }
    __MapActor_WaitMovement(slot);
    OvlFunc_903_2008db8(a, 0x80 << 14);
    a->f6c = 0;
    a->f50->f1e = 0x80 << 5;
    s.f18 = 0xd6;
    s.f08 = 0x80 << 8;
    s.f0c = 0xcccc;
    s.f10 = 0xc0 << 9;
    s.f14 = 0x13333;
    OvlFunc_common0_10c(a->x, a->y, a->z, 0, 0, 0, 0xe0 << 13, &s);
    __PlaySound(0x9a);
    __MapActor_SetAnim(slot, 3);
    __Func_8012350();
}
