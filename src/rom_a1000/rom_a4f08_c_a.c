/* Cluster Func_80a51d0 extracted from goldensun/asm/rom_a1000/rom_a4f08_c_a.s.
 *
 * Opens the item-detail panel: narrates the held item through _Func_801bcd4,
 * pokes the panel descriptor's three header fields, re-lays it out, then draws
 * the owner's name and the item's name into the panel window.
 *
 * Whole-file conversion: the piece holds ONE function and no data section --
 * datacheck is clean, so no `.global` and no split.  No pins, no flag group.
 *
 * MATCHES.  All 53 encodings and all five call relocations are identical at
 * identical offsets, size 124 both sides.  The one objcmp difference is the
 * final pool word becoming an R_ARM_ABS32 against _MSG_182, which is this
 * tree's signature for an admitted symbol-table entry and which the linker
 * resolves back to the reference bytes.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_a1000/rom_a4f08_c_a.c \
 *     asm/rom_a1000/rom_a4f08_c_a.s --whole
 *
 * ============================================================
 * THREE LEVERS, AND THE PARK HAD TWO OF THEM BACKWARDS.
 *
 * 1. _MSG_182 FOR THE ITEM-NAME BASE.  The reference spends a pool word on
 *    0x182 and adds it to the masked window id.  0x182 = 0xc1 << 1, so
 *    constraint `K` matches at alternative 3 of *thumb_movsi_insn while the
 *    pool path `mn`/`mi` is alternative 6 -- recog takes the first match, so an
 *    SImode const_int 0x182 can NEVER reach the pool.  That is the exact
 *    structural argument message.sym records for this symbol, and the consumer
 *    is the same message sink: _Func_801e7c0's first argument.  Five
 *    device-free spellings of the addend were measured (bare literal; a
 *    (u16) cast on the sum; a (u16) cast on each operand; an unsigned short
 *    temp; an unsigned short temp with `+=`) and ALL FIVE build it with
 *    `mov/lsl`, which is one instruction too many -- see the inert list.
 *
 * 2. A ONE-MEMBER UNION ROUND THE DESCRIPTOR SLOT, for the FIVE RE-READS.
 *    The reference re-loads the descriptor pointer before each of the three
 *    field stores and again for the layout call.  Plain `(*s)->f5 = 1;` lets
 *    gcc keep the pointer in one register, because the store's alias set and
 *    the pointer load's alias set do not conflict.  A union MEMBER access is
 *    MEM_ALIAS_SET 0 regardless of the member list -- c-common.c:3334-3345,
 *    "permit type-punning when accessing a union" -- and alias.c:237-238 makes
 *    a zero set conflict with every other reference, so every store kills the
 *    cached load.  This is the same lever, with the same one-member shape, that
 *    src/overlays/rom_780898/ovl_30_a_a_a_c_c_a.c landed on; a struct with the
 *    same member does nothing.
 *
 *    The park reached this shape with `volatile` and rejected it, correctly,
 *    as a lie about the object -- but it then recorded the volatile result as
 *    "overshoots by one instruction" and read the overshoot as fatal.  The
 *    overshoot was lever 1, an INDEPENDENT deficit at the other end of the
 *    function.  Two residues, one length: fixing either alone looks like a
 *    wash.
 *
 * 3. THE WINDOW POINTER IS ASSIGNED BEFORE THE DESCRIPTOR SLOT, and the
 *    _GetUnit result is named so the second slot pointer is formed AFTER that
 *    call.  Assignment order decides which of r2 and r3 carries which offset,
 *    and with it how sched2 interleaves the two address computations: the
 *    reference finishes one before starting the other, and the reversed order
 *    interleaves them for eight encodings.  Declaration order is inert; only
 *    assignment order moves it.  Inlining the window pointer at its two uses,
 *    or naming the two offsets in one reused int, are both inert.
 *
 * MEASURED AND INERT OR WORSE -- do not re-run.  Figures are differing
 * encodings of the reference's 53 at stream length 53 unless noted:
 *   the park body as installed ......................... 43, 42 instructions,
 *     SIZE 116 against 124, and relocations differing in their SYMBOLS
 *   the park body with the four dereferences written out
 *     instead of a named slot pointer .................. 35, 42 instructions
 *   a char ** slot with byte and halfword stores through it ... 44, 54
 *     encodings / 128 bytes: the byte store DOES force one re-read (a
 *     char-precision INDIRECT_REF is alias set 0 by the same c-common rule)
 *     but each halfword store through an explicit `short *` lvalue POOLS its
 *     constant, where the reference has `mov r3,#0x70`
 *   a void ** slot with a cast at each use ............. 38, 41 instructions
 *   byte store via an INDIRECT_REF, halfwords via struct fields ... 35, 42
 *   all three stores via INDIRECT_REFs, constants named in int locals ... 39
 *   all three via INDIRECT_REFs with bare literals ..... 37, 55 encodings
 *   union slot, bare 0x182 ............................. 30 then 26 once the
 *     _GetUnit result is named, both at 45 instructions
 * The corpus is also evidence here: of the 4,515 generated `.s` files, NONE
 * re-loads a pointer slot between two plain stores, so the construct the
 * reference used is unattested outside the union class.
 */
struct S {
    unsigned char pad00[5];
    unsigned char f5;
    unsigned short f6;
    unsigned short f8;
    unsigned char pad0a[4];
    unsigned char fe;
};

/* The descriptor slot is reached by type-punning: every access through it must
 * be assumed to alias the fields written through the pointer it holds. */
union Slot {
    struct S *p;
};

extern int _MSG_182;
extern char *iwram_3001f2c;
extern void _Func_801bcd4(int a, int b, int c, int d);
extern void Func_80a17c4(void *s);
extern int _GetUnit(int id);
extern void _Func_801e8b0(int a, void *b, int c, int d);
extern void _Func_801e7c0(int msg, void *win, int x, int y);

void Func_80a51d0(void)
{
    char *p;
    union Slot *s;
    unsigned short *w;
    void **q;
    int unit;

    p = iwram_3001f2c;
    w = (unsigned short *)(p + (0xbc << 1));
    s = (union Slot *)(p + (0x87 << 2));
    _Func_801bcd4(2, *w, s->p->fe, 0);
    s->p->f5 = 1;
    s->p->f6 = 0x70;
    s->p->f8 = 8;
    Func_80a17c4(s->p);
    unit = _GetUnit(*(unsigned char *)(p + 0x21a));
    q = (void **)(p + (0x86 << 1));
    _Func_801e8b0(unit, *q, 0x10, 0);
    _Func_801e7c0((*w & 0x1ff) + (int)&_MSG_182, *q, 0x10, 8);
}
