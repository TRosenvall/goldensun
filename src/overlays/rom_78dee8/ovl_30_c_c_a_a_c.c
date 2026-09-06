// fakematch
/* OvlFunc_895_2008258  --  0x02008258
 *
 * Was the whole of goldensun/asm/overlays/rom_78dee8/ovl_30_c_c_a_a_c.s: ONE
 * `.thumb_func_start`, no `.word`/`.byte` tail, so this is a WHOLE-FILE .c and
 * needs NO linker edit. The single .ld line naming the .o is
 * `asm/overlays/rom_78dee8/ovl_30_c_c_a_a_c.o(.text)` in
 * overlays/rom_78dee8/overlay.ld, and it keeps working through the generic
 * `asm/%.o: src/%.c` rule. NO FLAG GROUP: plain GCC296_CFLAGS.
 *
 * The passage-A half of the pair whose passage-B half is
 * src/overlays/rom_78dee8/ovl_30_c_c_a_c_a.c (OvlFunc_895_2008420). The whole
 * back half -- both `__Func_8012330` triples, the `__MapActor_Emote` shift, the
 * three shifted `__Func_8092adc` sites, both `__MapActor_Jump`s and the
 * `-1, -1, 0xe666` settle -- is instruction-for-instruction the sibling's, so
 * its crossed-fill pins transfer verbatim. Everything else was re-measured and
 * MOST OF THE SIBLING'S LEVERS TURNED OUT UNNECESSARY HERE:
 *
 *   sibling lever                                  measured here
 *   ------------------------------------------     -------------
 *   PIN4 anchor on __CopyMapTiles #1 (r2,r1,r3,r0)  INERT -- plain literals
 *   PIN4 anchor on __CopyMapTiles #3 (r3,r0,r1,r2)  INERT -- plain literals
 *   `register int e __asm__("r5")` for the 1        INERT -- gcc commons the
 *                                                   two literal 1s into r5 and
 *                                                   stores both stack slots
 *   named stack locals s0/s1 for __Func_8010704     INERT
 *   `do { } while (0)` ahead of the message base    INERT -- the pool load does
 *                                                   not move here
 *   call-clobbered `p0` pin for the save bit        REQUIRED (see below)
 *
 * Only two things are load-bearing and neither is in the sibling.
 *
 * 1. THE CALL-CLOBBERED PIN, same mechanism as the sibling's. 0x81a is read by
 *    __GetFlag and written by __SetFlag ~120 instructions apart; left as two
 *    plain literals gcc commons the pool load into a THIRD callee-saved
 *    register and the prologue grows to `push {r5, r6, r7, lr}` /
 *    `mov r7, r8` / `push {r7}` -- 140 lines against 139, 133 differing.
 *    Binding both sites to `p0` in r0 says WHERE the value lives, and nothing
 *    in r0-r3 survives a `bl`, so gcc must reload it. Exact length, 0 differing.
 *
 * 2. THE MESSAGE BASE IS IN r8, AND THE DESTRUCTIVE-ADD RULE DOES NOT REACH IT.
 *    `int m` is enough to get r8 (three call-crossing values, r5 and r6 are
 *    already taken by the commoned 1 and 3) -- an explicit
 *    `register int m __asm__("r8")` is inert, and dropping the variable for
 *    plain 0x1032/0x1033 literals is 131 lines against 139.
 *
 *    The recorded rule is: where the ROM's last use is the destructive
 *    `add rN, #k`, write `m += k;` and gcc emits the destructive form because
 *    the variable is dead after it. THAT IS A LOW-REGISTER RULE. Thumb-1 has no
 *    `add r8, #1`: the destructive form costs `mov r3, #1` + `add r8, r8, r3`,
 *    exactly the same two instructions as the fold `mov r0, r8` + `add r0, #1`,
 *    so the tie that the low-register case wins is a dead heat here and gcc
 *    takes the fold. `m += 1;` alone is 138 lines (one short, 17 differing) in
 *    every placement tried, and so is a bare r8 pin, `do { } while (0)` on
 *    either side, and `volatile int m` (141 lines).
 *
 *    `__asm__ __volatile__("" : "+r"(m))` is the only cure: an in/out operand
 *    is an opaque read AND write of m, so the incremented value cannot be
 *    forwarded into the argument and has to survive in m's own register.
 *
 *    ITS POSITION IS PART OF THE LEVER, and this is new. Outside the pinned
 *    fill it is a fill-order anchor in its own right and reverses the pair:
 *    `mov r0, r8 / mov r1, #1` instead of the ROM's `mov r1, #1 / mov r0, r8`.
 *    Written BETWEEN `q1 = 1;` and `q0 = m;` -- immediately before the read of
 *    the barriered value -- it lands both. Every other point measured 2 or 17.
 *
 *    Its cost is not local, which is on record at three instructions upstream
 *    and is EIGHT here, across a whole call: the barrier pushed
 *    `ldr r2, =0xe666` in the preceding __Func_8012330 group below `neg r1`.
 *    Writing that group in the ROM's own MOV order (`q2` seeded before the two
 *    negations, rather than the sibling's `q2` last) puts it back. The sibling's
 *    spelling of that one block is 2 differing here for no other reason.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_800fe9c(void);
extern void __Func_801776c(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_808e118(void);
extern void __Func_8092adc(int a, int b, int c);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_895_2008258(void)
{
    int m;
    register int p0 __asm__("r0");

    if (__GetFlag(0xf01) != 0) {
        p0 = 0x81a;
        if (__GetFlag(p0) == 0) {
            __CutsceneStart();
            __Func_808e118();
            __PlaySound(0xb6);
            __CopyMapTiles(0, 0x46, 0x1e, 0x2a, 1, 1);
            __Func_800fe9c();
            __CutsceneWait(0x28);
            m = 0x1032;
            { PIN2; q1 = 1; q0 = m; __Func_801776c(q0, q1); }
            __CutsceneWait(0x14);
            __PlaySound(0xb7);
            __CopyMapTiles(0, 0x1d, 3, 1, 3, 2);
            __Func_8010704(0, 0x1d, 3, 2, 3, 1);
            __CopyMapTiles(1, 0x6d, 4, 0x51, 1, 1);
            __Func_800fe9c();
            { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 9; q1 <<= 9; q2 <<= 9;
              __Func_8012330(q0, q1, q2); }
            __CutsceneWait(0x14);
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0;
              __MapActor_Emote(q0, q1, q2); }
            { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 10; q1 <<= 10; q2 <<= 9;
              __Func_8012330(q0, q1, q2); }
            __CutsceneWait(0x14);
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0x28;
              __Func_8092adc(q0, q1, q2); }
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 8; q2 = 0x14;
              __Func_8092adc(q0, q1, q2); }
            __Func_8092adc(0, 0, 0x14);
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0xa;
              __Func_8092adc(q0, q1, q2); }
            __MapActor_Jump(0, 4, 0x14);
            __MapActor_Jump(0, 6, 0x28);
            { PIN3; q0 = 1; q1 = 1; q2 = 0xe666; q0 = -q0; q1 = -q1;
              __Func_8012330(q0, q1, q2); }
            __CutsceneWait(0x28);
            m += 1;
            { PIN2; q1 = 1; __asm__ __volatile__("" : "+r"(m)); q0 = m;
              __Func_801776c(q0, q1); }
            __SetFlag(0x143);
            p0 = 0x81a;
            __SetFlag(p0);
            __CutsceneEnd();
        }
    }
}
