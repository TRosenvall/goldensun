/* Cluster OvlFunc_888_200b098..OvlFunc_888_200b098 extracted from goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a.s.
 *
 * Total .text for this TU = 172 bytes (= 0xac).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_b.o and asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_b.o in
 * goldensun/overlays/rom_7892c8/overlay.ld.
 *
 * Never attempted before batch 273. No pins, no flags.
 *
 * Spawns actor 0x16, uploads an item icon into its sprite, holds it for 0x3c frames
 * clearing a byte whenever the actor is within +/-0xff, then swaps its script.
 *
 * "ONE REGISTER FOR TWO VALUES MEANS ONE VARIABLE" CUTS BOTH WAYS, AND THE CUT IS
 * PER REGISTER. This function needs the ROM's r8 MERGED -- the parameter and the
 * loop's zero are one variable, so the parameter is reused as the zero -- and the
 * ROM's r5 SPLIT, the zero and the buffer pointer being two. Merging both, or
 * splitting both, measures 20 to 46 differing; only the asymmetric pairing reaches 4.
 *
 * `.18.greg`'s `;; N regs to allocate:` order is what decides which pairing is even
 * reachable: in the 20-differing variants the order was `41 44 34 38 33 36 35 32`
 * with `o` taking r5 first, and the fix was to lower `o`'s priority relative to `buf`
 * rather than to argue with the allocator.
 *
 * THE LAST TWO INSTRUCTIONS WERE AN ARGUMENT FILL ORDER, fixed by declaring
 * __UploadSpriteGFX `int`. That is the fourth function in two batches to turn on a
 * callee's return type -- and note the direction: batch 272's three needed `void`,
 * this one needs `int`. The lever is "get the return type right", not "prefer void".
 * Deleting the prototype entirely also reaches exact, because an undeclared call is
 * an implicit `int` call; the explicit `int` is the honest spelling of the same thing.
 *
 * LADDER: first transcription 78 of 80 lines and 78 differing (it needed r10);
 * merging the zero with `buf`, 46 of 77; hoisting `i = 0` above `item = 0` so CSE
 * gives the ROM's `mov r5, #0 / mov r8, r5`, 23; splitting the zero back out of `buf`
 * and reusing the PARAMETER as the loop's zero, 4; swapping `i = 0` before
 * `p = act + 0x55`, 2; the `int` declaration, 0.
 */
extern unsigned char gScript_888__0200b8f8[];
extern unsigned char gScript_888__0200ba9c[];
extern unsigned char *__CreateActor(int a);
extern void __Actor_SetScript(void *a, void *s);
extern unsigned char *__galloc_iwram(int a, int b);
extern void __gfree(int a);
extern void __LoadItemIcon(int id);
extern int __UploadSpriteGFX(int a, int b, void *p);
extern void __WaitFrames(int n);

void OvlFunc_888_200b098(int item)
{
    unsigned char *act;
    unsigned char *o;
    unsigned char *buf;
    unsigned char *p;
    int z;
    int m;
    unsigned int i;

    act = __CreateActor(0x16);
    z = 0;
    if (act == 0)
        return;
    __Actor_SetScript(act, gScript_888__0200b8f8);
    o = *(unsigned char **)(act + 0x50);
    o[0x26] = z;
    o[0x27] = z;
    m = -0x21;
    o[5] &= m;
    o[9] &= 0xf;
    *(int *)(act + 0x28) = 0x80 << 10;
    *(int *)(act + 0x48) = 0x80 << 7;
    buf = __galloc_iwram(0x11, 0xc1 << 3);
    __LoadItemIcon(item);
    buf += 0x80 << 3;
    __UploadSpriteGFX(o[0x1c], 0x80, buf);
    __gfree(0x11);
    i = 0;
    p = act + 0x55;
    item = 0;
    for (; i < 0x3c; i++) {
        if (*(int *)(act + 0x28) >= -0xff && *(int *)(act + 0x28) <= 0xff)
            *p = item;
        __WaitFrames(1);
    }
    __Actor_SetScript(act, gScript_888__0200ba9c);
}
