/* asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_c.s -- BOTH functions, EXACT.
 *
 *   OvlFunc_890_2009ca8  0x02009ca8   1120 bytes, 456 encodings, 92 relocations
 *   OvlFunc_890_200a108  0x0200a108   1192 bytes, 484 encodings, 85 relocations
 *   whole object         2312 bytes, 940 encodings, 177 relocations
 *
 * The file converts WHOLE: the reference holds these two and nothing else, no
 * data, and the one linker line (overlays/rom_78b2ac/overlay.ld:34) keeps its
 * asm/ prefix because the Makefile's asm/%.o: src/%.c rule builds it from here.
 * tryc.makefile_flags() is empty for both paths -- plain GCC296_CFLAGS at -O2.
 *
 * ------------------------------------------------------------------------
 * 1. OvlFunc_890_2009ca8 -- the park's ceiling claim was wrong
 *
 * This was parked at 5 of 456 with size, encoding count and relocations all
 * identical, under the class "CONSTANT COMMONING WITH NO BASIC BLOCK TO PUT
 * cprop AT".  The residue was one place: the cutscene's actor tail runs the
 * same block twice, and it holds three literal zeros --
 *
 *       e[0x5a] &= 0xfe;   e[0x55] = 0;   ...   f[0x26] = 0;   i = 0;
 *
 * In the SECOND copy the ROM commons all three into r5, which the park already
 * reproduced.  In the FIRST copy the ROM splits them TWO AND ONE: e[0x55]'s
 * zero gets r7 and dies at once (r7 is then reloaded with `f`), while f[0x26]'s
 * zero and the loop counter share r5.  gcc commoned e[0x55] with f[0x26]
 * instead and rebuilt the counter separately -- the same three `mov`s, grouped
 * the other way.
 *
 * The park recorded this as structurally unreachable: cprop is cross-block and
 * the tail is straight-line, so there is no join to hang a split on.  That is
 * the wrong lever, not a ceiling.  A BARE HARD-REGISTER DECLARATION removes the
 * pseudo the constant would flow through, so there is nothing left for cse1 to
 * common and no basic block is needed:
 *
 *       { register int z7 __asm__("r7") = 0; e[0x55] = z7; }
 *       ...
 *       { register int z5 __asm__("r5") = 0; f[0x26] = z5; }
 *
 * The separate-assignment spelling `{ register int z7 __asm__("r7"); z7 = 0;
 * e[0x55] = z7; }` is byte-identical here; the initialiser form is shipped only
 * because gcc-2.96 reports a spurious "might be used uninitialized" on the
 * other one.
 *
 * > BOTH SITES OF THE COMMONED CONSTANT MUST BE PINNED, AND THE PAIR IS WHAT
 * > KILLS THE COMMONING.  Measured, from the park's 5 of 456:
 * >
 * >       z7 alone   3 of 456 -- AND IT MISCOMPILES (recorded in batch 257:
 * >                  `f` is also allocated r7, gcse commons f[0x26]'s zero into
 * >                  the pinned pseudo and `strb r7, [r3]` stores the POINTER)
 * >       z5 alone   5 of 456 -- inert, and it moves the residue earlier
 * >       z7 + z5    EXACT
 * >
 * > The pair does not just score better than z7 alone, it REPAIRS it: with
 * > f[0x26]'s zero pinned to its own hard register there is no longer a pseudo
 * > for gcse to common into r7, so the wrong-value store cannot form.  A pin
 * > that miscompiles is a pin that is under-dosed, not a pin that is wrong.
 *
 * Everything else in this function is the batch-257 park verbatim: the eviction
 * pins that fixed the push mask, the eleven ordering pins, the __CopyMapTiles
 * stack slots as plain literals, and `zb2` for the second copy's e[0x55].
 *
 * ------------------------------------------------------------------------
 * 2. OvlFunc_890_200a108 -- first attempt, 451 of 484 to exact in six steps
 *
 * | step                                                    | differing of 484 |
 * |---------------------------------------------------------|-----------------|
 * | first transcription (pushes r10 as well as r8)          | 451, 16 B short  |
 * | + eviction pins on the nine repeated flag ids           | 414, 32 B short  |
 * | + `off = 0xe1 << 1` for the three gState reads          |                  |
 * |   and an ordering pin on __MapActor_SetPos              | 198,  12 B short |
 * | + a second accumulator for the .L23e4 block             | 185              |
 * | + two named locals for the (0xc, 5) stack slots         | 182              |
 * | + two named locals for the (2, 1) slots, all four blocks|  49,   4 B short |
 * | + two named locals for the (0x11, 6) slots, three sites |                  |
 * |   and a PIN3 on __Func_8012330(0x80<<9, 0x80<<9, 0x80<<9)|      0          |
 *
 * Four things worth keeping:
 *
 * > THE OFFSET IS NAMED, NOT THE BASE, and it is named AT EVERY READ.  Written
 * > as `*(short *)(gState + (0xe1 << 1))` gcc folds gState+450 into one pooled
 * > word; the ROM keeps `ldr r3, =gState` and rebuilds `mov r2,#0xe1 / lsl #1 /
 * > add r3,r3,r2` at each of the three reads.  `off = 0xe1 << 1;` immediately
 * > before each read reproduces all three.  This is the recorded rom_7b9cb4
 * > idiom; the new part is that the assignment is repeated per site, because
 * > one assignment hoisted to the top would let cse share the offset.
 *
 * > AN ACCUMULATOR RE-ZEROED INSIDE A BLOCK THAT IS ONLY REACHED WHEN IT IS
 * > ALREADY ZERO IS A SECOND VARIABLE.  The ROM emits `mov r5,#0` at .L23e4,
 * > which is reached only through `cmp r5,#0 / beq`.  Reusing the outer
 * > variable makes gcc delete the store -- correctly -- so the instruction can
 * > only be recovered by giving the inner block its own local.
 *
 * > THE "TWO DIFFERENT VALUES IN THE TWO STACK SLOTS => TWO NAMED LOCALS" RULE
 * > HOLDS WHERE THE VALUES DO NOT RECUR ACROSS MANY SITES.  Batch 257 found
 * > plain literals right for thirteen __CopyMapTiles sites sharing (2,3)/(2,1).
 * > Here the opposite applies at three shapes -- (0xc,5) once, (2,1) in four
 * > guarded blocks, (0x11,6) at three __Func_8010704 sites -- and named locals
 * > are worth 133 of the 182.  The discriminator is not the values but the
 * > REACH: literals win when one commoned pair serves a long run of sites in
 * > one block, named locals win when each site or guarded block builds its own.
 * > With literals the four (2,1) blocks put 2 in r6 and 1 in r5 and then
 * > coalesced the accumulator's `= 1` into the slot register; naming them puts
 * > 2 in r5 and 1 in r6 and the accumulator's `mov r5,#1` comes back.
 *
 * > YOU-HAVE-MORE-REGISTERS IS A COMMONING TELL, confirmed again.  The first
 * > transcription pushed r10 on top of the ROM's r5/r6/r7/r8; the extra tenant
 * > was the flag ids 0x80b..0x80e, each read two or three times.  Pinning the
 * > repeated ids to r0 removed the push.
 *
 * PINS ACTUALLY NEEDED, after a greedy drop-and-retest of all 25 candidate
 * sites: SEVEN, and they are not the ones the tell pointed at.  The four
 * __ClearFlag pins, every __SetFlag pin and thirteen of the twenty GetFlag pins
 * are inert once the rest are in place.  What stays is GF() on 0x813 and 0x812
 * in the state switch, on 0x80b..0x80e in the four-flag run, and on 0x309 --
 * that is, the FIRST read of each repeated id and nothing else, which is the
 * recorded "launder the first occurrence, never the second" rule.  Each drop
 * was re-tested against the cumulative set, per the batch-257 warning that a
 * drop inert alone can stop being inert once another has been taken.
 *
 * ------------------------------------------------------------------------
 * Both functions use hard-register pins, so both take a fakematch.txt row.
 * External symbols beyond the callees: iwram_3001ebc (0x03001EBC) and gState
 * (0x02000240), both absolute in wram.sym.
 */
