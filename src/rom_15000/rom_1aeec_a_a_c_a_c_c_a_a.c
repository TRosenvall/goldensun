/* Func_801b424 (0x0801b424) -- LANDS.
 * BYTE-IDENTICAL: 0 of 87 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1aeec_a_a_c_a_c_c_a_a.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_a_c_c_a_a.s --func Func_801b424
 * objcmp: "OK Func_801b424 -- 200 bytes, 87 encodings and 12 relocations
 * identical", and the same verdict on the whole file.
 *
 * LANDING PREREQUISITES
 *   1. NO SPLIT.  split_s.py says in as many words that the reference holds only
 *      this function and no data, so it converts directly.
 *   2. tools/datacheck.py prints NOTHING and exits zero -- no text/data split and
 *      no data-object export.
 *   3. tools/shimcount.py reports no pins -- no fakematch.txt row.  Production
 *      flags, no per-file Makefile row.
 *   4. the generated asm/rom_15000/rom_1aeec_a_a_c_a_c_c_a_a.s belongs in the
 *      commit.
 *
 * WHAT CLOSED IT, AND IT IS ONE THING: THE BASE IS A `struct Party *`, NOT A
 * BYTE POINTER.  The park reached twelve aligned and called the residue "an
 * addressing mode gcc will not give up":
 *
 *     rom   add r3, r5, r1 / add r1, #2 / ldrh r2, [r3] / add r3, r5, r1 /
 *           ldrh r3, [r3]
 *     park  ldrh r2, [r5, r3] / add r3, #2 / ldrh r3, [r5, r3]
 *
 * That reading was right about the instructions and wrong about the cause.  gcc
 * takes the register-offset form when the address is a PLUS OF TWO PSEUDOS, which
 * is what `p + off` through an `unsigned char *` with a variable `off` always is.
 * A COMPONENT_REF at a constant offset too large for the halfword displacement is
 * not that: the whole address is forced into a register and the load gets a zero
 * displacement, which is the reference's form exactly.  The reference's
 * `add r1, #2` is then not a source-level increment at all -- it is gcc reaching
 * the ADJACENT MEMBER by bumping the offset register it already built, and it
 * appears on its own once the two reads are two members of one struct.
 *
 * SO THE PARK'S THREE NEGATIVES WERE ALL IN ONE DIMENSION -- WHERE THE POINTER IS
 * NAMED -- and the answer was in the dimension nobody varied, the TYPE OF THE
 * BASE.  Notebook rule confirmed again.
 *
 * AND THE ANSWER WAS IN A LANDED MODULE-MATE THE PARK ITSELF NAMED.  The park
 * declared `Func_801b664(unsigned char *p)`; the real Func_801b664 is landed at
 * src/rom_15000/rom_1aeec_a_a_c_a_c_c_c_b.c, takes `struct Party *`, and its own
 * byte-matching output contains this window four times over -- `mov r1, #231 /
 * lsl r1, r1, #2 / add r6, r7, r1 / ldrh r3, [r6]` for `p->f39c`, and
 * `mov r2, #210 / lsl r2, r2, #2 / add r3, r7, r2 / ldr r5, [r3]` for `p->head`.
 * Both are instruction-for-instruction what this function wanted.  THE STRUCT
 * DECLARATION SHIPPED HERE IS THAT FILE'S, VERBATIM, with its `pad5[2]` named
 * f3a0 because this function reads it; keeping the two declarations identical is
 * what lets the module become one translation unit in pass 2.5.
 *
 * HOW IT WAS FOUND, as a method note: grep the GENERATED half of asm/ for the
 * two-instruction window `add rX, rY, rZ` followed by `ldrh rW, [rX]`, rank the
 * files by hit count, and the second-densest file in the whole tree is this
 * function's own module-mate.  Three hundred and twenty-six hits in a hundred and
 * seventy-seven generated files, so the construct was never rare -- only
 * unlooked-for.
 *
 * EVERY PARK OBSERVATION THAT SURVIVES, and they all did:
 *   * THE LOOP HEAD MUST BE A goto LABEL, NOT A LOOP.  A `for (;;)` wrapper
 *     hoists the poll address out, buys a seventh callee-saved register and loses
 *     the reference's per-iteration rebuild.  The backward target has THREE
 *     predecessors, which is the structural tell that no single `for`/`while`
 *     produces it.
 *   * `volatile` on both polled key globals, which is what makes the reference's
 *     separate reload of each appear.
 *   * THE PARAMETER IS REUSED AS THE RETURN VALUE in the arm that returns a sum;
 *     the reference writes the sum into the register the parameter arrived in.
 *   * the `0xd2 << 2` indirection was already exact and stays exact -- it is now
 *     spelled `p->head->id`, the same member chain the landed module-mate uses.
 *
 * RETIRED BY MEASUREMENT: the park's "register-numbering tail" (it predicted the
 * reference's r1/r2 against our r2/r3 would close with the addressing mode, and
 * it did, with nothing further done); and its appeal to the ldrsh REG+REG finding
 * recorded for OvlFunc_899_200a564, which describes a real mechanism but not this
 * function's -- the lever here is the MEMBER REFERENCE, not the operand shape.
 */
