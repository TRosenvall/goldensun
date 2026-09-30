/* OvlFunc_897_20090c4 -- 0x020090c4.  NON-MATCHING, 268 of 295 encodings differ.
 * UNATTEMPTED before batch 303 (recovered by the census address-suffix fix; no
 * park in the tree is about this function).  Reference
 * asm/overlays/rom_791794/ovl_30_c_c_a_c_a_a_a.s.
 *
 * 268 IS SATURATED AND RANKS NOTHING.  SIZE is NOT exact (ref 664 bytes, ours
 * 660) and the instruction COUNT is NOT exact (ref 295, ours 293 -- TWO SHORT),
 * so objcmp compares index-by-index past a deletion.  Every ranking below is
 * tools/aligncmp.py's figure instead:
 *
 *     aligned-equal 215 (72.9% of ref), 80 differing/ins/del in 44 hunks
 *
 * RELOCATIONS: the SYMBOL SEQUENCE IS EFFECTIVELY IDENTICAL -- all 44
 * relocations, same symbols in the same order, ON THE FIRST CANDIDATE, with the
 * single exception that we emit `__udivsi3` / `__umodsi3` where the ROM's
 * hand-written .s writes `_udivsi3_RAM` / `_umodsi3_RAM`.  THAT IS NOT A
 * RESIDUE AND NEEDS NO WORK: overlays/rom_791794/overlay.ld:71-72 ALREADY
 * carries `__udivsi3 = _udivsi3_RAM;` and `__umodsi3 = _umodsi3_RAM;` for
 * exactly this reason (gcc-2.96 has no flag to rename the helpers, and the
 * RAM-resident copies are different functions at different addresses).  objcmp's
 * "RELOCATIONS differ" line here is that aliasing plus the offset shifts caused
 * by being 4 bytes short; read them separately.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_791794/20090c4.c \
 *       asm/overlays/rom_791794/ovl_30_c_c_a_c_a_a_a.s --func OvlFunc_897_20090c4
 *
 * SPLIT SHAPE: NO SPLIT AT ALL.  tools/split_s.py --dry-run says
 * "asm/overlays/rom_791794/ovl_30_c_c_a_c_a_a_a.s holds only
 * OvlFunc_897_20090c4 and no data; convert it directly, no split needed", and
 * tools/datacheck.py is silent.  The landed file is therefore
 * src/overlays/rom_791794/ovl_30_c_c_a_c_a_a_a.c and NOTHING needs a new
 * `.global`.  Every symbol it names already exists: iwram_3001ec4 and
 * iwram_3001e40 in wram.sym, `.L3ac0` / `.L3b00` / `.L3b68` as overlay data
 * labels, gScript_897__0200ba00 already `.global` at
 * asm/overlays/rom_791794/ovl_30_c_c_c_c_c_dat.s:14.
 *
 * NOT PIN-FREE -- A LANDING NEEDS A fakematch.txt ROW.  tools/shimcount.py
 * reports "register pins : 2" and exits 1.  Both pins are inside the `call_via`
 * helper, which is the TREE'S OWN established form for the ROM's `.call_via r9`
 * indirect multiply and is copied verbatim from the LANDED
 * src/rom_c9000/rom_e3958_c_c_c_a.c -- that file is already booked as
 * `Func_80e3994  src/rom_c9000/rom_e3958_c_c_c_a.c` at fakematch.txt:543, so
 * the row to add on landing is
 *
 *     OvlFunc_897_20090c4  src/overlays/rom_791794/ovl_30_c_c_a_c_a_a_a.c
 *
 * There are NO pins outside the helper, no "+r" barriers, no volatile, no
 * do{}while(0) and NO per-file flag override (see FLAGS).
 *
 * ================================================================
 * THE LEVERS THAT PAID, IN THE ORDER THEY PAID
 * ================================================================
 *
 * The first candidate was already 204 aligned (69.2%) with the relocation
 * symbol sequence exact and only 4 instructions / 8 bytes short, so the program
 * shape was right before any allocation work.
 *
 * (1) THE `s[9]` READ-MODIFY-WRITE MUST GO THROUGH AN `int` LOCAL, worth
 *     204 -> 215 aligned (69.2% -> 72.9%) AND +2 instructions -- half the whole
 *     length deficit.  THE ONLY LEVER THIS FUNCTION HAS SO FAR.
 *
 *     The ROM clears two bits of a byte through a FULL-WIDTH mask:
 *
 *         movs r0, #0xd
 *         ldrb r3, [r1, #9]
 *         negs r0, r0          @ -13 == ~0xc, THIRTY-TWO bits
 *         adds r2, r0, #0
 *         ands r3, r2
 *         movs r2, #4
 *         orrs r3, r2
 *         strb r3, [r1, #9]
 *
 *     Written as `s[9] = (s[9] & ~0xc) | 4;` the C integer promotion is undone
 *     again by gcc's narrowing: it knows only 8 bits reach the `strb`, folds
 *     ~0xc to the byte 0xf3, and emits ONE `movs r3, #243` where the ROM has
 *     `movs / negs / adds`.  Reading the byte into an `int` first blocks that:
 *
 *         f = s[9];
 *         f = (f & ~0xc) | 4;
 *         s[9] = f;
 *
 *     MEASURED EQUIVALENT: a named `int m = ~0xc;` mask local with the
 *     expression left inline (215 as well, same output class).  MEASURED INERT:
 *     spelling the mask `-13` instead of `~0xc` (204, identical to the
 *     unlevered form -- the spelling of the constant cannot reach it, only the
 *     WIDTH of the variable the expression flows through).
 *
 * ALSO LOAD-BEARING, found on the first candidate and never moved:
 *   - `unsigned char i` for BOTH loop counters, so `i++` is the ROM's
 *     `add r3,#1 / lsl r3,#24 / lsr r3,#24` and the exit test is an unsigned
 *     `bls #0xf`.  An `int` counter loses the zero-extend pair per loop.
 *   - `q = L3b00 / 10;` with `L3b00` declared `unsigned int`, which is what
 *     picks the UNSIGNED helper `__udivsi3` (aliased to `_udivsi3_RAM`); the
 *     same for `__Random() % 0xffff` and `__umodsi3`.  A signed type here
 *     scores the whole function against the wrong two relocations.
 *   - `__Func_8012330(q << 16, q << 16, 0x80 << 9)` -- the first two arguments
 *     are the SAME expression, which is why the ROM does `lsl r1, r0, #16`
 *     followed by `mov r0, r1` rather than shifting twice.
 *   - `if (*(int *)(a + 0x38) == (0x80 << 24) && *(int *)(a + 0x40) == (0x80 << 24))`
 *     as TWO comparisons against the same literal: gcc reuses the r3 that holds
 *     the first field for the second compare, exactly as the ROM does, and then
 *     reuses the SECOND field's register for the three `0x80 << 24` stores in
 *     the `== 0x13` arm.  Writing those stores as a fresh literal is the same
 *     output; writing the guard as one variable is not.
 *   - `L3ac0[i]` RE-READ for each of the `== 2`, `== 0x13` and `== 0x14` tests
 *     rather than held in a local -- the ROM reloads it after
 *     __Actor_SetAnim, which a local would not do.
 *   - `short ang = __Random() % 0xffff;` (SIGNED, so `lsl #16 / asr #16`) and
 *     then `u = (unsigned short)ang` for the four trig calls (`lsl #16 /
 *     lsr #16`).  The sign-extend-then-zero-extend round trip is in the ROM and
 *     both halves are needed.
 *   - the `L3ac0[i] == 0` arm ends in `break`, not a flag: the ROM's
 *     `b .L1314` leaves the loop AND the function in one jump.
 *   - `((__Random() << 8) >> 16) << 16) + (0x80 << 17)` for the first two
 *     multiply arguments and `((__Random() & 0x3f) << 16) + (0x80 << 12)` for
 *     the last two, with `__Random()` UNSIGNED so the middle shift is `lsr`.
 *   - `call_via(mulfn, ...)` with `mulfn` a NAMED local assigned
 *     `Func_8000888` before the loop -- the ROM parks it in r9 for all four
 *     sites, which is what the helper's register form reproduces.
 *
 * ================================================================
 * THE BLOCKER, ATTRIBUTED TO A PASS: cse2 (rerun-cse-after-loop's pass), which
 * SHARES ONE -1 THE ROM BUILDS TWICE
 * ================================================================
 *
 * One of the two remaining missing instructions is in the `q == 0` arm.  The
 * ROM materialises -1 SEPARATELY for each of the two arguments:
 *
 *         movs r0, #1
 *         ...
 *         movs r1, #1
 *         movs r3, #1
 *         str  r3, [r2, #0]        @ *(int *)(base + 0x40c) = 1
 *         negs r0, r0              @ arg 1 = -1
 *         negs r1, r1              @ arg 2 = -1
 *
 * -- THREE `movs #1` and TWO `negs`.  We emit `movs r1, #1 / negs r1, r1 /
 * adds r0, r1, #0`: gcc builds -1 once and COPIES it, one instruction short.
 *
 * WHAT RULES OUT THE ALTERNATIVES.
 *   - NOT a flag, and this is the finding that matters, because this function
 *     looks like the pool-constant CSE class and IS NOT.  It reads save bit
 *     0x246 exactly ONCE, so the documented precondition (an id used two or
 *     three times with one use dominating another) is absent, and the sharing
 *     here is of a plain -1, not of a pooled id.  MEASURED, all identical to
 *     the default at 204 aligned: -fno-rerun-cse-after-loop (the CSE_CFLAGS
 *     rule), -fno-gcse, -fno-cse-follow-jumps, and there is no
 *     -fno-crossjumping in gcc-2.96.  -fno-cse-skip-blocks is far WORSE (151
 *     aligned, 287 instructions, 58 hunks) and -fno-expensive-optimizations is
 *     worse (190).  So NO Makefile row would help and none should be written.
 *   - NOT the source so far: `~0` and `0xffffffff` are the same tree as `-1`
 *     after fold, and a named `int` holding -1 shares MORE, not less.  Untested,
 *     therefore not disproved: nothing yet forces the two arguments onto
 *     distinct pseudos without a shim.
 *
 * The second missing instruction is a register COPY the ROM has and we do not:
 * `adds r1, r5, #0` immediately before __Actor_TravelTo, where the ROM moves
 * the accumulated x out of r5 into the argument register and we compute it into
 * r1 directly.  That is reload/local-alloc, not a pass that a flag reaches.
 *
 * THE REST OF THE RESIDUE IS ONE REGISTER ROTATION, repeated.  The ROM holds
 * the actor pointer in r5, the scaled index in r6 and `.L3ac0` in sl; we use
 * r7, r5 and sl.  Operand POSITIONS match everywhere (`ldr r3, [rTable,
 * rIndex]`, `str r2, [rActor, #k]`, and the six-store zeroing block is
 * instruction-for-instruction identical but for the base register), so this is
 * purely which hard register each pseudo received, with r5, r6 and r7 all free
 * at every one of those points.  Two smaller scheduling ties ride along: the
 * `lsl r2, #9` of `0x80 << 9` lands one slot early, and `movs r2, #0x3f` lands
 * two slots early at the third multiply.
 */
