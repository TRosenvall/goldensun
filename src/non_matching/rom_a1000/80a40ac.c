/* Func_80a40ac (DiscardFirstUnlockedItem) -- NON-MATCHING.
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
