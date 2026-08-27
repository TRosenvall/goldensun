/* OvlFunc_945_2009a08  --  0x02009a08
 *
 * A short cue that only runs once its flag is set.
 *
 * Three constants need the BASIC-BLOCK LEVER with the `if` as the boundary --
 * one shifted argument and, less obviously, the two POOL constants of
 * `__MapActor_SetSpeed`. Those two are the pool-loads-first shape:
 *
 *     rom    mov r0, #0 / ldr r1, =0x19999 / ldr r2, =0xcccc
 *     ours   ldr r1, =0x19999 / ldr r2, =0xcccc / mov r0, #0
 *
 * Named above the `if` they are rematerialised at the call and land after r0.
 */
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8093304(int n);
extern void __Func_8019aa0(int a, int b, int c);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __Func_80921c4(int a, int b, int c);
extern int OvlFunc_945_200c880(int slot, int v);

void OvlFunc_945_2009a08(void)
{
    int v;
    int s1;
    int s2;

    v = 0xcc << 1;
    s1 = 0x19999;
    s2 = 0xcccc;
    if (__GetFlag(0x301)) {
        __CutsceneStart();
        __Func_8093304(8);
        __Func_8019aa0(0x1e48, 1, 8);
        __MapActor_SetSpeed(0, s1, s2);
        __Func_80921c4(0, v, 0x86);
        OvlFunc_945_200c880(0, 0x80 << 7);
        __CutsceneEnd();
    }
}
