/* OvlFunc_common1_15b8 -- NOT MATCHING. 6 of 34, same length.
 *
 * Source asm: goldensun/asm/overlays/common/common1_a_c_c_c.s
 *
 * Blocker: instruction ORDER around a pointer walk, not the values.
 *
 *     rom    asr r3,#1 / str r3,[r5,#0x34] / mov r3,r5 / mov r2,#0
 *            add r3,#0x5b / strb r2,[r3]
 *     ours   ... the +0x5b address computed BEFORE the second store, and the
 *            zero materialised after it
 *
 * WHAT IS ALREADY RIGHT AND SHOULD NOT BE UNDONE:
 *
 * THE SECOND CONSTANT IS DERIVED FROM THE FIRST BY A SHIFT, in the source.
 * `v = 0xa0 << 9; ...store...; v >>= 1; ...store...` reproduces the ROM's
 * `asr r3, #1`. Written as two literals (`0xa0 << 9` and `0xa0 << 8`) gcc emits
 * a fresh `mov r3,#0xa0 / lsl r3,#8` -- 8 of 34 instead of 6.
 *
 * THAT REFINES BATCH 48's RULE, which says an add/sub chain on a constant is
 * usually gcc's own strength reduction and the source had literals. The test
 * that actually decides it is simpler: WRITE THE LITERALS FIRST AND SEE WHETHER
 * gcc PRODUCES THE CHAIN. If it does, the chain is gcc's. If it emits fresh
 * constants instead -- as here -- the chain was in the source. The mnemonic
 * (add/sub against asr) is not the discriminator; who generates it is.
 *
 * TRIED for the ordering: `p = a; z = 0; p += 0x5b; *p = z;` -- 33 lines, 11
 * of 34, worse. gcc reassociates the whole prologue when the pointer copy is
 * named.
 *
 * NEXT: the remaining six are scheduling around one byte store. Worth another
 * look only if a second function in this file shows the same pattern.
 */
extern void *__GetFieldActor(int id);
extern void __Actor_Stop(void *a);
extern void __Actor_SetAnim(void *a, int anim);
extern void __Actor_TravelTo(void *a, int x, int y, int z);
extern void __Actor_WaitMovement(void *a);

void OvlFunc_common1_15b8(int id, int x, int z)
{
    unsigned char *a;
    int v;

    a = (unsigned char *)__GetFieldActor(id);
    if (a == 0)
        return;
    v = 0xa0 << 9;
    *(int *)(a + 0x30) = v;
    v >>= 1;
    *(int *)(a + 0x34) = v;
    a[0x5b] = 0;
    __Actor_Stop(a);
    __Actor_SetAnim(a, 5);
    __Actor_TravelTo(a, x << 16, *(int *)(a + 0xc), z << 16);
    __Actor_WaitMovement(a);
    __Actor_SetAnim(a, 1);
}
