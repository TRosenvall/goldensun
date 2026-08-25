/* Cluster OvlFunc_891_200901c..OvlFunc_891_200901c extracted from goldensun/asm/overlays/rom_78c76c/ovl_30_c_c_a_c_c_a_a.s.
 *
 * Slotted between ovl_30_c_c_a_c_c_a_a_a.o and the rest of the overlay.
 *
 * THE BASIC-BLOCK LEVER, TWICE IN ONE FUNCTION, on two different blocker
 * shapes. Both calls sit inside the `if` body, and all three constants are
 * assigned in the entry block, which dominates the body and contains none of
 * their uses -- the conditions from batch 43 as amended in batch 44.
 *
 *   `n = 0xfc << 1;`   fixes an ARG-INTERLEAVE: the ROM writes r0 between
 *                      `mov r1, #0xfc` and its `lsl r1, #1`, and gcc will not
 *                      put it there while the constant is written at the call.
 *
 *   `s1`, `s2`         fix POOL-LOADS-FIRST on __MapActor_SetSpeed: the ROM
 *                      does `mov r0, #9` BEFORE the two `ldr =` pool loads and
 *                      gcc emits the pool loads first. 3 of 21 with only `n`
 *                      named.
 *
 * The two are one mechanism -- batch 43 established that -- and this is the
 * clearest single example of it, since fixing one leaves the other standing.
 *
 * The __MapActor_GetActor result is genuinely discarded; the ROM never reads
 * r0 after that call. Declaring it `void` and withholding its prototype both
 * change nothing.
 */
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_80921c4(int slot, int a, int b);

void OvlFunc_891_200901c(void)
{
    int n;
    int s1;
    int s2;

    n = 0xfc << 1;
    s1 = 0x3333;
    s2 = 0x1999;
    if (!__GetFlag(0x80b)) {
        __CutsceneStart();
        __MapActor_GetActor(9);
        __MapActor_SetSpeed(9, s1, s2);
        __Func_80921c4(9, n, 0x98);
        __CutsceneEnd();
    }
}
