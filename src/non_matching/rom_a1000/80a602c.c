/* Func_80a602c -- 0x080a602c, asm/rom_a1000/rom_a5534_c_a_a.s (6 functions).
 *
 * NOT MATCHING: 41 differing, 76 lines against the ROM's 76 -- LENGTH EXACT.
 * Candidate below.
 *
 * READ src/non_matching/rom_a1000/80a355c.c FIRST -- that is this function's TWIN and
 * carries the shared analysis. Same selection byte, same -1 reset, same glide, same
 * tail. Whatever solves one solves the other.
 *
 * EVERYTHING FROM `L1:` TO `bx r1` -- the last thirty instructions -- IS IDENTICAL. The
 * residue is the preheader, and it is the same `sel*2`-into-the-join-register contest the
 * twin describes.
 *
 * THREE LEVERS DID LAND HERE, 73 differing to 41, and all three are worth keeping:
 *
 * 1. A QImode LITERAL STORE GOES TO THE POOL. The ROM's `ldr r3, =0 / strb r3, [r7, r4]`
 *    is a plain `r7[off] = 0;` -- Thumb's `movqi` has no immediate alternative, so gcc
 *    calls force_const_mem. This is the COMPANION to the known HImode rule, and it is
 *    the proof that the twin's corresponding `mov r3, r8` is an int VARIABLE while this
 *    function's is a LITERAL. The same function also has `mov r5,#0 / strh r5,[r0,#0xc]`
 *    -- a HImode store fed from the int `ret = 0` -- so write the literal there instead
 *    and gcc pools it and shares one register across both stores.
 * 2. `sub r0, #3` FOR 0x219 IS `k -= 3` ON AN INT VARIABLE, and 0x21c must be `0x87 * 4`
 *    in a variable so it survives. As the folded literal `k - 3`, 0x219 goes to the pool
 *    (`ldr r4, =0x219`). But the destructive `k -= 3` then lets combine fold
 *    `ldrb r3, [r7, r2]`, while `k - 3` keeps the ROM's `add r3, r7, r0 / ldrb r3, [r3]`.
 *    Using `k - 3` TOGETHER WITH named pointers for both accesses produced both the
 *    destructive `sub r4, #3` and the materialised `add`: 73 to 46.
 * 3. THE TWO `node` READS MUST RECOMPUTE `+0x14`. `n = which*4 + 0x14` as one expression
 *    is CSEd and gcse'd into a single value kept in r8 -- four instructions where the ROM
 *    has six. Splitting it as `n1 = which * 4; n1 += 0x14;` TWICE lets gcse share only
 *    the inner `which*4`, which is what r8 holds, and leaves the two
 *    `mov r3, r8 / add r3, #0x14` pairs.
 *
 * MEASURED: first candidate 72 at 73 lines; register-offset node load 72; `int ret = 0`
 * for the halfword store 72; named pointers 71 at 74 lines; split `n1 += 0x14` 73 at 74
 * lines; named pointers + `k - 3` + split n1 46 at 76 lines; `idx = sel` join 41;
 * `idx = sel*2` before the call 72; `(idx+sel)*8` explicit 72; `n1 = which; n1 <<= 2` 46;
 * declaration-order and `n1 = n1 + 0x14` variants 41, i.e. no change.
 */
extern unsigned char *iwram_3001f2c;
extern void Func_80a1ac0(int x, int y);
extern unsigned int _GetUnit(unsigned int arg0);
extern unsigned int Func_80a68ec(unsigned int unit, unsigned short *ptr, int flag);
extern int Func_80a60d4(unsigned short *roster, unsigned short *list);
extern void Func_80a17c4(void *node);
extern void WaitFrames(int n);

int Func_80a602c(int which)
{
    int ret = 0;
    unsigned char *r7;
    unsigned char *node;
    unsigned char *p;
    unsigned char *q;
    unsigned char **s;
    unsigned char *t;
    unsigned short *r6;
    int n1;
    int n2;
    int k;
    int sel;
    int off;
    int idx;
    int ofs;
    unsigned int u;

    r7 = iwram_3001f2c;
    n1 = which * 4;
    n1 += 0x14;
    node = *(unsigned char **)((int)r7 + n1);
    node[5] = 1;
    *(unsigned short *)(node + 0xc) = ret;
    k = 0x87 * 4;
    s = (unsigned char **)((int)r7 + k);
    p = *s;
    p[5] = 0xd;
    off = which + 0x1c;
    q = r7 + 2;
    sel = *(signed char *)(r7 + off);
    t = (unsigned char *)((int)r7 + (k - 3));
    q[off] = *t;
    if (sel == -1) {
        r7[off] = 0;
        idx = 0;
    } else {
        idx = sel;
        Func_80a1ac0(idx * 24 - 0xa, 0x10);
    }
    ofs = idx * 2 + 0x82 * 4;
    u = _GetUnit(*(unsigned short *)((int)r7 + ofs));
    r6 = (unsigned short *)(r7 + 0xe4 * 2);
    *(r7 + 0x86 * 4) = (unsigned char)Func_80a68ec(u, r6, 2);
    ret = Func_80a60d4((unsigned short *)(r7 + 0x82 * 4), r6);
    n2 = which * 4;
    n2 += 0x14;
    Func_80a17c4(*(unsigned char **)((int)r7 + n2));
    WaitFrames(1);
    return ret;
}
