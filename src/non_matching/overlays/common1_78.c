/* OvlFunc_common1_78 -- NOT MATCHING, a SIZE mismatch: ref 208 bytes against ours 212.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/Lc78.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/common1_78.c \
 *     asm/overlays/common/common1_a_a_a_a_a_c_a_b.s --func OvlFunc_common1_78
 *
 * The original prose park is preserved below.
 */
extern unsigned int gState;
extern void __Func_8079664(int n);
extern void __AddPartyMember(int n);
extern void __SetCameraTarget(int slot, int n);
extern unsigned char *__GetUnit(int slot);
extern void __Func_8091858(void);

void OvlFunc_common1_78(int slot)
{
    unsigned char *u;
    unsigned int g;
    unsigned int off;
    unsigned int o2;
    int q;
    int w;

    __Func_8079664(0);
    __Func_8079664(1);
    __Func_8079664(2);
    __Func_8079664(3);
    __Func_8079664(5);
    __AddPartyMember(slot);
    g = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    g += off;
    *(int *)g = slot;
    __SetCameraTarget(slot, 0);
    u = __GetUnit(slot);
    *(short *)(u + 0x38) = *(unsigned short *)(u + 0x34);
    o2 = 0x131;
    *(short *)(u + 0x3a) = *(unsigned short *)(u + 0x36);
    *(unsigned char *)(u + o2) = 0;

    q = (*(short *)(u + 0x38) << 14) / *(short *)(u + 0x34);
    if (q > (0x80 << 7))
        w = 0x80 << 7;
    else if (q < 0)
        w = 0;
    else
        w = q;
    *(short *)(u + 0x14) = w;
    if ((w << 16) == 0 && *(short *)(u + 0x38) != 0)
        *(short *)(u + 0x14) = 1;

    q = (*(short *)(u + 0x3a) << 14) / *(short *)(u + 0x36);
    if (q > (0x80 << 7))
        w = 0x80 << 7;
    else if (q < 0)
        w = 0;
    else
        w = q;
    *(short *)(u + 0x16) = w;
    if ((w << 16) == 0 && *(short *)(u + 0x3a) != 0)
        *(short *)(u + 0x16) = 1;
    __Func_8091858();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_common1_78 -- asm/overlays/common/common1_a_a_a_a_a_c_a.s
 *
 * 90 of 90 lines, 59 differing.  Candidate at scratch/Lc78.c.
 * The whole structure is right -- five __Func_8079664 calls, the gState store
 * through the integer-local chain, two identical clamp-and-divide blocks -- and
 * the divergence is one optimiser decision repeated four times.
 *
 * BLOCKER: gcc reuses the value it just stored; the ROM re-loads it.
 *
 *      rom   strh r3, [r5, #0x38] ... mov r2, #0x38 / ldrsh r0, [r5, r2]
 *      ours  strh r1, [r5, #0x38] ... lsl r1, #0x10 / asr r1, #0x10
 *
 * The ROM stores a halfword to +0x38 and then genuinely re-reads it with
 * `ldrsh`; gcc knows the stored value and sign-extends the register it already
 * has.  Both clamp blocks do this, on +0x38/+0x34 and +0x3a/+0x36, so it
 * accounts for most of the 59.
 *
 * The intervening `strb` at +0x131 does not break the CSE for gcc, and it would
 * not for any alias analysis either -- the offsets are provably distinct.  So
 * this is not an aliasing problem to be defeated; the original source must have
 * expressed the re-read in a form gcc could not relate to the store, and I do
 * not know what that is.
 *
 * SOLVED and worth keeping: the zero stored at +0x131 is POOLED in this
 * function (`ldr r2, =0x0`), where its sibling OvlFunc_953_200a3e0 needed an
 * int intermediate to AVOID the pool.  Writing the literal 0 directly into a
 * byte store gives the pooled form.  That is the third distinct behaviour for a
 * zero constant in this codebase and they are decided by the destination width
 * and the spelling together, not by either alone.
 *
 * NOTE FOR WIRING: this is common-overlay code and the screen shows
 * `bl __divsi3` against the ROM's `bl _divsi3_RAM`.  If it is ever matched,
 * check whether the common overlay's linker script carries the
 * `__divsi3 = _divsi3_RAM;` alias -- the per-overlay scripts do, but common1
 * has not needed it before.
 *)

*/
#endif
