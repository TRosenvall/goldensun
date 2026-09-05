/* Func_80ad274 (0x080AD274) -- rebuild the four status/Djinn screen actors.
 *
 * Whole-file replacement for asm/rom_a1000/rom_ad274_a.s, which holds this ONE
 * function and no data (1 .thumb_func_start, no .data/.rodata/.lcomm/.global).
 * Exactly one linker line names the object, on its full path:
 *
 *     stage1.ld:  asm/rom_a1000/rom_ad274_a.o(.text)
 *
 * (line number omitted deliberately -- it drifted 1241 -> 1244 during this
 * session while another elevation edited stage1.ld.  Match on the full path.)
 * That line is UNCHANGED by this elevation and must not be repointed at src/:
 * the generic `asm/%.o: src/%.c` rule rebuilds the same object path from this
 * .c, so no linker edit and no split are needed -- just add the .c and delete
 * the .s.
 *
 * BUILD FLAGS: this TU needs ALIAS_CFLAGS (-fno-strict-aliasing).  See "the
 * scheduling residue" below; the object is byte-identical with it and 6
 * encodings out without it.
 *
 *     asm/rom_a1000/rom_ad274_a.o: src/rom_a1000/rom_ad274_a.c
 *             $(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
 *             printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *             arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
 *
 * The one caller is asm/rom_a1000/rom_a7380_a_c.s:176, which passes
 * *(int *)(state + 0x10c) in r0 and 0 in r1.  Neither is read here; the two
 * parameters are the call site's, not scaffolding, and dropping them measures
 * inert.
 *
 * WHAT IT DOES.  iwram_3001f2c points at the field-state block.  Four sprite
 * slots live at +0x224, four x halfwords at +0x234 and four y halfwords at
 * +0x23c.  (The `.s` header comment says the second halfword array is at +0x244;
 * it is not -- `strh r3, [r6, #8]` off a +0x234 base is +0x23c.  +0x244 is the
 * u32 scale array that src/rom_a1000/rom_ad274_c_a_b.c already names.)  Every
 * live slot is destroyed, then four sprites are created from the resource table
 * .Laf304, started on animation 2, and stored back with x = 0x10 and y = 0x20.
 * Finally Func_80ad35c is registered at sort key 0xC80.
 *
 * ------------------------------------------------------------------ levers --
 *
 * 1. LOOP 1 IS THE FAMILY TEMPLATE, taken verbatim from the solved sibling
 *    src/rom_a1000/rom_ad274_b.c (Func_80ad318) and src/rom_a1000/rom_ad274_c_b.c
 *    (Func_80ad658): `for (i = 0x89; i <= 0x8c; i++)` over
 *    `*(unsigned int *)(base + (i << 2))`.  gcc turns it into the ROM's
 *    `mov r5,#0x89 / lsl r5,#2` walking offset plus a synthetic `mov r2,#3`
 *    down-counter, and `base` staying live for loop 2 is what keeps it in a
 *    register and makes the addressing register-offset.
 *
 * 2. LOOP 2 MUST NOT BE LEFT TO STRENGTH REDUCTION -- the whole match turns on
 *    this (69 differing of 73 -> 10).  Written the obvious way, with `tbl[i]`
 *    or `tbl + (i << 2)` as the only address expression, gcc-2.96 strength-
 *    reduces it to a POINTER walk and emits `ldmia rN!, {r0}`.  A pointer that
 *    is a memory base needs LO_REGS, so all four loop values compete for
 *    r5/r6/r7, gcc spills two to the stack (`sub sp, #8`), and neither r8 nor
 *    r10 is ever allocated -- the ROM's `push {r6, r7}` of r8/r10 disappears.
 *    Giving the loop its four induction variables EXPLICITLY -- `off` stepped
 *    by 4, `slot` post-incremented, `coord` incremented, `i` counting -- leaves
 *    `off` and `tbl` as plain integers with no LO_REGS constraint, so they land
 *    in r8 and r10 and reload copies them down (`mov r1, r8 / mov r3, sl`)
 *    exactly as the ROM does.  This is the recorded "un-rotated loops / goto
 *    loops disable loop optimisation" family reached without a goto: the
 *    address arithmetic is already in the form loop.c would have produced, so
 *    there is nothing left for it to rewrite.
 *
 *    `tbl` MUST be a named local (dropping it and writing `(int)Laf304` inline
 *    is 72 differing): a symbol address folded into the expression is not a
 *    loop-invariant register, and gcc goes back to the pointer walk.
 *
 * 3. `off + (int)tbl`, INDEX FIRST.  Thumb encodes the two registers of
 *    `ldr Rd, [Rn, Rm]` in fixed positions.  The ROM has `ldr r0, [r1, r3]`
 *    with r1 = the walking offset and r3 = the table base, which is the
 *    subscript form; `tbl + off` gives base-first and costs 2 encodings.
 *    (docs/elevation.md, "A register-offset load's operand order".)
 *
 * 4. `priority = 0xc8 << 4;` NAMED.  The ROM builds the shift before the pool
 *    load -- `mov r1,#0xc8 / lsl r1,#4 / ldr r0,=Func_80ad35c` -- and inline it
 *    comes out `mov / ldr / lsl`, costing 2.  This is the recorded
 *    OvlFunc_common1_1490 case (a shifted value competing with an EXPENSIVE
 *    operand, not with a cheap mov), and the same spelling the solved
 *    src/rom_a1000/rom_a1814_c_a_c_c_a_c_a_b.c uses for the same StartTask
 *    priority.
 *
 * 5. THE HALFWORD CONSTANTS STAY BARE LITERALS.  gcc-2.96 has no immediate
 *    alternative for an HImode constant, so `coord[0] = 0x10;` pools the value;
 *    the ROM pools it too (`.word 0x10`, `.word 0x20` ahead of the three symbol
 *    words, and a real `b` over the mid-function pool).  The usual int-
 *    intermediate cure is the WRONG direction here and would give `mov r3,#0x10`.
 *
 * 6. THE SCHEDULING RESIDUE, and why the flag.  With 1-5 in place the object is
 *    6 encodings out, all of them one instruction: the ROM issues
 *    `stmia r7!, {r5}` in the load-use shadow of the first pooled halfword,
 *    ours sinks it five slots to the end of the block.  At -O2 gcc-2.96 turns
 *    on -fstrict-aliasing, the post-reload scheduler proves the `int` store to
 *    +0x224 cannot alias the `short` stores to +0x234/+0x23c, and the store is
 *    free to move; the ROM's build did not have that freedom.
 *    -fno-strict-aliasing restores the output dependence, the store stays where
 *    the source puts it -- FIRST in the body, which is load-bearing: moving it
 *    after the two coordinate stores is 7 differing even with the flag.
 *
 *    Measured, and NONE of these reach it: all nine orderings of the five body
 *    statements (6-8 differing); `unsigned short *coord`, `void **slot`,
 *    `unsigned int *slot`, `void *`-typed sprite handles (all 6, i.e. inert);
 *    -fno-schedule-insns2 (20 -- the pass is wanted, only its alias information
 *    is not), -fno-gcse (6), -fno-schedule-insns (6),
 *    -fno-rerun-cse-after-loop (65).
 *
 *    NEW, and not the recorded presentation: docs/elevation.md's
 *    "-fno-strict-aliasing: a per-TU flag, and how to recognise it" describes
 *    the symptom as a LOAD hoisted above a STORE.  Here it is a STORE SUNK
 *    BELOW TWO OTHER STORES of a different width.  Same pass, same alias
 *    information, mirrored presentation -- worth adding to the recognition list.
 *
 *    A source-only cure exists and is exact under the default flags, but it is
 *    a type-punning hack rather than a reading: routing the slot store through
 *    a union member (`((union U *)slot)->i = sprite;`) makes gcc-2.96's
 *    lang_get_alias_set return alias set 0 for it -- c-common.c:3339, "permit
 *    type-punning when accessing a union" -- which is the recorded "a char
 *    lvalue is alias set 0 and cannot move past the other stores" lever reached
 *    at word width.  Kept as scratch_elev/b234/f80ad274/alt_union.c in case the
 *    Makefile rule is not wanted; the flag is the honest reading.
 *
 * MEASURED INERT, so not shipped: an ascending vs descending loop-2 counter
 * (`for (i = 3; i >= 0; i--)` also matches -- gcc synthesises the same
 * down-counter either way), `*slot = sprite; slot++;` split in two statements,
 * the signedness of `coord`, the width of `slot`, and dropping the two unused
 * parameters.
 */

