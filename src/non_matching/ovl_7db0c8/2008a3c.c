/* OvlFunc_954_2008a3c -- NON-MATCHING, 17 encodings of 367 against the tree
 * reference; 16 against a symbolised copy.  SIZE AND RELOCATIONS EXACT.  348
 * instructions.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7db0c8/2008a3c.c \
 *     asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_a_a.s
 * ONE function, no data section -- CONVERTS WHOLE when it lands.
 *
 * ================================================================
 * SYMBOL TELL -- `_FILE_e4 = 0xe4;` REPORTED AND WITHHELD, with the strongest
 * control this bank can construct
 * ================================================================
 *
 * This function builds TWENTY-THREE distinct values under 0x100 with `mov r0, #N`
 * (0xa, 0xd, 0xe, 0xf, 0x11-0x19, 0x24, 0x25, 0x2b, 0x2f, 0x3f, 0x40, 0x64, 0x7f,
 * 0xa2, 0xc4) and pools EXACTLY ONE: `ldr r0, =0xe4`.  That value is the argument to
 * OvlFunc_common1_1fb4, whose landed C (src/overlays/common/common1_c_a_c_c_b.c)
 * passes it straight to __GetFile(arg) -- so this is the FILE id space, checked
 * rather than assumed.  Two sibling callers pass 0xe5 and 0xe6, both pooled, and
 * _FILE_e6 was ADDED in batch 282 on this same evidence (it completed its function),
 * while _FILE_e7 and _FILE_e8 were already in file_table.sym.
 *
 * Measured: with the symbol 16 of 367 and relocations exact; without it 17 plus a
 * relocation mismatch.  WITHHELD BECAUSE IT DOES NOT COMPLETE THE FUNCTION -- the
 * separation batch 281 drew between evidence quality and completion, and this is
 * better-evidenced than _MSG_d27, which went in.
 *
 * ================================================================
 * WHAT PAID, and one construct that became inert
 * ================================================================
 *
 * NAMING THE 5TH AND 6TH ARGUMENTS OF THE 6-ARG MAP ROUTINES.  The ROM computes both
 * into two DISTINCT registers then stores both; gcc reuses one (`mov/str/mov/str`)
 * and, where a stack argument repeats a register argument's value, CSEs them into
 * one.  Dropping the `sa`/`iv2` carriers costs 117 (16 -> 133); dropping the
 * `two`/`twelve` carriers costs 268 (16 -> 284).  A repeated value shares ONE local
 * across two calls -- visible in the ROM as one callee-saved register spanning both.
 *
 * A BESPOKE SINGLE-REGISTER PIN IS A DIFFERENT TOOL FROM PIN4.  `PIN4` pinned r0-r3
 * and pushed the stack arguments into r4 (under -fcall-used-r4); a lone
 * `register int t3 __asm__("r3")` left r2 free and got the ROM's register.  PIN4 on a
 * call WITH STACK ARGUMENTS was catastrophic twice here (16 -> 164, 16 -> 207).
 *
 * AND THAT t3 PIN IS NOW INERT AND IS NOT SHIPPED.  It was worth 33 -> 24 when
 * introduced, and a later change -- computing the two `>> 20` values at their loads
 * rather than shifting at the call, 22 -> 16 -- removed the pressure that made it
 * pay.  THIRD INSTANCE THIS BATCH OF A CONSTRUCT WHOSE VALUE WENT TO ZERO AFTER AN
 * UNRELATED STRUCTURAL CHANGE.  Re-run the ladder after every one.
 *
 * Loop-setup statement order was swept, not deduced.
 *
 * No per-file Makefile flag override applies to this stem.
 *
 * NEXT: settle the _FILE_e4 decision, which is worth 1 encoding and the relocation
 * set; then the remaining 16 want .23.sched2 rather than spellings.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern int _FILE_e4;

extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern int __GetFlagByte(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetExtra(int slot, int v);
extern int __Func_8011f54(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __DeleteFieldActor(int slot);
extern void __PlaySound(int id);
extern int __StartTask(void (*fn)(void), int n);
extern short __Func_8091e9c(short n);
extern void OvlFunc_954_200804c(void);
extern void OvlFunc_954_2008a10(int a);
extern void OvlFunc_954_2008974(int a);
extern void OvlFunc_954_2008db8(int a);
extern void OvlFunc_common1_0(void);
extern void OvlFunc_common1_78(int a);
extern void OvlFunc_common1_148(void);
extern void OvlFunc_common1_488(void);
extern void OvlFunc_common1_ea0(int a);
extern void OvlFunc_common1_1608(int a, int b);
extern void OvlFunc_common1_1ecc(int a, int b, int c, int d, int e, int f, int g);
extern void OvlFunc_common1_1fb4(int file);

int OvlFunc_954_2008a3c(void)
{
    unsigned char *a;
    unsigned char *s;
    int t;
    int n;
    int hi;
    int z;
    int two;
    int twelve;
    int v;
    int e22;
    int e7;
    int v1;
    int v2;
    int iv2;
    int sa;

    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0;
    __SetFlag(0xa2 << 1);
    a = __MapActor_GetActor(9);
    t = __Func_8011f54(0, *(int *)(a + 8), *(int *)(a + 0x10));
    if (*(int *)(a + 0xc) == 0 && t == 0) {
        a[0x23] = 2;
        a[0x55] = t;
        v1 = *(int *)(a + 8) >> 20;
        v2 = *(int *)(a + 0x10) >> 20;
        __Func_8010704(0xe, 0xd, 1, 1, v1, v2);
    }
    n = __GetFlagByte(0xc4 << 2);
    if (n == 0)
        n = 0x19;
    a = __MapActor_GetActor(0xa);
    hi = 0x80 << 12;
    *(int *)(a + 8) = (n << 20) + hi;
    z = 0;
    a[0x55] = z;
    two = 2;
    a[0x23] = two;
    twelve = 0xc;
    __Func_8010704(0xe, 0xd, 1, 1, n, twelve);
    __StartTask(OvlFunc_954_200804c, 0xc8 << 4);
    a = __MapActor_GetActor(0xf);
    a[0x22] = 1;
    if (__GetFlag(0x303) != 0) {
        __Actor_SetAnim(a, 4);
        __Actor_SetSpriteFlags(a, 0);
        a[0x59] = z;
        a[0x23] = 3;
        __Func_8010704(0x2f, 0x18, 1, 1, 0x2f, twelve);
    }
    a = __MapActor_GetActor(0x11);
    v = *(int *)(a + 0x10) >> 20;
    a[0x55] = z;
    a[0x23] = two;
    __Func_8010704(0x40, 0x18, 3, 1, 0x40, v);
    a = __MapActor_GetActor(0x12);
    v = *(int *)(a + 8) >> 20;
    a[0x55] = z;
    a[0x23] = two;
    __Func_8010704(0x3f, 0x19, 1, 3, v, 9);
    if (__GetFlag(0x302) != 0) {
        e22 = 0x22;
        e7 = 7;
        __Func_8010704(0x25, 7, 1, 4, e22, e7);
        __Func_8010704(0x24, 7, 1, 4, 0x25, e7);
        __Func_80105d4(0x64, 0x1d, 1, 3, e22, 0x26);
    }
    a = __MapActor_GetActor(0xd);
    if (__GetFlag(0x301) != 0) {
        __Func_8010704(0x2b, 0xc, 1, 1, 0x29, twelve);
        a[0x55] = z;
        *(int *)(a + 0x34) = 0x6666;
        *(int *)(a + 0x30) = 0xcccc;
        *(int *)(a + 0xc) = hi;
        __Actor_SetAnim(a, 3);
    } else {
        __Actor_SetAnim(a, 2);
    }
    __MapActor_GetActor(0xe)[0x23] = 2;
    OvlFunc_common1_1608(0x18, 0x78);
    OvlFunc_common1_1608(0x19, 0x7f);
    s = gState;
    switch (*(short *)(s + (0xe1 << 1))) {
    case 1:
        OvlFunc_common1_1ecc(0, 8, 4, 0xa3 << 19, 0xc0 << 16, 0x18, 0x19);
        sa = 0x13;
        iv2 = 2;
        __Func_8010788(0x7f, 0, 1, 2, sa, iv2);
        __DeleteFieldActor(0x13);
        __DeleteFieldActor(0x14);
        __DeleteFieldActor(0x15);
        __DeleteFieldActor(0x16);
        __DeleteFieldActor(0x17);
        if (__GetFlag(0x109) == 0) {
            __PlaySound(0x11);
            OvlFunc_common1_78(0);
            OvlFunc_common1_0();
            OvlFunc_954_2008a10(1);
            OvlFunc_954_2008a10(2);
            OvlFunc_954_2008a10(3);
            OvlFunc_common1_ea0(1);
        }
        __MapActor_SetExtra(1, 0);
        __MapActor_SetExtra(2, 0);
        __MapActor_SetExtra(3, 0);
        OvlFunc_common1_1fb4((int)(&_FILE_e4));
        break;
    case 2:
        __StartTask(OvlFunc_common1_148, 0xc8 << 4);
        __DeleteFieldActor(0x18);
        __DeleteFieldActor(0x19);
        if (__GetFlag(0x109) == 0) {
            OvlFunc_common1_0();
            OvlFunc_common1_78(1);
            OvlFunc_common1_ea0(0);
        }
        break;
    case 3:
        if (__GetFlag(0x109) == 0) {
            OvlFunc_954_2008db8(0x13);
            OvlFunc_common1_488();
        }
        break;
    case 4:
        OvlFunc_954_2008974(1);
        __Func_8091e9c(4);
        break;
    case 5:
        OvlFunc_954_2008974(-1);
        __Func_8091e9c(5);
        break;
    }
    return 0;
}
