/* Cluster Func_8096d84..Func_8096d84 extracted from goldensun/asm/rom_8a000/rom_96cdc_a_a_c.s.
 *
 * Total .text for this TU = 88 bytes (= 0x58).
 * Preserves the original ROM layout when slotted between
 * asm/rom_8a000/rom_96cdc_a_a_b.o and asm/rom_8a000/rom_96cdc_a_a_c_c.o in
 * goldensun/stage1.ld.
 *
 * Solved in batch 271 and landed in 272. No pins, no flags.
 *
 * THE PARK'S DIAGNOSIS WAS WRONG ABOUT THE CAUSE, and the correction generalises.
 * It read this as gcc sinking the load toward its use -- a scheduling priority
 * problem -- and swept statement orders. The sched2 dump says insn 21
 * (`ldr r6, [r5, #0x68]`) is READY FROM t=1 and simply has priority 0, so it loses
 * every slot until nothing else is left. The ROM's order is not a priority
 * difference at all: it is an ANTI-DEPENDENCY, the load having to precede the
 * `strh` to a+0x64.
 *
 * gcc never built that dependency because `char *` (the load) and `unsigned short`
 * (the store) live in different alias sets, so -fstrict-aliasing disambiguated
 * them and freed the load to sink. Routing either access through a UNION restores
 * it -- c_get_alias_set returns 0 for a direct union access, which conflicts with
 * everything.
 *
 * THE TELL, worth more than the function: an insn READY EARLY with low priority is
 * an ALIAS problem; one that becomes ready LATE is a real dependence chain. Only
 * the second is a scheduling question.
 *
 * THREE BYTE-IDENTICAL SPELLINGS were measured. This one puns the LOAD and names
 * both types involved, which documents why the union is here. A single-member
 * `union { char *p; }` works, and so does punning the STORE instead
 * (`((union PtrPun *)(a + 0x64))->h = t;`) with the load left plain.
 *
 * -fno-strict-aliasing on the whole TU also matches with no source change, and
 * ALIAS_CFLAGS already exists in the Makefile -- but the union is preferable: this
 * is one function's problem, not the translation unit's.
 *
 * MEASURED AND REJECTED, both still 4 differing: `o = (char *)*(volatile int *)(a
 * + 0x68)` -- VOLATILE DOES NOT CREATE THE DEPENDENCE, which is the obvious first
 * try; and retyping `o` as `unsigned short *`, because gcc-2.96 canonicalises
 * pointer alias sets per target so that still does not conflict with
 * `unsigned short`.
 */
extern void _Actor_SetScript(void *a, void *script);
extern int sin(int x);
extern unsigned char Data_9f0b0[];

union PtrPun { char *p; unsigned short h; };

void Func_8096d84(char *a)
{
    char *o;
    int t;
    int s;
    int k;

    t = *(unsigned short *)(a + 0x64) + 1;
    o = ((union PtrPun *)(a + 0x68))->p;
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
