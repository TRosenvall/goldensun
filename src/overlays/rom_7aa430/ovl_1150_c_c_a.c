/* OvlFunc_923_20091b4 -- lands at src/overlays/rom_7aa430/ovl_1150_c_c_a.c
 *
 * THE EMPTY asm SPLITS A SCHEDULING REGION; IT DOES NOT WIN THE TIE-BREAK.
 *
 * Without it the two pool loads come out swapped -- `ldr r2, =0x22b` before
 * `ldr r3, =gState` -- and the literal pool follows, for 5 differing. Both are
 * ready in the same cycle at priority 68 and class 3, so `rank_for_schedule`
 * falls through to DEPENDENT COUNT, and the 0x22b load has three against
 * gState's two:
 *
 *     ;;  45  173  0  3  68  2  core : 61 52        ldr r3, =gState
 *     ;;  77  173  0  3  68  2  core : 61 54 52     ldr r2, =0x22b
 *
 * The extra edge is a REG_DEP_OUTPUT from `mov r2, #3` -- the store's VALUE
 * reusing the offset load's register. That is a register write-after-write, not
 * a memory dependence, so no alias set and no struct tag can reach it: both
 * recorded dependent-count levers move MEM dependences and neither applies.
 *
 * Nor can the count be equalised, because the ROM's own register assignment is
 * what creates it. Measured both ways: every spelling that yields the ROM's
 * `add r3,r2 / mov r2,#3 / strb r2,[r3]` re-creates the output dependence and
 * sits at 5, and every spelling that removes it makes gcc pick the register-
 * offset store `strb rV,[r3,r2]` instead, which puts the value in r1 and costs
 * the `add`. The two are inseparable under sched2.
 *
 * So the barrier does not beat the tie-break, it bypasses it: with the asm
 * between the two statements the gState load is ALONE in the ready list at
 * t=113 (it was `60 58 45 77`) and the 0x22b load only enters at t=146, after
 * the barrier insn. rank_for_schedule is never asked.
 *
 * NO FLAG DOES THIS. -fno-schedule-insns2 fixes these two loads and breaks two
 * other pairs the ROM needs sched2 to produce (`mov r1,#2` before `mov r0,#8`,
 * and `mov r2,#0xe0` interleaved into the `ldr r3,[r3]`), both of which are
 * interleaves no source order can express. Sixteen further flags are inert.
 *
 * _AREA_35, NOT 0x35, and the object comparison cannot show it. area.sym has
 * `_AREA_35 = 0x35;`, so our object holds a relocation placeholder where the
 * ROM's object holds the resolved word -- objcmp reports 1 differing at index
 * 29 with size, instruction count and every other encoding identical. The
 * LITERAL is the evidence the symbol is right: it emits `mov r0, #0x35` for 80
 * bytes against 84, one pool word short, because the ROM pools a value that an
 * eight-bit mov could build. `make compare` is the authority here.
 *
 * The stored value is DERIVED from the offset: the ROM builds 0x1c0 as
 * `mov r2,#0xe0 / lsl r2,#1` and 0x200 as that plus 0x40, so neither is a
 * constant of its own.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern int _AREA_35;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80925cc(int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_8091f90(int a, int b);
extern void __Func_8091eb0(int a, int b);

void OvlFunc_923_20091b4(void)
{
    unsigned char *p;
    unsigned char *gp;

    __CutsceneStart();
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = (0xe0 << 1) + 0x40;
    __Func_8091f90((int)&_AREA_35, 0x1f);
    gp = gState;
    __asm__ __volatile__("");
    gp[0x22b] = 3;
    __Func_8091eb0(0x24, 1);
    __CutsceneEnd();
}
