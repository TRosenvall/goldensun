/* Cluster Func_80b2328..Func_80b2328 extracted from goldensun/asm/rom_b0000/rom_b0070_a_c.s.
 *
 * Total .text for this TU = 444 bytes (= 0x1bc). Never attempted before batch 279.
 * No pins, no flags, no Makefile row -- requires _MSG_cbe, added to message.sym this batch.
 *
 * THE WHOLE FUNCTION TURNS ON THE ROM HOLDING 0xcbe IN r11 and reaching 0xcbf / 0xcc0 / 0xcc1
 * with `add r0, #K`. That one register decision is what forces `slot` to the stack at sp+0xc, and
 * all 141 other differing encodings cascade from it. This is the SAME SHAPE AND SAME MODULE as
 * _MSG_d1c, whose own message.sym entry records it for Func_80b3284 (the inn) -- read the two
 * together.
 *
 * THE PASS TRACE IS ON FILE, which is stronger evidence than this class usually gets:
 *   `.03.cse` ALREADY emits the ROM's shape for a named `int msg = 0xcbe` --
 *     `(set (reg 92) (plus (reg/v 41) (const_int 1)))` with `REG_EQUAL 3263` -- because cse1
 *     keeps the `plus` as the cheaper form.
 *   `.07.gcse` then UNDOES it: "CONST-PROP: Replacing reg 41 in insn 229 with constant
 *     (const_int 3262)", and again at insns 273 and 354, because cprop_insn skips only uses in
 *     the SAME basic block as the def -- so uses 2, 3 and 4 always fold.
 *   With bare literals the relation never forms at all: four independent (set (reg) (const_int)).
 *
 * THE ALTERNATIVE WAS MEASURED AND REJECTED. `-fno-gcse` also reaches exact, but needs a
 * GCSE_CFLAGS Makefile rule AND the array accesses rewritten as named offsets to replace the
 * global CSE that flag removes (ladder: flag alone 37 differing, + named offsets 9, + a
 * declaration reorder 5, + the `int kind` below 0). The symbol needs neither, so it is the
 * cheaper and the more honest of the two.
 *
 * TWO FURTHER LEVERS, both required:
 *   `int kind = info[0xc];` -- the repeated byte read reused for `info[3] & kind`. Without it gcc
 *   emits an extra `mov r3, r0` before the `cmp #2` and uses r0 where the ROM uses r1 (5 to 1).
 *   `unsigned char kind` is 3; casting on the compare instead is 5.
 *
 *   `eq` DECLARED IN A NESTED BLOCK. The ROM puts `eq` at sp+0 and the `slot << 1` temp at sp+4;
 *   a function-scope `eq` gets a pseudo number BELOW the temp's and the two slots swap. Spill
 *   slots go ascending pseudo to descending sp offset, and `expand_decl` numbers function-scope
 *   locals BEFORE any statement -- so AN INNER SCOPE IS THE ONLY WAY TO PUSH A LOCAL PAST A
 *   COMPILER-GENERATED TEMP. That is new, and it is the actionable form of the recorded
 *   "spill-slot order is a consequence" note: it is a consequence of pseudo numbering, and
 *   scope is the one handle on that.
 *
 * NOTE ON THE FALSE-LEAD WARNING: docs/elevation.md's caution about the symbol reading being
 * "right text, wrong bytes" applies to POOLED ZEROS IN MID-FUNCTION POOLS, where an SImode
 * symbol's 1020-byte range moves the pool to the end. This function's pool is already at the
 * end, so the caution does not bite here.
 */
typedef struct { unsigned char pad00[0xd8]; unsigned short items[1]; } Unit;
typedef struct { unsigned char pad00[0x20]; int f20; } State;
typedef struct { unsigned char pad00[0x10]; unsigned int f10; unsigned char pad14[0x2ac]; } GlobalState;

extern unsigned char iwram_3001f2c[];
extern GlobalState gState;
#include "message.h"


extern Unit *_GetUnit(int unit);
extern unsigned char *_GetItemInfo(int item);
extern int _GetEquippedItem(int unit, int kind);
extern int Func_80b20e8(int item);
extern void _Func_8019908(int a, int b);
extern void Func_80b0574(int msg);
extern int Func_80b0634(int a);
extern void Func_80b1dec(int a, int unit);
extern void _Func_8019a54(void);
extern void WaitFrames(int n);
extern void _PlaySound(int sfx);
extern void _RepairItem(int unit, int slot);
extern void _AddCoins(int n);
extern void Func_80b10cc(void);
extern int Func_80b1868(int unit, int slot);
extern void Func_80b196c(int unit, int eq);

void Func_80b2328(int unit, int slot)
{
    State *st;
    Unit *u;
    unsigned char *info;
    int item;
    int cost;
    int saved;
    int msg;
    int kind;

    st = *(State **)iwram_3001f2c;
    u = _GetUnit(unit);
    item = u->items[slot] & 0x1ff;
    info = _GetItemInfo(item);
    {
        int eq;

        eq = _GetEquippedItem(unit, info[2]);
        cost = Func_80b20e8(u->items[slot]);
        kind = info[0xc];
        if (kind != 2) {
            _Func_8019908(item, 2);
            Func_80b0574(0xcba);
            return;
        }
        if ((u->items[slot] & (0x80 << 3)) == 0) {
            _Func_8019908(item, 2);
            Func_80b0574(0xcbb);
            return;
        }
        if ((u->items[slot] & (0x80 << 2)) != 0 && (info[3] & kind) != 0) {
            _Func_8019908(item, 2);
            Func_80b0574(0xcbc);
            return;
        }
        if (cost > gState.f10) {
            Func_80b0574(0xcbd);
            return;
        }
        _Func_8019908(item, 2);
        _Func_8019908(cost, 5);
        msg = MSG_cbe;
        Func_80b0574(msg);
        if (Func_80b0634(0) != 0) {
            Func_80b0574(msg + 1);
            return;
        }
        saved = u->items[slot];
        u->items[slot] = 0;
        Func_80b1dec(st->f20, unit);
        _Func_8019908(item, 2);
        Func_80b0574(msg + 2);
        _Func_8019a54();
        WaitFrames(0xa);
        _PlaySound(0x64);
        WaitFrames(0x6e);
        _PlaySound(0x64);
        WaitFrames(0x6e);
        _PlaySound(0x64);
        WaitFrames(0x6e);
        _PlaySound(0x70);
        WaitFrames(0x14);
        u->items[slot] = saved;
        _RepairItem(unit, slot);
        _AddCoins(-cost);
        Func_80b10cc();
        Func_80b1dec(st->f20, unit);
        _Func_8019908(item, 2);
        Func_80b0574(msg + 3);
        if (Func_80b1868(unit, slot) != 0)
            Func_80b196c(unit, eq);
    }
}