extern unsigned int iwram_3001f2c;              /* @ 0x03001F2C */
extern int  _DeleteSprite(int sprite);
extern int  _CreateSprite(int resource);
extern void _Sprite_SetAnim(int sprite, int anim);
extern void StartTask(void *task, int priority);
extern int  Func_80ad35c;

/* The resource table is a file-local label in asm/rom_a1000/rom_ad274_c_c_c.s,
 * which already carries `.global .Laf304`.  gcc's asm-label extension reaches
 * it without renaming anything, which matters because two other `.s` files in
 * this directory name it as well.
 */
extern unsigned char Laf304[] __asm__(".Laf304");

void Func_80ad274(int window, int unused)
{
    unsigned char *base;
    unsigned char *tbl;
    unsigned int *slot;
    short *coord;
    int i;
    int off;
    int sprite;
    int priority;

    base = (unsigned char *)iwram_3001f2c;
    for (i = 0x89; i <= 0x8c; i++) {
        if (*(unsigned int *)(base + (i << 2)) != 0) {
            _DeleteSprite(*(unsigned int *)(base + (i << 2)));
            *(unsigned int *)(base + (i << 2)) = 0;
        }
    }

    tbl = Laf304;
    coord = (short *)(base + (0x8d << 2));
    slot = (unsigned int *)(base + (0x89 << 2));
    off = 0;
    for (i = 0; i < 4; i++) {
        sprite = _CreateSprite(*(unsigned int *)(off + (int)tbl));
        if (sprite != 0) {
            _Sprite_SetAnim(sprite, 2);
        }
        *slot++ = sprite;
        coord[0] = 0x10;
        coord[4] = 0x20;
        off += 4;
        coord++;
    }

    priority = 0xc8 << 4;
    StartTask((void *)&Func_80ad35c, priority);
}
