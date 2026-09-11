/* OvlFunc_918_2008334 -- asm/overlays/rom_7a5214/ovl_314_c_a_a.s
 *
 * VERDICT: EXACT, modulo one unresolved absolute symbol.
 *   OK OvlFunc_918_2008334 -- 352 bytes, 150 encodings and 21 relocations identical
 * measured 3x on a verification copy of this file carrying one extra line,
 * `__asm__(".set _AREA_2d, 0x2d");`, which resolves the symbol inside the
 * translation unit so objcmp can see through it.  WITHOUT that line the file
 * measures, also 3x:
 *
 *   XX ENCODINGS differ in 1 place(s) (ref 150, ours 150)
 *      first at index 85: ref 0000002d  ours 00000000
 *   XX RELOCATIONS differ   (ours has one extra: 000000c4 R_ARM_ABS32 _AREA_2d)
 *
 * i.e. the ONLY residue is the pool word holding `_AREA_2d`, which objcmp
 * cannot resolve because it compares OBJECTS and `_AREA_2d = 0x2d;` lives in
 * area.sym.  `make compare` is the gate for that one word.
 *
 * WHY IT IS A SYMBOL.  `__SetDestMap`'s first argument is pooled -- the ROM
 * writes `ldr r0, =0x2d` where `mov r0, #0x2d` would do -- which is the
 * recorded "a pooled constant below 256 is a tell the source referenced a
 * SYMBOL".  It does not meet a halfword anywhere, so the known exception to
 * that tell does not apply.  `_AREA_2d` was ALREADY in area.sym (line 224); no
 * new symbol is added.  The precedent is src/overlays/rom_7fb4a8/ovl_30_c_a_a_a_b.c,
 * which spells the same call `__SetDestMap((int)(&_AREA_01), 1)`.  Spelled as
 * the literal `0x2d` the file is 4 bytes SHORT with 62 of 150 differing.
 * area.sym is INCLUDEd only by stage1.ld, but every overlay links with
 * `-R stage1.o`, so the symbol is in scope here.
 *
 * LANDING.  WHOLE FILE.  `tools/asmfacts.py` says `WHOLE  convert directly`:
 * the .s holds exactly ONE `.thumb_func_start` and no `.section`/`.data`/
 * `.byte`/`.incbin` line.  Its single `.word 0x2000` (line 160) is the
 * function's own in-text literal pool, not data.  NO LINKER EDIT: the only line
 * naming this object is
 *
 *     overlays/rom_7a5214/overlay.ld:23   asm/overlays/rom_7a5214/ovl_314_c_a_a.o(.text)
 *
 * and it stays VERBATIM -- the generic `asm/%.o: src/%.c` rule builds that .o
 * from this .c.  `tryc.makefile_flags()` is the empty set with no wildcard
 * hits, so plain GCC296_CFLAGS at -O2 with -fcall-used-r4.  No basename
 * collision: `ovl_314_c_a_a.s` exists nowhere else in asm/.
 *
 * EXTERNAL SYMBOLS.  `gState`; `.L2dd0`, a 4-byte `.lcomm` in
 * asm/overlays/rom_7a5214/ovl_314_c_c_c_c.s which ALREADY declares it
 * `.global` (twice), so no `.global` needs adding anywhere.  It is reached with
 * the asm-name trick as a `short *` VARIABLE -- the ROM does
 * `ldr r5, =.L2dd0 / ldr r2, [r5]`, a double indirection, and re-loads the
 * pointer through the surviving r5 after __GetFlag.
 *
 * WHAT IT IS.  A one-shot map-transition beat.  It reads three words out of
 * gState (0x1dc and 0x1e4 as 12.20 fixed-point tile coordinates, 0x1f4 as the
 * party leader's map-actor slot), bails if `.L2dd0` already holds the caller's
 * destination id, and otherwise records it.  First time through the story flag
 * it is handed is clear: it just runs __Func_80105d4 with those coordinates and
 * sets the flag.  Every later time the flag is set, so it clears `.L2dd0` to
 * -1, runs __Func_80105d4 one id higher, and plays the full cutscene -- sting,
 * camera, surprise, a fade, the leader's actor poked at +0x55/+0x14/+0x48/+0x22,
 * and 30 frames of scrolling the actor's halfword at +6 by 0x2000 a frame.
 * Unless the destination is 0x32 it then sets flag 0x122.
 *
 * THE LEVERS, in the order they mattered.
 *
 *  1. gState's OFFSET MUST BE BUILT, NOT FOLDED -- `gs = gState;` first, then
 *     `*(int *)(gs + 0x1dc)`.  0x1dc is 0xee << 1 and the ROM spends
 *     `mov r1, #0xee / lsl r1, #1 / add r3, r2, r1` building it; gcc then
 *     reaches 0x1e4 and 0x1f4 off the same register with `add r1, #8` /
 *     `add r1, #0x10` by itself.  This is the recorded rule and the solved
 *     sibling src/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_a.c uses it.
 *
 *  2. TWO PINS, both minimal, both measured in both directions:
 *
 *     * `__Func_80933f8(-1, -1, -1, 0)`: the ROM rebuilds -1 THREE TIMES
 *       (`mov r0,#1 / mov r1,#1 / mov r2,#1 / neg r1 / neg r2 / neg r0`).
 *       Unpinned gcc commons it into one `mov r2,#1 / neg r2,r2` and copies --
 *       2 instructions and 4 bytes SHORT, 62 of 150 differing.  An EVICTION pin
 *       of WIDTH TWO (`q0`, `q1`; the third -1 and the 0 stay literals) is
 *       enough; width 4 and width 3 are equal to it, width 1 is 61 differing.
 *       Three plain `int` locals instead of the pins are INERT (62) -- the
 *       recorded "the split can be UNDONE before it reaches the allocator" --
 *       so the hard register is what makes it survive.
 *
 *     * `__MapActor_Surprise(slot, 0x101)`: the ROM fills r0 from r7 BEFORE the
 *       pool load of 0x101; gcc emits the `ldr` first.  An ORDERING pin of
 *       WIDTH ONE on r0 fixes it.  Dropping it is 2 differing with the
 *       relocations silent -- exactly the recorded "2-3 differing,
 *       relocations-silent is an ordering pin" signature.
 *
 * THREE SPELLINGS MEASURED WORSE AND NOT SHIPPED.  All three were prompted by
 * real rules and all three are the wrong reading here:
 *
 *   spelling                                                     differing/150
 *   ------------------------------------------------------------  -----------
 *   SHIPPED (this file)                                            0 (1 = sym)
 *   `int m = -1; *L2dd0 = m;` for the halfword store .........         137
 *   an `int` temp + an `int 0x2000` local for the loop's `+=` .         94
 *   both of those plus the symbol, before any pin ............         132
 *
 * The second and third are the "use an `int` local for a halfword store"
 * rule misapplied.  Unpinned, gcc pools BOTH constants in HImode -- `ldrh r3,
 * .L12` over `.word -1` and `ldrh r7, .L15` over `.word 8192` where the ROM has
 * `ldr` in both places -- and an `int` local does move the mode, but it also
 * swaps the two parameter spill slots (`str r1, [sp, #0xc]` for the ROM's
 * `str r1, [sp, #8]`) and shortens the frame.  THE TWO HImode POOL LOADS FIX
 * THEMSELVES once the `__Func_80933f8` eviction pin is in: with the commoned
 * -1 broken up, gcc has the register pressure the ROM had and emits SImode
 * loads at both sites unaided.  That is the recorded "order the search by
 * mechanism size" doing its job -- the allocation lever had to go in before
 * either narrow-constant site could be read at all.
 *
 * `r4` IS ABSENT FROM THE PUSH MASK and stays absent: `push {r5, r6, r7, lr}`
 * plus the three high saves, with no `-ffixed-r4` and no hand-holding.  The
 * unit builds with -fcall-used-r4 like the rest of the tree.
 */
