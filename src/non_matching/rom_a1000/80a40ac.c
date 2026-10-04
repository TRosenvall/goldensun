/* Func_80a40ac (DiscardFirstUnlockedItem) -- NON-MATCHING.
 *
 * NON-MATCHING, 36 of 47 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 47, ours 45) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   *** RELOCATIONS DIFFER IN THEIR SYMBOLS, NOT ONLY THEIR OFFSETS --
 *   R_ARM_ABS32, _CONST_200
 *   SO THIS FIGURE IS NOT A DISTANCE: `make compare` cannot pass a
 *   relocation difference.  Fix this before trusting the encoding count. ***
 *   SIZE ref 100 bytes, ours 96.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a40ac.c \
 *     asm/rom_a1000/rom_a1814_c_c_a_a.s --func Func_80a40ac
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: REGISTER-ROLE SWAP. 53 lines against the ROM's 55, 38
 * differing, and the opening is now structurally identical -- only r2 and r3
 * are exchanged.
 *
 *     rom    mov r3, #0xd8 / ldrh r3, [r0, r3]
 *     ours   mov r2, #0xd8 / ldrh r2, [r0, r2]
 *
 * THREE LEVERS GOT IT FROM 45 TO HERE, all previously documented:
 *
 *   1. NAME THE OFFSET for the register-offset read, leaving the pointer
 *      advance a literal. The ROM uses 0xd8 in both forms in one function,
 *      which is what that double use was telling us.
 *   2. AN INT FOR THE LOADED VALUE, so gcc emits the zero-extending `ldrh`
 *      rather than `ldrsh`. Same signedness family as the lsr/asr tell.
 *   3. CLOBBER THE OFFSET WITH THE LOADED VALUE -- `v = 0xd8; v = *(unsigned
 *      short *)(u + v);` -- which is what makes gcc load into the register
 *      that held the offset, as the ROM does. The offset-clobber lever applied
 *      to a load rather than a store.
 *
 * THE POOLED MASK IS SOLVED and is now _CONST_200 in const.sym. Eight literal
 * spellings were probed and none pools 0x200; the entry records them. With
 * `(int)&_CONST_200` the pool appears exactly as the ROM has it.
 *
 * What remains is which of r2 and r3 holds the offset, which is the
 * register-pressure category and which no source form has ever selected.
 */
extern int _CONST_200;
extern char *_GetUnit(int id);
extern int _Func_80788c4(int a, int b);

int Func_80a40ac(int who)
{
    char *u;
    unsigned short *p;
    int v;
    int i, r, q, n, m;

    u = _GetUnit(who);
    v = 0xd8;
    v = *(unsigned short *)(u + v);
    r = 0;
    i = 0;
    p = (unsigned short *)(u + 0xd8);
    goto test;
body:
    v = *p;
    m = (int)&_CONST_200;
    if ((v & m) != 0)
        goto next;
    q = v >> 11;
    n = q + 1;
    if (q == 0)
        n = 1;
    if (n == 0)
        goto done;
    do {
        r = _Func_80788c4(who, i);
        n--;
    } while (n != 0);
done:
    if (r != 2)
        return 0;
    goto one;
next:
    i++;
    p++;
    if (i > 0xe)
        goto ret;
    v = *p;
test:
    if (v != 0)
        goto body;
one:
    r = 1;
ret:
    return r;
}
