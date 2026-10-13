/* OvlFunc_925_200835c (0x0200835c) -- MATCHES BYTE-IDENTICAL.  Batch 346, brief B.
 *
 *   OK whole file -- 528 bytes, 206 encodings and 53 relocations identical
 *
 * Production -O2, NO Makefile flag row.  PIN-FREE: tools/shimcount.py reports
 * nothing -- no register pin, no "+r" barrier, no empty asm, no .equ.
 *
 * LANDING SHAPE: NO SPLIT.  asm/overlays/rom_7b0400/ovl_314_c_c_c_a_a_a.s holds
 * exactly ONE function and tools/datacheck.py on it prints nothing (no data
 * section), so split_s.py is not needed and no label needs `.global`.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/overlays/rom_7b0400/ovl_314_c_c_c_a_a_a.c \
 *     asm/overlays/rom_7b0400/ovl_314_c_c_c_a_a_a.s --whole
 *
 * THE ONE RELOCATION THE RAW REFERENCE CANNOT SHOW.  The reference spells the
 * area comparison `ldr r3, =0x3a`, a pooled value gcc would never pool (0x3a
 * fits a Thumb `cmp` imm8), which is the documented symbol tell -- and
 * `_AREA_3a = 0x3a;` is ALREADY in this tree's area.sym:256, used by several
 * landed files.  Against the raw .s this reads `1 of 206`, the pool word alone.
 * Against a symbolised copy (`ldr r3, =_AREA_3a`) it is OK; that copy is the
 * standard move from docs/elevation.md:24311 and is kept as
 * scratch_elev/b346/B/p4_ref_symbolised.s.
 *
 * ==================================================================
 * THE PARK WAS AT 143 of 206 AND ITS VERDICT IS REFUTED.  Re-derived figure:
 * ref 147 16-bit instructions against ours 146, ONE SHORT -- the park's
 * "204 encodings" is the STREAM count, not the instruction count, and the
 * 4-byte size gap is that one instruction plus the pad it displaces.
 * Closing it closed the byte gap with no other change.
 * ==================================================================
 *
 * The park said the blocker was a dead callee-saved r6 and that
 *   "gcc-2.96 will not emit that -- constant propagation finds the zero and
 *    deletes the def in all EIGHT source shapes measured"
 * That is wrong, and the counter-example was in this tree's own LANDED,
 * byte-matching output.  Scanning the GENERATED half of asm/ for a callee-saved
 * register mentioned exactly three times -- push mask, one `mov rX, #imm`, pop
 * mask -- finds exactly two functions, both matching
 * (scratch_elev/b346/B/b_deadsave.py):
 *
 *   OvlFunc_945_200b7b4  rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_a
 *   OvlFunc_969_200871c  rom_7f6e64/ovl_314_c_a_c_c_a_a_a   <- a near-twin of
 *     this function: same SetSpriteFlags/GetActor run, same +0x18 = 0xffff0000
 *     pair, same 0xe1<<1 gState switch, same GetFlag(0x109) tail.
 *
 * LEVER 1 -- THE DEAD REGISTER IS HALF OF A TWO-WORD AGGREGATE.
 * Mechanism and dump citation are in the first of those two files' headers
 * (batch 317 brief D): gcc holds `int bz[2]` as ONE 8-byte pseudo, so a store
 * to one half is not dead while the other half is live -- `.18.greg` prints
 * `32 (2)`, an allocno needing two consecutive hard registers.  That file also
 * records that the pair is forced to (r5, r6) when the COUNTER is in the
 * aggregate, which is exactly this reference's `push {r5, r6, lr}`.  So the
 * actor loop runs on `n[0]` with `n[1] = 0` as the dead half.
 *
 *   A CORRECTION TO THAT CLASS FILE, measured here: it says the dead half must
 *   be READ ("`bz[0]` alone will not keep the pair alive -- cse propagates the
 *   constant out and the whole pseudo dies").  True when the live half is a
 *   propagatable CONSTANT; FALSE when it is a loop counter, which cse cannot
 *   propagate.  Reading it (`n[0] | n[1]`) costs a real `orrs r0, r6` and makes
 *   the body one instruction LONG (148 of 147).  No read: exact.
 *   The INIT ORDER matters: `n[0] = 0xe;` must precede `n[1] = 0;` (reversed is
 *   13 of 206).
 *
 * LEVER 2 -- THE iwram POINTER NEEDS A NAMED LOCAL (`q`), worth 5 encodings.
 * The reference keeps the OFFSET in r2 and the POINTER in r1; the park's
 * `*(int *)(iwram_3001ebc + off)` gets them swapped, which propagates into
 * `str r3,[r1,r2]`, `lsls r2,r2,#1` and the next `ldrsh r2,[r3,r2]`.
 * REG_ALLOC_ORDER is {3, 2, 1, 0, 12, 14, 4, ...} (config/arm/arm.h:989-995),
 * so r3 goes first and r2 second, and which quantity takes r2 is decided by
 * QTY_CMP_PRI (local-alloc.c:1496-1498,
 * floor_log2(n_refs)*n_refs*size / (death - birth) * 10000).  Naming the
 * dereference lengthens the pointer quantity's live range, drops its priority
 * under the offset's, and the offset takes r2.  It also fixes, for free, the
 * zero-index register of the `ldrsh r3,[r3,r1]` four blocks later.
 *
 * LEVER 3 -- THE LAST TWO ENCODINGS ARE AN ALIAS SET, NOT A SCHEDULING TIE.
 * The residue was `str r5, [r0, #0x18]` five slots late, after the `ldrsh` the
 * reference puts it before.  rank_for_schedule's FIRST rung is INSN_PRIORITY
 * (haifa-sched.c:4061-4064), and a store with no dependents can never outrank
 * the `lsls` that feeds the compare -- so the reference's order is only
 * reachable if the store DEPENDS on the load, i.e. if they may alias.  They did
 * not: the store is alias set `int`, the load alias set `short`.
 * lang_get_alias_set returns 0 -- aliases everything -- for a reference made
 * DIRECTLY THROUGH A UNION (c-common.c:3340-3345), so spelling the +0x18 store
 * as a union member restores the dependence.  Confirmed from both directions:
 * `-fno-strict-aliasing` on the non-union body is also 1 of 206.  The union is
 * preferred -- it is one access, where the flag is a whole file.
 *
 * MEASURED INERT OR WORSE (all against the gap-closed body, 11 of 206):
 *   - a third offset local so `off` stops spanning two blocks: 11, inert
 *   - `off = 0xe0 << 1;` as one statement:                     11, inert
 *   - the offset computed above __SetFlag(0x111):              21, worse
 *   - the 0xd actor pointer named and the gState read moved
 *     between its call and its store (to raise the str's LUID): 15, worse
 *   - re-crossed flags on the 6-of-206 body: -fno-gcse,
 *     -fno-rerun-cse-after-loop, -fno-rerun-loop-opt and
 *     -fno-strength-reduce are all 6 (inert); -fno-schedule-insns2 is 27.
 *   - the pair used as the loop BOUND rather than the counter
 *     (`n <= (bz[0] | bz[1])`) also equalises the count at 147/147 and reads
 *     13 of 206 -- but for the wrong reason: loop.c hoists the bound into r6
 *     and the compare becomes `cmp r5, r6` where the ROM has `cmp r5, #0x13`.
 *   - BOTH CHEAP AXES WERE ALREADY CLOSED on the parked body and stay closed:
 *     scratch_elev/b346/B/autosweep_p4.txt, 8 flag groups and 52 of 52 anchor
 *     boundaries, no --max cap.
 *
 * WHAT CLOSED THE REST (carried forward from the park, re-verified): the
 * iwram_3001e70 store needs a NAMED OFFSET LOCAL so 0xc stays a `str`
 * displacement; __CopyMapTiles's 3rd and 6th arguments need ONE SHARED named
 * local (`c4`) so gcc reuses `mov r2,#4` for both the argument and the stack
 * slot.
 */
