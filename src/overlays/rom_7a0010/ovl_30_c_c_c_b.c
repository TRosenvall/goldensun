/* OvlFunc_912_20081c4 -- MATCHING.  Batch 345, brief C.
 *
 *   OK whole file -- 244 bytes, 106 encodings and 14 relocations identical
 *
 * Verify with (the INSTALLED path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7a0010/ovl_30_c_c_c.c \
 *     asm/overlays/rom_7a0010/ovl_30_c_c_c.s --whole
 *
 * SPLIT SHAPE: none needed.  The reference carries exactly one
 * `.thumb_func_start` (OvlFunc_912_20081c4) and no data sections, so this
 * CONVERTS WHOLE: delete asm/overlays/rom_7a0010/ovl_30_c_c_c.s, write this
 * file to src/overlays/rom_7a0010/ovl_30_c_c_c.c.  No exports, no flag group.
 * PINS: 0 (tools/shimcount.py reports no device rows).  No inline asm, no
 * fictitious symbol, no .equ, no union, no volatile.
 *
 * THE RESIDUE WAS: ref 89 16-bit instructions against our 88, 244 bytes against
 * 240, 86 of 106 encodings positional, 95 of 106 aligned-equal.  The gap was
 * ONE instruction and it was in neither arm and neither loop -- it was the
 * `ldrh` before the `== 7` branch.  Three levers closed it, in this order.
 *
 * 1. THE SECOND READ HAS TO BE WRITTEN AGAIN IN THE ELSE ARM.  The ROM reads
 *    the same halfword twice, once unsigned and once signed:
 *
 *        ldrh  r2, [r3]        @ kept live ACROSS the branch
 *        mov   r1, #0
 *        ldrsh r3, [r3, r1]    @ cmp r3, #7
 *
 *    The park read it once into `e` before the branch and used `e` only in the
 *    else arm, so the load SANK into that arm and merged with the `ldrsh`.
 *    Writing the subtrahend's operand as a fresh read of the same address makes
 *    `cse` unify the two and keep the EARLIER one, which is where the ROM has
 *    it.  86 -> 5 differing.  The park's verdict -- "the merge is LEGAL ...
 *    which is why no spelling of the source prevents it" -- is refuted: the
 *    merge is legal and a second textual read still prevents it, because what
 *    was wrong was not the legality but the SINKING.
 *
 * 2. THE FIRST STORE'S DESTINATION NEEDS ITS OWN POINTER LOCAL.  The ROM emits
 *    `lsls r1, r1, #1` (building the 0x1c0 offset) BEFORE `ldr r2, =0x209`;
 *    gcc schedules it after.  Splitting the store into
 *    `pw = (int *)(base + (0xe0 << 1)); v = 0x209; *pw = v;` fixes the order.
 *    5 -> 3 differing.
 *
 * 3. NAME THE SUBTRAHEND.  `s1 = 8;` then `... - s1`.  With a bare `8`,
 *    combine's `force_to_mode` narrows the SImode `(plus e -8)` to the HImode
 *    destination's mask and truncates the constant (the PLUS/MINUS/MULT case
 *    sets `mask = fuller_mask` and then returns `INTVAL (x) & mask` for a
 *    CONST_INT), giving `(plus:SI e 65528)`.  65528 fits no Thumb `addsi3`
 *    immediate -- `*thumb_addsi3`'s `I` is 0..255 and `J` is -255..0 -- so it
 *    cost a POOL WORD and a three-operand add, `ldr r1,=0x0000fff8 /
 *    adds r3,r2,r1`, where the ROM has `adds r3,r2,#0 / subs r3,#8`.  Naming
 *    the 8 keeps the subtraction in SImode; the 0x0000fff8 pool word
 *    disappears and the pool returns to the ROM's four words.  3 -> 0.
 *
 *    This is the SAME batch-153 inverse lever the park already recorded for
 *    the 0x209 store, applied to a subtrahend -- the third constant in this one
 *    function that has to be named.
 *
 * ALSO MEASURED (positional, on the body named):
 *   on the park body, EIGHT spellings of `d = e - 8` are BIT-IDENTICAL at 86 --
 *   `(unsigned short)(e-8)` inline, `d = e; d -= 8`, `d < 2`,
 *   `(unsigned int)d <= 1`, `e == 8 || e == 9`, `e + -8`, `d = (u16)(e-8)`.
 *   on the double-read body: int temp for the halfword 35, int temp for the
 *   difference 37, `s1 -= 8` in place 89, `& 0xffff` instead of the cast 88,
 *   `(short)` round trip 5, `< 2` 5, the shifted-domain compare written out 87.
 *   INSTRUMENT: a `volatile` read 38 -- so volatile is NOT the escape from the
 *   halfword narrowing here; naming the operand is.
 *   prologue: `v` first 5, literal 0x1c0 5, `0xe0 * 2` 5, the offset in the
 *   reused `off` local 105, an anchor on either side 10 and 11.
 *   autosweep: 0 under -fno-gcse, -ffixed-r7, -fno-strict-aliasing,
 *   -fno-strength-reduce, -fno-rerun-cse-after-loop, -fno-rerun-loop-opt and
 *   on 3 of 26 anchor placements -- the match is not flag-dependent.
 *
 * KEPT FROM THE PARK, still true: `v = 0x209;` named in the same block is what
 * stops gcc strength-reducing 0x209 into the offset register.
 *
 * PASS-3 DEBT, labelled: `e = *(unsigned short *)(gState + off);` reads as a
 * DEAD STORE at source level and it is load-bearing -- it is the occurrence
 * `cse` keeps, i.e. the ROM's `ldrh`, and the else arm's identical read is what
 * is folded onto it.  Removing it sinks the load into the else arm and the
 * function goes back to 86 of 106.  Likewise the `pw` pointer local exists only
 * to order two independent insns for the scheduler.  Both are matching
 * artifacts of the kind docs/humanization.md covers; neither is a device
 * (shimcount reports 0) and neither changes the relocations.
 *
 * Source asm: asm/overlays/rom_7a0010/ovl_30_c_c_c.s
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;

extern int __GetFlag(int flag);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int y);

int OvlFunc_912_20081c4(void)
{
    unsigned char *base;
    int off;
    unsigned int i;
    unsigned short e;
    unsigned short d;
    int s1, s2;
    int v;
    int *pw;

    base = iwram_3001ebc;
    pw = (int *)(base + (0xe0 << 1));
    v = 0x209;
    *pw = v;
    if (__GetFlag(0x845) == 0) {
        for (i = 8; i <= 0x16; i++)
            __Actor_SetSpriteFlags(__MapActor_GetActor(i), 0);
    }
    off = 0xe1 << 1;
    e = *(unsigned short *)(gState + off);
    if (*(short *)(gState + off) == 7) {
        s1 = 0xd; s2 = 8;
        __CopyMapTiles(0x22, 0x22, 0x12, 0x10, s1, s2);
        __CopyMapTiles(0x22, 0x5e, 0x12, 0x4c, s1, s2);
        __CopyMapTiles(0x5e, 0x22, 0x4e, 0x10, s1, s2);
    } else {
        s1 = 8;
        d = *(unsigned short *)(gState + off) - s1;
        if (d <= 1) {
            s1 = 0xb; s2 = 8;
            __CopyMapTiles(0x22, 0x2b, 0x13, 0x17, s1, s2);
            __CopyMapTiles(0x22, 0x5e, 0x13, 0x53, s1, s2);
            __CopyMapTiles(0x5e, 0x22, 0x4f, 0x17, s1, s2);
            __MapActor_SetPos(0xa, 0, 0);
            __MapActor_SetPos(0xb, 0, 0);
            __MapActor_SetPos(0xc, 0, 0);
        }
    }
    return 0;
}
