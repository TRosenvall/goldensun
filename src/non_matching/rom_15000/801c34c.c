/*
 *
 * NON-MATCHING, 17 of 69 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 69, ours 70) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   *** RELOCATIONS ALSO DIFFER -- this figure is NOT a distance. ***
 *   SIZE ref 156 bytes, ours 160.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801c34c.c \
 *     asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_c_c_c.s --func Func_801c34c
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Func_801c34c -- asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_c_c_c.s
 *
 * BLOCKER: two independent residues. 64 lines against 64, 10 differing.
 *
 * RESIDUE A (8 lines) -- prologue schedule, IDENTICAL instruction multiset:
 *   rom  sub sp,#0x14 / ldr r6,[r3] / ldr r2,=gState / mov r3,#8 / mov r1,#0xe0
 *   ours ldr r2,=gState / mov r1,#0xe0 / sub sp,#0x14 / ldr r6,[r3] / ...
 *
 *   MEASURED with -fno-schedule-insns2: the PRE-SCHEDULER order for our
 *   spelling is EXACTLY the ROM's. Our sched2 then hoists both pool loads and
 *   `mov r1,#0xe0` above `sub sp`; the ROM's does not. This is "pool loads come
 *   first" appearing in a PROLOGUE rather than an argument block, and the
 *   function is wholly straight-line -- no conditional branch anywhere -- so
 *   the basic-block lever, the only construct known to reach this shape, has no
 *   boundary to work with.
 *
 * RESIDUE B (2 lines) -- a pooled HImode literal:
 *   rom  mov r1,#0xc8 / mov r3,#0x5a      ours  ldrh r3,=0x5a (pool) / mov r1,#0xc8
 *
 *   0x5a is a fourth counter-example to the narrowed halfword rule that only 0
 *   and values >= 0x8000 pool. Nothing else in the statement pools -- the
 *   offset 0x234 is built `mov #0x8d / lsl #2` -- so the "an adjacent pooled
 *   offset drags it in" explanation used elsewhere does not apply here.
 *
 * TRIED AND REJECTED, all measured:
 *   all six permutations of the three opening statements (10/10/10/11/11/17);
 *   gState through a `short *` (10); `int e = 8; w = e; h = e;` (10); halfword
 *   reads hoisted into locals before the stores (19); a named `int o = 0xe0<<1`
 *   reused for both reads (60 -- breaks it).
 *   For residue B: `int v` beside the store (14); `int v = 0x5a` at the top
 *   (17, adds an r7 push); `unsigned short *` destination (10, still pooled);
 *   `(char)0x5a` (10); `0x5a + 0` (10); named destination pointer (10);
 *   reusing a dead variable as the value carrier (14); a named local for the
 *   StartTask argument (12).
 *
 * Every int-local remedy for B costs four lines in register roles, because the
 * ROM reuses the OFFSET register for the value and a named pseudo cannot.
 */
typedef struct { unsigned char b[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _GetLocationName(int a, int b);
extern void TextBox(int id, int *a, int *b, int *c, int *d);
extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void DrawSmallText(int id, void *w, int x, int y);
extern int StartTask(void *f, int prio);
extern void Func_801c3e8(void);

void Func_801c34c(void)
{
    unsigned char *st;
    unsigned char *g;
    int w, h, tw, th;
    int id;
    void *box;

    st = iwram_3001ebc;
    w = 8;
    h = 8;
    g = (unsigned char *)&gState;
    id = _GetLocationName(*(short *)(g + (0xe0 << 1)), *(short *)(g + (0xe1 << 1)));
    id += 0x99b;
    TextBox(id, &w, &h, &tw, &th);
    w = (0x1e - tw) >> 1;
    h = (0xa - th) >> 1;
    box = CreateUIBox(w, h, tw, th, 2);
    *(void **)(st + (0x8c << 2)) = box;
    DrawSmallText(id, box, 0, 0);
    *(short *)(st + (0x8d << 2)) = 0x5a;
    StartTask(Func_801c3e8, 0xc8 << 4);
}
