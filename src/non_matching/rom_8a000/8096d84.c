/* ==================== SOLVED IN BATCH 271, NOT YET LANDED ====================
 *
 * EXACT and verified three ways:
 *   OK Func_8096d84 -- 88 bytes, 41 encodings and 3 relocations identical
 *
 * NOT LANDED ONLY BECAUSE OF THE ROUND BUDGET. asm/rom_8a000/rom_96cdc_a_a_c.s
 * holds Func_8096ddc as well, so landing needs a two-way text split (no data
 * section -- datacheck is clean). Land this first next round.
 *
 * THE PARK'S DIAGNOSIS WAS WRONG ABOUT THE CAUSE. It read this as gcc sinking the
 * load toward its use, a scheduling priority problem. The sched2 dump says insn 21
 * (`ldr r6, [r5, #0x68]`) is READY FROM t=1 and simply has priority 0, so it loses
 * every slot until nothing else is left. The ROM's order is not a priority
 * difference at all -- it is an ANTI-DEPENDENCY: the load must precede the `strh`
 * to a+0x64.
 *
 * gcc never built that dependency because `char *` (the load) and
 * `unsigned short` (the store) are in different alias sets, so -fstrict-aliasing
 * disambiguated them and freed the load to sink. Route either access through a
 * UNION -- c_get_alias_set returns 0 for a direct union access, which conflicts
 * with everything -- and the anti-dependency appears, pinning the load ahead of
 * the store. No pins, no flags.
 *
 * THREE BYTE-IDENTICAL SPELLINGS, pick on readability:
 *   - pun the LOAD:  `o = ((union PtrPun *)(a + 0x68))->p;`  with
 *                    `union PtrPun { char *p; unsigned short h; };`
 *   - the same with a single-member union `{ char *p; }`
 *   - pun the STORE instead, keeping the load plain:
 *                    `((union PtrPun *)(a + 0x64))->h = t;`
 *
 * -fno-strict-aliasing on the whole TU also matches with NO source change, and
 * ALIAS_CFLAGS already exists in the Makefile -- but the union is preferable: this
 * is one function's problem, not the translation unit's.
 *
 * MEASURED AND REJECTED, both still 4 differing: `o = (char *)*(volatile int *)(a
 * + 0x68)` -- VOLATILE DOES NOT CREATE THE DEPENDENCY, worth knowing because it is
 * the obvious first try; and retyping `o` as `unsigned short *` with the load as
 * `*(unsigned short **)`, because gcc-2.96 canonicalises pointer alias sets
 * per-target so that still does not conflict with `unsigned short`.
 *
 * The verified candidates are scratch_elev/b271/sched/D2.c, D4.c and D5.c.
 */

/* Func_8096d84 -- asm/rom_8a000/rom_96cdc_a_a.s
 *
 * BLOCKER: POST-RELOAD SCHEDULING of a single load. 4 of 42, LENGTH EXACT.
 *
 * An animation tick: bump a halfword counter, hand off to a script once it
 * passes 0x1f, otherwise take a sine of the counter and write five fields.
 * Everything reproduces -- the sign-extended compare, the negated sine, the
 * shared 0x10000 constant used both to advance a field and as `k - sin`, the
 * multiply-by-five built as `(x << 2) + x`, and the 0x100000 bias.
 *
 * THE ENTIRE RESIDUE is where one load sits:
 *
 *     rom    ldrh / add #1 / ldr r6, [r5,#0x68] / strh / lsl / asr
 *     ours   ldrh / add #1 / strh / lsl / asr / ldr r6, [r5,#0x68]
 *
 * The pointer at +0x68 is only USED in the else-arm, so gcc sinks its load
 * toward the use; the ROM loads it before the counter store.
 *
 * MEASURED -- source order does not decide it. All three BYTE-IDENTICAL at 4:
 *   the load written between the increment and the store    42 lines, 4 differ
 *   the increment split into two statements first           42 lines, 4 differ
 *   the pointer declared after the counter                  42 lines, 4 differ
 *
 * PASS DIAGNOSTICS, run to locate the decision rather than to change the build:
 *   --no-sched2      9 differ, first divergence moves 6 -> 19   (WORSE)
 *   --no-rerun-cse   4 differ, unchanged
 *
 * The sched2 result is the useful one and it matches what ovl_780898/2008fec
 * and ovl_7cb2c0/200dca4 found: post-reload scheduling is what makes the REST
 * of this function come out right, and turning it off only breaks what already
 * works. Neither flag is a candidate for the build.
 *
 * This is a sink-toward-use decision on a value whose only consumer is inside
 * a branch. Nothing in the source can make the load wanted earlier without
 * inventing a use the ROM does not have.
 */
extern void _Actor_SetScript(void *a, void *script);
extern int sin(int x);
extern unsigned char Data_9f0b0[];

void Func_8096d84(char *a)
{
    char *o;
    int t;
    int s;
    int k;

    t = *(unsigned short *)(a + 0x64) + 1;
    o = *(char **)(a + 0x68);
    *(unsigned short *)(a + 0x64) = t;
    if ((short)t > 0x1f) {
        _Actor_SetScript(a, Data_9f0b0);
    } else {
        s = sin((short)t << 10);
        *(int *)(a + 0x18) = s;
        *(int *)(a + 0x1c) = -s;
        *(int *)(a + 8) = *(int *)(o + 8);
        k = 0x80 << 9;
        *(int *)(a + 0xc) = *(int *)(a + 0xc) + k;
        k = k - s;
        *(int *)(a + 0x10) = *(int *)(o + 0x10) - (k * 5) + (0x80 << 13);
    }
}
