/* Cluster OvlFunc_895_2008200..OvlFunc_895_2008200 extracted from goldensun/asm/overlays/rom_78dee8/ovl_30_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_78dee8/ovl_30_c_c_a_a_a.o and asm/overlays/rom_78dee8/ovl_30_c_c_a_a_c.o in
 * goldensun/overlays/rom_78dee8/overlay.ld.
 *
 * TalkPassageA. Says line 0x1034 once the passage has been opened (save bit
 * 0x81a), otherwise 0x1031 -- and in the not-yet-opened case, if the arming
 * bit 0xf01 is set, writes 1 to [iwram_3001ebc]+0x172.
 *
 * A SECOND MEMBER of the shape in src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_b.c
 * and it needs both of that file's levers, unchanged:
 *
 *   - the halfword store goes through a named `int` (`v = 1; *(u16 *)p = v;`).
 *     Written `*(u16 *)p = 1` gcc narrows the whole expression to sixteen bits
 *     and materialises the literal as a HALFWORD POOL ENTRY -- the inverted
 *     narrow_constant case, which looks exactly like the pool tell.
 *   - the address is built in three named steps, because the ROM computes the
 *     offset at runtime (`mov r1,#0xb9 / lsl r1,#1`) and puts the sum in a
 *     THIRD register (`add r2, r3, r1`, not destructive).
 *
 * Written as a template it matched first try, which is the point of recording
 * it: 0xb9 << 1 = 0x172 is the same interaction halfword the other overlay
 * writes, so the whole family is likely to go the same way.
 */
extern unsigned int iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int flag);
extern void __Func_801776c(int id, int b);

void OvlFunc_895_2008200(void)
{
    unsigned char *base;
    unsigned char *p;
    unsigned int off;
    int v;

    __CutsceneStart();
    if (__GetFlag(0x81a)) {
        __Func_801776c(0x1034, 1);
    } else {
        __Func_801776c(0x1031, 1);
        if (__GetFlag(0xf01)) {
            base = (unsigned char *)iwram_3001ebc;
            off = 0xb9;
            off <<= 1;
            p = base + off;
            v = 1;
            *(unsigned short *)p = v;
        }
    }
    __CutsceneEnd();
}
