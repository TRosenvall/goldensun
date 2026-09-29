/* Func_8017e88  --  0x08017e88, split out of asm/rom_15000/rom_17e88_a_a.s;
 * BufferString (0x08018038) stays behind in the _b half.  EXACT:
 *
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b297b/f8017e88/Func_8017e88.c \
 *     asm/rom_15000/rom_17e88_a_a.s --func Func_8017e88
 *   OK Func_8017e88 -- 432 bytes, 205 encodings and 1 relocations identical
 *
 * Size 432 bytes; 205 instructions; 0 shims (pin-free); production flags (-O2,
 * no Makefile row needed).
 *
 * WHAT IT DOES.  It writes an English article and/or plural suffix around one
 * run of text into the caller's 0x200-entry halfword ring, wrapping every index
 * with `& 0x1ff` and returning the updated index.  mode 1 inserts the article,
 * mode 2 the plural "s"/"es", mode 3 picks between them on arg6.  *pOut carries
 * "the last character written was 's'/'S'", which is what chooses "es" over "s".
 * flag brackets the whole run with 0x20,0x0a,0x0a ... 0x0a,0x08,0x20.
 *
 * WHAT CLOSED IT, in the order the deltas were measured (211 -> 205 -> exact):
 *
 *  - THE 8-WORD FRAME (`sub sp, #0x20`) IS A LOCAL STRUCT ASSIGNED FROM THE
 *    ROM's OWN .rodata TABLE.  `ldr r3, =.L33e40` + three ldmia/stmia pairs is a
 *    STRUCT COPY, not an initialiser list and not memcpy: an 8-element array
 *    cannot be assigned, so the source wrapped the pointers in a struct.  The
 *    table (`{"", "a ", "an ", "some ", "", "the ", "", ""}` at ROM 0x33e40) is
 *    already `.global .L33e40` in asm/rom_15000/rom_17e88_c_c.s, so it is
 *    reached with gcc's asm-label extension (docs/elevation.md:954) and NO new
 *    data is emitted here -- the one R_ARM_ABS32 matches the reference's.
 *
 *  - `signed char ch;` -- a QImode LOCAL, not `int ch` -- is what gives the
 *    ROM's `ldrb r3,[r4] / lsl r1,r3,#24` with the `asr r3,r1,#24` deferred into
 *    the loop body.  `int ch = *p++` off a `signed char *` gives
 *    `mov r1,#0 / ldrsb r1,[r2,r1]` instead (thumb has no immediate-offset
 *    LDRSB, so gcc materialises a zero index register).  Measured on a probe:
 *    int -> ldrsb, signed char -> ldrb+lsl.
 *
 *  - THE ARTICLE LOOP IS A COUNTED `for`, NOT A `while` WITH A BREAK.
 *    `for (n = 0; n < 8; n++) { ch = *p++; if (ch == 0) break; ... }` is the only
 *    spelling that rotates the way the ROM does (test in the preheader, `cmp
 *    r6,#7 / bgt` after the store, reload at the bottom).  Every
 *    `while ((ch = *p++) != 0) { ...; if (++n > 7) break; }` form either peels
 *    the first iteration or leaves the load at the top: -12 instructions.
 *
 *  - `kind = 0;` MUST BE WRITTEN BEFORE `t = L33e40;`.  This is the whole
 *    register allocation.  With the struct copy first, `kind` gets a LOW
 *    register (r1) and the table base gets r12, three callee-saved registers
 *    become two, and the stack-argument offsets come out 0x38/0x40 instead of
 *    0x3c/0x44 -- eight instructions short.  With `kind = 0;` first, `kind`
 *    takes r12, the table base takes r9 (`mov r9, sp` / `mov r1, r9`), the
 *    prologue pushes r8/r9/r10 and every offset lands.  197 -> 205 encodings,
 *    sizes equal, in one edit.
 *
 *  - `unsigned short *str` (NOT `short *`): `str[1] - 1` is `ldrh r3,[r5,#2]`
 *    in the ROM.  A `short *` gives `mov r7,#2 / ldrsh r3,[r4,r7]`.
 *
 *  - THE MAIN LOOP'S `lsl #16 / asr #16 ... lsl #16 / lsr #16` PAIR needs the
 *    guard on the POINTER and the body's copy in a `short`:
 *    `while (*str != 0) { c = *str; ring[idx] = c; ... (unsigned short)c == 'S' }`.
 *    Any form that loads into the loop variable first (`c = *str;
 *    while (c != 0)`, `while ((c = *str) != 0)`) collapses to `ldrsh` with a
 *    zero index register and loses the ROM's `ldrh r2,[r5] / mov r3,r2` guard.
 *
 *  - `str++` BEFORE `idx = (idx + 1) & 0x1ff;` -- sched2 then emits `add r5,#2`
 *    ahead of `and r0,r6`, which is the ROM's order.  Worth 2 encodings.
 *
 *  - THE ELSE ARM NEEDS ITS OWN NAMED LOCAL.  `else { if (*str == 0x1d) str += 2; }`
 *    lets cse2 fold the arm's own load into gcse's PRE-inserted copy of the
 *    loop guard's load, giving `ldrh r2,[r5] / mov r3,r2` where the ROM has two
 *    real loads `ldrh r3,[r5] / ldrh r2,[r5]`.  Assigning to a block-scoped
 *    `unsigned short e` first keeps both loads.  The last 2 encodings.
 *    (-fno-rerun-cse-after-loop does NOT fix this: it turns the PRE copy into
 *    `add r2,r3,#0` and makes the region three instructions instead of two.)
 *
 * SPLIT SHAPE (datacheck.py on the reference reports NO data section and NO
 * required .global data export, so this is a pure text/text split):
 *     stage1.ld:355   asm/rom_15000/rom_17e88_a_a.o(.text)
 *   becomes
 *                     asm/rom_15000/rom_17e88_a_a_a.o(.text)   <- this .c
 *                     asm/rom_15000/rom_17e88_a_a_b.o(.text)   <- BufferString
 *   No .rodata line is added: this TU emits none (the table is external).
 */

