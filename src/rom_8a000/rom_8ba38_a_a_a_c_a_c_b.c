/* Func_808c30c -- 0x0808c30c
 *
 * Applies an HP change to the whole party: play the appropriate cue, then for
 * each member either apply the flat amount or scale it by that member's max HP,
 * falling back to the flat magnitude when the scaled result rounds to zero.
 *
 * ONE LEVER CARRIED THIS, AND IT IS NEW: INVERTING THE TEST SO THE FALLBACK IS
 * THE `THEN` ARM. Everything about the ROM reads as `d = amount; if (scaled)
 * { ... }` -- `mov r1, r5` sits above `cmp r3, #0`, exactly the shape gcc emits
 * when it hoists a single-instruction else arm. Written that way the function
 * is 15 differing, and the visible fault is three instructions: the multiply
 * reads the COPY where the ROM reads the parameter.
 *
 * With `d = amount` dominating the multiply, CSE merges the two into one
 * quantity and rewrites the multiply's operand. That is not cosmetic -- it
 * drops `amount`'s in-loop reference count from three to two, which demotes it
 * below the walking pointer and the counter in the allocator's priority order
 * and ROTATES r5/r6/r7 THROUGH THE WHOLE FUNCTION. Fifteen differences from one
 * substitution.
 *
 * Writing the fallback as `if (scaled == 0) { d = amount; } else { ... }` keeps
 * that assignment off the multiply's dominator path. gcc still hoists the
 * one-instruction arm above the compare, so the EMITTED shape is byte-identical
 * to the other polarity -- but the RTL operand stays `amount` and the whole
 * allocation falls into place. Exact.
 *
 * THE COROLLARY WORTH FILING: when a two-operand `mul` (or any destructive op)
 * sources a COPY where the ROM sources the ORIGINAL, that is CSE quantity
 * merging, and THE FIX IS CFG-SHAPED, NOT EXPRESSION-SHAPED. And when a hoisted
 * single-instruction arm is visible in the ROM, BOTH POLARITIES must be
 * screened -- the emitted code does not tell you which one the source used.
 *
 * A FLAG SWEEP WAS RUN AND REJECTED, which matters because the residue looks
 * exactly like a -fno-gcse park: -fno-gcse, -fno-cse-follow-jumps,
 * -fno-cse-skip-blocks, -fno-thread-jumps, -fno-expensive-optimizations,
 * -fno-regmove and -fno-rerun-cse-after-loop all leave it at 14 or 15, and all
 * four CSE flags together only reach 12. No flag reaches what the source
 * inversion reaches. Do not add a Makefile rule for this shape.
 *
 * The loop index form `gState[(0xfc << 1) + i]` is what puts the base build in
 * the loop preheader after the guard rather than in the entry block.
 *
 * ON THE HIGH-REGISTER PREDICTION: templated.py's four r8-r11 references were a
 * false alarm. r8 is simply the second parameter, live across five calls; a
 * plain `int` parameter puts it there with no coaxing. Nothing was pinned.
 *
 * Forty-two variants were screened to get here; the losing forty all assumed
 * the fallback assignment precedes the test in source order.
 *
 * Verified with tools/objcmp.py: 152 bytes, 65 encodings and 10 relocations
 * identical.
 */
extern unsigned char gState[];
extern int _GetPartySize(void);
extern void *_GetUnit(int id);
extern void _ModifyHP(int id, int delta);
extern void Func_8091220(int a, int b);
extern void Func_8091254(int a);
extern void _PlaySound(int id);

void Func_808c30c(int amount, int scaled)
{
    char *u;
    int n;
    int i;
    int d;

    if (amount < 0) {
        Func_8091220(0x1ff, 0);
        Func_8091254(4);
        if (amount < -0xa)
            _PlaySound(0x86);
        else
            _PlaySound(0x85);
    } else {
        _PlaySound(0x7e);
    }
    n = _GetPartySize();
    for (i = 0; i < n; i++) {
        u = (char *)_GetUnit(gState[(0xfc << 1) + i]);
        if (scaled == 0) {
            d = amount;
        } else {
            d = *(short *)(u + 0x34) * amount / 0x64;
            if (d == 0) {
                d = amount;
                if (d < 0)
                    d = -d;
            }
        }
        _ModifyHP(gState[(0xfc << 1) + i], d);
    }
}
