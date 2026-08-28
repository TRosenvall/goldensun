/* OvlFunc_968_2009780 -- PARKED, 2 of 49 aligned.
   Ref: asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_c.s
   MUST BE SCREENED/BUILT AT -O2.  The Makefile wildcard rule
   src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c applies O1_CFLAGS to this
   stem and it is WRONG for this TU: -O1 gives 26 differing, -O2 gives 2.
   An explicit -O2 rule is needed when this is wired.
   Residue:  rom  mov r1,#0xe0 / mov r2,#0 / mov r0,#0xa / lsl r1,#8
             ours mov r1,#0xe0 / mov r2,#0 / lsl r1,#8   / mov r0,#0xa
   Blocker class: arg-interleave, shift racing a `mov`, in a STRAIGHT-LINE
   function -- no basic-block boundary for the rematerialisation lever.
   Tried: `int`/`void`/no-prototype on the mismatching callee and on the
   preceding one (4 combinations), the constant spelled 0xe000 instead of
   0xe0<<8, and -fno-schedule-insns.  All 2 of 49.  */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __MessageID(int id);
extern void __Func_809280c(int a, int b, int c);
extern void __CutsceneWait(int a);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __ActorMessage(int a, int b);
extern void __CutsceneEnd(void);

void OvlFunc_968_2009780(void)
{
    unsigned char *p;
    unsigned char *q;
    int z;
    int one;

    p = iwram_3001ebc;
    q = p + 0xcba;
    z = 0;
    *(short *)q = z;
    p += 0xcb6;
    one = 1;
    *(short *)p = one;
    __CutsceneStart();
    __MessageID(0x267d);
    __Func_809280c(0xa, 0, 0);
    __CutsceneWait(0xa);
    __Func_8093040(0xa, 0, 0x14);
    __Func_8092adc(0xa, 0xe0 << 8, 0);
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    __Func_80933f8(0xe0 << 17, -1, 0xd8 << 17, 1);
    __Func_8093530();
    __ActorMessage(0xa, 0);
    __CutsceneEnd();
}
