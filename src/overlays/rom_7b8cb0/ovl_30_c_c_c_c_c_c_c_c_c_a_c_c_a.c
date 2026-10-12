/* OvlFunc_931_20086f0 -- 0x020086f0
 *
 * MATCHES.  200 bytes, 92 encodings and 6 relocations identical.
 *
 * *** REQUIRES THE PER-FILE FLAG GROUP ALIAS_CFLAGS (-fno-strict-aliasing). ***
 * At the production -O2 this body reads 5 of 92; the parked body read 17 of 92.
 * Makefile rule to add (the group is Makefile:235, already applied to ~20 TUs):
 *
 *   asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_c_a.o: src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_c_a.c
 *       $(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
 *       printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *       arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     -e OBJCMP_EXTRA=-fno-strict-aliasing goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_c_a.c \
 *     asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_c_a.s --func OvlFunc_931_20086f0
 *
 * PIN-FREE (shimcount.py reports nothing), DEVICE-FREE, no split: the reference
 * .s holds this one function (tools/piece.py).  Install path is the reference's
 * sibling .c, so the hand-written .s is deleted and the generated one replaces
 * it in the same commit.
 *
 * NOTE FOR THE PARK INDEX: the park's FIRST LINE named
 * asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c.s, which a later split
 * consumed and which does not exist.  Only its `Verify with:` recipe named the
 * live file.  Its two figures also disagreed -- "17 of 92" in the header and
 * "17 of 96" nine lines down; 92 is the stream count and 96 the tryc line count.
 *
 * THE ROUTE, four steps, each one measured against the 92-encoding stream:
 *
 *    17   parked body at the production -O2
 *    12   -fno-strict-aliasing, no source change   (tools/autosweep.py, flag axis)
 *    11   + split the reused `s` into s/s2, one per halfword site
 *     3   + split the reused `v` into v/vy/v2
 *     0   + `e6 = 0xe666;` named in the entry block beside m1/m2
 *
 * The two splits are a CROSSED result: `v` alone is inert (12) and `s` alone is
 * worth one (11); together they are worth nine.  Neither would have been
 * reported as a lever on its own.
 *
 * THE MECHANISM, with the lines, because the park had the symptom and no cause.
 * Its diagnosis -- "which register holds the SIGNED and which the UNSIGNED read
 * of the same halfword", and that source read order picks the registers while
 * sched2 fixes the emission order -- was RIGHT, and x.c.18.greg names the pass
 * in three lines:
 *
 *      ;; 8 regs to allocate: 34 35 32 37 57 60 33 36   <- 34 (s) allocated FIRST
 *      ;; 35 preferences: 3                            <- 35 (v) prefers r3
 *      34 in 2   35 in 3                               <- and s loses r3 to it
 *
 *   * find_reg's pass 0 ors `regs_someone_prefers` into the unusable set,
 *     global.c:1016.
 *   * prune_preferences put r3 there for allocno 34 because the LOWER-priority
 *     allocno 35 prefers it, global.c:931-943 -- and it spares a register only
 *     when the higher-priority allocno prefers it TOO (the
 *     AND_COMPL against hard_reg_full_preferences at global.c:941).
 *   * 35 prefers r3 because set_preference (global.c:1440 -> 1585, where
 *     src = XEXP (PLUS, 0) for a non-copy set) sees
 *     (set (reg w) (plus (reg 35) (const_int -1))) with `w` LOCAL-allocated to r3.
 *   * REG_ALLOC_ORDER is { 3, 2, 1, 0, 12, 14, 4, ... } (arm.h:988-994), so
 *     losing r3 costs exactly one place and lands `s` in r2 -- which is also why
 *     the ROM's three-operand `sub r3, r2, #1` / `add r3, r2, #1` came out as
 *     the destructive two-operand form.
 *
 * So the handle is NOT a priority lever and NOT statement order: it is how many
 * allocnos exist and in what order allocno_compare (global.c:598-620) puts them,
 * which splitting the two reused locals changes directly.
 *
 * MEASURED INERT, all at 12 of 92 under the flag, so nobody re-screens them:
 *   - read order at either site: 4 spellings x 2 sites x 2 declaration orders,
 *     32 variants.  Unsigned-first at BOTH sites is the only 12; signed-first at
 *     site 2 is 16, at site 1 is 19.  DECLARATION ORDER OF s AND v IS COMPLETELY
 *     INERT -- all 16 pairings identical in both orders.
 *   - `*(short *)((char *)p + (unsigned int)0)`, which the park carried as if it
 *     were load-bearing, is BYTE-IDENTICAL to plain `*(short *)p` in all 16
 *     pairings.  Dropped here: a pass-3 artifact removed for free.
 *   - `short s` instead of `int s`, with and without the splits.
 *   - three attempts to break v's r3 preference at the source --
 *     `vd = v; w = vd - 1;`, `w = v; w -= 1;`, `w = -1 + v;` -- all still leave
 *     the PLUS's first operand a pseudo, so set_preference fires anyway.
 *   - `sc = s; if (sc != 0)`, the attempt to give allocno 34 its own r3
 *     preference.
 *   - `e6` assigned AT the call site rather than in the entry block: 86 of 92,
 *     size -12.  Inlining `-1, -1`: 82 of 92, size -4.  The park's entry-block
 *     naming of m1/m2 is load-bearing and the third argument needed it too.
 *
 * Three placements of `e6 = 0xe666;` inside the entry block all reach 0, so the
 * lever is that the constant is a NAMED ENTRY-BLOCK LOCAL and not where in the
 * block it sits.  It removes the whole `ldr r2, =0xe666 / neg / neg` ordering
 * group the park had written off as unreachable.
 *
 * UNCLAIMED, NOT MEASURED: the park names a twin, OvlFunc_932_2009770 in
 * asm/overlays/rom_7b9cb4/, "the same function over a different actor record ...
 * Solving one solves both."  The route above is the thing to try on it first.
 */
