/* Cluster OvlFunc_927_2009150..OvlFunc_927_2009150 extracted from
 * goldensun/asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a_c.s.
 *
 * The body is the OvlFunc_927 cutscene template of batch 146 -- see
 * OvlFunc_927_2009de0 for the same call sequence at a different actor slot.
 * It matched on the first screen once the FLAGS were right, and the flags are
 * the only thing worth recording here.
 *
 * THE SPLITTER'S CHILD NAME FALLS INSIDE A NEIGHBOUR'S O1 WILDCARD.
 * `asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o` exists for the unrelated stem
 * ovl_30_c_c_a_c_a_b, which byte-matches only at -O1; this file's split-derived
 * name matches that pattern by accident and would inherit -O1, where it comes
 * out 98 lines with 40 differing.  The explicit rule above it in the Makefile
 * pins it to the default flags.  Explicit rules beat pattern rules in make, so
 * position in the file does not matter -- but tools/tryc.py did not model that
 * precedence and kept reporting the -O1 diff after the rule was written; it
 * does now.  If a candidate screens badly and tryc prints the WILDCARD hint,
 * believe the hint before believing the diff.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008ae8(int a, int b, int c, int d, int e, int f, int g, int h);
extern void __SetCameraTarget(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MapActor_Surprise(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __SetFlag(int id);

void OvlFunc_927_2009150(void)
{
    unsigned char *e;
    int y;

    e = __MapActor_GetActor(0xa);
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xa, 1);
    OvlFunc_927_2008d90(0xa, 0x58, 0x78, 0xc0 << 11);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + (0xc0 << 13), 0,
                        0, 0, 1, 0);
    __SetCameraTarget(0xa, 1);
    __Func_8092848(0xa, 0, 0);
    __CutsceneWait(0x14);
    __Func_809259c(0xa, 2);
    __MapActor_Surprise(0xa, 0x81 << 1);
    y = 0xc0 << 10;
    __CutsceneWait(0x3c);
    OvlFunc_927_2008d90(0xa, 0x58, 0x98, y);
    __Func_809280c(0, 0xa, 0);
    __CutsceneWait(0xa);
    OvlFunc_927_2008d90(0xa, 0x78, 0xc0, y);
    __Func_809280c(0, 0xa, 0);
    __CutsceneWait(0xa);
    OvlFunc_927_2008d90(0xa, 0x78, 0xf0, y);
    __Func_809280c(0, 0xa, 0);
    __CutsceneWait(0xa);
    __SetFlag(0xc0 << 2);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    __CutsceneEnd();
}
