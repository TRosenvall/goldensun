/* Func_809ade8  --  0x0809ade8
 *
 * The second function of goldensun/asm/rom_8a000/rom_9ad70_a_a_a_c.s;
 * Func_809ad90 stays as assembly in rom_9ad70_a_a_a_c_a.s.
 *
 * Hands a field actor back to whatever owned it before Func_809ad70 took it
 * over: if the actor's callback slot still points at that function, restore the
 * saved callback out of gState, clear the save slot, and put the colourswap
 * back from a signed byte beside it. Either way, clear the flag at +0x5b and
 * reset the animation speed.
 *
 * TWO READINGS, both ordinary but both load-bearing:
 *
 *   THE gState BASE IS A LOCAL. The ROM loads it once into r2 and reaches both
 *   0x250 and 0x249 from it (`add r1, r2, r3` and `add r2, r3`). Naming
 *   `gState` at each site reloads the address.
 *
 *   THE COLOURSWAP BYTE IS SIGNED. `ldrsb r1, [r2, r1]` with r1 zeroed is the
 *   register-offset form, which is the only ldrsb thumb has -- so the source
 *   read a `signed char`, and an `unsigned char` would have given `ldrb`.
 *
 * The actor pointer is still in r0 when _Actor_SetColorswap is called, which is
 * why there is no `mov r0, r5` in front of it -- gcc noticed GetFieldActor's
 * return value had not been disturbed. Nothing in the source says that.
 *
 * Matched on the first screen.
 */
struct Actor {
    unsigned char pad00[0x5b];
    unsigned char f5b;
    unsigned char pad5c[0x6c - 0x5c];
    void *f6c;
};

extern unsigned char gState[];
extern void Func_809ad70(void);
extern struct Actor *GetFieldActor(int id);
extern void _Actor_SetColorswap(struct Actor *a, int c);
extern void _Actor_SetAnimSpeed(struct Actor *a, int s);

void Func_809ade8(int id)
{
    struct Actor *a;
    unsigned char *g;

    a = GetFieldActor(id);
    if (a != 0) {
        if (a->f6c == (void *)Func_809ad70) {
            g = gState;
            a->f6c = *(void **)(g + 0x250);
            *(void **)(g + 0x250) = 0;
            _Actor_SetColorswap(a, *(signed char *)(g + 0x249));
        }
        a->f5b = 0;
        _Actor_SetAnimSpeed(a, 0x10);
    }
}
