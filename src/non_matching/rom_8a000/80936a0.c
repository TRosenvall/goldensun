/* Func_80936a0 (0x080936a0) -- NON-MATCHING.
 * Blocker class: REGISTER ROTATION in an offset/address chain.
 *
 * 43 lines against the ROM's 43, SIXTEEN differing, and the first fourteen
 * instructions are exact -- including the `_call_via_r3` indirect call, which
 * came free from a function-pointer local as docs/elevation.md records.
 *
 * The divergence is a rotation, starting at the halfword offset build:
 *
 *     rom    mov r1, #0xcf / lsl r1, #1 / add r3, r0, r1 / mov r2, #0
 *     ours   mov r2, #0xcf / lsl r2, #1 / add r3, r0, r2 / mov r2, #0
 *
 * The ROM leaves r2 free for the `ldrsh` index register (Thumb-1 has no
 * immediate form); we build the offset in r2 and then reuse it for the zero.
 * The same rotation runs through the two address chains that follow, where the
 * ROM keeps the OFFSET live in its own register and puts each derived address
 * in a fresh one, and we overwrite the offset with the address.
 *
 * MEASURED, none better than the 16 above:
 *   naming the halfword offset in a local            41 lines, 35 differing
 *   one offset variable reused for both chains       43 lines, 18
 *   `((short *)p)[0xcf]` instead of a cast-and-add   43 lines, 16 (identical)
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT:
 *   - `int (*f)(int, int); f = Func_80008ac; r = f(a, 0x80 << 9);` gives the
 *     ROM's `ldr r3, =Func_80008ac / bl _call_via_r3` exactly.
 *   - the two DERIVED offsets. The ROM builds 0xd4 << 2 and then `add r3, #4`
 *     for the next field, and 0xd6 << 2 then `add r1, #2`; written as
 *     `off = 0xd4 << 2; q1 = g + off; off += 4; q2 = g + off;` both chains come
 *     out with the ROM's arithmetic. Rebuilding each offset from scratch does
 *     not.
 *   - the global dereferenced BEFORE the galloc call, which is what puts it in
 *     a callee-saved register across the call.
 *
 * NEXT: nothing source-level outstanding.
 */
extern unsigned char *iwram_3001e70;
extern unsigned char *galloc_ewram(int a, int b);
extern int Func_80008ac(int a, int b);
extern void Func_80935d4(void);
extern void StartTask(void *f, int a);

void Func_80936a0(int a, int b)
{
    unsigned char *g;
    unsigned char *p;
    unsigned char *q1;
    unsigned char *q2;
    unsigned char *q3;
    unsigned char *q4;
    int (*f)(int, int);
    int off;
    int off2;
    int r;
    int z;

    g = iwram_3001e70;
    p = galloc_ewram(0x1b, 0xccc);
    if (*(short *)(p + (0xcf << 1)) == 3) {
        f = Func_80008ac;
        r = f(a, 0x80 << 9);
        off = 0xd4 << 2;
        q1 = g + off;
        off += 4;
        q2 = g + off;
        *(int *)q1 = *(int *)q2;
        off2 = 0xd6 << 2;
        q3 = g + off2;
        off2 += 2;
        *(int *)q2 = r;
        *(short *)q3 = b;
        z = 0;
        q4 = g + off2;
        *(short *)q4 = z;
        StartTask(Func_80935d4, 0xc94);
    }
}