struct Tbl { const signed char *e[8]; };

/* {"", "a ", "an ", "some ", "", "the ", "", ""} at ROM 0x33e40 -- exported by
 * asm/rom_15000/rom_17e88_c_c.s.  gcc's asm-label extension, because `.L33e40`
 * is not a C identifier.  See docs/elevation.md:954. */
extern const struct Tbl L33e40 __asm__(".L33e40");

int Func_8017e88(int lineBreak, unsigned short *str, int idx,
                 unsigned short *ring, int mode, int plural, int *endsInS)
{
    struct Tbl t;

    if (lineBreak) {
        ring[idx] = 0x20;
        idx = (idx + 1) & 0x1ff;
        ring[idx] = 0x0a;
        idx = (idx + 1) & 0x1ff;
        ring[idx] = 0x0a;
        idx = (idx + 1) & 0x1ff;
    }
    if (mode == 1 || (mode == 3 && plural == 0)) {
        const signed char *p;
        signed char ch;
        unsigned short c;
        int kind;
        int n;

        kind = 0;
        t = L33e40;
        c = *str;
        if (c == 0x1d) {
            kind = str[1] - 1;
            str += 2;
        }
        if (kind == 0) {
            if (c == 0x41 || c == 0x49 || c == 0x55 || c == 0x45 || c == 0x4f)
                kind = 2;
            else
                kind = 1;
        }
        p = t.e[kind & 7];
        for (n = 0; n < 8; n++) {
            ch = *p++;
            if (ch == 0)
                break;
            ring[idx] = ch;
            idx = (idx + 1) & 0x1ff;
        }
    } else {
        unsigned short e;

        e = *str;
        if (e == 0x1d)
            str += 2;
    }
    {
        short c;

        while (*str != 0) {
            c = *str;
            ring[idx] = c;
            str++;
            idx = (idx + 1) & 0x1ff;
            if ((unsigned short)c == 0x53 || (unsigned short)c == 0x73)
                *endsInS = 1;
            else
                *endsInS = 0;
        }
    }
    if (mode == 2 || (mode == 3 && plural != 0)) {
        if (*endsInS != 0) {
            ring[idx] = 0x65;
            idx = (idx + 1) & 0x1ff;
        }
        ring[idx] = 0x73;
        idx = (idx + 1) & 0x1ff;
    }
    if (lineBreak) {
        ring[idx] = 0x0a;
        idx = (idx + 1) & 0x1ff;
        ring[idx] = 8;
        idx = (idx + 1) & 0x1ff;
        ring[idx] = 0x20;
        idx = (idx + 1) & 0x1ff;
    }
    return idx;
}
