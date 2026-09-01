/* Func_801bd98 (0x0801bd98) -- NON-MATCHING.
 * Blocker class: constant-pool PLACEMENT -- ours needs a skip jump, the ROM's
 * does not. Plus a two-register role swap.
 *
 * 113 lines against the ROM's 112. The switch's comparison tree, all three
 * icon-loading arms, the shared `+ off` store and the entire bitfield tail come
 * out instruction-for-instruction. The single extra line is this:
 *
 *     ours   ... strh r4, [r5, #0xe] / b L9 / L9: / and r4, r3
 *                                              ^ nothing between them
 *
 * A branch to the immediately following label, exactly as
 * src/non_matching/rom_f6000/80f6148.c documents -- except THAT function has
 * the jump in the ROM and we lack it, and this one has it in OUR output and the
 * ROM lacks it. Both are gcc placing a literal pool mid-function and stepping
 * over it; the two pool loads that follow here (`=0x3ff`, `=0xfffffc00`) are
 * what it is stepping over.
 *
 *   **Pool scaffolding cuts both ways.** A one-line difference in either
 *   direction, next to a `b` to the next label, is placement and not source.
 *
 * The other residue is that the ROM holds `kind` in r7 and `id` in r6 while gcc
 * does the reverse. Nothing in the source selects that.
 *
 * MEASURED (rom 112 lines):
 *   baseline                   113, 80
 *   -fno-gcse                  113, 80
 *   -fno-strict-aliasing       113, 80
 *   -fno-strength-reduce       113, 80
 *   -fno-schedule-insns2       113, 85 (worse)
 *
 * WHAT IS RIGHT, and is the reusable part:
 *
 *   THE +0x28 STRUCTURE IS BITFIELDS. The ROM does ONE `ldrb [r0, #5]`, four
 *   separate ANDs (`~0xc`, `~0x20`, `~0x10`, `0x3f`) and ONE `strb`. Four
 *   constant masks that gcc would fold into a single `& 3` if they were written
 *   as `&=` statements -- it does NOT fold them when they are four bitfield
 *   assignments, because each is a separate insert into a different field.
 *   **A run of separate constant ANDs on one loaded byte, folding to something
 *   simpler than the ROM shows, is the bitfield tell.**
 *
 *   The 16-bit field at +8 is reached three ways in the ROM -- `ldrb [r0,#9]`
 *   masked with 0xf, an `ldrh [r0,#8]` read-modify-write, and `ldrb [r0,#9]`
 *   masked with ~0xc -- which is gcc using the SMALLEST load that covers each
 *   bitfield. `unsigned short f8a : 10, f8b : 2, f8c : 4;` produces all three.
 *
 *   The five-argument icon loaders match the already-elevated LoadItemIconID:
 *   `(id, a1, &slot, &gfx, flag)` with the fifth argument passed on the stack,
 *   which is the `str r1, [sp]` before each call.
 *
 * NEXT: nothing source-level.
 */
struct Obj {
    unsigned char pad00[5];
    unsigned char f5a : 2, f5b : 2, f5c : 1, f5d : 1, f5e : 2;
    unsigned char pad06;
    unsigned char f7a : 6, f7b : 2;
    unsigned short f8a : 10, f8b : 2, f8c : 4;
};

extern void LoadOldUIIcon(int id, int a1, int *slot, int *gfx, int flag);
extern void LoadItemIconID(int id, int a1, int *slot, int *gfx, int flag);
extern void LoadMoveIcon(int id, int a1, int *slot, int *gfx, int flag);

void Func_801bd98(int kind, int id, unsigned char *e, int flag)
{
    struct Obj *o;
    int slot;
    int gfx;
    int off;

    switch (kind) {
    case 1:
    case 6:
        if (flag != 0)
            slot = *(unsigned short *)(e + 0xc);
        LoadOldUIIcon(id, 0, &slot, &gfx, flag);
        off = 0x1f;
        break;
    case 2:
        if (flag != 0)
            slot = *(unsigned short *)(e + 0xc);
        LoadItemIconID(id, 1, &slot, &gfx, flag);
        off = 0x182;
        break;
    case 4:
        if (flag != 0)
            slot = *(unsigned short *)(e + 0xc);
        LoadMoveIcon(id, 1, &slot, &gfx, flag);
        off = 0x333;
        break;
    default:
        goto tail;
    }
    *(short *)(e + 0x20) = id + off;
tail:
    *(short *)(e + 8) = id;
    *(short *)(e + 0xc) = slot;
    *(short *)(e + 0xe) = gfx;
    *(short *)(e + 0xa) = kind;
    *(short *)(e + 0x22) = 0x80 << 1;
    *(short *)(e + 0x26) = 0x80 << 1;
    o = (struct Obj *)(e + 0x28);
    o->f5b = 0;
    o->f5d = 0;
    o->f5c = 0;
    o->f5e = 0;
    o->f7b = 1;
    o->f8c = 0;
    o->f8a = gfx;
    o->f8b = 0;
}