extern int Func_8000888(int a, int b);

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12"
    );
    return _a;
}

extern unsigned char *iwram_3001ec4;
extern unsigned int iwram_3001e40;
extern unsigned int L3b00 __asm__(".L3b00");
extern int L3ac0[] __asm__(".L3ac0");
extern int L3b68 __asm__(".L3b68");
extern unsigned char gScript_897__0200ba00[];

extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetScript(unsigned char *a, void *s);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Func_80929d8(unsigned char *a, int n);
extern void __Func_8012330(int a, int b, int c);
extern int __GetFlag(int id);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_897_20090c4(void)
{
    unsigned char *base;
    unsigned char *a;
    unsigned char *s;
    int (*mulfn)(int, int);
    short ang;
    unsigned int u;
    unsigned int q;
    int x;
    int y;
    unsigned char i;
    int f;

    base = iwram_3001ec4;
    q = L3b00 / 10;
    if (q != 0) {
        *(int *)(base + 0x40c) = 0;
        __Func_8012330(q << 16, q << 16, 0x80 << 9);
    } else {
        *(int *)(base + 0x40c) = 1;
        __Func_8012330(-1, -1, 0xe666);
    }
    if (L3b00 != 0)
        L3b00 = L3b00 - 3;
    i = 0;
    do {
        if (L3ac0[i] != 0) {
            a = __MapActor_GetActor(i + 0x10);
            if (*(int *)(a + 0x38) == (0x80 << 24) &&
                *(int *)(a + 0x40) == (0x80 << 24)) {
                L3ac0[i] = L3ac0[i] + 1;
                if (L3ac0[i] == 2)
                    __Actor_SetAnim(a, 3);
                if (L3ac0[i] == 0x13) {
                    *(int *)(a + 8) = 0;
                    *(int *)(a + 0xc) = 0;
                    *(int *)(a + 0x10) = 0;
                    *(int *)(a + 0x24) = 0;
                    *(int *)(a + 0x28) = 0;
                    *(int *)(a + 0x2c) = 0;
                    *(int *)(a + 0x38) = 0x80 << 24;
                    *(int *)(a + 0x3c) = 0x80 << 24;
                    *(int *)(a + 0x40) = 0x80 << 24;
                    __Func_80929d8(a, 0xf);
                } else if (L3ac0[i] == 0x14) {
                    L3ac0[i] = 0;
                }
            }
        }
        i++;
    } while (i <= 0xf);
    if (L3b68 != 0x3e7 && (iwram_3001e40 & L3b68) == 0) {
        mulfn = Func_8000888;
        i = 0;
        do {
            ang = __Random() % 0xffff;
            a = __MapActor_GetActor(i + 0x10);
            if (L3ac0[i] == 0) {
                if (__GetFlag(0x246) == 0)
                    __PlaySound(0xf6);
                L3ac0[i] = 1;
                a[0x55] = 0;
                *(int *)(a + 0x30) = 0x80 << 12;
                *(int *)(a + 0x34) = 0x80 << 9;
                __Actor_SetSpriteFlags(a, 0);
                s = *(unsigned char **)(a + 0x50);
                f = s[9];
                f = (f & ~0xc) | 4;
                s[9] = f;
                __Actor_SetAnim(a, 2);
                __Actor_SetScript(a, gScript_897__0200ba00);
                u = (unsigned short)ang;
                *(int *)(a + 8) = call_via(mulfn, __cos(u),
                        (((__Random() << 8) >> 16) << 16) + (0x80 << 17)) + 0x1450000;
                *(int *)(a + 0xc) = 0;
                *(int *)(a + 0x10) = call_via(mulfn, __sin(u),
                        (((__Random() << 8) >> 16) << 16) + (0x80 << 17)) + (0x97 << 17);
                x = call_via(mulfn, __cos(u),
                        ((__Random() & 0x3f) << 16) + (0x80 << 12));
                y = call_via(mulfn, __sin(u),
                        ((__Random() & 0x3f) << 16) + (0x80 << 12));
                __Actor_TravelTo(a, x + 0x1450000, 0, y + (0x8f << 17));
                __Func_80929d8(a, 0);
                L3b00 = 0x1e;
                break;
            }
            i++;
        } while (i <= 0xf);
    }
}
