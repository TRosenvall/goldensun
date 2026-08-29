/* OvlFunc_901_2008804  --  0x02008804
 * OvlFunc_901_2008864  --  0x02008864
 *
 * Cut from the head of
 * goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_a.s; the
 * remaining function follows as ovl_314_c_c_a_a_c_c_a_c_c_a_a_c.o.
 *
 * The same cutscene bookend as src/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a_b.c
 * -- see that file for why the ORed 2 is `_CONST_2` from const.sym rather than
 * a literal, and for the twelve spellings that settled it.
 *
 * BUILT WITH CSE_CFLAGS (`-fno-rerun-cse-after-loop`), and only because of
 * OvlFunc_901_2008804. That function reads and then sets the same save flag,
 * 0x307, and at plain -O2 gcc hoists the id into r5 across the call -- paying a
 * push and a pop to save one pool load, which is the constant-CSE shape
 * pick_candidates.py screens for. The ROM loads it twice. With the flag it
 * loads it twice too, and the function drops from 29 differing lines to none.
 * OvlFunc_901_2008864 is unaffected by the flag and rides along.
 *
 * THE STORED CONSTANTS ARE VARIABLES, NOT LITERALS. `*p = 0` and `*p = 1` on a
 * `u16` pool the value as a HImode constant -- `ldr r3, =0x0` is real and wrong.
 * The ROM has `mov r5, #0` and `mov r3, #1`. Declaring an `int` local and
 * storing that reproduces both, and WHERE the local is assigned decides the
 * register: 0x2008864's zero is assigned at the top of the function, so it is
 * live across the calls and lands in a pushed r5, which is exactly what the ROM
 * does; 0x2008804's one is assigned just before its use and lands in a
 * caller-saved r3. Moving either assignment loses the match.
 */
struct A { unsigned char pad00[0x64]; unsigned short f64; };

extern int _CONST_2;
extern int _MSG_1cc0;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void OvlFunc_901_20084b4(int slot);
extern void OvlFunc_901_200858c(void);

void OvlFunc_901_2008804(void)
{
    unsigned short *p;
    unsigned short two;
    int one;

    p = &__MapActor_GetActor(0xe)->f64;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    if (__GetFlag(0x307)) {
        __MessageID((int)&_MSG_1cc0);
        OvlFunc_901_20084b4(0xe);
    } else {
        OvlFunc_901_200858c();
        __SetFlag(0x307);
    }
    __CutsceneEnd();
    p = &__MapActor_GetActor(0xe)->f64;
    one = 1;
    *p = one;
}

void OvlFunc_901_2008864(void)
{
    unsigned short *p;
    unsigned short two;
    int z;

    z = 0;
    p = &__MapActor_GetActor(0xf)->f64;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    __MessageID(0x1cc1);
    OvlFunc_901_20084b4(0xf);
    __CutsceneEnd();
    p = &__MapActor_GetActor(0xf)->f64;
    *p = z;
}
