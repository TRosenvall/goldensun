/* Func_80e73a0 -- 0x080e73a0, asm/rom_c9000/rom_e7320_c_c.s (eight functions).
 *
 * BLOCKER CLASS: A LOOP THE ROM DOES NOT OPTIMISE. 50 instructions against our
 * best 46, 35 differing. The function BODY is settled; only the loop's shape is
 * open.
 *
 * IT IS A STATIC CHAIN, and that part is closed. All three tells of the
 * recorded class are present -- r9 saved in the prologue and restored in the
 * epilogue, a `mov r2, r9` that nothing in the function defines, and a stack
 * slot (`mov r3, sp / str r2, [r3]`) that is never read back. The batch-265
 * transcription reproduces the whole prologue exactly:
 *
 *     volatile u32 chain;  register u32 _chain __asm__("r9");
 *     chain = _chain;  c = chain;          -- the volatile-forwarding read
 *
 * `*(void **)(chain - 0x88)` is therefore a LOCAL OF THE PARENT, and the table
 * it points at has 28-byte entries at +0x7080 with a status word at +0x18.
 * This is the THIRD specimen of the class (Func_8015fb8, Func_80270d8).
 *
 * WHAT IS OPEN. The ROM's loop is entirely unoptimised, and we cannot make gcc
 * leave it alone:
 *
 *     rom    lsl r3,r4,#3 / sub r3,r4 / ldr r2,[r1] / lsl r3,#2 / add r2,r3
 *            mov r3,#0xe1 / lsl r3,#7 / add r2,r3 / mov r6,#1 / ... / neg r6,r6
 *     ours   ldr r3,[r0] / add r3,r3,r4 / add r2,r3,r5        (r4 += 28 hoisted,
 *                                                              r5 = 0x7080, ip = -1)
 *
 * Three separate things the ROM does NOT do and we do: it re-loads the table
 * base every iteration, it recomputes `i * 28` from `i` every iteration instead
 * of carrying a walking offset, and it rebuilds BOTH loop-invariant constants
 * (0x7080 and -1) in the body.
 *
 * PARTIAL PROGRESS, MEASURED. Declaring the pointee volatile
 * (`u8 *volatile *pp`) DOES keep the base load inside the loop -- that half is
 * solved at source level and should be kept. It does not stop strength
 * reduction of the offset, and it does not stop the constants being hoisted.
 *
 *   plain                                            42 differ, 45 insn
 *   volatile pointee                                 45 differ, 44 insn
 *   volatile pointee + -fno-strength-reduce          35 differ, 46 insn  <- best
 *   -fno-strength-reduce alone                       43 differ, 44 insn
 *   -fno-gcse / -fno-move-all-movables               45 / 42, both shorter
 *   index written as `+ i * 28` rather than `[i]`    42 differ, unchanged
 *
 * EVERY FLAG TRIED MAKES THE FUNCTION SHORTER OR LEAVES IT, never longer. The
 * ROM has FIVE instructions we do not, so the difference is not an optimisation
 * that needs switching off -- it is register pressure making gcc rematerialise
 * rather than hoist. Something in the original keeps more values live across
 * this loop than our reading does.
 *
 * NEXT: find what raises the pressure. The parent-local table base is read
 * through the static chain, so the natural suspicion is that the ORIGINAL is
 * the nested function and the parent's frame pointer plus a second parent local
 * are both live here -- which the transcription cannot express. The parent is
 * not identified yet; `add rN, sp, #K / mov r9, rN / bl Func_80e73a0` is the
 * grep for it, and identifying it is worth more than another spelling sweep.
 */
#include "gba/types.h"

struct Ent {
    int f0;
    int f4;
    u8 pad08[0x10];
    int f18;
};

void Func_80e73a0(int a, int b)
{
    volatile u32 chain;
    u8 *volatile *pp;
    struct Ent *e;
    int i;
    register u32 _chain __asm__("r9");
    u32 c;

    chain = _chain;
    c = chain;
    pp = (u8 *volatile *)(c - 0x88);
    e = (struct Ent *)(*pp + (0xe1 << 7));
    i = 0;
    while (e->f18 != -1) {
        i++;
        if (i == 0x20)
            return;
        e = (struct Ent *)(*pp + (0xe1 << 7)) + i;
    }
    e->f18 = 0;
    e->f0 = a;
    e->f4 = b;
}
