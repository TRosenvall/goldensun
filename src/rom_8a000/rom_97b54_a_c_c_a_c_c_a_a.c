/* Func_8099070 -- 0x08099070.  EXACT.
 * ref: asm/rom_8a000/rom_97b54_a_c_c_a_c_c_a_a.s  (ONE function, no data
 *      section -- grep -c thumb_func_start = 1; converts the WHOLE FILE,
 *      no split needed).
 * install: src/rom_8a000/rom_97b54_a_c_c_a_c_c_a_a.c   (path is free)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_97b54_a_c_c_a_c_c_a_a.c \
 *     asm/rom_8a000/rom_97b54_a_c_c_a_c_c_a_a.s --whole
 *
 * batch 322, brief D, target 3.  PIN-FREE.  No shims, no .equ, no volatile,
 * no do{}while(0), default flags.  datacheck/split_s: not applicable, the
 * reference holds one function and no data.
 *
 * THE PARK MEASURED 7 of 44 AND ITS FIGURE WAS RIGHT.  Its diagnosis of the
 * CAUSE was wrong, and the wrong diagnosis is what kept it parked:
 *
 *   park: "ours is in r0 -- which is also where the second shift's result
 *          must go -- so the shift becomes destructive and moves after the
 *          add".
 *
 * REFUTED TWO WAYS.  (a) `lsl r0, r0, #17` is NOT forced after the add:
 * `lsl r3, r0, #11 / lsl r0, r0, #17 / add r1, r1, r3` is a legal order gcc
 * was free to pick, so destructiveness constrains nothing here.  (b) The
 * park's implied instrument says so too -- `register int s __asm__("r2")`
 * measures 30 of 44 at 46 instructions with RELOC and COUNT both flagged.
 * Putting s in r2 BY FORCE does not produce the ROM; so "the whole residue
 * follows from that one choice" is not a mechanism you can act on.
 *
 * THE ACTUAL MECHANISM -- A MANUFACTURED HARD-REGISTER CONFLICT.  Read out of
 * the .17.lreg/.18.greg dumps and then confirmed against the compiler:
 *
 *   `s` is global allocno `reg/v:SI 35` (one of only three; .18.greg says
 *   ";; 3 regs to allocate: 32 47 35", so global.c decides it and
 *   global.c:allocno_compare -- the formula WITH floor_log2 -- is the one that
 *   applies).  The base's greg prints
 *
 *       ;; 35 preferences: 0
 *
 *   and global-alloc honours it.  That preference is set by
 *   global.c:set_preference: `(set (reg 52) (ashift (reg 35) 17))` has an
 *   expression source, so set_preference takes XEXP(src,0) = reg 35 with
 *   copy=0, and reg 52 is the pseudo local-alloc already renumbered to r0 for
 *   the first argument.  So r0 reaches `s` THROUGH THE ARGUMENT, and the only
 *   `s` use that is not an argument fill cannot outvote it.
 *
 *   In the base's RTL order the argument shift is LAST: insn 63 `s << 11`,
 *   insn 65 the ang add, insn 68 `s << 17` carrying `REG_DEAD (reg 35)`.
 *   An input that dies AT the insn setting an output does not conflict with
 *   that output, so reg 35 never conflicts with r0 and takes it.
 *
 *   NAMING THE ARGUMENT SHIFT AND ASSIGNING IT FIRST (`d = s << 17;` BEFORE
 *   the ang statement) moves that insn ahead of `s`'s other use.  r0 is then
 *   live ACROSS `s << 11`, reg 35 genuinely conflicts with r0, the preference
 *   is unavailable, and `s` falls to r2 -- which is the ROM's register.  The
 *   reload scratch of the `ldrsh` then finds r3/r2/r1/r0 all taken and takes
 *   r4 (REG_ALLOC_ORDER is {3,2,1,0,12,14,4,...}), which is the ROM's
 *   `mov r4, #0`; and with s in r2 the second shift is non-destructive, so
 *   sched2 puts both shifts before the add.  ALL SEVEN go at once.
 *
 *   THIS IS A SEPARATE LEVER FROM THE ALIAS-SET-0 ONE and it is cheap to aim:
 *   where a residue is one allocno taking an ARGUMENT register, name the
 *   argument expression and assign it BEFORE the last other use of its input.
 *   It manufactures the conflict that deletes the preference.  The converse
 *   placement is inert, which is the whole point of crossing:
 *     decl d, nothing else                                7  (inert)
 *     decl d + `d = s << 17` immediately before the call   4
 *     decl d + `d = s << 17` as the block's FIRST stmt     7  (inert)
 *     decl d + `d = s << 17` before the ang statement      0
 *     decl d + d-first-stmt + the call reading d           0
 *   Two of those three placements are EXACTLY INERT and the third is the
 *   landing.  Tested one at a time this lever reads dead.
 *
 * MEASURED AND INERT at 7 (a dividend, not a failure -- all are free code
 * quality): `short s`; `unsigned int t`; a union-typed 0x6c store (alias set
 * 0, the batch-321 lever) with and without the union declared; a named
 * pointer for the a[2..4] stores.  The alias lever is NOT in this function:
 * nothing here is a two-memory-access or two-ready-insn sched2 tie.
 * MEASURED AND WORSE: hoisting the ang statement above the v[] loads, 17;
 * `register int s __asm__("r2")`, 30 at 46 insns.
 *
 * WHAT THE PARK GOT RIGHT, kept verbatim: the block layout (`if (s != 0)
 * {...} else { a[0x6c] = ...; }` rather than an early return, worth 29 -> 7),
 * two variables for `t` and `s` rather than one (merging reads 13), and the
 * `+ (unsigned int)0` on the 0x66 read, which is what makes the halfword load
 * the register-offset `ldrsh r1, [r3, rN]` the ROM has.
 *
 * ALSO CORRECTED: the park says its twin Func_80990cc is "in the same .s, and
 * the .s holds only these two".  It is not -- this .s holds ONE function.
 * rom_97b54_a_c_c_a_c_c_a_b.c is already landed and holds no func_start.
 */
extern void vec3_translate(int a, int b, int *v);
extern void Func_8099040(void);

void Func_8099070(int *a)
{
    int v[3];
    unsigned short *c;
    int t;
    int s;
    int ang;
    int d;

    if (a == 0)
        return;
    c = (unsigned short *)((char *)a + 0x64);
    t = *c - 1;
    *c = t;
    s = (short)t;
    if (s != 0) {
        v[0] = a[0xe];
        v[1] = a[0xf];
        v[2] = a[0x10];
        d = s << 17;
        ang = *(short *)((char *)a + 0x66 + (unsigned int)0) + (s << 11);
        vec3_translate(d, ang, v);
        a[2] = v[0];
        a[3] = v[1];
        a[4] = v[2];
    } else {
        *(void **)((char *)a + 0x6c) = (void *)Func_8099040;
    }
}