extern unsigned char gState[];
extern short *L2dd0 __asm__(".L2dd0");
extern int _AREA_2d;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __SetDestMap(int map, int entrance);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Surprise(int slot, int a);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8092b08(int slot, int a);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")

void OvlFunc_918_2008334(int flag, int a, int b, int c)
{
    unsigned char *gs;
    unsigned char *p;
    int x;
    int y;
    int slot;
    int i;

    gs = gState;
    x = (*(int *)(gs + 0x1dc) >> 20) + 0x40;
    y = *(int *)(gs + 0x1e4) >> 20;
    slot = *(int *)(gs + 0x1f4);
    p = __MapActor_GetActor(slot);
    if (c == *L2dd0)
        return;
    *L2dd0 = c;
    if (__GetFlag(flag) == 0) {
        __Func_80105d4(a, b, 1, 1, x, y);
        __SetFlag(flag);
        return;
    }
    *L2dd0 = -1;
    __Func_80105d4(a, b + 1, 1, 1, x, y);
    __PlaySound(0xce);
    __CutsceneStart();
    __SetDestMap((int)&_AREA_2d, c);
    __MapActor_SetAnim(slot, 0x1b);
    __Actor_SetSpriteFlags(__MapActor_GetActor(slot), 0);
    { PIN1; q0 = slot;
      __MapActor_Surprise(q0, 0x101); }
    __CutsceneWait(0x1e);
    { PIN2; q0 = -1; q1 = -1;
      __Func_80933f8(q0, q1, -1, 0); }
    p[0x55] = 2;
    *(int *)(p + 0x14) = 0xff600000;
    *(int *)(p + 0x48) = 0x80 << 8;
    __PlaySound(0xcc);
    __CutsceneWait(3);
    p[0x22] = 2;
    __Func_8092b08(slot, 3);
    i = 0x1d;
    do {
        *(unsigned short *)(p + 6) += 0x2000;
        __WaitFrames(1);
        i--;
    } while (i >= 0);
    if (c == 0x32)
        return;
    __SetFlag(0x91 << 1);
}