struct Node {
    unsigned char pad0[4];
    struct Node *next;                  /* 0x04 */
    unsigned char pad1[0xa - 8];
    unsigned short id;                  /* 0x0a */
    unsigned char pad2[0x10 - 0xc];
    short y;                            /* 0x10 */
    unsigned char pad3[2];
    short step;                         /* 0x14 */
    unsigned char pad4[2];
    short ty;                           /* 0x18 */
    short f1a;                          /* 0x1a */
};
struct Party {
    unsigned char pad0[8];
    unsigned short f8;                  /* 0x008 */
    unsigned short fa;                   /* 0x00a */
    unsigned char pad1[0x3c - 0xc];
    unsigned short f3c;                 /* 0x03c */
    unsigned short f3e;                 /* 0x03e */
    unsigned char pad2[0x348 - 0x40];
    struct Node *head;                  /* 0x348 */
    unsigned char pad3[0x354 - 0x34c];
    unsigned short xs[0x10];            /* 0x354 */
    unsigned short ys[0x10];            /* 0x374 */
    unsigned short f394;                /* 0x394 */
    volatile unsigned short f396;       /* 0x396 */
    volatile unsigned short f398;       /* 0x398 */
    unsigned char pad4[0x39c - 0x39a];
    unsigned short f39c;                /* 0x39c */
    unsigned short f39e;                /* 0x39e */
    unsigned short f3a0;                /* 0x3a0 */
    unsigned short f3a2;                /* 0x3a2 */
};

extern struct Party *iwram_3001e98;
extern volatile int gKeyRepeat;
extern volatile int gKeyPress;

extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void Func_801b664(struct Party *p);
extern void Func_801b810(struct Party *p);

int Func_801b424(int v)
{
    struct Party *p;

    p = iwram_3001e98;
top:
    WaitFrames(1);
    if (p->f3a0 != 0)
        goto top;
    if (v != 0x3e7) {
        if (gKeyRepeat & 0x10) {
            _PlaySound(0x6f);
            Func_801b664(p);
        } else if (gKeyRepeat & 0x20) {
            _PlaySound(0x6f);
            Func_801b810(p);
        }
        if (gKeyPress & 1) {
            v = p->f39c + p->f39e;
            if (p->head->id == 6) {
                if (v == 0)
                    _PlaySound(0x70);
                else
                    _PlaySound(0x71);
            } else {
                _PlaySound(0x70);
            }
            return v;
        }
    }
    if (v == 0)
        goto top;
    if (!(gKeyPress & 2))
        goto top;
    _PlaySound(0x71);
    return -1;
}
