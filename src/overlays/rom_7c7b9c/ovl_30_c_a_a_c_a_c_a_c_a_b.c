/* OvlFunc_943_20093d4  --  0x020093d4
 *
 * Cut out of goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_b.s.
 *
 * Resets the fight-scene state: clear the flag, stamp the scene id, zero the
 * timer, and reseed two random words. Matched on the first screen.
 *
 * The stored 0x209 is gcc's own arithmetic on the 0x1c0 offset already in a
 * register (`add r2, #0x49`) -- plain literals reproduce it.
 *
 * Drafted by a parallel screening agent and re-screened here before wiring.
 */
extern unsigned char iwram_3001e70[];

extern unsigned int L5b38 __asm__(".L5b38");
extern unsigned int L5b50[] __asm__(".L5b50");
extern unsigned int L5b58 __asm__(".L5b58");
extern unsigned int L5b60 __asm__(".L5b60");

extern void __ClearFlag(int flag);
extern int __Random(void);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);
extern void OvlFunc_943_2009444(void);

int OvlFunc_943_20093d4(void)
{
    unsigned char *p;

    p = *(unsigned char **)iwram_3001e70 + 0x104;
    __ClearFlag(0x11c);
    *(int *)(*(unsigned char **)(iwram_3001e70 + 0x4c) + 0x1c0) = 0x209;
    *(int *)(p + 0x1c) = 0;
    L5b58 = (unsigned short)__Random();
    L5b38 = (unsigned short)__Random();
    L5b50[0] = 0;
    L5b50[1] = 0;
    L5b60 = 0;
    __Func_800fe9c();
    __WaitFrames(1);
    OvlFunc_943_2009444();
    return 0;
}
