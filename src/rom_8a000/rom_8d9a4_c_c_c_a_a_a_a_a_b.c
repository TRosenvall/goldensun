/* Func_80903bc -- 0x080903bc
 *
 * A per-frame interpolation task: step a counter towards its limit, place the
 * interpolated value, and drive two window registers from it -- retiring
 * itself and clearing the scanline handler once the counter runs out.
 *
 * THE FINDING WORTH KEEPING: a main-ROM function CAN legitimately reach the
 * RAM-resident divide. `bl _call_via_r3` here is a call through an explicit
 * function-pointer local holding `divsi3_RAM` -- NOT the `/` operator. Writing
 * the division would emit `__divsi3` and run into the alias blocker that parks
 * the neighbour src/non_matching/rom_8a000/809088c.c. When the ROM reaches the
 * helper INDIRECTLY, a pointer local is the honest spelling and the blocker
 * does not apply. That park is worth re-reading with this in mind.
 *
 * NAME THE CALL'S ARGUMENT TO CHOOSE THE VENEER REGISTER. With the multiply
 * written inline in the call, the function pointer took r4, which pushed the
 * second global into r8 and cost a `push {r7} / mov r7, r8` pair plus
 * high-register two-address adds. Hoisting the product into its own statement
 * before the pointer assignment selected `_call_via_r3` and freed r4:
 * 88 differing to 2. Naming the OTHER argument as well buys nothing -- only the
 * product needs it.
 *
 * THE LAST TWO LINES WERE A RETURN TYPE. `extern void StopTask(void *)` -- the
 * spelling every sibling in src/rom_8a000/ uses -- fills r0 first; `extern int
 * StopTask(void *)` fills it last, which is the ROM's order. The callee's
 * return type decides r0 fill order for a DIRECT call too, not just an indirect
 * one. Introducing a named zero for the adjacent store was tried instead and
 * changed nothing, which confirms the documented negative that source birth
 * order does not reach an operand of one statement.
 *
 * Two more on file: the second global is DERIVED from the first at a fixed
 * distance off a single `extern unsigned char iwram_3001ecc[]` (two externs
 * would pool two addresses), and the two narrow literal stores go through a
 * TYPED STRUCT FIELD -- the cast form pools 0xc8 and 0xfa where the ROM builds
 * them with `mov`, and the struct also gets the address build for free.
 *
 * `ldrb / add #1 / strb / lsl #24 / asr #24` is a PRE-increment of a signed
 * char used as a value; operand order matters, since putting it first in the
 * multiply evaluates it early and lengthens its live range.
 *
 * Verified with tools/objcmp.py: 204 bytes, 90 encodings and 6 relocations
 * identical.
 */
struct G {
    unsigned char pad[0x100];
    unsigned short a;
    unsigned short b;
};

extern unsigned char iwram_3001ecc[];
extern int StopTask(void *task);
extern void SetIntrHandler(int a, int b, void (*f)(void));
extern int divsi3_RAM();

void Func_80903bc(void);

void Func_80903bc(void)
{
    int (*fp)(int, int);
    signed char *t;
    struct G *g;
    int n;
    unsigned int v;

    t = *(signed char **)iwram_3001ecc;
    g = *(struct G **)(iwram_3001ecc - 0x5c);
    if (t[0x53c] != 0) {
        if (t[0x53d] >= t[0x53c]) {
            t[0x53c] = 0;
            StopTask(Func_80903bc);
            SetIntrHandler(1, 0, 0);
            return;
        }
        n = (t[0x53b] - t[0x53a]) * ++t[0x53d];
        fp = divsi3_RAM;
        n = fp(n, t[0x53c]);
        *(unsigned short *)(t + 0x52a) = t[0x53a] + n;
    }
    v = *(unsigned short *)(t + 0x52a);
    if (v > 0x4f) {
        g->a = 0xc8;
        g->b = 0xfa;
    } else {
        g->a = v;
        g->b = 0x9f - v;
    }
}
