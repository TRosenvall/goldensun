/* OvlFunc_881_200b678
 *
 * Cut out of goldensun/asm//overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_b.s.
 *
 * A per-frame countdown on a save byte, with a scene word stamped at one.
 *
 * THE OFFSET VARIABLE DOES BOTH JOBS. The ROM overwrites the register holding
 * the field offset with the halfword it is about to store, so one variable
 * carries both -- batch 44s offset-reuse shape. A separate short constant is 7
 * of 42 and, less obviously, it also moves the gState offset from r2 to r1 four
 * instructions earlier. Reusing the variable fixes both.
 *
 * Screened by a parallel agent; re-verified here before wiring.
 */
extern unsigned char gState[];
extern unsigned char iwram_3001ebc[];
extern unsigned char iwram_3001e40[];

extern unsigned char *__MapActor_GetActor(int id);
extern int  __GetFlagByte(int id);
extern void __SetFlagByte(int id, int v);
extern int  __GetFlag(int id);

void OvlFunc_881_200b678(void)
{
    unsigned char *gs;
    unsigned char *actor;
    unsigned char *base;
    unsigned char *p;
    unsigned int off;
    int t;
    int v;

    gs = gState;
    gs += 0xfa << 1;
    actor = __MapActor_GetActor(*(int *)gs);
    base = *(unsigned char **)iwram_3001ebc;
    t = *(int *)iwram_3001e40;
    t <<= 12;
    *(short *)(actor + 6) = t;
    v = __GetFlagByte(0xbe << 2);
    if (v != 0) {
        if (v == 1) {
            off = 0xc1 << 1;
            p = base + off;
            off = 0x63;
            *(short *)p = off;
        } else if (__GetFlag(0x83 << 1) == 0) {
            v--;
        }
    }
    __SetFlagByte(0xbe << 2, v);
}
