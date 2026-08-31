/* Func_808d828 -- 0x0808d828  (asm/rom_8a000/rom_8d5dc_c.s)
 *
 * BLOCKER: one argument-setup transposition. 4 of 94, EXACT length, and it
 * needs ALIAS_CFLAGS (-fno-strict-aliasing) -- see below, that flag is worth
 * more than the residue.
 *
 * PROGRESSION, each step isolated:
 *
 *   74  naive
 *   71  `ret = -1` assigned before the gState pointer; an `int` local for the
 *       0 stored to a HALFWORD (a bare literal there POOLS: `ldr r3, =0x0`
 *       against the ROM's `mov r3, #0`)
 *    7  -fno-strict-aliasing            <-- the whole difference
 *    4  read the event word into a local BEFORE materialising the mask
 *
 * THE ALIASING FLAG IS THE FINDING. The ROM RELOADS `e[2]` after storing a
 * halfword through the gState pointer:
 *
 *     rom   strh r3, [r2, #0] / ldr r1, [r5, #0x8] / ...
 *     ours  strh r3, [r2, #0] / (no reload -- the old r1 is reused)
 *
 * At -O2 gcc-2.96 has strict aliasing on, so a `short` store cannot alias an
 * `int` read and the load is commoned away. That single decision cost 61
 * instructions of divergence -- everything after it shifted. With
 * -fno-strict-aliasing the reload appears and the function goes from 68
 * differing to 7.
 *
 * **A missing RELOAD after a store of a different width is an aliasing tell,
 * not a codegen mystery.** It is cheap to test and the payoff is large; this is
 * the shape to check before spending screens on the instructions around it.
 *
 * IT ALSO REFINES A DOCUMENTED "UNREACHABLE". docs/elevation.md's "INVERSE
 * constant problem" says a `sub rN, #K` applied to a pooled constant is the ROM
 * deriving one offset from another and is not reachable, because writing
 * `off = A; ... off -= K;` folds at each use. The ROM here does exactly that:
 *
 *     mov r2, #0x80 / lsl r2, #2 / and r3, r2 / ... / sub r2, #0x64
 *
 * and `m = 0x80 << 2; if ((f & m) != 0) { m -= 0x64; ... }` REPRODUCES IT.
 * The difference from the parked case is that here the value has a REAL USE --
 * as the mask of the `and` -- between its definition and the subtraction, so it
 * is one live variable being mutated rather than two compile-time constants
 * that gcc can fold independently. The rule should read: deriving is
 * unreachable when both values are dead constants, and reachable when the first
 * one is genuinely consumed first.
 *
 * WHAT REMAINS, four instructions at one site:
 *
 *     rom   mov r3, #0xb8 / ldr r2, [r5, #0x8] / lsl r3, #1
 *           / add r3, r8 / strh r2, [r3, #0]
 *     ours  ldr r2, [r5, #0x8] / mov r3, #0xb8 / lsl r3, #1
 *           / mov r1, r8 / strh r2, [r1, r3]
 *
 * The ROM slots the load between the `mov` and the `lsl`, and folds the base
 * into the address; we emit the load first and address with reg+reg. Measured,
 * none better than 4: splitting the shift around a named load (4, unchanged);
 * naming the store address in a pointer local (5); mutating the offset variable
 * into the address, `k += (int)g` (5); both together (5). The interleave here is
 * a LOAD rather than a constant build, and the entry-block lever does not move
 * it.
 *
 * INSTALL NOTE: this TU needs an ALIAS_CFLAGS rule in the Makefile when it is
 * elevated. It is parked rather than installed because 4 differing is not a
 * match, and adding a flag rule for a non-matching TU would be misleading.
 */
extern int iwram_3001ebc;
extern void *FindMapActorEvent(int kind, int arg);
extern void CutsceneStart(void);
extern void CutsceneEnd(void);
extern void MessageID(int id);
extern void ActorMessage(int a, int b);
extern void _PlaySound(int id);
extern void Player_EnterStairsUp(void);
extern void Player_EnterStairsDown(void);

int Func_808d828(int arg)
{
    int *e;
    char *g;
    int ret;
    int m;
    int v;
    int z;
    unsigned short *q;
    int f;
    int k;

    e = (int *)FindMapActorEvent(2, arg);
    k = 0xb8 << 1;
    ret = -1;
    g = (char *)iwram_3001ebc;
    if (e != 0 && e[2] != 0) {
        f = e[0];
        m = 0x80 << 2;
        if ((f & m) != 0) {
            m -= 0x64;
            q = (unsigned short *)(g + m);
            z = 0;
            *q = z;
        }
        if (e[2] < (0x80 << 9)) {
            CutsceneStart();
            MessageID(e[2]);
            ActorMessage(ret, 0);
            ret = 0;
            CutsceneEnd();
        } else {
            ((void (*)(int))e[2])(arg);
            ret = 0;
        }
    } else {
        e = (int *)FindMapActorEvent(1, arg);
        if (e != 0) {
            v = e[0] & 0x30;
            switch (v) {
            case 0:
                _PlaySound(0x7b);
                break;
            case 0x20:
                _PlaySound(0x80);
                Player_EnterStairsUp();
                break;
            case 0x30:
                _PlaySound(0x81);
                Player_EnterStairsDown();
                break;
            }
            *(unsigned short *)(g + k) = e[2];
            ret = 0;
        }
    }
    return ret;
}
