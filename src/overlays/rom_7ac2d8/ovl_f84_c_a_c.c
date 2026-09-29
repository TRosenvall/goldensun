// fakematch
/* OvlFunc_924_20099b8 -- EXACT under the tree's PRODUCTION flags, no Makefile row.
 * 544 bytes, 229 encodings and 39 relocations identical; --whole also OK.
 *
 * Supersedes the park's "61 of 229 under production flags, 4 of 229 under a proposed
 * -fno-rerun-cse-after-loop rule".  ONE shim closes BOTH defects: a hard-register pin
 * on the FIRST (dominating) use of the flag id 0x307.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/overlays/rom_7ac2d8/ovl_f84_c_a_c.c \
 *     asm/overlays/rom_7ac2d8/ovl_f84_c_a_c.s --whole
 *
 * SHIMS: one `register ... __asm__` declaration (`register int k __asm__("r0")`).
 * Zero `__asm__(".equ ...)` lines.  Needs a fakematch.txt row:
 *   OvlFunc_924_20099b8  src/overlays/rom_7ac2d8/ovl_f84_c_a_c.c
 *
 * THE GUARD/SET CSE, same mechanism as rom_7e7574's pair.  Dump counts of
 * `(set (reg) (const_int 775))` in the insn stream: .00.rtl 2, .03.cse 1 (CSE1 ALREADY
 * COMMONS), .07.gcse 2 (gcse's cprop restores the ROM's second literal), .08.loop 2,
 * .09.cse2 1.  So the flag works by letting cprop's result stand; path extension is
 * not involved (the park's own cross-check that -fno-cse-follow-jumps and
 * -fno-cse-skip-blocks are inert is correct, and this is why).
 *
 * AND THE movmem8b OPERAND TIE IS THE SAME SHIM, not a separate defect.  The park
 * recorded a residue of 4 -- `mov r3, sp / add r2, sp, #0x18` against ours with r2/r3
 * exchanged, at the THIRD of four block moves -- and called it a local-alloc qty_compare
 * tie that the flag's fix causes as a side effect, with nine source spellings all
 * measuring 4.  It is neither unreachable nor a side effect of the flag: it is the
 * presence of an extra block-local PSEUDO in that basic block.
 *   qty_compare_1 ranks by QTY_CMP_PRI = floor_log2(n_refs)*n_refs*size/(death-birth)
 *   and tiebreaks by quantity number.  thumb_expand_movstrqi (arm.c) creates `out`
 *   first and `in` second, so out lives one insn longer, gets the LOWER priority, and
 *   `in` takes REG_ALLOC_ORDER's first slot (r3) -- which is ours at all four sites.
 *   Adding or removing a pseudo in the block moves the priorities and flips the pair.
 * Measured, all under production flags:
 *   register pin on the GetFlag id, no barrier          EXACT      (this file)
 *   pin + __asm__ __volatile__ ("" : : "r" (k))         EXACT
 *   int k; __asm__ ("" : "+r" (k))  after the move      4 of 229   (the tie remains)
 *   ... with "+l"                                       4 of 229
 *   ... volatile form                                   4 of 229
 *   the same barrier HOISTED above the block move       65 of 229  (worse)
 *   the barrier on __SetFlag (the LATER use) instead    61 of 229  (inert -- the
 *       barrier must go on the DOMINATING use; breaking the later one leaves the
 *       earlier pseudo available for cse2 to substitute into it)
 * So the answer to "is the operand tie reachable from source" is YES, through the
 * pseudo count: the pin removes the pseudo entirely and the ROM's out=r3 falls out.
 *
 * The park's two NEW findings are unaffected and still correct: the whole 24-byte
 * struct passed by value with `partial = 4` (not an 8-byte struct), MOVE_RATIO 2
 * because HAVE_movstrqi is defined, and `mov r1, #8` being record_jump_equiv
 * propagating `s.f04 == 8` into the argument fill rather than a second parameter.
 */
struct Ctx {
    int f00;
    int f04;
    int f08;
    int f0c;
    int f10;
    void (*fn)(void);
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __GetFlag(int id);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

extern int OvlFunc_924_2008758(struct Ctx *s);
extern void OvlFunc_924_20088ec(struct Ctx s);
extern void OvlFunc_924_200b860(void);
extern void OvlFunc_924_200b948(void);
extern void OvlFunc_924_20098f8(void);
extern void OvlFunc_924_20097a8(int n);
extern void OvlFunc_924_20096c4(int n);

void OvlFunc_924_20099b8(void)
{
    struct Ctx s;

    __CutsceneStart();
    if (OvlFunc_924_2008758(&s)) {
        if (s.f04 == 8) {
            if ((s.f08 >> 20) == 0xb) {
                OvlFunc_924_20088ec(s);
                __CutsceneWait(0x1e);
                __PlaySound(0xd3);
                OvlFunc_924_200b860();
                __CopyMapTiles(0x4c, 0x3c, 0x4a, 0x26, 3, 1);
                __CopyMapTiles(0x4d, 0x3c, 0x4c, 0x26, 2, 1);
                __CopyMapTiles(0x4b, 0x3a, 0x56, 0x29, 1, 3);
                __CopyMapTiles(0x4b, 0x3b, 0x56, 0x2b, 1, 2);
                __CopyMapTiles(0x4c, 0x3b, 0x50, 0x31, 2, 1);
                __CopyMapTiles(0x4d, 0x3b, 0x52, 0x31, 2, 1);
                __SetFlag(0x302);
            } else {
                s.fn = OvlFunc_924_200b948;
                __CopyMapTiles(0x4b, 0x39, 0x56, 0x29, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x56, 0x2a, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x56, 0x2b, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x56, 0x2c, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x50, 0x31, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x51, 0x31, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x52, 0x31, 1, 1);
                __CopyMapTiles(0x4e, 0x3a, 0x53, 0x31, 1, 1);
                OvlFunc_924_20088ec(s);
                __ClearFlag(0x302);
            }
        } else if (s.f04 == 0xa) {
            if ((s.f10 >> 20) == 0x28) {
                OvlFunc_924_20088ec(s);
                {
                    register int k __asm__("r0") = 0x307;
                    if (__GetFlag(k) == 0) {
                        __Func_80933d4(0xc0 << 9, 0xc0 << 6);
                        __Func_80933f8(0x2ca0000, -1, 0x94 << 18, 1);
                        __Func_8093530();
                        __SetFlag(0x307);
                        OvlFunc_924_20097a8(5);
                        __CutsceneWait(0x32);
                    } else {
                        OvlFunc_924_20097a8(5);
                    }
                    __SetFlag(0x306);
                }
            } else if ((s.f10 >> 20) == 0x2a) {
                s.fn = OvlFunc_924_20098f8;
                OvlFunc_924_20088ec(s);
                OvlFunc_924_20096c4(5);
                __ClearFlag(0x306);
            }
        }
    }
    __CutsceneEnd();
}