extern void __Func_8012330(int a, int b, int c);
extern void __PlaySound(int id);

void OvlFunc_931_20086f0(unsigned char *a)
{
    unsigned short *p;
    int s;
    int s2;
    int v;
    int vy;
    int v2;
    int t;
    int lim;
    int w;
    int m1;
    int m2;
    int e6;
    int four;
    int q1;
    int q2;
    int u;
    int zero;

    q1 = 0x80 << 9;
    q2 = 0x80 << 9;
    m1 = -1;
    m2 = -1;
    e6 = 0xe666;
    p = (unsigned short *)(a + 0x66);
    v = *p;
    s = *(short *)p;
    if (s != 0) {
        w = v - 1;
        *p = w;
        if ((short)w == 1)
            __Func_8012330(m1, m2, e6);
    }
    t = *(int *)(a + 0x28);
    if (t == 0) {
        __Actor_SetAnim(a, 1);
        vy = *(int *)(a + 0xc) + 0xfffe8000;
        lim = *(int *)(a + 0x14);
        *(int *)(a + 0xc) = vy;
        if (vy < lim) {
            if (*(int *)(a + 0x68) != 0) {
                __PlaySound(0xe5);
                *(int *)(a + 0x68) = t;
                four = 4;
                *p = four;
                __Func_8012330(0, q1, q2);
                lim = *(int *)(a + 0x14);
            }
            *(int *)(a + 0xc) = lim;
        }
        a[0x5b] = 1;
    } else {
        a[0x5b] = 0;
    }
    p = (unsigned short *)(a + 0x64);
    v2 = *p;
    s2 = *(short *)p;
    if (s2 == 0) {
        __PlaySound(0x98);
        *(int *)(a + 0x68) = 1;
        __Actor_SetAnim(a, 2);
        *(int *)(a + 0x28) = 0xc0 << 10;
        v2 = *p;
    }
    u = v2 + 1;
    *p = u;
    if ((short)u == 0x3c) {
        zero = 0;
        *p = zero;
    }
}
