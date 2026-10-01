/* OvlFunc_882_2009154 -- 0x02009154.  *** EXACT *** (batch 316).
 *
 *   OK OvlFunc_882_2009154 -- 412 bytes, 160 encodings and 33 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_c_b.c \
 *     asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_c_b.s \
 *     --func OvlFunc_882_2009154
 * (before the split, against the tracked two-function reference:
 *     ... tools/objcmp.py <this file> \
 *       asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_c.s \
 *       --func OvlFunc_882_2009154 )
 *
 * SPLIT: a clean TAIL SPLIT.  The .s holds TWO functions, OvlFunc_882_20090a4
 * then this one, and this one is LAST.  `tools/split_s.py
 * asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_c.s OvlFunc_882_2009154
 * --dry-run` writes _c_a.s (20090a4, 86 lines) + _c_b.s (this, 161 lines) and
 * rewrites overlays/rom_77dd1c/overlay.ld.  tools/datacheck.py prints NOTHING
 * for this file: no data section, no exports to add.  No Makefile pattern rule
 * reaches this TU -- objcmp prints no "(built with: ...)" line, so plain -O2.
 *
 * SHIMS: tools/shimcount.py says 17 register pins (via the PIN2/PIN3 macros,
 * six expansions) and 1 `"+r"` barrier.  A fakematch.txt ROW IS REQUIRED.
 * Every one is load-bearing; single drops from this file:
 *     the `"+r"` barrier on `t`               ->   2 of 160
 *     `p0` unpinned from r0                   -> 152 of 160, +12 bytes
 *     `t` unpinned from r8                    ->  92 of 160
 *     the __Func_8012330 PIN3 (first site)    -> 121 of 160
 *     `y` unpinned from r5                    ->  11 of 160
 *     `sv`/r3 dropped (`*t = saved`)          ->   3 of 160
 *     the __MapActor_SetPos PIN3              ->   2 of 160
 *
 * ========= HOW THE LAST 2 FELL: THE BLOCKER DID NOT EXIST ==========
 *
 * The park closed itself with a CORPUS PROOF that gcc-2.96 as configured here
 * cannot emit the ROM's instruction:
 *
 *     rom    movs r2,#35 / ldr r3,[r0,#0x50] / mov r8,r0     / add r8,r2
 *     ours   movs r2,#35 / ldr r3,[r0,#0x50] / adds r2,r2,r0 / mov r8,r2
 *
 *     `add rHIGH, rN`   ROM disassembly:  1213 in 244 of 1083 .s files
 *                       gcc-generated:       0 in   0 of 3729 .s files
 *
 * *** THE CORPUS TEST WAS MEASURING THE ASSEMBLY TEXT, NOT THE INSTRUCTION. ***
 * Thumb-1's hi-register ADD is a two-operand, destructive instruction, so
 * `add r8, r2` and `add r8, r8, r2` are THE SAME ENCODING (0x4490) -- and gcc
 * always writes the THREE-OPERAND spelling.  Re-measured in batch 316 over this
 * same tree, with the three-operand pattern:
 *
 *     `add rHI, rHI, rN`   gcc-generated .s files:  162 of 4388
 *
 * e.g. `add ip, ip, r2` and `add sl, sl, r1`, in files that are part of a green
 * `make compare`.  The instruction is squarely inside this compiler's output
 * set.  This is exactly the normalisation tools/tryc.py already performs for
 * destructive ops (its DESTRUCTIVE regexp) and the park's corpus grep did not.
 *
 *   *** RULE: NEVER RUN A CORPUS TEST FOR A THUMB DATA-PROCESSING OP ON ITS
 *   *** TWO-OPERAND TEXT.  gcc writes `op rD, rD, rS`; the ROM disassembly
 *   *** writes `op rD, rS`.  Match BOTH spellings or match encodings.
 *
 * WHAT THE RESIDUE ACTUALLY WAS, and it is one line of C.  The park's own RTL
 * reading was right: `t += 0x23` on a hard-register variable expands as
 * `(set (reg 49) (plus (reg 8) 35))` + `(set (reg 8) (reg 49))`, and COMBINE
 * then folds BOTH the temporary and the preceding copy `(set (reg 8) (reg p))`
 * into `(set (reg 8) (plus (reg p) 35))` -- which is no longer an IN-PLACE add,
 * so reload takes the low-register alternative and emits
 * `adds r2,r2,r0 / mov r8,r2`.  The cure is to stop combine deleting the copy:
 *
 *     t = p;
 *     __asm__ ("" : "+r" (t));      <- combine cannot fold across this
 *     t += 0x23;
 *     r = *(unsigned char **)(p + 0x50);
 *
 * The add then stays `(set (reg 8) (plus (reg 8) 35))`, reload materialises 35
 * into a low register (the ROM's `movs r2,#35`) and emits the in-place hi add.
 *
 * *** AND THE PARK HAD TRIED THIS EXACT CONSTRUCT AND MEASURED IT AT 2 -- WITH
 * *** ONE WORD DIFFERENT.  Measured side by side here, same file, same flags:
 *
 *     __asm__          ("" : "+r" (t))      ***  0 of 160 ***
 *     __asm__ volatile ("" : "+r" (t))           2 of 160
 *     __asm__          ("" :: "r"  (t))          2 of 160
 *     no barrier at all                          2 of 160
 *
 * The park's recorded row is `t = p; asm volatile("":"+r"(t)); t += 0x23;  2`.
 * THE `volatile` IS WHAT MADE IT INERT.  A volatile asm is an unknown
 * side-effecting region, which changes what gcc will keep live around it; a
 * plain output asm is only a data barrier, which is all that is wanted here.
 * An INPUT-ONLY `"r"` barrier is also inert, because it does not re-define `t`
 * and so does not stand between the copy and the add at all.
 *
 *   *** RULE: A `"+r"` BARRIER AND AN `asm volatile("+r")` BARRIER ARE
 *   *** DIFFERENT LEVERS.  Where a barrier is meant to stop a FOLD, write the
 *   *** non-volatile form; measure both before recording either as inert.
 *
 * ORDER IS FREE ONCE THE BARRIER IS RIGHT: with the barrier in place, taking
 * the `+ 0x23` before or after the `p + 0x50` load is byte-identical (both 0),
 * and `t = t + 0x23` is byte-identical to `t += 0x23`.  The park's largest
 * recorded lever -- "THE BASE POINTER MUST STILL BE LIVE AT THE ADD", worth
 * 122 positions -- was a consequence of the fold, and is no longer a
 * constraint.  Keep the shipped order anyway; it is the ROM's statement order.
 *
 * ============== THE LEVERS THAT GOT IT FROM 127 TO 2, ALL KEPT ==============
 *
 * 1. THE r8 PIN ON `t` -- 92 of 160 without it (gcc takes r7, prologue
 *    `push {r5,r6,r7,lr}`).  gcc's Thumb call-saved order is r5, r6, r7, r8,
 *    r10 and the ROM skips r7.
 * 2. THE r5 PIN ON `y` -- 11 of 160 without it.
 * 3. NAME THE STORED BYTE IN A LOW REGISTER (`sv` pinned r3) -- 3 of 160
 *    without it.  With the address in r8 and the value in r10 both need a low
 *    register, and reload emits the INPUT reload before the OUTPUT-ADDRESS
 *    reload, so the value grabs r2.  `sv = saved; *t = sv;` makes the value a
 *    real move in source position, leaving the address as the only reload --
 *    it takes r2 and goes first, as the ROM has it.
 * 4. THE `p0`/r0 PINS ON THE __GetFlag / __SetFlag IDS -- 152 of 160 and
 *    +12 bytes without them.
 * 5. THE PIN3 BLOCKS ON __Func_8012330 (121) AND __MapActor_SetPos (2).
 * 6. THE OvlFunc_882_2009a64 CALLS WANT NO PIN AT ALL: plain C is
 *    byte-identical to the descending pin and the ascending one is wrong.
 *    Both __Func_8092b08 pins are inert too.  The greedy minimiser dropped
 *    8 of 20 levers; 12 survive, plus the barrier.
 * 7. THE TEMPLATE NEIGHBOUR'S "NAMED SHIFTED LOCAL" CURE IS INERT HERE.
 *    src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_b.c records
 *    `v = 0xc0; v <<= 9;` as its lever for the five actor-offset stores; all
 *    three measure byte-identical written inline, so inline ships.
 *
 * ALSO NOW DEAD, and recorded so it is not re-derived: the park's "the ROM's
 * `add` says which form to write table does NOT extend to a high destination"
 * finding, and its conclusion that `add r8, r2` is "a RELOAD ARTIFACT rather
 * than a source form at all".  It IS a source form -- the walk form, with the
 * fold blocked.  `-ffixed-r7` remains a dead end (160 encodings and 412 bytes
 * from pin-free C, but a register permutation at 127 differing, and inert on
 * top of the shipping pins), so no Makefile rule is wanted.
 */
