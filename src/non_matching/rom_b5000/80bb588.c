/* Func_80bb588 (0x080bb588) -- NON-MATCHING.
 * Blocker class: how gcc bookkeeps the addresses of a run of constant-offset
 * byte stores.
 *
 * 98 lines against the ROM's 98, 56 differing. The instruction COUNT agrees
 * exactly; what differs is the offset arithmetic between the stores.
 *
 * The function zeroes bytes 0x12c..0x12f (a four-iteration descending loop,
 * which matches) and then bytes 0x131..0x148 -- 24 contiguous bytes, fully
 * unrolled in the ROM. Written as 24 explicit `u[0x131] = 0;` statements, gcc
 * CSEs the addresses into ONE offset register incremented by 1 per store. The
 * ROM uses TWO offset registers, each incremented by 2, alternating:
 *
 *     rom    add r2,r1,r0 / strb / add r0,#2 / add r2,r1,r4 / strb / add r4,#2 ...
 *     ours   add r2,r1,r0 / strb / add r0,#1 / add r2,r1,r0 / strb / add r0,#1 ...
 *
 * MEASURED:
 *   24 explicit `u[K] = 0;` statements          98 lines, 56 differing  <- best
 *   two alternating named offset locals
 *     (`a = 0x131; b = 0x132; u[a] = z; a += 2; u[b] = z; b += 2; ...`)
 *                                               72 lines, 94 -- gcc folds the
 *                                               offsets, since both are
 *                                               compile-time known, and the
 *                                               stores collapse to immediates
 *   `for (i = 0; i < 12; i++) { u[0x131+i*2] = 0; u[0x132+i*2] = 0; }`
 *                                               34 lines, 94 -- a real loop
 *   the same loop with -funroll-loops            75 lines, 89
 *
 * The two-register-stepping-by-2 pattern is the signature of a strength-reduced
 * loop over a stride-2 pair, which is why the loop forms were tried. They do
 * not reproduce it: unrolled, gcc emits 75 lines, not the ROM's 98. So the ROM
 * is NOT an unrolled loop, and the explicit form has the right instruction
 * count but the wrong address bookkeeping.
 *
 * WHAT IS RIGHT: the length, the four-iteration descending loop with its named
 * offset and named zero, and the tail -- `_CalcStats(id)` then
 * `Func_80b78e4(id, GetBattleActor(id))` with the nested call's result passed
 * straight through.
 *
 * NEXT: whatever makes gcc keep two independent offset variables across a run
 * of constant-offset stores without folding them. Nothing recorded does.
 */
extern unsigned char *_GetUnit(int id);
extern void _CalcStats(int id);
extern void *GetBattleActor(int id);
extern void Func_80b78e4(int id, void *a);

void Func_80bb588(int id)
{
    unsigned char *u;
    unsigned char *p;
    int i;

    u = _GetUnit(id);
    p = u + 0x12f;
    i = 3;
    do {
        i--;
        *p = 0;
        p--;
    } while (i >= 0);
    u[0x131] = 0;
    u[0x132] = 0;
    u[0x133] = 0;
    u[0x134] = 0;
    u[0x135] = 0;
    u[0x136] = 0;
    u[0x137] = 0;
    u[0x138] = 0;
    u[0x139] = 0;
    u[0x13a] = 0;
    u[0x13b] = 0;
    u[0x13c] = 0;
    u[0x13d] = 0;
    u[0x13e] = 0;
    u[0x13f] = 0;
    u[0x140] = 0;
    u[0x141] = 0;
    u[0x142] = 0;
    u[0x143] = 0;
    u[0x144] = 0;
    u[0x145] = 0;
    u[0x146] = 0;
    u[0x147] = 0;
    u[0x148] = 0;
    _CalcStats(id);
    Func_80b78e4(id, GetBattleActor(id));
}
