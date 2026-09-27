/* Func_80a77a4 -- NON-MATCHING, 8 encodings of 76 differ (objcmp: ref 76 / ours 76; the
 * relocation offsets shift by 2 only because of the mid-function pool's alignment pad).
 * tryc: rom 77 lines, ours 77, first diff at 12, 8 differ.
 *
 * Verify with:
 *   python3 tools/objcmp.py /tmp/claude-0/-home-user-goldensun/ad08b1ee-c1a0-56c8-b3c2-c0ff6481844c/scratchpad/L/Func_80a77a4.park.c \
 *     asm/rom_a1000/rom_a7380_a_c_a_c.s --func Func_80a77a4
 *
 * FRESH TARGET (batch 286).  Its caller Func_80a76d0 (same .s) is EXACT with the same
 * StatusState/Cursor structs, so the two can share a file once this closes.
 *
 * EVERYTHING FROM INSTRUCTION 13 ON IS THE ROM'S, and so is every register role:
 * st r5, r (and the zero stored into cursor+0xc) r6, sel r7, which+0x1c r8, which*4 r10.
 * What differs is 8 instructions in the first block -- ours:
 *     ldrsb r7,[r5,r2] / mov r8,r2 / ldr r2,[r5,r3] (cur) / ... / strb / strh
 * ROM:
 *     ldr r0,[r5,r3] (cur) / ... / strb / strh / ldr r0,win / sub sp / mov r8,r2 / ldrsb
 * i.e. cur is in r0 (ours r2) and sched2 sinks the ldrsb below the two cursor stores.
 *
 * THE LEVERS THAT GOT HERE (each measured):
 *  - `r = 0; cur->timer = r;` -- the zero is the RESULT variable.  That is why the ROM
 *    has `mov r6,#0` in a callee-saved register and `strh r6` instead of a pooled HImode
 *    literal.  A separate int zero gets mov but in r1.
 *  - `sel = st->sel[which];` must be the FIRST statement: sel has to be born while r's
 *    zero is live, or global.c gives sel and r the same r6 (no conflict) and every
 *    call-saved register shifts (76 differ).  All 25 legal orders of the 5 opening
 *    statements measured; only the 5 with sel first reach 8 (r=0 first then sel: 13).
 *  - Cursor and StatusState as STRUCTS (not u8* casts) -- distinct alias sets are what
 *    would let sched2 move the ldrsb past the QImode/HImode cursor stores; with u8*
 *    everything is alias set 0.  Same count here, but it is the only way the ROM order
 *    is reachable at all.
 *  - member-array addressing (`st->cursor[which]`) gives the ROM's
 *    `(which<<2)+0x14` then `ldr [st, off]`.
 *
 * BLOCKER: RTL ORDER vs ALLOCATION.  thumb gcc-2.96 runs NO sched1 (no .sched dump is
 * produced), so allocation sees source order.  cur in r0 needs the which+0x1c temp still
 * live in r2 when cur is loaded (ldrsb after cur in RTL), but sel-after-cur loses the
 * sel/r conflict.  `cur, sel, r=0, stores` gets every call-saved role right and only swaps
 * r0/r2 between the temp and cur -- but then the temp is computed late (`add r0,#0x1c`),
 * 71 differ.  An explicit `idx = which + 0x1c` first makes idx its own pseudo (r8 set at
 * the add, extra copy), 69.
 *
 * INERT: sel as signed char/short/unsigned; const-qualified load; re-reading
 * st->cursor[which] instead of a cur local; a static inline ShowCursor() returning the
 * zero (76); `signed char *psel` pointer (58+).
 */
extern void _Func_8016498(unsigned int win);
extern int _GetFlag(int id);
extern void _Func_801e41c(unsigned int win, int a, int b, int c, int e);
extern void Func_80a1ac0(int x, int y);
extern int Func_80a7d68(void);
extern int Func_80a7a34(void);
extern void WaitFrames(int n);

struct Cursor {
    unsigned char pad0[5];
    unsigned char visible;
    unsigned char pad6[6];
    unsigned short timer;
};
extern void Func_80a17c4(struct Cursor *p);

struct StatusState {
    unsigned char pad0[0x10];
    unsigned int win;
    struct Cursor *cursor[2];
    signed char sel[4];
    unsigned char pad20[0x200];
    unsigned short mode;
};
extern struct StatusState *iwram_3001f2c;

int Func_80a77a4(int which)
{
    struct StatusState *st = iwram_3001f2c;
    struct Cursor *cur;
    int sel;
    int r;

    sel = st->sel[which];
    cur = st->cursor[which];
    r = 0;
    cur->visible = 1;
    cur->timer = r;
    
    _Func_8016498(st->win);
    if (_GetFlag(0x172))
        _Func_801e41c(st->win, 9, 1, 9, 3);
    if (sel == -1)
        st->sel[which] = 0;
    else
        Func_80a1ac0(sel * 24 - 10, 0x10);
    if (st->mode == 3)
        r = Func_80a7d68();
    else
        r = Func_80a7a34();
    Func_80a17c4(st->cursor[which]);
    WaitFrames(1);
    return r;
}