union ActorWord { int w; };
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern char *iwram_3001e70;
extern int _AREA_3a;
extern int __SetFlag(int id);
extern int __GetFlag(int id);
extern int __StartTask(void (*fn)(void), int prio);
extern void __Func_8092b08(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);
extern int OvlFunc_925_20088cc(void);
extern void OvlFunc_925_200b4bc(void);
extern void OvlFunc_925_200856c(void);
extern void OvlFunc_925_2009af0(void);

int OvlFunc_925_200835c(void)
{
    unsigned int off;
    unsigned int o2;
    unsigned int n[2];
    unsigned char *a;
    unsigned char *q;
    int t;
    int v;
    int c4;

    __SetFlag(0x111);
    q = iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    *(int *)(q + off) = 0x81 << 2;
    if (*(short *)((char *)&gState + off) == (int)(&_AREA_3a)) {
        __SetFlag(0xa2 << 1);
        __StartTask(OvlFunc_925_200b4bc, 0xc8 << 4);
        __Func_8092b08(0, 1);
        __Func_8092b08(1, 1);
        __Func_8092b08(2, 1);
        __Func_8092b08(3, 1);
        __Func_8092b08(5, 1);
        __Func_8092b08(0x14, 1);
        __Func_8092b08(0x15, 1);
        __Func_8092b08(0x16, 1);
        __Func_8092b08(0x17, 1);
        __Func_8092b08(0x18, 1);
        __Func_8092b08(8, 1);
        __Func_8092b08(9, 1);
        __Func_8092b08(0xa, 1);
        __Func_8092b08(0xb, 1);
        __Func_8092b08(0xc, 1);
        __Func_8092b08(0xd, 1);
        n[0] = 0xe;
        n[1] = 0;
        do {
            __Func_8092b08(n[0], 1);
            __MapActor_GetActor(n[0])[0x55] = 4;
            __MapActor_GetActor(n[0])[0x23] |= 2;
            *(int *)(__MapActor_GetActor(n[0]) + 0xc) = 0xffcd8000;
            n[0]++;
        } while (n[0] <= 0x13);
        if (__GetFlag(0x109) != 0) {
            t = OvlFunc_925_20088cc();
            if (t != 0) {
                a = __MapActor_GetActor(t);
                if (a != 0)
                    a[0x55] = 0;
            }
        }
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xd), 0);
        ((union ActorWord *)(__MapActor_GetActor(0xc) + 0x18))->w = 0xffff0000;
        ((union ActorWord *)(__MapActor_GetActor(0xd) + 0x18))->w = 0xffff0000;
        off = 0xe1;
        off <<= 1;
        v = *(short *)((char *)&gState + off);
        if (v == 1) {
            if (__GetFlag(0x109) == 0)
                OvlFunc_925_200856c();
        } else if (v == 2) {
            if (__GetFlag(0x251) == 0) {
                o2 = 0xb2;
                o2 <<= 1;
                *(int *)(iwram_3001e70 + o2 + 0xc) = 0x80 << 19;
                __Func_800fe9c();
                __WaitFrames(1);
                c4 = 4;
                __CopyMapTiles(4, 0x46, c4, 0x4a, 5, c4);
                __MapActor_SetPos(9, 0, 0);
                if (__GetFlag(0x109) == 0)
                    OvlFunc_925_2009af0();
            }
        } else if (v == 5) {
            __SetFlag(0x251);
        }
    }
    return 0;
}
