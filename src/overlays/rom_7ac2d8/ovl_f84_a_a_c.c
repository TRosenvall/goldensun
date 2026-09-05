/* OvlFunc_924_2008ffc  --  0x02008ffc
 *   [asm/overlays/rom_7ac2d8/ovl_f84_a_a_c.s, 1st of 1 -- NO SPLIT NEEDED]
 *
 * A solved_twins.py HIT, worked by the method its own elevation.md entry lays
 * out: sed the names and any differing immediates, screen, and only start
 * moving statements if that fails -- trusting the template's recorded levers
 * over my own reading of the ROM.
 *
 * Template: OvlFunc_923_2008f48 (src/overlays/rom_7aa430/ovl_e90_c_c_a_a_c_c.c).
 * NOT ONE IMMEDIATE DIFFERS across the 36 instructions; only the function name
 * does. So step 1 of that method was the whole job.
 *
 * The template's `register int q0 __asm__("r0")` on __MapActor_SetSpeed is
 * inherited UNCHANGED and deliberately: the template's header records it as
 * measured, and the recorded failure mode for twins is "correcting" a
 * template's levers from the ROM listing, where the instruction ORDER you are
 * reading is the optimiser's rather than the source's.
 *
 * FLAG GROUP CHECKED, as a twin inherits its template's flags along with its
 * shape: the Makefile names no rule for rom_7aa430/ovl_e90_c_c_a_a_c_c, so the
 * template builds at the tree default -O2 and so does this.
 *
 * FOUND BY THE TOOL THAT ALREADY EXISTED. A duplicate twin-finder I wrote this
 * session used a 40-instruction floor and missed this one at 36;
 * solved_twins.py uses 12 and reports it. The duplicate is deleted.
 */
extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_8092b08(int slot, int n);
extern void __Func_809228c(int a, int b, int c);

void OvlFunc_924_2008ffc(int a)
{
    __CutsceneStart();
    __PlaySound(0xe4);
    {
        register int q0 __asm__("r0");
        q0 = 0;
        __MapActor_SetSpeed(q0, 0x6666, 0x3333);
    }
    __Func_8092b08(0, 2);
    __Func_809228c(0, 0, -8);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __CutsceneWait(8);
    __MapActor_SetPos(0, (a << 19) + (0x80 << 12), 0);
    __CutsceneWait(0x1e);
}
