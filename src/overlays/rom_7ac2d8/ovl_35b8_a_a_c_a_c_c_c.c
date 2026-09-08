// fakematch
/* ovl_35b8_a_a_c_a_c_c_c.c  --  OvlFunc_924_200cc68 + OvlFunc_924_200cf44
 *   [the WHOLE of asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_c_c.s -- both
 *    `.thumb_func_start`s, so NO SPLIT is required and overlay.ld:97 stays
 *    VERBATIM as `asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_c_c.o(.text)`]
 *
 *   OK WHOLE OBJECT          -- 808 bytes, 325 encodings and 70 relocations identical
 *   OK OvlFunc_924_200cc68   -- 732 bytes, 295 encodings and 63 relocations identical
 *   OK OvlFunc_924_200cf44   --  76 bytes,  30 encodings and  7 relocations identical
 *
 * Each line re-measured three times; the whole-object line is the deciding one
 * (objcmp's --func filters the REFERENCE but not the candidate, so a merged TU
 * has to go through wholecmp.py).  `python3 tools/asmfacts.py` prints
 * "2 functions  split first" for this path -- that verdict is per-file and
 * assumes one function is being converted; BOTH are closed here, so the .s is
 * fully covered and NOTHING in the linker script changes.  makefile_flags() is
 * the EMPTY set (objcmp prints no `built with:` line): tree default -O2
 * -mthumb -mthumb-interwork -fcall-used-r4, rule `asm/%.o: src/%.c`.  The .s
 * carries no .section/.data/.bss/.lcomm/.word/.byte, and all six `.L` symbols
 * are branch targets DEFINED IN THIS FILE, so nothing needs exporting.
 * FAKEMATCH: register-pin idiom on both names.
 *
 * hi = 17, hiv = 2 (r7 the field base, sl the scroll step, r8 a held
 * -0x890000).  Plain C is 223 differing and FOUR BYTES SHORT, and the
 * interesting part is that almost none of that was the high registers: gcc
 * picks r7/sl/r8 for the same three values unprompted.
 *
 * TWO IDENTICALLY-SHAPED LOOPS THAT NEED OPPOSITE SPELLINGS.  Both scroll
 * loops are `body; test; adjust; WaitFrames(1)` and the ROM lays them out
 * DIFFERENTLY -- loop 1 keeps the adjust inline and branches out on `ble`,
 * loop 2 is rotated with `b` into the middle and `ble` on the CONTINUE edge.
 * The difference is the sense of the exit test: loop 1 exits when the value
 * falls to 0x80<<19, loop 2 exits when it rises past 0x488ffff.  In C:
 *
 *     loop 1   if (v > K) { adjust; WaitFrames(1); } else break;   <- if/else
 *     loop 2   if (v > K) break; adjust; WaitFrames(1);            <- break
 *
 * Writing loop 1 in the break form is 96 differing; writing loop 2 in the
 * if/else form is 38.  NEW: for a mid-loop exit, the C spelling that survives
 * is the one whose FALL-THROUGH edge is the ROM's fall-through edge -- read
 * which side of the conditional branch the ROM lets drop through and write
 * that side as the un-negated arm.  Two loops in one function, opposite
 * answers, same rule.
 *
 * Writing loop 1 in the break form ALSO commoned `0x80 << 19` between the loop
 * test and the post-loop store (the ROM rebuilds it), and pinning that store's
 * constant to r3 was worth 223 -> 144 on its own -- but once loop 1 is spelled
 * if/else the pin is INERT and is dropped.  Recorded because the pin looked
 * like the fix for two hours: a commoning tell downstream of a control-flow
 * defect goes away when the control flow is right.
 *
 * THE STACK-ARGUMENT PIN, ON A SECOND CALLEE PAIR.  __CopyMapTiles and
 * __Func_8010704 take six arguments; the ROM builds the fifth and sixth in TWO
 * scratch registers and stores both (`mov r2, #4 / mov r3, #5 / str r3, [sp] /
 * str r2, [sp, #4]`) where gcc recycles r3.  `register int s0 __asm__("r3");
 * register int s1 __asm__("r2");` assigned in that order is exact at both
 * sites; dropping them is 4 and 5 differing.  Same idiom as the sibling
 * ovl_30_c_a_c_c_c_a_a.c records for its own two six-argument callees.
 *
 * A LOCAL'S DECLARATION ORDER PICKS ITS REGISTER, AND IT IS WORTH TWO BYTES.
 * `p[0x71c] |= 8` in OvlFunc_924_200cf44 is `ldr r3, =0x71c / add r5, r3 /
 * ldrb r2, [r5] / mov r3, #8 / orr r3, r2 / strb r3, [r5]` -- the pooled offset
 * and the bit constant SHARE r3, and the bit constant is in the ORR's
 * destination.  The recorded rule gives the `unsigned char bit` local for the
 * destination; what is NEW is that the offset temp must be declared FIRST:
 *
 *     { unsigned char *q = p + 0x71c; unsigned char bit = 8; *q = bit | *q; }   exact
 *     { unsigned char bit = 8; unsigned char *q = p + 0x71c; *q = bit | *q; }   2 differing
 *
 * gcc-2.96's REG_ALLOC_ORDER starts at r3, and local-alloc walks the pseudos
 * in declaration order, so whichever local is declared first gets r3.  Two
 * locals, one line apart, and the order is load-bearing.  (`p += 0x71c;` as a
 * statement before the block is equally exact; the two-local form ships
 * because it does not clobber p.)
 *
 * ONE ZERO NEEDED A HARD r6 PIN AND A NAMED LOCAL MADE IT WORSE.  The ROM
 * stores 0 to `GetActor(0)[0x44]` out of r6, a CALLEE-SAVED register it
 * re-materialises on the spot (`movs r6, #0 / str r6, [r0, #68]`) even though
 * r6 already held 0 from the prologue and is dead afterwards.  gcc picks r3.
 * Threading a function-scope `int z = 0;` through the four zero stores that
 * bracket it is 261 differing AND FOUR BYTES LONG -- it makes the value live
 * across the whole function and changes the allocation everywhere.  A
 * function-scope `int z;` assigned only at that site is INERT (2, unchanged).
 * `register int z __asm__("r6")` in a block at that one site is exact.  NEW,
 * and it is the eviction pin used for its third purpose: not to stop commoning
 * and not to order a fill, but to name a CALLEE-SAVED register for a value
 * that is not live across anything.
 *
 * FILL-ORDER SITES.  Two sites needed the pin's fill spelled in the ROM's own
 * materialisation order, and all six permutations were measured at each:
 * __Func_8092adc(0, 0x80 << 8, 0x1e) is 2/4/4/5/5/4 for 012/021/102/120/201/210
 * and __MapActor_SetPos(0xb, ...) in OvlFunc_924_200cf44 is 2/4/4/4/4/6, so
 * ASCENDING wins at both -- the descending fill that the sibling function
 * OvlFunc_882_2008a10 needed is not a property of the callee class.
 * Both then narrow: the 8092adc site is exact at PIN2 and at PIN1 and 2
 * differing bare, so PIN1 ships; the cf44 SetPos site is exact at PIN2 and 2
 * differing at PIN1, so PIN2 ships.
 *
 * MEASURED WORSE / INERT.  The LADDER rows are OvlFunc_924_200cc68 alone
 * (295 encodings / 732 bytes); the PROBE rows are whole-object (325 / 808).
 *
 *   ladder step                                          differing
 *   --------------------------------------------------  ---------
 *   plain C, no pins                                          223  (-4 bytes)
 *   + the 0x80<<19 post-loop store pinned to r3               144  (exact size)
 *   + loop 1 rewritten if/else                                 56
 *   + the two 0x92<<2 sites and both stack-arg pairs pinned      6
 *   + `unsigned char *q` / `unsigned char bit` at the tail ORR   4
 *   + __Func_8092adc pinned, filled ascending                    2
 *   + `register int z __asm__("r6")` on the 0x44 store           0
 *
 *   probe on the finished file                           differing
 *   --------------------------------------------------  ---------
 *   loop 1 back to the break form                              96
 *   loop 2 rewritten if/else                                   38
 *   dropping the __Func_80921c4(0, 0xb0<<2, 0x92<<2) pin        43
 *   ... narrowing that same pin to PIN2                         43
 *   dropping either stack-argument pair                       4 / 5
 *   dropping the r6 zero pin                                    2
 *   dropping the `bit` local at either ORR site                 2
 *   dropping the __MapActor_TravelTo pin                         0  (DROPPED)
 *   dropping the post-loop r3 pin once loop 1 is if/else         0  (DROPPED)
 *   `int z = 0` threaded through all four zero stores          261  (+4 bytes)
 *   a function-scope `int z;` assigned at the one site           2  (INERT)
 *
 * Harness: scratch_elev/b255/a3 -- wholecmp.py + w.sh (whole-TU objcmp),
 * r2.sh (per-function objcmp), dis.py + d.sh (side-by-side objdump diff),
 * min2.py (drop-one / narrow probe table).
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __WaitFrames(int n);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __PlaySound(int id);
extern void __Func_800fe9c(void);
extern void __Func_8011ae0(void);
extern void __Func_80921c4(int slot, int x, int y);
extern void __Func_8092adc(int a, int b, int c);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8096fb0(int a, int b);
extern void __Func_80970f8(int a, int b);
extern void __Func_809728c(void);
extern void __FieldMove(int a);
extern void __Func_8097174(void);
extern unsigned char *iwram_3001e70;
extern unsigned char *iwram_3001f30;
extern volatile int iwram_3001e40;
#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")


void OvlFunc_924_200cc68(void)
{
    unsigned char *b;
    int n;

    b = iwram_3001e70 + 0x164;
    n = 0x9c28;
    *(int *)(b + 0xc) = 0x4890000;
    *(int *)(b + 0x1c) = 0;
    __MapActor_GetActor(0)[0x55] = 0;
    *(int *)(__MapActor_GetActor(0) + 0x10) += 0xff770000;
    *(int *)(__MapActor_GetActor(0) + 0x40) = *(int *)(__MapActor_GetActor(0) + 0x10);
    __MapActor_GetActor(0xd)[0x55] = 0;
    __MapActor_SetPos(0xd, 0xaa << 18, 0xdc << 17);
    *(int *)(__MapActor_GetActor(0xd) + 0x10) += 0xff770000;
    *(int *)(__MapActor_GetActor(0xd) + 0x40) = *(int *)(__MapActor_GetActor(0xd) + 0x10);
    __Func_800fe9c();
    __WaitFrames(1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x3c);
    __Func_8011ae0();
    __PlaySound(0xdf);
    while (1) {
        *(int *)(b + 0xc) -= n;
        *(int *)(__MapActor_GetActor(0) + 0x10) += n;
        *(int *)(__MapActor_GetActor(0) + 0x40) = *(int *)(__MapActor_GetActor(0) + 0x10);
        *(int *)(__MapActor_GetActor(0xd) + 0x10) += n;
        *(int *)(__MapActor_GetActor(0xd) + 0x40) = *(int *)(__MapActor_GetActor(0xd) + 0x10);
        if (*(int *)(b + 0xc) > 0x80 << 19) {
            if ((iwram_3001e40 & 0xf) == 0 && n > 0xccb)
                n -= 0x560;
            __WaitFrames(1);
        } else {
            break;
        }
    }
    *(int *)(b + 0xc) = 0x80 << 19;
    __Func_800fe9c();
    __WaitFrames(2);
    __MapActor_GetActor(0)[0x55] = 3;
    *(int *)(__MapActor_GetActor(0xd) + 0x10) = 0xdc << 17;
    *(int *)(__MapActor_GetActor(0xd) + 0x40) = *(int *)(__MapActor_GetActor(0xd) + 0x10);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0; q1 = 0xb0 << 2; q2 = 0x92 << 2;
      __Func_80921c4(q0, q1, q2); }
    { register int z __asm__("r6"); z = 0; *(int *)(__MapActor_GetActor(0) + 0x44) = z; }
    __MapActor_SetAnim(0, 6);
    __CutsceneWait(6);
    __MapActor_SetAnim(0, 7);
    *(int *)(__MapActor_GetActor(0) + 0x30) = 0xc0 << 10;
    *(int *)(__MapActor_GetActor(0) + 0x34) = 0x80 << 10;
    __PlaySound(0x98);
    *(int *)(__MapActor_GetActor(0) + 0x28) = 0x80 << 11;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __MapActor_TravelTo(0, 0xb8 << 2, 0x92 << 2);
    __MapActor_WaitMovement(0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    *(int *)(__MapActor_GetActor(0) + 0x44) = 0x80 << 7;
    __MapActor_SetAnim(0, 6);
    __CutsceneWait(6);
    { PIN1; q0 = 0;
      __Func_8092adc(q0, 0x80 << 8, 0x1e); }
    __PlaySound(0xdf);
    while (1) {
        *(int *)(b + 0xc) += n;
        *(int *)(__MapActor_GetActor(0xd) + 0x10) -= n;
        *(int *)(__MapActor_GetActor(0xd) + 0x40) = *(int *)(__MapActor_GetActor(0xd) + 0x10);
        if (*(int *)(b + 0xc) > 0x488ffff)
            break;
        if ((iwram_3001e40 & 7) == 0 && n <= 0xcccc)
            n += 0x1999;
        __WaitFrames(1);
    }
    *(int *)(b + 0xc) = 0x80 << 19;
    { register int s0 __asm__("r3"); register int s1 __asm__("r2");
      s0 = 5; s1 = 4;
      __CopyMapTiles(0x2d, 0x5b, 0x28, 0x5b, s0, s1); }
    { register int s0 __asm__("r3"); register int s1 __asm__("r2");
      s0 = 0x28; s1 = 0x22;
      __Func_8010704(0x68, 0x22, 5, 4, s0, s1); }
    __Func_800fe9c();
    __WaitFrames(2);
    __MapActor_SetPos(0xd, 0, 0);
    __CutsceneWait(0x1e);
    { unsigned char *q = __MapActor_GetActor(0) + 0x23; unsigned char bit = 1; *q = bit | *q; }
}

void OvlFunc_924_200cf44(void)
{
    unsigned char *p;

    p = iwram_3001f30;
    { PIN2; q0 = 0xb; q1 = 0xd2 << 18;
      __MapActor_SetPos(q0, q1, 0x96 << 18); }
    __Func_8096fb0(0x5d, 1);
    __Func_80970f8(3, 0xb);
    { unsigned char *q = p + 0x71c; unsigned char bit = 8; *q = bit | *q; }
    __Func_809728c();
    __FieldMove(1);
    __Func_8097174();
}
