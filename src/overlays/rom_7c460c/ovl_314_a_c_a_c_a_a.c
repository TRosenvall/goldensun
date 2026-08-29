/* OvlFunc_939_2008764
 *
 * Cut out of goldensun/asm//overlays/rom_7c460c/ovl_314_a_c_a_c_a_a.s.
 *
 * A villager whose line depends on a flag, with a counter bumped on one path.
 *
 * THE TWO IDENTICAL INCREMENTS ARE SPELLED DIFFERENTLY ON PURPOSE. The ROM
 * emits the offset constant first in one copy and the pointer first in the
 * other, a clean r2/r3 exchange. Writing both as the walk form gives the second
 * copys allocation for both (7 differing); writing the first as offset-first in
 * ONE expression and the second as the walk is exact. That is operand order
 * inside one expression deciding register assignment.
 *
 * Screened by a parallel agent; re-verified here before wiring.
 */
extern unsigned char iwram_3001ebc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __GetFlag(int id);
extern int __Func_8092c40(int slot, int a);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_939_2008764(void)
{
    unsigned char *p;
    unsigned short *s;

    __CutsceneStart();
    if (__GetFlag(0x85a) == 0) {
        __MessageID(0x1be1);
        __ActorMessage(0x12, 0);
    } else {
        __MessageID(0x1b9f);
        __Func_8092c40(0x12, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            s = (unsigned short *)((0xec << 1) + *(unsigned char **)iwram_3001ebc);
            *s = *s + 1;
            __Func_8092c40(0x12, 0);
            if (__Func_8091c7c(0, 0) == 1) {
                p = *(unsigned char **)iwram_3001ebc;
                p += 0xec << 1;
                s = (unsigned short *)p;
                *s = *s + 1;
            }
        }
        __ActorMessage(0x12, 0);
    }
    __CutsceneEnd();
}
