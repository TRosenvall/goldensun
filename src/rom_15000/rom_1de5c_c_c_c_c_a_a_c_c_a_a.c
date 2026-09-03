/* Func_801faa8  --  0x0801faa8
 *
 * The whole of goldensun/asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_c_a_a.s: one
 * function, no data.
 *
 * A save-write. Allocate a 0x1000 scratch, bail with -1 if no slot is selected,
 * then run the write and report the first failure as a negative code: -9 if the
 * pre-check fails, -2 if the body write fails, -3 if the header write fails, 0
 * if all of it succeeded. The scratch is freed on every path but the -1 one.
 *
 * MATCHED WITHOUT THE FLAG, though filtered.py offered it as [cse]. That marker
 * fired on the two `ldr r0, =_MSG_0b` sites, and they are in arms that cannot
 * reach one another, which is the case rerun-CSE does not common. Second
 * confirmation that [cse] is a hint rather than a verdict; the first was
 * OvlFunc_932_200a934 last batch.
 *
 * THE INDIRECT CALL is the shape src/rom_c9000/rom_e0524.c opened: the ROM does
 * not `bl Func_8001af8`, it loads the address and goes through the interworking
 * veneer, which is what gcc emits for a call through a POINTER. Assigning the
 * callee to a local of function-pointer type reproduces it, because gcc-2.96
 * does not constant-propagate the pointer back into a direct call at -O2.
 *
 * TWO PLACEMENT QUESTIONS DECIDED THE LAST FIVE INSTRUCTIONS, and both are
 * recorded levers landing on the same statement.
 *
 * ASSOCIATION. The ROM computes `add r0, r5, r1 / sub r0, r3` -- that is
 * `(buf + &field) - &base`, grouped left to right. Written as
 * `buf + (&field - &base)` gcc folds the two symbol addresses together first
 * and emits `sub r0, r1, r0 / add r0, r5`, the same three instructions in the
 * wrong order. Splitting it into two statements over a named pointer gives the
 * ROM's grouping directly. 5 differing to 3.
 *
 * LIVE RANGE. The last three were the veneer register: ours took r4 and the ROM
 * takes r3, and r3 is the FIRST entry in REG_ALLOC_ORDER, so the ROM's pointer
 * pseudo has the higher priority and therefore the shorter live range.
 * Assigning `copy = Func_8001af8;` immediately before the call rather than
 * ahead of the address arithmetic is the whole fix.
 *
 * The control here is worth keeping: declaring the pointer inside the block
 * WITH AN INITIALISER measures 3, the same as assigning it early, because an
 * initialiser puts the definition at the top of its scope. Same value, same
 * instruction, one register worse. That is the batch-182 initialiser lever
 * showing up on a function pointer, and it is now the cheapest thing to try
 * whenever a `_call_via_rN` picks the wrong N.
 */
typedef void (*CopyFn)(void *src, void *dst, int n);

extern short ewram_2002004;
extern unsigned char ewram_20004e4[];
extern unsigned char ewram_2000000[];
extern int _MSG_0a;
extern int _MSG_0b;

extern void Func_8001af8(void *src, void *dst, int n);
extern void *Func_8004970(int n);
extern int Func_80056cc(void);
extern int Func_8005a78(int slot, void *buf);
extern int SomethingSaveHeader(int slot, void *buf);
extern void Func_801776c(int msg, int b);
extern void Func_8005cf8(void);
extern void free(void *p);

int Func_801faa8(void)
{
    void *buf;
    int rc;
    CopyFn copy;

    buf = Func_8004970(0x80 << 5);
    rc = 0;
    if (ewram_2002004 == -1)
        return -1;
    if (Func_80056cc()) {
        Func_801776c((int)&_MSG_0a, 1);
        rc = -9;
    } else {
        if (Func_8005a78(ewram_2002004, buf)) {
            Func_801776c((int)&_MSG_0b, 1);
            rc = -2;
        }
        { char *s = (char *)buf + (int)ewram_20004e4; s -= (int)ewram_2000000; copy = Func_8001af8; copy(s, ewram_20004e4, 0x10); }
        if (SomethingSaveHeader(ewram_2002004, buf)) {
            Func_801776c((int)&_MSG_0b, 1);
            rc = -3;
        }
    }
    Func_8005cf8();
    free(buf);
    return rc;
}
