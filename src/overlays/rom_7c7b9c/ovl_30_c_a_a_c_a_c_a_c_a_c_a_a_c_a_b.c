// fakematch
/* OvlFunc_943_20099c0  --  0x020099c0
 *
 * Cut out of goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c_a.s.
 *
 * Map setup with a two-armed actor placement, 82 instructions. Three levers,
 * all of them already on file and all three needed at once.
 *
 * PINS MUST NOT STRADDLE THE CALL INSIDE THE STATEMENT THEY WRAP. The ROM
 * builds two argument movs, then stores a halfword through a fetched actor,
 * then shifts the arguments:
 *
 *     mov r1, #0xa2 / mov r2, #0xa9 / strh r5, [r0, #6] / lsl r1, #16 / ...
 *
 * Written with the store as `*(short *)(__MapActor_GetActor(0x16) + 6) = m;`
 * BETWEEN the pinned assignments, the call inside that expression clobbers r1
 * and r2, and gcc moves both movs below the store -- 46 of 79. Fetching the
 * actor into its own statement first, so the pins' live range contains no `bl`,
 * gives the ROM's order. That is the batch-210 hazard in its milder form: there
 * the pin was dropped outright, here it was merely relocated, and the cause is
 * the same.
 *
 * A CROSS-JUMPED TAIL NEEDS THE CALL IN EACH ARM AND THE STORE AFTER THE JOIN.
 * Both arms end by fetching actor 0x15 and storing a different shifted
 * constant; the ROM emits the fetch twice and shares only `lsl r3, #8 / strh`.
 * Writing the whole store in each arm folds the shift into a constant and comes
 * out three instructions short. Assigning the actor and the constant per arm
 * and putting one store after the `if` gives it.
 *
 * THE LAST TWO INSTRUCTIONS took a barrier. The gState access wants
 * `ldr r3, =gState` before `mov r2, #0xe1`; ours emitted them the other way and
 * PINNING BOTH IS INERT -- a pool load against an immediate mov is the class
 * docs/elevation.md records the pin as not reaching. A volatile asm on the base
 * after its load is exact.
 */
extern unsigned char *iwram_3001e70;
extern unsigned char gState[];
extern int L5418 __asm__(".L5418");
extern void OvlFunc_943_200bf30(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __LoadFieldActors(void *p);
extern void __DeleteFieldActor(int slot);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern int __GetFlag(int id);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_943_20099c0(void)
{
    register int m __asm__("r5");
    unsigned char *g;
    unsigned char *p;
    int n;

    {
        register unsigned char *w __asm__("r3");
        register int v __asm__("r2");
        w = iwram_3001e70;
        v = 0x82;
        w += 0xec;
        v <<= 15;
        *(int *)w = v;
    }
    __CutsceneStart();
    __LoadFieldActors(&L5418);
    __WaitFrames(1);
    __DeleteFieldActor(0x18);
    { PIN3; q1 = 0xee; q0 = 0x17; q1 <<= 16; q2 = 0x2720000;
      __MapActor_SetPos(q0, q1, q2); }
    m = 0xc0;
    m <<= 6;
    *(short *)(__MapActor_GetActor(0x17) + 6) = m;
    if (__GetFlag(0x903) != 0) {
        { PIN3; q1 = 0xa2; q1 <<= 16; q2 = 0x27a0000; q0 = 0x16;
          __MapActor_SetPos(q0, q1, q2); }
        p = __MapActor_GetActor(0x16);
        {
            PIN3;
            q1 = 0xa2; q2 = 0xa9;
            *(short *)(p + 6) = m;
            q1 <<= 16; q0 = 0x15; q2 <<= 18;
            __MapActor_SetPos(q0, q1, q2);
        }
        p = __MapActor_GetActor(0x15);
        n = 0xd0;
    } else {
        { PIN3; q1 = 0xa0; q2 = 0xa3; q1 <<= 16; q2 <<= 18; q0 = 0x16;
          __MapActor_SetPos(q0, q1, q2); }
        p = __MapActor_GetActor(0x16);
        {
            PIN3;
            q1 = 0xa6; q2 = 0xa7;
            *(short *)(p + 6) = m;
            q1 <<= 16; q0 = 0x15; q2 <<= 18;
            __MapActor_SetPos(q0, q1, q2);
        }
        p = __MapActor_GetActor(0x15);
        n = 0xb0;
    }
    *(short *)(p + 6) = n << 8;
    {
        register unsigned char *gb __asm__("r3");
        register int go __asm__("r2");
        gb = gState;
        __asm__ volatile ("" : : "r" (gb));
        go = 0xe1; go <<= 1;
        gb += go;
        if (*(short *)gb == 6)
            OvlFunc_943_200bf30();
    }
    __CutsceneEnd();
}
