/* Func_80a355c -- 0x080a355c, asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_a.s (2 functions).
 *
 * NOT MATCHING: 45 differing, 70 lines against the ROM's 69. Candidate below.
 *
 * ==================== READ THIS WITH ITS TWIN ====================
 *
 * src/non_matching/rom_a1000/80a602c.c IS THE SAME FUNCTION with different callees --
 * same `state + 0x1c + which` selection byte, same `-1` reset, same Func_80a1ac0 glide,
 * same 0x208/0x1c8/0x218 triple, same Func_80a17c4 + WaitFrames(1) tail. WHATEVER
 * SOLVES ONE SOLVES THE OTHER. Work them as a pair; do not spend a round on one.
 *
 * ONE RESIDUE, TWO SYMPTOMS: `off` and `sel` are swapped between r5 and r6, and every
 * later difference follows from it. From .18.greg the global order is `34 38 39 37 33 32`
 * with 38 (`off`) taking r5 and 39 (`sel`) r6; the ROM needs 39 first. allocno_compare
 * gives 38 `floor_log2(4)*4/30 = 0.267` against 39's `8/34 = 0.235`, so 38 is processed
 * first and takes r5 -- the first callee-saved LO register, r4 being call-used under
 * -fcall-used-r4.
 *
 * SO THE ROM'S ALLOCATION NEEDS A REFERENCE COUNT THIS SOURCE DOES NOT HAVE: `off` at 3
 * refs or `sel` at 6 or more. Both are 4 in the ROM's own shape. That is a priority
 * contest, not a tie, so batch 272's declaration-order lever does not apply -- and the
 * measurements confirm it, with declaration-order swaps all exactly 45.
 *
 * THE SHARED UNREACHED INSTRUCTION, stated precisely because it is what to attack: gcc
 * must compute `sel * 2` into the callee-saved join register AND use `sel` itself for the
 * later `(sel*2 + sel) << 3` product. `idx = sel;` gets the join register right and costs
 * one `mov` on the multiply (48); `idx = sel * 2;` gets the multiply right and costs one
 * `mov` on the join (45). They are one instruction apart in OPPOSITE directions and
 * eleven spellings measured identically, so it is not a spelling -- it is which pseudo
 * regmove coalesces with `sel`.
 *
 * MEASURED (70 lines against 69 unless noted): cy-style locals 55; named `q = r7 + 2`
 * plus `(int)r7 + off` for the guard load 48; `idx = sel` join 48; `sel` direct with
 * `idx = sel * 2` 45; no `ret = 0`, storing the literal instead 58; `int ret = 0` with
 * `sel = 0` as the join 47; `r7[which+0x1e]` in place of `q[off]` 59 and 61 (71 lines);
 * `off` computed before `r7 = iwram_3001f2c` 45 and 48, i.e. no change. ALL EXACTLY 45,
 * NO CHANGE: declaration-order swaps of sel/off/ret, `(int)r7 + off` against
 * `off + (int)state`, `0x82*4 + idx` against `idx + 0x82*4`, an `off2 = off` copy split,
 * and reordering the if-branch stores.
 *
 * TWO THINGS THIS DID PIN DOWN:
 *   - `_Func_80164ac` takes ONE argument, and the `mov r1,#0 / mov r8,r1` before the
 *     call is `int ret = 0;` -- a declared-and-initialised int whose register is later
 *     reused for the return value. A literal `r7[off] = 0` does not produce it.
 *   - `int x = 0;` declared and initialised at the top is a RECURRING rom_a1000 source
 *     shape: it is what puts a zero in a callee-saved register before the first call.
 */
extern unsigned char *iwram_3001f2c;
extern void _Func_80164ac(unsigned int arg0);
extern void Func_80a1ac0(int x, int y);
extern unsigned int _GetUnit(unsigned int arg0);
extern unsigned int Func_80a3ddc(unsigned int unit, unsigned short *ptr, int flag);
extern int Func_80a35f8(unsigned short *roster, unsigned short *list);
extern void Func_80a17c4(void *node);
extern void WaitFrames(int n);

int Func_80a355c(int which)
{
    int ret = 0;
    unsigned char *r7;
    unsigned char *q;
    unsigned short *r6;
    int idx;
    int off;
    int sel;
    int ofs;
    int ofs2;
    unsigned int u;

    r7 = iwram_3001f2c;
    off = which + 0x1c;
    sel = *(signed char *)(r7 + off);
    _Func_80164ac(*(unsigned int *)(r7 + 0x2c));
    q = r7 + 2;
    q[off] = r7[0x219];
    if (sel == -1) {
        r7[off] = ret;
        idx = 0;
    } else {
        Func_80a1ac0(sel * 24 - 0xa, 0x10);
        idx = sel * 2;
    }
    ofs = idx + 0x82 * 4;
    u = _GetUnit(*(unsigned short *)((int)r7 + ofs));
    r6 = (unsigned short *)(r7 + 0xe4 * 2);
    *(r7 + 0x86 * 4) = (unsigned char)Func_80a3ddc(u, r6, 0);
    ret = Func_80a35f8((unsigned short *)(r7 + 0x82 * 4), r6);
    ofs2 = which * 4 + 0x14;
    Func_80a17c4(*(void **)((int)r7 + ofs2));
    WaitFrames(1);
    return ret;
}
