/* GetMercuryDjinni  --  0x080965a8  (READ FROM THE LINKED ELF:
 * `arm-none-eabi-nm goldensun.elf | grep GetMercuryDjinni` -> 080965a8 T
 * GetMercuryDjinni.  NOT inferred from the .s stem.)
 *
 * PARKED at 17 differing encodings of 239, exact length, exact relocations.
 *
 * The ONLY function in asm/rom_8a000/rom_944ec_a_c_a_c_c_c.s (254 lines, one
 * .thumb_func_start, no .section .data, no .incbin) -- so this is a WHOLE-FILE
 * landing with no split.  stage1.ld names the object on exactly one line,
 * line 955:
 *
 *              asm/rom_8a000/rom_944ec_a_c_a_c_c_c.o(.text)
 *
 * and that is the ONLY reference to it anywhere in *.ld or the Makefile (the
 * generic `asm/%.o: src/%.c` rule builds it).  The line must NOT be rewritten
 * to a src/ path.
 *
 * THE .s's OWN HEADER COMMENT IS WRONG.  It reads "@ RunVehicleCutscene ...
 * Takes no arguments", which is a stale structural guess; the
 * .thumb_func_start says GetMercuryDjinni and the body takes a slot argument.
 * Do not trust it.
 *
 * NOT A TWIN OF Mars/Jupiter.  `python3 tools/solved_twins.py` reports 0 across
 * 0 templates (2156 solved shapes, 1469 remaining) -- a twin miss, and a real
 * one: this function shares only the null-return opening, the v[3] copy into
 * Func_80974d8, a 24-iteration Func_809ba90/7c/70 sprite loop and the
 * gState+0x1f4 tail with GetMarsDjinni.  Everything in between -- an 8-iteration
 * _CreateActor spawn loop writing an `unsigned char *arr[8]` through a walking
 * pointer, a Func_8096c48 chain, a 15-iteration drift loop, a delete loop over
 * arr -- has no counterpart in either sibling.  The Mars levers were
 * re-measured, not transplanted; two of the five do not apply at all.
 *
 * LEVERS THAT MATTERED, with mechanisms:
 *
 *  1. THE gState TAIL WANTS *ONE* LOCAL HERE, NOT TWO -- the Mars lever
 *     INVERTED.  Mars needed `g0` at the top and `g` after the loop because its
 *     two gState sites are far apart and one local spanning them took a
 *     callee-saved register.  Here both `ldr r0,[r5]` sites are adjacent, so a
 *     single `g = gState; g += (0xfa << 1);` is right and the destructive
 *     `add r5, r3` is the ROM's.  Confirms the standing rule that Mars's levers
 *     are SUFFICIENT, NOT NECESSARY.
 *
 *  2. FRAME LAYOUT IS DECLARATION ORDER; REGISTER ALLOCATION IS NOT.
 *     Declaring `unsigned char *arr[8]` BEFORE `int v[3]` puts v at sp+8 and arr
 *     at sp+0x14, which is the ROM.  The reverse order puts arr at sp+8 and v at
 *     sp+0x28.  Worth 0 differing on its own but it is a prerequisite: with the
 *     wrong frame every stack reference is wrong.
 *     Declaring `base` BEFORE `ptr` gives the ROM's spill-slot ORDER (ptr at
 *     sp+0, base at sp+4).  53 -> 49.
 *     REGISTER allocation, by contrast, is completely inert to declaration
 *     order: see the measured-worse table.
 *
 *  3. THE SECOND ARRAY POINTER MUST BE BORN AT ITS LOOP.  The ROM keeps
 *     `&arr[0]` in r11 across the whole body and spills the loop-1 walker to
 *     sp+0 (`ldr r3,[sp] / stmia r3!,{r5} / mov r2,r3 / str r2,[sp]`), then
 *     starts the delete loop with `mov r5, r11`.  Writing `q = arr;` next to
 *     `ptr = arr;` before loop 1 makes `q` a long-lived allocno that WINS the
 *     register and pushes `ptr` out the other way -- symmetric, length-neutral,
 *     and wrong.  Moving `q = arr;` to immediately before the delete loop lets
 *     CSE common `&arr[0]` into one pseudo (r11) and reduces `q` to a
 *     short-lived copy, reproducing `mov r5, r11` exactly.
 *
 *  4. THE ROM'S STORE ORDER AT actor+0x18 / +0x1c IS SOURCE ORDER.  gcc does
 *     not reorder the pair; write +0x1c first.  (Mars's file writes +0x18 first
 *     for its own ROM -- again sufficient, not necessary.)
 *
 *  5. THE SUBWORD-REUSE ZERO, and it is the recorded `Func_809a3c4` shape
 *     (docs/elevation.md, "A byte-sized zero can be served by a register whose
 *     LOW BYTE is already zero").  With `*(char *)(a + 0x55) = 0;` written as a
 *     literal, gcc reuses the register still holding 0xf0<<8 = 0xf000 -- whose
 *     low byte is zero -- and emits `strb r3,[r2]` with no `mov r3,#0`, coming
 *     out ONE INSTRUCTION SHORT.  `z = 0; *(char *)(a + 0x55) = z;` restores it.
 *     Worth 175 -> 58 in combination with lever 4.
 *
 *  6. A SEPARATE POINTER FOR THE FINAL VALUE LOOP.  The last loop has NO CALLS,
 *     so the ROM walks it in r2 -- a CALLER-SAVED register -- with the stored
 *     constant in r1.  Sharing the sprite loop's `p` forces one long-lived
 *     callee-saved pointer across both loops and puts the constant in r2.
 *     A distinct `w` makes the last loop's pointer a short caller-saved allocno
 *     and the whole loop goes exact.  26 -> 20.
 *     GENERAL FORM: a loop with no calls in it can hold its induction pointer in
 *     a caller-saved register; if the ROM does that, the pointer is a SEPARATE
 *     VARIABLE from the one used by a neighbouring loop that does call.
 *
 *  7. `i--` AFTER THE CALL IN THE DRIFT LOOP.  The ROM has
 *     `str r3,[r6,#0xc] / mov r0,#1 / sub r7,#1 / bl WaitFrames`; with
 *     `i--; WaitFrames(1);` the post-reload scheduler hoists the `sub` above the
 *     store.  Writing `WaitFrames(1); i--;` (identical semantics -- `i` is not
 *     an argument) produces the ROM's order.  20 -> 17.  The same reorder in the
 *     sprite loop is inert or worse, so this is per-loop, not a rule.
 *
 *  8. A HARD REGISTER ON `e`.  `register unsigned char *e asm("r6");` is worth
 *     42 -> 17 and is the difference between the plain-C floor and this file.
 *     See the blocker note.
 *
 * BLOCKER CLASS -- CALLEE-SAVED ROTATION, and it is the recorded
 * "TWO CALLEE-SAVED REGISTERS SWAPPED: PIN EITHER MEMBER" family.
 * The residue is 17 encodings, ALL of them one three-cycle over the last 60
 * instructions and nothing else:
 *
 *     ROM   bp -> r5   p -> r6   loop-4 counter -> r7
 *     ours  p  -> r5   counter -> r6   bp -> r7
 *
 * Every instruction, in order, with the ROM's operands, up to index 175 of 239.
 * The recorded discriminator for the class is "several unrelated spellings, ONE
 * number", and it is present twice over:
 *   - 80 random permutations of the 18 local declarations: 20 differing, EVERY
 *     ONE of them.
 *   - 32 combinations of `goto`-loop vs `do { } while` across the five loops:
 *     58 differing, EVERY ONE.  Loop FORM is inert in this function; gcc emits
 *     the same RTL either way and hoists nothing, so the loop-note weighting
 *     that the allocator would otherwise see never appears.
 * The recorded remedy is a pin on either member.  `e asm("r6")` IS that pin and
 * it closes the first 175 instructions; the surviving three-cycle is a SECOND,
 * disjoint instance in the tail, and its members are `bp`, `p` and the counter.
 * A pin cannot reach it: r5/r6/r7 are all live elsewhere in the body, and
 * pinning any of the three to a high register explodes the function (thumb
 * cannot use r8-r11 as a memory base, so every access grows a copy -- measured
 * at 321 lines against 244 on the Venus sibling).
 *
 * NO FLAG GROUP REACHES IT.  Swept on three different candidates:
 * -fno-schedule-insns2 (70 differing), -fno-gcse (32), -fno-rerun-cse-after-loop
 * (182), -fno-strict-aliasing (20, INERT), -fno-strength-reduce (20, INERT).
 * THIS FUNCTION DOES NOT DEPEND ON A FLAG GROUP -- the two that change nothing
 * change nothing, and the three that change something make it worse.
 *
 * MEASURED WORSE / INERT (all against the same reference, 244-line ROM):
 *   declaration order, 60 permutations, at the 49-differing base     49 every one
 *   declaration order, 80 permutations, at the 20-differing base     20 every one
 *   loop form g/d, all 32 combinations across five loops             58 every one
 *   `int arr[8]` + `int *` walkers instead of `unsigned char *arr[8]`   inert
 *   direct `v[0]`/`v[2]` instead of a named `bp`                        inert
 *   one shared counter for all five loops (mask 0)                   248 lines
 *   per-loop counters for all five (mask 31)                         246 lines
 *   loop-4 counter split alone (mask 8)                              the match shape
 *   `q = arr` before loop 1 instead of before loop 3                 118 differ
 *   `*(char *)(a+0x55) = 0` as a literal                             one SHORT
 *   `*(int*)(a+0x18)` before `+0x1c`                                 +1 differ
 *   `i--` before `WaitFrames` in the drift loop                      +3 differ
 *   sharing `p` between the sprite loop and the value loop           +6 differ
 *   `register int i asm("r7")`                                       238 lines --
 *       r7 is the thumb frame-pointer register; pinning it drops it from the
 *       push list and rewrites the prologue.  Do not pin r7.
 *   pins of `e` to r5 / r7                                           worse
 *
 * NEW FINDING (grepped first: docs/elevation.md has "A zero survives in a
 * callee-saved register only inside a LOOP", "`goto` loops disable loop
 * optimisation ENTIRELY", "The `goto`-loop lever's SIGNATURE IS NECESSARY, NOT
 * SUFFICIENT" and "MATCH THE ROM'S LOOP SHAPE"; none of them states this) --
 * WHEN A FUNCTION'S LOOPS CARRY NO HOISTABLE INVARIANT AND NO STRENGTH-REDUCIBLE
 * GIV, `goto` AND `do { } while` ARE INDISTINGUISHABLE, INCLUDING TO THE
 * REGISTER ALLOCATOR.  32 form combinations across five loops, all 58 differing.
 * The `goto` lever is recorded as acting through loop.c; the loop-note
 * WEIGHTING of REG_N_REFS is a separate consequence of the same notes, and the
 * natural expectation is that it moves allocation even when loop.c has nothing
 * to do.  It does not, here.  So a residue that is purely an allocator ranking
 * cannot be attacked through loop form -- read the loop bodies for an invariant
 * FIRST, and if there is none, do not spend the sweep.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001f30;
extern unsigned char *MapActor_GetActor(int slot);
extern void Func_80958a8(void);
extern void _Func_80b0840(int a);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void Func_80925cc(int a, int b);
extern void MapActor_Jump(int a, int b, int c);
extern unsigned char *_CreateActor(int id, int x, int y, int z);
extern void _Actor_SetColorswap(unsigned char *a, int n);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _Actor_SetSpriteFlags(unsigned char *a, int n);
extern unsigned char *Func_8096c48(int a, unsigned char *b);
extern void Func_8096574(void);
extern void _DeleteActor(unsigned char *e);
extern void Func_8003f3c(int n);
extern void Func_8092adc(int a, int b, int c);
extern void Func_80974d8(int *v);
extern void Func_809ba90(unsigned char *p, int a, int b, int c);
extern void Func_809ba7c(unsigned char *p, void (*f)(void));
extern void Func_809641c(void);
extern void Func_809ba70(unsigned char *p, int n);
extern void _Sprite_SetColorswap(int a, int b);
extern void _Func_80b0894(void);
extern void Func_80958e4(void);

void GetMercuryDjinni(int slot)
{
    unsigned char *arr[8];
    int v[3];
    int *bp;
    unsigned char **q;
    register unsigned char *e asm("r6");
    unsigned char *base;
    unsigned char **ptr;
    unsigned char *a;
    unsigned char *prev;
    unsigned char *h;
    unsigned char *p;
    unsigned char *w;
    unsigned char *g;
    int id;
    int last;
    int val;
    int i;
    int i4;
    int z;

    e = MapActor_GetActor(slot);
    if (e == 0)
        return;
    Func_80958a8();
    base = iwram_3001f30;
    _Func_80b0840(0x204084);
    WaitFrames(0x1e);
    *(char *)(e + 0x5b) = 0;
    _PlaySound(0xad);
    Func_80925cc(slot, 1);
    _PlaySound(0xaf);
    Func_80925cc(slot, 1);
    WaitFrames(0x14);
    _PlaySound(0x98);
    MapActor_Jump(slot, 3, 0xe);
    _PlaySound(0x98);
    MapActor_Jump(slot, 5, 0x10);
    _PlaySound(0x98);
    MapActor_Jump(slot, 7, 0x12);
    WaitFrames(0x14);
    id = *(short *)(*(int *)(*(int *)(e + 0x50) + 0x28));
    h = 0;
    prev = e;
    ptr = arr;
    i = 7;
loop1:
    a = _CreateActor(id, *(int *)(e + 8), *(int *)(e + 0xc), *(int *)(e + 0x10));
    *ptr++ = a;
    if (a != 0) {
        *(int *)(a + 0x1c) = 0xf0 << 8;
        *(int *)(a + 0x18) = 0xf0 << 8;
        z = 0;
        *(char *)(a + 0x55) = z;
        *(char *)(a + 0x23) = 2;
        *(unsigned char *)(a + 0x5a) |= 1;
        *(void **)(a + 0x6c) = Func_8096574;
        *(unsigned short *)(a + 6) = *(unsigned short *)(e + 6);
        _Actor_SetColorswap(a, 9);
        _Actor_SetAnim(a, 0);
        _Actor_SetSpriteFlags(a, 0);
        h = Func_8096c48(*(int *)(a + 0x50), h);
        *(unsigned char **)(a + 0x68) = prev;
        prev = a;
    }
    i--;
    if (i >= 0)
        goto loop1;
    last = *(unsigned char *)(h + 0x1c);
    _PlaySound(0x99);
    *(char *)(e + 0x55) = 0;
    i = 0xe;
loop2:
    *(int *)(e + 0xc) += 0x80 << 12;
    WaitFrames(1);
    i--;
    if (i >= 0)
        goto loop2;
    _DeleteActor(e);
    q = arr;
    i = 7;
loop3:
    _DeleteActor(*q++);
    i--;
    if (i >= 0)
        goto loop3;
    if (last != 0x60)
        Func_8003f3c(last);
    WaitFrames(0xa);
    g = gState;
    g += (0xfa << 1);
    Func_8092adc(*(int *)g, 0x80 << 7, 0);
    WaitFrames(0x14);
    _Actor_SetAnim(MapActor_GetActor(*(int *)g), 0x1c);
    WaitFrames(0x14);
    bp = v;
    bp[0] = *(int *)(e + 8);
    bp[1] = *(int *)(e + 0xc);
    bp[2] = *(int *)(e + 0x10);
    Func_80974d8(bp);
    p = base + 0x58;
    i4 = 0x17;
loop4:
    Func_809ba90(p, 0xf0, bp[0], bp[2]);
    Func_809ba7c(p, Func_809641c);
    Func_809ba70(p, 7);
    _Sprite_SetColorswap(*(int *)p, 9);
    i4--;
    WaitFrames(1);
    p += 0x48;
    if (i4 >= 0)
        goto loop4;
    WaitFrames(0x78);
    w = base;
    val = 2;
    w += 0x98;
    i = 0x17;
loop5:
    if (*(signed char *)(w + 5) != 0)
        *w = val;
    i--;
    w += 0x48;
    if (i >= 0)
        goto loop5;
    WaitFrames(0x32);
    _Func_80b0894();
    Func_80958e4();
}
