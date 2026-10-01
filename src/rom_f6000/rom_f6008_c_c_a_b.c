/* Func_80f7e34  --  LANDS BYTE-IDENTICAL.  Batch 317, brief D.
 *
 * Install as:   src/rom_f6000/rom_f6008_c_c_a_b.c
 * Requires:     a SPLIT of asm/rom_f6000/rom_f6008_c_c_a.s   (2 functions)
 * Requires:     a per-file GCSE_CFLAGS Makefile rule (-fno-gcse)
 * Pins:         0 (tools/shimcount.py reports none)
 * fakematch.txt row: NOT needed.  No pin, no volatile, no device.
 *
 * FIGURE: 44 bytes, 21 encodings and 1 relocation IDENTICAL, both
 * `objcmp --func` and `objcmp --whole`.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_f6000/rom_f6008_c_c_a_b.c asm/rom_f6000/rom_f6008_c_c_a_b.s \
 *     --whole
 * (before the split, and before the Makefile rule exists, the same figure is
 *  reproducible with
 *    OBJCMP_EXTRA=-fno-gcse python3 tools/objcmp.py <this file> \
 *      asm/rom_f6000/rom_f6008_c_c_a.s --func Func_80f7e34 )
 *
 * SPLIT SHAPE -- `python3 tools/datacheck.py asm/rom_f6000/rom_f6008_c_c_a.s`
 * prints NOTHING: no data section, no exported .L symbol to preserve.
 *
 *   python3 tools/split_s.py asm/rom_f6000/rom_f6008_c_c_a.s Func_80f7e34
 *
 * `--dry-run` output, exactly:
 *   would write  asm/rom_f6000/rom_f6008_c_c_a_a.s  (1 function, 38 lines)  Func_80f7df0
 *   would write  asm/rom_f6000/rom_f6008_c_c_a_b.s  (1 function, 27 lines)  Func_80f7e34
 *   would REMOVE asm/rom_f6000/rom_f6008_c_c_a.s
 *   would rewrite stage1.ld
 * so the resulting file set is {_a.s kept hand-written, _b.s replaced by this
 * .c} and the only linker-script change is stage1.ld, where the one
 * rom_f6008_c_c_a.o line becomes two lines (_a.o then _b.o) in that order.
 * `make compare` must be green AFTER the split and BEFORE this .c is written.
 *
 * MAKEFILE RULE (the TU holds this one function, so the rule is tight):
 *
 *   asm/rom_f6000/rom_f6008_c_c_a_b.o: src/rom_f6000/rom_f6008_c_c_a_b.c
 *     $(GCC296_CC) $(GCSE_CFLAGS) -S -o $(@:.o=.s) $<
 *     printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *     arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
 *
 * REPOINT: Func_80f7df0 has TWO parks, src/non_matching/rom_f6000/80f7df0.c
 * (claims 27 of 30) and src/non_matching/rom_f6000/f7df0.c (claims 18 of 30).
 * Both name asm/rom_f6000/rom_f6008_c_c_a.s, which this split deletes; both
 * must be repointed to asm/rom_f6000/rom_f6008_c_c_a_a.s.  Neither carries a
 * `Verify with:` recipe and they disagree about the figure, so they are also a
 * duplicate-park pair for one function.
 *
 * WHAT THE PARK SAID, AND WHAT IS TRUE
 *
 * The park claimed "15 instructions in disagreeing regions, of 21 (rom 21,
 * ours 24)" and "BLOCKER CLASS: CSE across a possibly-aliasing store -- gcc
 * caches, the ROM reloads".  Measured, the park's own body is 20 of 21 with
 * ours 23, and its mechanism is wrong in a way that matters:
 *
 *   gcc DOES reload.  Both explicit reassignments survive.  What gcc adds is
 *   TWO REGISTER COPIES and a redirected branch -- `mov r5,r1` / `mov r0,r2`
 *   before the guarded store, and the `p == 0` arm branching PAST the two
 *   reloads to the final `str`.  That is gcse's PARTIAL REDUNDANCY ELIMINATION
 *   (`pre_insert_copies` putting the copies in, `pre_edge_insert` putting the
 *   reloads on the store path only), not cse keeping a value in a register.
 *   The park's cure-of-last-resort -- "the only way to defeat that analysis is
 *   `volatile`, which is a fakematch" -- is therefore not the only way, and no
 *   fakematch is incurred.
 *
 * This is the class docs/elevation.md already names at "`-fno-gcse` reaches a
 * SUNK LOAD but not a SHARED CONSTANT": *"a redundant LOAD sunk onto the only
 * path that needs it ... The ROM reloads unconditionally"*, with Func_807a550
 * as the worked example, landed on the flag.  The Makefile already carries the
 * GCSE_CFLAGS group for it.
 *
 * AND IT IS A CROSS, NOT A FLAG FIX.  Neither half lands alone:
 *
 *   park body, plain -O2                      20 of 21 (ours 23)
 *   park body + -fno-gcse                      9 of 21 (count now exact)
 *   distinct reload locals, plain -O2         10 of 21 (count now exact)
 *   distinct reload locals + -fno-gcse         0
 *
 * The flag closes the instruction COUNT (it removes the two PRE copies); the
 * distinct locals close the ALLOCATION.  Re-using `n` and `p` for the reloads
 * makes one web each, so the reload inherits the first load's register; the
 * ROM's reloads land in the registers freed by the values they replace
 * (n2 -> r2, p's old register; p2 -> r3, k's).  Four pseudos, not two.
 */
extern unsigned char *ewram_2004c00;

void Func_80f7e34(int i)
{
    unsigned char *b;
    unsigned int k;
    unsigned int j;
    unsigned char *n;
    unsigned char *p;
    unsigned char *n2;
    unsigned char *p2;

    b = ewram_2004c00;
    k = i * 3;
    k <<= 2;
    j = k + 4;
    n = *(unsigned char **)(b + j);
    if (n == 0)
        return;
    p = *(unsigned char **)(b + k);
    if (p != 0)
        *(unsigned char **)(p + 4) = n;
    n2 = *(unsigned char **)(b + j);
    p2 = *(unsigned char **)(b + k);
    *(unsigned char **)n2 = p2;
}
