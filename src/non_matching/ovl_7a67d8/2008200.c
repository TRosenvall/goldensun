/* OvlFunc_919_2008200 -- NON-MATCHING.
 * Blocker class: REGISTER-ROLE SWAP. 36 lines against the ROM's 36, 18
 * differing, and the INSTRUCTION SEQUENCE IS IDENTICAL throughout -- r2 and r3
 * are exchanged in the opening block and every later reference follows.
 *
 *     rom    mov r2,#0xe0 / ldr r3,[r5] / lsl r2,#1 / add r3,r2
 *            / sub r2,#0xc0 / str r2,[r3]
 *     ours   mov r3,#0xe0 / ldr r2,[r5] / lsl r3,#1 / add r2,r3
 *            / sub r3,#0xc0 / str r3,[r2]
 *
 * SOLVED, and this is the reusable part: REUSING THE OFFSET VARIABLE AS THE
 * VALUE forces the address to be materialised.
 *
 * The store writes 0x100 to [ptr + 0x1c0], and the ROM makes the value by
 * DESTROYING the offset -- `sub r2, #0xc0` on the register that held 0x1c0 --
 * which means the address must already exist in another register. Written the
 * obvious way:
 *
 *     *(int *)(p + off) = off - 0xc0;
 *
 * gcc keeps the offset live, addresses with `str r3, [r1, r2]`, and never
 * computes an address at all -- 17 differing with the whole block wrong.
 * Written as:
 *
 *     d = (int *)(p + off);
 *     off -= 0xc0;
 *     *d = off;
 *
 * the offset is dead after the subtraction, so gcc must materialise the
 * address first, and the block becomes instruction-for-instruction identical.
 *
 * That generalises: when the ROM computes an address explicitly where
 * register-offset addressing would do, look for a later instruction that
 * CLOBBERS the offset. The clobber is the reason, and reproducing it in the
 * source is what forces the address out.
 *
 * Tried for the register roles, no change: declaring the offset before the
 * pointer, which is the source-order lever for allocation. 18 differing,
 * byte-identical.
 *
 * The rest of the function -- the two blend-register stores, the three
 * halfword field writes through iwram_3001ebc[4], the call and the zero
 * return -- is exact, using the same int-intermediate pool ordering that
 * elevated OvlFunc_881_200b8fc and OvlFunc_common1_1490.
 */
#include "gba/types.h"
#include "gba/io.h"

extern char *iwram_3001ebc[];
extern void __Func_808fe38(int n);
extern void OvlFunc_919_20082e0(void);

int OvlFunc_919_2008200(void)
{
    char *p;
    unsigned short *q;
    int *d;
    int off;
    int v;

    p = iwram_3001ebc[0];
    off = 0xe0 << 1;
    d = (int *)(p + off);
    off -= 0xc0;
    *d = off;
    __Func_808fe38(9);
    REG_BLDCNT = 0x3f42;
    REG_BLDALPHA = 0xc04;
    p = iwram_3001ebc[4];
    off = 0x534;
    q = (unsigned short *)(p + off);
    v = 0x3f3f;
    *q = v;
    off = 0x536;
    q = (unsigned short *)(p + off);
    v = 0x1f;
    *q = v;
    off = 0x52a;
    p += off;
    v = 0xa;
    *(unsigned short *)p = v;
    OvlFunc_919_20082e0();
    return 0;
}
