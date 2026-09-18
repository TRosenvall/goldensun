/* Cluster OvlFunc_945_20082f4..OvlFunc_945_20082f4 extracted from goldensun/asm/overlays/rom_7cb2c0/ovl_30_a_c_c_a.s.
 *
 * Total .text for this TU = 76 bytes (= 0x4c).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7cb2c0/ovl_30_a_c_c_a_b.o and asm/overlays/rom_7cb2c0/ovl_30_a_c_c_b.o in
 * goldensun/overlays/rom_7cb2c0/overlay.ld.
 *
 * Was parked at 3 of 36. No pins, no flags.
 *
 * THE LEVER IS THE TYPE OF THE MEMORY OPERAND, not statement order. The park's
 * residue was `mov r1, r6 / add r1, #0x23` landing one slot too late, which the
 * sched2 dump confirms is a genuine tie -- at t=55 the ready list ranks the
 * `strb [r5, #0x15]` above the address copy, the tie-break falls through to
 * source order, and the address insn is created by RELOAD glued to its use, so
 * no statement order can reach it.
 *
 * Reading the sprite through a STRUCT with named byte fields instead of
 * `unsigned char *s` with `s[9]` / `s[0x15]` changes the store's alias set and
 * the scheduler lands the ROM's order. That is the recorded typed-field lever
 * ("a typed field load schedules differently from a cast dereference") doing the
 * work on a STORE.
 *
 * THE MASK IS A 2-BIT FIELD, which is why this spelling is the one landed. The
 * park's `m = -0xd` is exactly `~0xc`, and `struct Sprite` -- already declared in
 * src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_a.c with `b0:2, b2:2, b4:4`, so NOT
 * invented here -- makes `(x & ~0xc) | 4` simply `b2 = 1`. Both spellings are
 * byte-identical (76 bytes, 36 encodings); the bitfield one needs no mask local
 * at all and says what the code means.
 *
 * MEASURED AND WORSE, none of them in the park's list: splitting the `s[0x15]`
 * statement so the pointer is born between value and store, 26 (the temp local
 * wrecks the shared-mask allocation); the same with a split base/offset, 26; plus
 * a named loaded byte, 28; hoisting the flags LOAD above the `s[0x15]` store, 26
 * (gcc parks the pointer in r12). And three at 7, all informative: `fp =
 * &a->flags` assigned before either byte statement still leaves the address
 * computation after the `strb`, because `&a->flags` folds into the memrefs and is
 * re-created by reload -- a third confirmation of the recorded "a foldable name
 * does not survive" rule. A no-op struct cast of `a` folds away entirely (3,
 * identical to baseline).
 */
#include "gba/types.h"
#include "actor.h"

extern void __Actor_SetSpriteFlags(struct Actor *a, int n);
extern void __Func_80929d8(struct Actor *a, int n);

struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
    unsigned char pad0a[0x15 - 0xa];
    unsigned char c0 : 2,
                  c2 : 2,
                  c4 : 4;
};

int OvlFunc_945_20082f4(struct Actor *a)
{
    struct Sprite *s;

    s = (struct Sprite *)a->sprite;
    a->interactFlags = 8;
    __Actor_SetSpriteFlags(a, 0);
    s->b2 = 1;
    s->c2 = 1;
    a->flags = (a->flags & 0xfe) | 2;
    __Func_80929d8(a, 0xf);
    return 1;
}
