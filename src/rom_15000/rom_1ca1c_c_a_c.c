/* Func_801d014  @  0x0801d014  [rom_15000]   *** LANDING -- 0 of 91 ***
 * MATCHING. Byte-identical. PIN-FREE (0 pins). DEVICE-FREE. No flag group.
 *
 * FIGURE (measured, batch 328 brief E, on this body):
 *   objcmp --func : OK Func_801d014 -- 220 bytes, 91 encodings and 4
 *                   relocations identical.
 *   objcmp --whole: ok Func_801d014 91 encodings; OK whole file -- 220 bytes,
 *                   91 encodings and 4 relocations identical.
 *   The park this replaces measured 3 of 91, re-derived first (ref 91 / ours 91,
 *   first at index 35, no SIZE line, no RELOCATIONS line, no INSTRUCTION COUNT
 *   line -- a real distance) -- figure CONFIRMED before it was beaten.
 *
 * Verify with (INSTALLED PATH, one line):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_15000/rom_1ca1c_c_a_c.c asm/rom_15000/rom_1ca1c_c_a_c.s --func Func_801d014
 *
 * SPLIT SHAPE: NONE NEEDED.
 *   grep -c thumb_func_start asm/rom_15000/rom_1ca1c_c_a_c.s -> 1 (Func_801d014)
 *   python3 tools/datacheck.py <that .s> -> no output, rc=0
 *   INSTALL PATH: src/rom_15000/rom_1ca1c_c_a_c.c
 *   RETIRES the park src/non_matching/rom_15000/801d014.c.
 *
 * ===========================================================================
 * WHAT CLOSED THE 3: `DIFFERENT_ALIAS_SETS_P`, WHICH THE PARK HAD RULED OUT
 * ON A PREMISE THAT BITFIELDS BREAK.
 * ===========================================================================
 * The park's analysis of the residue is CONFIRMED NUMBER FOR NUMBER from my own
 * `.23.sched2` -- insn 38/102/103/270/110/112/238, the costs, and the rigid
 * ladder `prio(102) = prio(270) + 1` for every X. The deciding edge is the TRUE
 * MEMORY dependence `103 -> 112` (the `strb` of the constant, and the `ldrb` of
 * the gState byte), worth +2 because `arm.md:263` gives `load,store1` two core
 * cycles when `ldsched` is `no`, and arm7tdmi carries no `FL_LDSCHED`
 * (`arm.c:254`, `:568`). Cut that edge and the 3 closes.
 *
 * The park closed `true_dependence`'s first escape like this:
 *   "1. DIFFERENT_ALIAS_SETS_P (alias.c:1573) -- cannot fire: both references
 *    are char-precision and `lang_get_alias_set` (c-common.c:3347-3352) returns
 *    0 for ANY reference whose type has char precision."
 * The premise is true of the references it tried and FALSE AS A BOUND, because
 * **a bitfield's alias set comes from the FIELD'S DECLARED TYPE, not from the
 * width of the access.** An 8-bit bitfield in an `unsigned int` field is a
 * BYTE-WIDE access (`get_best_mode` picks QImode, so it is still `ldrb`/`strb`)
 * whose `MEM_ALIAS_SET` is that of `unsigned int` -- NONZERO.
 *
 * `mems_in_disjoint_alias_sets_p` (`alias.c:207`, reached through the
 * `DIFFERENT_ALIAS_SETS_P` macro at `:117`) needs BOTH halves:
 *     if (MEM_ALIAS_SET (mem1) == 0 || MEM_ALIAS_SET (mem2) == 0) return 0;
 *     if (MEM_ALIAS_SET (mem1) == MEM_ALIAS_SET (mem2))           return 0;
 * -- both sets nonzero AND different. So BOTH objects must be reached through
 * non-char-precision references, and through DIFFERENT types. That is why
 * `gState` is `unsigned int : 8` here and the ewram record is
 * `unsigned short : 8`: they are two different objects, and saying so is a TRUE
 * statement about the program, not a false one.
 *
 * ---- THE CROSSING, WITH THREE CONTROLS --------------------------------------
 *   j1  gState `unsigned int : 8`, dest `unsigned short : 8`      0  *** MATCH ***
 *   j2  CONTROL: the SAME bitfield base type on both sides        3  first at 35
 *   j3  CONTROL: only the dest is a bitfield, gState stays char   3  first at 35
 *   j1 under -fno-strict-aliasing                                 3  first at 35
 *
 * j2 is the proof that the alias set comes from the field's declared type and
 * not from the struct type (same base type => same set => `== ` returns 0 and
 * the edge survives). j3 is the proof that both halves are needed (one set 0).
 * And the fourth row is the decisive one: **turning strict aliasing OFF puts
 * this body back to exactly 3 of 91 at exactly index 35**, so the 0 is the
 * alias-set disjointness and nothing else.
 *
 * ---- WHY THIS IS A LEVER AND NOT THE PARK'S DEVICE --------------------------
 * The park's 0 came from `extern const unsigned char gState[]`, and it was
 * correctly withheld: `RTX_UNCHANGING_P` asserts THE VALUE CAN NEVER CHANGE
 * (`expr.c:6507-6512` says so in the source), and gState is the game's mutable
 * global state -- a false statement whose only effect was to delete a
 * dependence. **Nothing of that kind is here.** This body makes no claim about
 * mutability; it says the two objects have different types, which they do. It
 * is the same shape as this project's ruling that a one-member union "reaches
 * an int-width field and beats -fno-strict-aliasing ... It is a LEVER, not a
 * device" -- a type used to express a genuine non-aliasing fact.
 * **The `const` is NOT re-proposed and is not present.**
 *
 * JUDGMENT FLAGGED FOR THE COORDINATOR: the one thing a reader should weigh is
 * that the two bitfield base types must DIFFER for the mechanism to fire (j2
 * proves it), so the `unsigned int` / `unsigned short` asymmetry is load-bearing
 * rather than decorative. If the owner judges a type chosen for its alias set to
 * be a device, the device-free fallback is the previous park body at 3 of 91,
 * which is unchanged in `src/non_matching/rom_15000/801d014.c`.
 *
 * ---- WHAT THE LANDED MODULE-MATES TOLD ME ----------------------------------
 * `tools/upstream_module.py Func_801d014` -> upstream `rom_15000/rom_1ca1c.s`,
 * 15 landed .c against 5 parks. The siblings establish that this module declares
 * its own record types per function rather than sharing a header, which is what
 * makes a per-function record type for the ewram block the module's own idiom.
 *
 * ---- THE PARK'S OTHER THREE ESCAPES ARE STILL CLOSED, AS IT SAID -----------
 * `RTX_UNCHANGING_P` (the device), `base_alias_check` and `memrefs_conflict_p`
 * (the store's base is `galloc_ewram`'s return value, which `find_base_term`
 * cannot resolve). The park's correction to `docs/elevation.md` -- that a const
 * POINTER cannot cut an alias-set-0 edge but a const OBJECT can -- also stands.
 * Only escape 1 was reachable, and only through a non-char-precision reference.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

typedef struct { unsigned int b : 8; } __attribute__((packed)) BU;
typedef struct { unsigned short b : 8; } __attribute__((packed)) BS;

extern unsigned char *galloc_ewram(int tag, int size);
extern BU gState[];
extern void Func_801cf48(void);
extern int StartTask(void *fn, int pri);

void Func_801d014(void)
{
    BS *p;
    BU *g;
    unsigned char t;

    p = (BS *)galloc_ewram(0x14, 0xc5 << 3);
    DMA3_CLEAR((unsigned char *)p, 0xc5 << 3);
    g = gState;
    t = g[0x205].b;
    p[0x594].b = t;
    p[0x599].b = 0x18;
    t = g[0x206].b;
    p[0x595].b = t;
    p[0x59a].b = 0xf;
    t = g[0x83 << 2].b;
    p[0x596].b = t;
    p[0x59b].b = 3;
    t = g[0x20a].b;
    p[0x597].b = t;
    p[0x59c].b = 2;
    t = g[0x22a].b;
    p[0xb3 << 3].b = t;
    p[0x59d].b = 2;
    StartTask(Func_801cf48, 0xc8 << 4);
}
