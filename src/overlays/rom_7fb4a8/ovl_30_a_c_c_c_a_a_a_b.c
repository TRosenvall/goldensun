/* OvlFunc_971_2008128 -- asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.s, 9 instructions.
 * ***** LANDS. BYTE-IDENTICAL. *****
 *
 * FIGURE IN  : 2 of 9 encodings (re-measured this batch on the INSTALLED park
 *              body src/non_matching/ovl_7fb4a8/2008128.c: 13 enc vs 13, first
 *              diff at index 2; `--whole` agrees at 2 of 13).
 * FIGURE OUT : 0.  `OK whole file -- 32 bytes, 13 encodings and 3 relocations
 *              identical`.
 *
 * INSTALL AT : src/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.c
 *              and delete asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.s
 *              (the next build writes the generated .s to that same path --
 *              commit it, per CLAUDE.md).
 *              Retire src/non_matching/ovl_7fb4a8/2008128.c and strike this
 *              function from src/non_matching/tiny_reg_order.c.
 * fakematch.txt ROW: NOT NEEDED.  tools/shimcount.py reports nothing; 0 pins,
 *              no inline asm, no device.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.c \
 *     asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.s --whole
 *   (while still parked, point the first argument at this file.)
 *
 * NO SPLIT NEEDED.
 *   python3 tools/datacheck.py asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.s
 *       -> rc=0, no data section; and `objcmp --whole` lists exactly one
 *          function, OvlFunc_971_2008128.  No linker-script change.
 *
 * WHAT MOVED IT: `__attribute__((packed))` ON THE BYTE TABLE'S AGGREGATE.
 * =====================================================================
 * Batch 317 had already reproduced the ROM's five quantities and all eight
 * register assignments by naming the three symbol bases as locals, leaving ONE
 * adjacent swap of two independent instructions:
 *
 *     rom   ... ldr r2,=CHAR | lsl r1,r0,#2    | ldrb r3,[r3,r0] | ldr r4,=ewram ...
 *     ours  ... ldr r2,=CHAR | ldrb r3,[r3,r0] | lsl r1,r0,#2    | ldr r4,=ewram ...
 *
 * and it diagnosed the residue exactly right down to the ranker rung:
 * `rank_for_schedule` ties on priority (36/36) and on CLASS (3/3) and falls to
 * the DEPENDENT COUNT, where the ldrb's four beats the lsl's three because the
 * ldrb carries a MEMORY ANTI-DEPENDENCE on the later `str` that the lsl cannot
 * have.  It also predicted the cure: "Removing it would tie the count at 3 and
 * drop the decision to INSN_LUID, where insn 17 already has the lower LUID --
 * i.e. it would land the function."
 *
 * The park then declared every route to removing that edge closed, on the
 * argument that the load must stay a BYTE load and every one-byte C type is a
 * character type, so `DIFFERENT_ALIAS_SETS_P` can never fire.  Both premises are
 * true.  The conclusion is false, because a PACKED field's access does not take
 * its alias set from the field's type.  `-fsched-verbose=6`, forward
 * dependences, side by side:
 *
 *     park   insn 17 (lsl r1,r0,#2)    prio 36  dependents: 49 48 31      = 3
 *            insn 28 (ldrb r3,[r3,r0]) prio 36  dependents: 49 48 37 34   = 4
 *                                                            ^^ 37 = the str
 *            t=4 -> dependent count 4 > 3 -> the ldrb takes the slot.
 *
 *     here   insn 16 (lsl)             prio 36  dependents: 47 46 29      = 3
 *            insn 26 (ldrb)            prio 36  dependents: 47 46 32      = 3
 *            (the store drops from `dep 4` to `dep 3` incoming)
 *            t=4 -> TIE -> INSN_LUID, 16 < 26 -> the lsl takes the slot = ROM.
 *
 * It really is the alias machinery and not an accident of lowering:
 * `-fno-strict-aliasing` (which forces every alias set to 0) takes this body
 * straight back to the park's figure.
 *
 * MEASURED, same body otherwise (objcmp encodings out of 13):
 *     struct B8 { unsigned char v; }      __attribute__((packed))      0  <-- this
 *     struct B8 { unsigned char v : 8; }  __attribute__((packed))      0
 *     struct B8 { unsigned int  v : 8; }  __attribute__((packed))      0
 *     the same three WITHOUT packed                                    6
 *     struct S1 { unsigned char v; }   (plain)                         6
 *     union  U8 { unsigned char v; }                                   6
 *     `unsigned char v __attribute__((packed));` in a plain struct     6
 *     packed struct with TWO unsigned char members                     9
 *     packed/plain aggregate on the int STORE instead                  2 (inert)
 *     packed/plain aggregate on the int SOURCE load instead            2 (inert)
 *     all four crossings of those two with the plain byte struct        6
 * -> the lever is POSITIONAL: it must be the aggregate the BYTE LOAD goes
 *    through, and it must be the struct that is packed, not the member.
 *
 * `__attribute__((packed))` already has precedent in this tree
 * (include/task.h, include/combatant.h).
 */
struct Byte1 { unsigned char v; } __attribute__((packed));

extern struct Byte1 L1940[] __asm__(".L1940");
extern int CHAR_ARRAY_ARRAY_971__02009928[];
extern unsigned char ewram_2002224[];

/* Leaf helper: copies one word out of a six-word table into the slot that a
 * byte lookup table selects.  ewram_2002224[L1940[i]] = CHAR_..._02009928[i],
 * with both accesses reached as scaled-index-plus-base so the index lands in
 * Rn and the symbol in Rm. */
void OvlFunc_971_2008128(int i)
{
    struct Byte1 *t;
    int cbase;
    int ebase;
    int off;
    int k;
    int v;

    ebase = (int)ewram_2002224;
    off = i * 4;
    cbase = (int)CHAR_ARRAY_ARRAY_971__02009928;
    t = L1940;
    k = t[i].v;
    v = *(int *)(off + cbase);
    k <<= 2;
    *(int *)(k + ebase) = v;
}