extern unsigned char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_80118a8(int a);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_8091e9c(int a);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_890_200a5fc(int a, int b);
extern unsigned char gState[];
extern void __ClearFlag(int id);
extern void __StartEarthquake(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8091220(int a, int b);
extern void __Func_8091ff0(int a);
extern void OvlFunc_890_2009380(void);
extern void OvlFunc_890_2009510(void);
extern void OvlFunc_890_2009790(void);
extern int OvlFunc_890_200a5b0(void);
extern void OvlFunc_890_200a614(void);

#define P0 register int q0 __asm__("r0")
#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define GF(x) ({ P0; q0 = (x); __GetFlag(q0); })

void OvlFunc_890_2009ca8(void)
{
    unsigned char *e;
    unsigned char *f;
    unsigned char *w;
    int i;
    int fl;
    int zb2;

    if (__GetFlag(0x811) != 0) {
        __CutsceneStart();
        { PIN2; q0 = 0x80; q1 = 0x80; q0 <<= 9; q1 <<= 6; __Func_80933d4(q0, q1); }
        { PIN2; q1 = 1; q0 = 0x11f0000; q1 = -q1;
          __Func_80933f8(q0, q1, 0x94 << 16, 1); }
        { PIN3; q1 = 0x90; q2 = 0x78; q0 = 0; q1 <<= 1;
          __Func_80921c4(q0, q1, q2); }
        __MapActor_SetAnim(0, 0);
        __MapActor_Jump(0, 4, 0x1e);
        { PIN3; q1 = 0x90; q2 = 0xf0; q0 = 0x10; q1 <<= 17; q2 <<= 15;
          __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x10; q1 <<= 9; q2 <<= 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q1 = 0x8a; q0 = 0x10; q1 <<= 1; q2 = 0x88;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q1 = 0x84; q0 = 0x10; q1 <<= 1; q2 = 0x88;
          __Func_809218c(q0, q1, q2); }
        { PIN3; q1 = 0x9c; q2 = 0x88; q0 = 0; q1 <<= 1;
          __Func_80921c4(q0, q1, q2); }
        __MapActor_SetAnim(0, 1);
        __MapActor_SetAnim(0x10, 1);
        { PIN3; q1 = 0xb0; q0 = 0; q1 <<= 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xd0; q0 = 0x10; q1 <<= 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        { register int q0 __asm__("r0"); q0 = 0x819; fl = __GetFlag(q0); }
        if (fl == 0)
            __PlaySound(0xdc);
        __CutsceneWait(0x28);
        { register int q0 __asm__("r0"); q0 = 0x819; fl = __GetFlag(q0); }
        if (fl == 0) {
            __CopyMapTiles(0x24, 0x3e, 0x11, 0x24, 2, 3);
            __CopyMapTiles(0x2c, 0x3b, 0x11, 0x26, 2, 1);
            __CutsceneWait(0xa);
            __CopyMapTiles(0x26, 0x3e, 0x11, 0x24, 2, 3);
            __CopyMapTiles(0x2c, 0x3b, 0x11, 0x27, 2, 1);
            __CutsceneWait(0xa);
            __CopyMapTiles(0x28, 0x3e, 0x11, 0x24, 2, 3);
            __CopyMapTiles(0, 0x20, 0x11, 0x27, 2, 1);
            __CopyMapTiles(0x2c, 0x3b, 0x11, 0x28, 2, 1);
            __CutsceneWait(0xa);
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0;
              __MapActor_Emote(q0, q1, q2); }
            { PIN3; q1 = 0x80; q0 = 0x10; q1 <<= 1; q2 = 0;
              __MapActor_Emote(q0, q1, q2); }
            __CopyMapTiles(0x2a, 0x3e, 0x11, 0x24, 2, 3);
            __CopyMapTiles(0, 0x20, 0x11, 0x28, 2, 1);
            __CopyMapTiles(0x2c, 0x3b, 0x11, 0x29, 2, 1);
            __CutsceneWait(0xa);
            __CopyMapTiles(0, 0x20, 0x11, 0x29, 2, 1);
            __CopyMapTiles(0x2c, 0x3b, 0x11, 0x2a, 2, 1);
            __CutsceneWait(0xa);
            __CopyMapTiles(0, 0x20, 0x11, 0x2a, 2, 3);
            __CutsceneWait(0x50);
            __Func_80118a8(9);
            __Func_80118a8(0xa);
            { register int q0 __asm__("r0"); q0 = 0x819; __SetFlag(q0); }
        }
        __Func_8092848(0x10, 0, 0x1e);
        __MapActor_SetAnim(0x10, 3);
        __MapActor_DoAnim(0, 3);
        __MapActor_SetAnim(0x10, 1);
        __MapActor_SetAnim(0, 0);
        __MessageID(0x102e);
        OvlFunc_890_200a5fc(0x8010, 6);
        __MapActor_DoAnim(0x10, 3);
        __MapActor_SetAnim(0x10, 1);
        __ActorMessage(0x8010, 0);
        __MapActor_SetAnim(0, 3);
        __CutsceneWait(0x3c);
        e = __MapActor_GetActor(0);
        __Func_80933d4(0x9999, 0x1333);
        { PIN2; q1 = 1; q0 = 0x11f0000; q1 = -q1;
          __Func_80933f8(q0, q1, 0xe4 << 15, 1); }
        { PIN3; q1 = 0x90; q1 <<= 1; q2 = 0x78; q0 = 0;
          __Func_80921c4(q0, q1, q2); }
        __CutsceneWait(0x14);
        { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0x4ccc; q2 = 0x2666;
          __MapActor_SetSpeed(q0, q1, q2); }
        e[0x5a] &= 0xfe;
        { register int z7 __asm__("r7") = 0; e[0x55] = z7; }
        __PlaySound(0xc9);
        { PIN2; q1 = 0x80; q0 = 0; q1 <<= 1; __Func_8092950(q0, q1); }
        f = *(unsigned char **)(e + 0x50);
        { register int z5 __asm__("r5") = 0; f[0x26] = z5; }
        i = 0;
        do {
            *(int *)(e + 0xc) += 0x3333;
            __WaitFrames(1);
            i++;
        } while (i != 0x78);
        __PlaySound(0xbe);
        i = 0;
        do {
            *(int *)(e + 0xc) += 0x1999;
            *(int *)(f + 0x18) += -0x400;
            __WaitFrames(1);
            i++;
        } while (i != 0x3c);
        __MapActor_SetPos(0, 0, 0);
        __MapActor_Jump(0x10, 4, 0x14);
        OvlFunc_890_200a5fc(0x10, 6);
        { PIN3; q1 = 0x90; q2 = 0x78; q0 = 0x10; q1 <<= 1;
          __Func_80921c4(q0, q1, q2); }
        __Func_80925cc(0x10, 2);
        __CutsceneWait(0x14);
        { PIN3; q1 = 0xc0; q0 = 0x10; q1 <<= 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x4ccc; q2 = 0x2666; q0 = 0x10;
          __MapActor_SetSpeed(q0, q1, q2); }
        e = __MapActor_GetActor(0x10);
        e[0x5a] &= 0xfe;
        zb2 = 0;
        e[0x55] = zb2;
        __PlaySound(0xc9);
        { PIN2; q1 = 0x80; q0 = 0x10; q1 <<= 1; __Func_8092950(q0, q1); }
        f = *(unsigned char **)(e + 0x50);
        f[0x26] = 0;
        i = 0;
        do {
            *(int *)(e + 0xc) += 0x3333;
            __WaitFrames(1);
            i++;
        } while (i != 0x78);
        __PlaySound(0xbe);
        i = 0;
        do {
            *(int *)(e + 0xc) += 0x1999;
            *(int *)(f + 0x18) += -0x400;
            __WaitFrames(1);
            i++;
        } while (i != 0x3c);
        __MapActor_SetPos(0x10, 0, 0);
        __CutsceneWait(0x50);
        w = iwram_3001ebc;
        *(int *)(w + (0xe0 << 1)) = 0x203;
        *(int *)(w + (0xe4 << 1)) = 0x18;
        __MapTransitionOut();
        __WaitMapTransition();
        __Func_8091200(0, 0);
        __Func_8091254(1);
        __WaitFrames(1);
        __Func_8091e9c(7);
        __CutsceneEnd();
    }
}

int OvlFunc_890_200a108(void)
{
    unsigned char *w;
    int k;
    int s;
    int off;
    int k2;
    int sa;
    int sb;
    int t1;
    int t2;
    int u;
    int v;

    __WaitFrames(1);
    w = iwram_3001ebc;
    *(int *)(w + (0xe0 << 1)) = 0x204;
    k = 0;
    if (__GetFlag(0x809) != 0) {
        if (__GetFlag(0x814) == 0) {
            if (__GetFlag(0x819) == 0)
                __SetFlag(0xa2 << 1);
        }
    }
    __Func_8091220(0x80 << 9, 0);
    if (__GetFlag(0x109) != 0) {
        if (__GetFlag(0x201) != 0) {
            __Func_8091200(0x2051cc, 1);
            __Func_8091254(1);
            __WaitFrames(1);
        } else if (__GetFlag(0x202) != 0) {
            __Func_8091200(0x202db1, 1);
            __Func_8091254(1);
            __WaitFrames(1);
        }
    } else {
        __SetFlag(0x80 << 2);
        if (__GetFlag(0x80a) != 0)
            { PIN3; q1 = 0x90; q2 = 0xf0; q0 = 0x10; q1 <<= 18; q2 <<= 15;
              __MapActor_SetPos(q0, q1, q2); }
    }
    off = 0xe1 << 1;
    s = *(short *)(gState + off);
    if (s == 4) {
        if (GF(0x813) == 0) {
            OvlFunc_890_2009380();
            __SetFlag(0x813);
            k = 1;
        }
    } else if (s == 5) {
        if (GF(0x812) == 0) {
            OvlFunc_890_2009510();
            __SetFlag(0x812);
            __ClearFlag(0x80b);
            __ClearFlag(0x80c);
            __ClearFlag(0x80d);
            __ClearFlag(0x80e);
            k = 1;
        }
    } else if (s == 6) {
        if (__GetFlag(0x812) != 0) {
            OvlFunc_890_2009790();
            __SetFlag(0x822);
        }
        k = 1;
    }
    if (GF(0x80b) != 0)
        __SetFlag(0x826);
    if (GF(0x80c) != 0)
        __SetFlag(0x827);
    if (GF(0x80d) != 0)
        __SetFlag(0x828);
    if (GF(0x80e) != 0)
        __SetFlag(0x829);
    __WaitFrames(4);
    if (k == 0) {
        if (OvlFunc_890_200a5b0() != 0) {
            u = 0xc;
            v = 5;
            __CopyMapTiles(0x1e, 0x2c, 0x1e, 0x26, u, v);
            __CopyMapTiles(0x1e, 0x2c, 0x22, 0x25, 4, 1);
            __CopyMapTiles(0xe, 0x29, 0x20, 0x29, 8, 4);
            __CopyMapTiles(0x2d, 0x1c, 0x22, 0xa, 4, 2);
            __CopyMapTiles(0x2d, 0x1e, 0x10, 0xa, 4, 2);
            __CopyMapTiles(0xe, 0x2d, 0xe, 0x29, 8, 4);
            off = 0xe1 << 1;
            if (*(short *)(gState + off) != 8 && __GetFlag(0x814) == 0) {
                if (__GetFlag(0x819) != 0) {
                    __CopyMapTiles(0, 0x20, 0x11, 0x27, 2, 1);
                    __CopyMapTiles(0x2a, 0x3e, 0x11, 0x24, 2, 3);
                    __CopyMapTiles(0, 0x20, 0x11, 0x28, 2, 1);
                    __CopyMapTiles(0, 0x20, 0x11, 0x29, 2, 1);
                    __CopyMapTiles(0, 0x20, 0x11, 0x2a, 2, 3);
                } else {
                    __CopyMapTiles(0x2c, 0x3b, 0x11, 0x25, 2, 6);
                }
                __Func_80118a8(9);
                __Func_80118a8(0xa);
            }
            __MapActor_SetPos(0x10, 0, 0);
        } else {
            k2 = 0;
            if (__GetFlag(0x80b) != 0) {
                sa = 2;
                sb = 1;
                __CopyMapTiles(0x2d, 0x1c, 0x22, 0xa, sa, sb);
                __CopyMapTiles(0x2d, 0x1e, 0x10, 0xa, sa, sb);
                k2 = 1;
            }
            if (__GetFlag(0x80c) != 0) {
                sa = 2;
                sb = 1;
                __CopyMapTiles(0x2f, 0x1c, 0x24, 0xa, sa, sb);
                __CopyMapTiles(0x2f, 0x1e, 0x12, 0xa, sa, sb);
                k2 = 1;
            }
            if (__GetFlag(0x80d) != 0) {
                sa = 2;
                sb = 1;
                __CopyMapTiles(0x2d, 0x1d, 0x22, 0xb, sa, sb);
                __CopyMapTiles(0x2d, 0x1f, 0x10, 0xb, sa, sb);
                k2 = 1;
            }
            if (__GetFlag(0x80e) != 0) {
                sa = 2;
                sb = 1;
                __CopyMapTiles(0x2f, 0x1d, 0x24, 0xb, sa, sb);
                __CopyMapTiles(0x2f, 0x1f, 0x12, 0xb, sa, sb);
                k2 = 1;
            }
            if (__GetFlag(0x812) != 0 || k2 != 0) {
                __CopyMapTiles(0x1e, 0x2b, 0x20, 0x28, 8, 3);
                __CopyMapTiles(0x1e, 0x2b, 0x21, 0x27, 8, 1);
                __CopyMapTiles(0x1e, 0x2b, 0x24, 0x26, 3, 3);
                __CopyMapTiles(0x24, 0x3a, 0x20, 0x29, 8, 4);
            }
            t1 = 0x11;
            t2 = 6;
            __Func_8010704(0xf, 6, 2, 1, t1, t2);
        }
    }
    off = 0xe1 << 1;
    if (GF(0x309) == 0 && *(short *)(gState + off) == 8) {
        OvlFunc_890_200a614();
        __SetFlag(0x309);
        t1 = 0x11;
        t2 = 6;
        __Func_8010704(0xf, 6, 2, 1, t1, t2);
    } else if (__GetFlag(0x814) != 0) {
        __Func_8091ff0(0x8d);
        { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 9; q1 <<= 9; q2 <<= 9;
          __Func_8012330(q0, q1, q2); }
        __StartEarthquake();
        t1 = 0x11;
        t2 = 6;
        __Func_8010704(0xf, 6, 2, 1, t1, t2);
    }
    return 0;
}
