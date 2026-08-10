/* OvlFunc_922_2009750  [ovl_7a8c8c]
 *
 * Source asm: goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_c_a.s
 *
 * NOT SPLIT, and it would not need one -- the .s holds only this function.
 *
 * Writes a mode word into the iwram block, then in one area sets a flag and
 * takes one branch, otherwise takes another. Thirty instructions against
 * thirty-three.
 *
 * THE POOL TELL AND THE AREA ID ARE BOTH FINE. `ldr r3, =0x34` where
 * `cmp r2, #0x34` would encode is the pool tell, the value is read from
 * gState + 0x1c0, and _AREA_34 was already in area.sym. That part reproduces.
 *
 * Blocker: CONSTANT-CSE ACROSS A CALL, on the OFFSET rather than on a flag id --
 * and it is a COUNTER-EXAMPLE TO THE RULE recorded in
 * src/non_matching/overlays/constant_reuse.c.
 *
 * The offset 0x1c0 is built twice in the ROM, once for the iwram pointer and
 * once for gState, with the __GetFlag call between them:
 *
 *     rom    mov r1, #0xe0 / lsl r1, #1 / add r3, r1 / str r2, [r3]
 *            ... bl __GetFlag ... mov r2, #0xe0 / lsl r2, #1 / add r3, r2
 *
 *     ours   mov r5, #0xe0 / lsl r5, #1 / str r3, [r2, r5]
 *            ... bl __GetFlag ... (r5 still live)
 *
 * gcc computes it once, keeps it in r5 across the call -- paying a push and a
 * pop -- and then uses register-offset addressing instead of folding the offset
 * into the pointer, which is where the three missing instructions go.
 *
 * That is repetition SEPARATED BY A CALL, which is exactly the shape the rule
 * says -fno-rerun-cse-after-loop handles. IT DOES NOT HELP HERE: byte-identical
 * with and without the flag. So the rule is narrower than "across a call" --
 * the CSE of this offset happens in a pass the flag does not disable.
 *
 * The distinction may be that the flag-responsive cases all repeat a value used
 * as a CALL ARGUMENT, while this one repeats a value used in ADDRESS
 * ARITHMETIC. That is a guess from three cases against one and is written here
 * as a lead rather than a finding.
 *
 * TRIED:
 *   1. the gState work before the `if`, as an `&&` chain -- 30 lines, 31 differ
 *   2. the gState work inside the `if` so it is only reached on the second test
 *      (the form below) -- 30 lines, 32 differ
 *   3. both of the above with --no-rerun-cse -- byte-identical to without
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_34;
extern unsigned int iwram_3001ebc;

int OvlFunc_922_2009750(void)
{
    unsigned char *base;
    unsigned char *g;
    unsigned int off;
    unsigned int o2;

    base = (unsigned char *)iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    base += off;
    *(unsigned int *)base = 0x81 << 2;
    if (__GetFlag(0x109) == 0) {
        g = (unsigned char *)&gState;
        o2 = 0xe0;
        o2 <<= 1;
        g += o2;
        o2 = 0;
        if (*(short *)(g + o2) == (int)(&_AREA_34)) {
            __SetFlag(0xa2 << 1);
            OvlFunc_922_20097a8();
            return 0;
        }
    }
    OvlFunc_922_20097e4();
    return 0;
}
