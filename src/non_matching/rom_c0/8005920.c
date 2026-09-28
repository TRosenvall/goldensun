/* SomethingSaveHeader -- NON-MATCHING at -O2: 2 of 157 differing, and that is the
 * NON-MATCHING, 2 encodings of 156.  A TRUE DISTANCE -- 156 = 156 encodings, no SIZE line, every relocation identical.  The residue is
 * two adjacent independent instructions swapped.  No shims.
 *
 * Blocked by dma.h's helpers being `static inline` rather than macros: integrate.c:743/748 expands
 * an inline argument with EXPAND_SUM and then UNCONDITIONALLY forces it into a register before any
 * body insn, so the dst `add` always precedes the src `mov`; both carry INSN_PRIORITY 4, so
 * rank_for_schedule falls to LUID and the ROM's order is unreachable.  Rewriting DMA3_CLEAR's body
 * as a MACRO removes this site's residue completely -- an OWNER DECISION about include/dma.h, whose
 * own comment already guesses "maybe they were macros"; this is the first measurement bearing on
 * it, and it must be screened tree-wide before any edit.  REFUTED: const-qualifying the parameter
 * to fail integrate.c:748's `! TREE_READONLY (formal)` test does NOT skip the pre-copy.
 *
 * The reference .s comment is stale: it calls this StartMusicTrack, "begins a music track", at
 * "160 lines"; it writes a save header and is 147 instructions.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c0/8005920.c \
 *     asm/rom_c0/rom_56cc_a_a_c_a_a.s --func SomethingSaveHeader
 * WHOLE residue -- same size, 156 encodings against 156, and every relocation
 * identical. objcmp: "ENCODINGS differ in 2 place(s) (ref 156, ours 156), first at
 * index 13".
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c0/5920.c asm/rom_c0/rom_56cc_a_a_c_a_a.s --func SomethingSaveHeader
 *
 * THE RESIDUE IS TWO ADJACENT INSTRUCTIONS IN THE WRONG ORDER:
 *     rom   mov r0, sp   /  add r1, r10
 *     ours  add r1, r10  /  mov r0, sp
 * `mov r0, sp` is DMA3_CLEAR/DMA3_FILL's `_src = &value`, the first statement of
 * the inline body. `add r1, r10` computes the `dst` argument, `base + 0x40`.
 *
 * IT IS A sched2 TIE THAT THE SOURCE CANNOT REACH, and a `-fsched-verbose=6` dump
 * gives the whole argument. Both insns have INSN_PRIORITY 4 -- the dump's own
 * table prints `22 ... prio 4` for the add and `28 ... prio 4` for the `mov`:
 *
 *   prio(add)   = prio(asm) + 1                     (the asm is a true dep on r1)
 *   prio(mov)   = prio(str) + 1 = prio(asm) + 0 + 1 (the store's dep on the
 *                 volatile asm is an ANTI dep, which insn_cost charges 0)
 *
 * so `rank_for_schedule` falls through to its last test, `INSN_LUID (tmp) -
 * INSN_LUID (tmp2)`, and the lower LUID is issued first. The add ALWAYS has the
 * lower LUID, because the dst is an INLINE FUNCTION ARGUMENT: integrate.c:743
 * expands it with EXPAND_SUM and integrate.c:748 then forces it into a register
 * with `copy_to_mode_reg` before a single insn of the body is copied. So the add
 * is emitted ahead of `_src = &value` no matter how the call site is written.
 *
 * NINETEEN SPELLINGS MEASURED, and none of them moves it (all at 157
 * instructions, the ROM's own count):
 *
 *   DMA3_CLEAR(base + 0x40, 0x1000)                              4 differing
 *   DMA3_CLEAR(&base[0x40], ...) / (void *)(base + 0x40) / ...   4
 *   DMA3_FILL(base + 0x40, 0, 0x1000)                            2
 *   DMA3_FILL with the fill value named (`z = 0;` first)         2  <- this file
 *   DMA3_FILL(&base[0x40], z, 0x1000)                            2
 *   the dst named into a local, assigned before the call         2
 *   the same with the value assigned first                       3
 *   `z = 0` hoisted above `base = iwram_3001f1c`                 4
 *   size spelled 0x400 * 4 / 0x400 << 2 / (0x10 << 2) offset     2 / 2 / 4
 *
 * THE CAUSE IS THAT dma.h's HELPERS ARE static inline FUNCTIONS AND NOT MACROS,
 * and this is the first measurement in the tree that bears on dma.h's own
 * standing comment ("there must be a way to unify those, maybe they were macros
 * instead of inline functions"). Rewriting DMA3_CLEAR's body as a macro in this
 * file -- so `(unsigned)(dst)` is substituted textually at `_dst = ...`, INSIDE
 * the body and after `_src = &value` -- removes this site's residue COMPLETELY:
 * the `mov r0, sp / add r1, r10 / str r3, [r0]` triple then matches the ROM
 * exactly. The probe is scratch_elev/b294/A/t2_probe2.c.
 *
 * It is NOT a net win as a probe, because switching that one call to a macro
 * perturbs the entry block's scheduling elsewhere (7 differing instead of 2 --
 * a permutation of `mov r10,r3 / mov r5,r1 / mov r1,#0x40 / mov r8,r0` and one
 * `ldr r1, =0x40000d4`). So this is EVIDENCE, not a fix: the owner's decision is
 * whether dma.h's DMA3_CLEAR/DMA3_FILL/DMA3_COPY should become macros, which
 * would change every DMA site in the tree at once and has to be screened as a
 * whole rather than per function. I did not touch include/.
 *
 * ALSO REFUTED, and recorded so nobody repeats it: const-qualifying the
 * parameter (`void *const dst`) to satisfy integrate.c:748's
 * `! TREE_READONLY (formal)` test does NOT skip the pre-copy -- a local static
 * inline with that signature scores exactly the same 2 (probe:
 * scratch_elev/b294/A/t2_probe.c). Whatever sets TREE_READONLY on a PARM_DECL,
 * that spelling is not it.
 *
 * FOUR LEVERS GOT IT FROM 47 TO 2, all measured (tryc --align, of 157):
 *
 *  1. A NAMED POINTER TO THE 16-BYTE HEADER BUFFER: 47 -> 28. The ROM computes
 *     `add r5, sp, #4` once and reaches every field through it; subscripting the
 *     array directly makes gcc build `add r2, sp, #0x12` for the +0xa field from
 *     `sp` instead. But the pointer must be ASSIGNED AFTER the DMA that fills the
 *     buffer, not at its declaration: assigned at the declaration, cse commons it
 *     with the DMA's own destination and the ROM's `add r1, sp, #4` there becomes
 *     `mov r1, r5`.
 *
 *  2. THE HEADER BUFFER MUST BE DECLARED IN AN INNER BLOCK. The ROM's frame is
 *     `sub sp, #0x14` with the DMA clear's zero word at sp+0 and the 16-byte
 *     buffer at sp+4. Stack slots are assigned in the order the declarations are
 *     EXPANDED, and the inlined helper's `u32 value` is expanded at the call --
 *     so a function-scope buffer takes sp+0 and the zero word sp+0x10. Declaring
 *     the buffer in a block that opens after the clear reverses that and gives
 *     the ROM's offsets.
 *
 *  3. NAMING THE TWO COMPUTED INDICES of the final three stores: 28 -> 4, and it
 *     is what fixes the instruction COUNT (156 -> 157). The ROM indexes ONE base
 *     with a computed index -- `mov r3, r6 / add r3, #0x10 / strb r1, [r2, r3]`
 *     -- where `base[0x10 + b]` written inline makes gcc reassociate to
 *     `(base + b) + 0x10` and fold the 0x10 into the store's immediate, one
 *     instruction shorter. `k1 = b + 0x10; base[k1] = id;` keeps the index whole.
 *     TWO SEPARATE index locals are needed: reusing one for both stores scores 21.
 *
 *  4. `int one = 1; *(unsigned short *)(h + 0xa) = one;` -- the HImode-literal
 *     lever, and this bank is the two-functions-opposite-answers case the docs
 *     describe. Here the ROM has `mov r3, #1 / strh`, so the int carrier is
 *     required; in Func_80056cc, nine instructions away in the same file's
 *     neighbour, the ROM has `ldr r3, =0` for the same kind of store and the bare
 *     literal is required. Same bank, same author, opposite answers.
 *
 * TWO REFERENCE-COMMENT CORRECTIONS, both in this .s's own banner (batch 293
 * found two more of these):
 *   - it names the function "StartMusicTrack" and describes it as beginning a
 *     music track through Func_5810/5868/5b24/5b64. It is save-media work: it
 *     DMA-clears base+0x40, copies 0xff0 bytes of caller data to base+0x50,
 *     stamps an 8-byte header from .L79b8 with the slot id, a checksum-ish word
 *     and a wrapping counter, and writes it back.
 *   - it says "160 lines"; the function is 147 instructions.
 *
 * ONE PROTOTYPE NOTE for whoever lands this: the ROM sets r0 before BOTH
 * `bl Func_8005b24` and `bl Func_8005810`, so at this call site they take an
 * argument -- but src/rom_c0/rom_56cc_a_a_a_b.c defines `int Func_8005810(void)`
 * and src/rom_c0/rom_56cc_a_a_c_a_b.c declares `Func_8005b24(void)`. That is
 * consistent, not a conflict: pre-prototype C calls them with an argument the
 * callee ignores, and each TU carries its own extern. Declaring them `(void)`
 * here drops the two `mov r0, r8` and loses the match.
 *
 * SHIMS: none. No `register ... __asm__` pins and no `__asm__(".equ ...)` lines.
 * The one `__asm__` is the asm-label on the `.L79b8` declaration, the tree's
 * standard spelling for a dot-label symbol, and `.L79b8` is already `.global` in
 * asm/rom_c0/rom_56cc_c_c_b.s so the reference needs no new export.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *iwram_3001f1c;
extern const unsigned char _TBL_79b8[] __asm__(".L79b8");
extern unsigned int Func_8005b24(int id);
extern unsigned int Func_8005810(int id);
extern int Func_8005ae0(void);
extern int Func_8005c2c(int id);
extern int Func_8005868(unsigned int n);
extern int Func_8005b64(unsigned int n);

int SomethingSaveHeader(int id, const void *src)
{
    unsigned char *base;
    unsigned int a;
    unsigned int b;
    unsigned int z;
    unsigned int k1;
    unsigned int k2;
    vu32 *dma;

    base = iwram_3001f1c;
    z = 0;
    DMA3_FILL(base + 0x40, z, 0x1000);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    a = Func_8005b24(id);
    b = Func_8005810(id);
    if (b > 0xf)
        return 1;
    DMA3_COPY(src, base + 0x50, 0xff0);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    {
        unsigned char hdr[16];
        unsigned char *h;

        DMA3_COPY(_TBL_79b8, hdr, 8);
        dma = (vu32 *)&REG_DMA3SAD;
        while (dma[2] & 0x80000000)
            ;
        h = hdr;
        h[7] = id;
        *(unsigned short *)(h + 8) = Func_8005ae0();
        *(unsigned short *)(h + 0xa) = Func_8005c2c(id) + 1;
        DMA3_COPY(h, base + 0x40, 0x10);
        dma = (vu32 *)&REG_DMA3SAD;
        while (dma[2] & 0x80000000)
            ;
        if (Func_8005868(b) != 0)
            return 1;
        if (a <= 0xf && Func_8005b64(a) != 0)
            return 1;
        if (*(unsigned short *)(h + 0xa) > 0xfde8) {
            int one = 1;
            *(unsigned short *)(h + 0xa) = one;
            DMA3_COPY(h, base + 0x40, 0x10);
            dma = (vu32 *)&REG_DMA3SAD;
            while (dma[2] & 0x80000000)
                ;
            if (Func_8005868(a) != 0)
                return 1;
            if (Func_8005b64(b) != 0)
                return 1;
            b = a;
        }
        base[b] = 1;
        k1 = b + 0x10;
        base[k1] = id;
        k2 = b * 2 + 0x20;
        *(unsigned short *)(base + k2) = *(unsigned short *)(h + 0xa);
    }
    return 0;
}