extern void OvlFunc_882_20092f0(void);
extern void OvlFunc_882_2009a64(int a, int b);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_809202c(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092b08(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_882_2009154(void)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *r;
    register unsigned char *t __asm__("r8");
    register int p0 __asm__("r0");
    unsigned char *q;
    register int sv __asm__("r3");
    register unsigned int y __asm__("r5");
    int saved, v, w, u, x, k, b;

    p0 = 0x312;
    if (__GetFlag(p0) == 0) {
        __CutsceneStart();
        p0 = 0x832;
        if (__GetFlag(p0) == 0) {
            a = __MapActor_GetActor(0xd);
            p = __MapActor_GetActor(0);
            t = p;
            __asm__ ("" : "+r" (t));
            t += 0x23;
            r = *(unsigned char **)(p + 0x50);
            y = r[9];
            saved = *t;
            { PIN3; q0 = 0x80 << 11; q1 = 0x80 << 11; q2 = 0x80 << 9;
              __Func_8012330(q0, q1, q2); }
            __PlaySound(0x8d);
            __WaitFrames(0x28);
            __PlaySound(0x91);
            __Func_8092b08(0, 3);
            q = __MapActor_GetActor(0) + 0x23;
            b = *q; k = 2; k |= b; *q = k;
            { PIN3; q0 = 0xd; q1 = 0; q2 = 0x2bf0000;
              __MapActor_SetPos(q0, q1, q2); }
            *(int *)(a + 0x30) = 0xc0 << 9;
            *(int *)(a + 0x34) = 0xc0 << 9;
            *(int *)(a + 0xc) += 0xa0 << 15;
            *(int *)(a + 0x3c) = *(int *)(a + 0xc);
            *(int *)(a + 0x44) = 0x80 << 8;
            { PIN3; q0 = 0xd; q1 = 0x40; q2 = 0x2bf;
              __Func_80921c4(q0, q1, q2); }
            __CutsceneWait(0x28);
            __PlaySound(0x121);
            { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
            y <<= 28;
            y >>= 30;
            __Func_8012350();
            __Func_809202c();
            p0 = 0x832;
            __SetFlag(p0);
            __Func_8092b08(0, y);
            q = __MapActor_GetActor(0) + 0x23;
            b = *q; k = 1; k |= b; *q = k;
            sv = saved;
            *t = sv;
        }
        OvlFunc_882_20092f0();
        p0 = 0x312;
        __SetFlag(p0);
        p0 = 0x837;
        if (__GetFlag(p0) != 0) {
            p0 = 0x841;
            if (__GetFlag(p0) == 0) {
                p0 = 0xc3; p0 <<= 2;
                if (__GetFlag(p0) == 0) {
                    p = __MapActor_GetActor(0);
                    if (*(int *)(p + 0x10) <= 0x2b4ffff) {
                        OvlFunc_882_2009a64(0x3e, 0x29d);
                        { PIN3; q0 = 0; q1 = 0x1b; q2 = 0x273;
                          __Func_80921c4(q0, q1, q2); }
                    } else {
                        OvlFunc_882_2009a64(0x4b, 0x2cb);
                        { PIN3; q0 = 0; q1 = 0x43; q2 = 0x2f5;
                          __Func_80921c4(q0, q1, q2); }
                    }
                    p0 = 0xc3; p0 <<= 2;
                    __SetFlag(p0);
                }
            }
        }
        __CutsceneEnd();
    }
}
