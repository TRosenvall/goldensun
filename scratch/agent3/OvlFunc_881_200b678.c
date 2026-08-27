/* OvlFunc_881_200b678 -- MATCHES on the default flags.
 * ref: asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c.s
 * tryc.py: OK (42 lines).  Byte-verified: 100 bytes of .text identical to the
 * ROM's, pool included (scratch/agent3/bytecheck.sh).
 *
 * The one lever it needed is batch 44's OFFSET-REUSE form: the ROM computes the
 * address into r2 and then overwrites the offset register r3 with the halfword
 * it is about to store --
 *     mov r3,#0xc1 / lsl r3,#1 / add r2,r6,r3 / mov r3,#0x63 / strh r3,[r2]
 * -- so one variable does both jobs.  Written with a separate `unsigned short
 * k = 0x63` the function is 7 of 42: the constant is materialised before the
 * address and the gState offset lands in r1 instead of r2.  Reusing `off`
 * fixed both at once.
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
