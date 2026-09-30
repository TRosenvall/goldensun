/* Func_809bcf8 -- NON-MATCHING, 243 of 499 encodings differ.
 * Unattempted before batch 302 (batch 301 opened recon and produced no figure).
 * Reference asm/rom_8a000/rom_9bb64_c_a_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_8a000/809bcf8.c \
 *       asm/rom_8a000/rom_9bb64_c_a_a.s --func Func_809bcf8
 *
 * INSTRUCTION COUNT IS EXACT: 499 against 499.  SIZE is 1092 against 1088 -- four
 * bytes long, and with the counts equal those four bytes are ONE EXTRA LITERAL POOL
 * WORD, not an instruction.  So 243 is very nearly a distance; treat it as one only
 * once the pool word goes.  tools/aligncmp.py reads 356 of 499 (71.3%), 190
 * differing/ins/del in 49 hunks.
 * tools/shimcount.py: 0 -- PIN-FREE.  No volatile, no register asm, no flag override.
 *
 * SPLIT SHAPE.  `python3 tools/datacheck.py asm/rom_8a000/rom_9bb64_c_a_a.s` prints
 * NOTHING: no text/data split is needed.  The .s holds TWO functions
 * (Func_809bcf8 at 0x0809bcf8, Func_809c138 at 0x0809c138) and this one is the
 * FIRST, so `python3 tools/split_s.py asm/rom_8a000/rom_9bb64_c_a_a.s Func_809bcf8`
 * is a HEAD split: _a is empty and not written, the target lands in
 * rom_9bb64_c_a_a_b.s -> src/rom_8a000/rom_9bb64_c_a_a_b.c, and Func_809c138 plus
 * this function's trailing pool lines land in rom_9bb64_c_a_a_c.s.
 * stage1.ld:1172 names asm/rom_8a000/rom_9bb64_c_a_a.o(.text), one line.
 * NO NEW `.global` IS REQUIRED.  Checked mechanically: of the 40 `.L` labels the
 * function's block mentions, the only two it does not itself define are `.L9f168`
 * and `.L9f188`, and asm/rom_8a000/rom_9bb64_c_c.s ALREADY declares
 * `.global .L9f168` / `.global .L9f188`.  Both are five hex digits, so the
 * one-and-two-digit asm-label capture hazard does not apply.
 *
 * *** THE SPLIT COLLIDES WITH THE Func_809c138 PARK. ***  src/non_matching/rom_8a000/
 * 809c138.c claims rom_9bb64_c_a_a_b.c for ITSELF (a TAIL split, leaving Func_809bcf8
 * in _c_a_a.s).  Both recipes are correct in isolation and MUTUALLY EXCLUSIVE: the
 * first of the two to land defines the split and the other park's recipe goes stale.
 * If Func_809bcf8 lands first, Func_809c138 must then be elevated as
 * rom_9bb64_c_a_a_c_a.c (or the _c file re-split), not as _c_a_a_b.c.
 *
 * ============ LEVERS THAT PAID, IN THE ORDER THEY PAID ============
 *
 *  L1. BLOCK ORDER OFF THE RELOCATION SEQUENCE, 344 -> 346 aligned but it is the
 *      gate on everything after.  The first candidate emitted the calls as
 *      _GetFlag / vec3_translate / GetFieldActor where the ROM has
 *      _GetFlag / GetFieldActor / vec3_translate.  Read straight off objcmp's
 *      relocation list, with no encoding analysis at all: the ROM lays the ACTOR
 *      path first, so the guard is `&&` (`_GetFlag(0x11c) == 0 && (gKeyHeld & 0x300)
 *      != 0`) and not the `||` the two-entry-point tail (`.L9bd9c` loading gKeyHeld,
 *      `.L9bd9e` not) first suggested.  The two entries are the && ELSE arm reached
 *      from two places, one of which already has &gKeyHeld live.
 *  L2. A BASE-POINTER VARIABLE FOR gBuffer IN THE ONE BLOCK THAT NEEDS IT -- the
 *      single biggest lever in the function.  508 instructions -> 499, EXACT, and
 *      size 1112 -> 1092.  `*(int *)(gBuffer + 4)` pools `gBuffer+4` as its own
 *      literal word; `gb = gBuffer; *(int *)(gb + 4)` gives the ROM's
 *      `ldr r0,=gBuffer / ldr r3,[r0,#4]`.  Each pooled addend is FOUR BYTES AND ONE
 *      INSTRUCTION of divergence, and this block had three of them.
 *  L3. `Func_8003dec(spr, az); spr++;` AND NOT `Func_8003dec(spr++, az)`.
 *      279 -> 249, aligned 318 -> 350.  Both spellings schedule `add r7,#0xc` BEFORE
 *      the `bl`, so the reference cannot be read to tell them apart -- but the
 *      post-increment-in-the-argument form costs 30 encodings elsewhere through the
 *      allocation.  This is the cheapest single edit in the whole reconstruction and
 *      it is invisible in the disassembly.  TRY IT BOTH WAYS ON EVERY
 *      pointer-walk-plus-call LOOP.
 *  L4. SPILL-SLOT ORDER IS DECLARATION ORDER, read off two `ldr`/`str` pairs.
 *      249 -> 243, aligned 350 -> 356.  The ROM does `ldr r3,[sp,#4] / str r3,[sp,#0]`
 *      where we did `ldr r3,[sp,#0] / str r3,[sp,#4]` -- the two slots were simply
 *      swapped, so `nm` is declared BEFORE `name`.
 *  L5. THE ADDRESS-TAKEN LOCALS ARE AT THE TOP OF THE FRAME AND IN DECLARATION
 *      ORDER, which fixes three slots for free.  `v[3]` occupies sp+0x30..0x3b,
 *      `w` sp+0x2c, `h` sp+0x28 in a 0x3c frame: expand_decl allocates downward from
 *      the top of the frame as the declarations are seen, so the order is v, w, h and
 *      reload's own spills then fill sp+0x24 downward.  Getting this right before the
 *      first measurement is why the frame size was never wrong.
 *  L6. `gSpriteSlots` IS AN ARRAY OF 4-BYTE STRUCTS, NOT `[][2]`.  Small but free:
 *      `gSpriteSlots[i].b` gives `lsl #2 / add base / ldrh [r3,#2]`, where
 *      `gSpriteSlots[i][1]` gives `lsl #2 / add #2 / ldrh [r2,r3]`.  A member access
 *      folds the +2 into the load's immediate; a second array index does not.
 *  L7. `w` AND `h` ARE THE SAME TWO VARIABLES AS THE LOOP'S `dx` AND `dz`.
 *      The loop's `str r2,[sp,#0x2c] / str r3,[sp,#0x28]` look like dead stores until
 *      you notice that sp+0x2c and sp+0x28 are what `_Func_8018790` is later handed
 *      the ADDRESSES of.  Address-taken locals live in memory, so every assignment is
 *      a store -- and the loop reusing the text-metrics pair is what makes those
 *      stores real.  Two variables, not four.
 *  L8. `mov #N / neg` NAMES THE BITFIELD MASK EXACTLY, AND IT IS OFF BY ONE FROM ~N.
 *      `mov r3,#0xd / neg r3,r3` is -13 = ~0xc, a 2-bit field at bit 2 of byte 5.
 *      `mov r3,#0x3f / neg r3,r3` is -63 = ~0x3e, a FIVE-bit field at bit ONE of byte
 *      7 -- not the six-bit field at bit 0 that `& ~0x3f` would suggest.  That single
 *      bit is what lets ONE struct carry both the 9-bit halfword field at +6 (bits
 *      0-8) and the byte-7 field (bits 9-13 of the same halfword): read as ~0x3f they
 *      overlap and the struct is impossible, read as ~0x3e they are adjacent.  gcc
 *      narrows the container to QImode for the byte-7 field, which is why it is a
 *      `ldrb`/`strb` on a `unsigned short` bitfield.
 *
 * ============ THE BLOCKER, NAMED BY THE PASS ============
 *
 * cse / reload REMATERIALISATION OF A symbol_ref -- the same pass and the same
 * mechanism as the sibling Func_809c138 park in this very .s file, which is blocked
 * on cse rematerialising a CONST_INT.  Here it is the ADDRESS of a global.
 *
 * The ROM reaches every global through a PLAIN symbol load plus a register or
 * immediate offset, and it RELOADS the plain symbol from the pool once per basic
 * block: `ldr r?,=gBuffer` appears at five separate sites, all four referring to the
 * SAME single pool word at 0x140, with `[r0,#4]`, `[r0,#8]`, `[r0,#24]`,
 * `add r7,#0x20` and `mov r0,#6 / ldrsh r3,[r3,r0]` hung off it.  Never once does the
 * ROM pool `gBuffer+offset`.  Our four remaining pool words carry addends 0x1f4
 * (gState), 0x18, 0x6, 0x12 and 0x1c (gBuffer) where the ROM's corresponding words
 * read `.word 0x00000000`.
 *
 * Two spellings bracket the ROM and NEITHER lands on it:
 *   - the MEM-address spelling (`*(int *)(gBuffer + 6)`) pools symbol+addend, which
 *     is one extra pool word and one extra instruction per distinct offset;
 *   - a base-pointer VARIABLE (`gb = gBuffer`) covering the whole function collapses
 *     the reloads into ONE live callee-saved register, which is FOUR INSTRUCTIONS
 *     TOO FEW (495 of 499) -- measured, see the table.
 * The ROM sits between them: one pseudo whose REG_EQUIV is the symbol_ref, which
 * cse declines to keep in a register (a Thumb pool load is not more expensive than a
 * spill reload) so reload rematerialises it at each use.  What decides that is cse's
 * cost comparison, not anything a declaration can move -- which is why the
 * gb-in-one-block compromise (L2) is the best available and still leaves one pooled
 * addend behind.
 *
 * WHAT RULES OUT THE ALTERNATIVES.
 *   - NOT a register-allocation order problem.  The only rotation left is r9/r10
 *     swapped on `hi` and `cz`, and moving `hi`'s declaration ahead of `cz`'s is
 *     EXACTLY INERT (249/350, byte-identical output).  Per allocno_compare the three
 *     inputs are n_refs, live_length and declaration order; order is disproved and
 *     the other two are consequences of the symbol loads.
 *   - NOT the frame.  Our frame is 0x3c, the ROM's is 0x3c, and every sp
 *     displacement below 0x28 already agrees.  Contrast the sibling park, where the
 *     frame is a word small and every displacement shifts.
 *   - NOT a relocation FORM difference.  The symbol SEQUENCE now matches entry for
 *     entry (27 against 27 after L2); what differs is the ADDENDS in the pool words,
 *     which are real extra words, not an unlinked-object artefact.
 *   - NOT the gState pointer.  The ROM keeps `r5 = gState + 500` live and RE-READS
 *     `*r5` after `_GetFlag` (aliasing forces it), building the address
 *     arithmetically as `ldr r3,=gState / movs r0,#250 / lsls r0,#1 / adds r5,r3,r0`.
 *     Writing that as a pointer variable reproduces the re-read and the sp+0x24/0x20
 *     store order, but gcc POOLS `gState+0x1f4` instead of synthesising 500 in a
 *     register, and the net is worse every time it was tried (501 and 497 against
 *     499).  So the re-read is real and correctly diagnosed but is NOT the blocker;
 *     it is the same rematerialisation question in a second place.
 *
 * ============ MEASURED -- DO NOT RE-RUN ============
 *   (figures are objcmp-differing / ours-count / aligncmp-aligned, ref 499)
 *
 *   `||` guard, actor path second                         496 / 508 / 344
 *   `&&` guard, actor path first (L1)                     496 / 508 / 346
 *   gSpriteSlots as a 4-byte struct (L6)                  496 / 508 / 346
 *   `ang != 0xffff` big block falling through             494 / 508 / 346
 *   + gb base pointer in the vec3 block (L2)              279 / 499 / 318   COUNT EXACT
 *   + `spr++` after the call (L3)                         249 / 499 / 350
 *   + `nm` declared before `name` (L4)                    243 / 499 / 356   SHIPPED
 *   `hi` declared before `cz`                             249 / 499 / 350   INERT on L3 base
 *   a `dist` temp for w*w+h*h                             279 / 499 / 318   INERT on L2 base
 *   `w`/`h` declaration order swapped                     279 / 499 / 318   INERT
 *   `base` as int rather than unsigned int                279 / 499 / 318   INERT
 *   `volatile` on gKeyHeld                                243 / 499 / 356   INERT -- dropped
 *   gBuffer as a STRUCT object, gb removed                485 / 495 / 362   best ALIGNED,
 *     but four instructions short: the struct member form lets cse collapse the
 *     symbol reloads.  Recorded because its aligned figure is the highest seen and it
 *     may be the right base once the reload question is answered.
 *   struct form + spr from `(char *)&gBuffer + 0x20`      485 / 495 / 362   INERT
 *   struct form + the gState re-read                      276 / 497 / 352
 *   gb covering the WHOLE function                        486 / 495 / 325
 *   gb reloaded for the +6/+0xa short reads               473 / 500 / 354
 *   gb reloaded for the tail +0x12/+0x1c accesses         471 / 497 / 320
 *
 * NEXT PROBE, if this is picked up: the four bytes are one pool word, and the
 * cheapest of the five remaining addends to remove is `gState+0x1f4`.  The ROM's
 * `movs #250 / lsls #1 / adds` is expand's route for a PLUS whose constant will not
 * fit an `add` immediate, which needs the symbol already in a pseudo -- so the
 * question is what spelling puts `gState` in a pseudo BEFORE the offset is added.
 * Every pointer-variable spelling tried so far pools the sum instead.
 */
#include "gba/types.h"
#include "gba/io.h"
struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

struct Spr {
    unsigned char pad0[4];
    unsigned char f4;
    unsigned char :2;
    unsigned char kind:2;
    unsigned char :4;
    unsigned short d:9;
    unsigned short e:5;
    unsigned short :2;
    unsigned short c:10;
    unsigned short :6;
    unsigned char pada[2];
};

extern struct DmaQueue gDMATaskCount;
extern unsigned char gState[];
extern unsigned char gBuffer[];
struct Slot { unsigned short a, b; };
extern struct Slot gSpriteSlots[];
extern unsigned int iwram_3001e40;
extern unsigned int gKeyHeld;
extern int Data_a0138[];
extern unsigned char L9f168[] __asm__(".L9f168");
extern unsigned short L9f188[] __asm__(".L9f188");
extern unsigned char ewram_2010338[];

extern int _GetFlag(int id);
extern unsigned char *GetFieldActor(int id);
extern void vec3_translate(int a, int b, int *v);
extern void Func_8003dec(struct Spr *s, int y);
extern void _Func_8016478(void *p);
extern int GetLocationName(int id, int a);
extern void _Func_8018790(int id, int *w, int *h);
extern void _DrawSmallText(int id, void *p, int x, int y);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

void Func_809bcf8(void)
{
    int v[3];
    int w, h;
    int playerId;
    int *p;
    unsigned int base;
    int blend;
    int best;
    int bestDist;
    int bx, bz;
    int nm;
    int name;
    struct Spr *spr;
    int cx, cz;
    int i;
    int hi, kind;
    int id;
    unsigned char *a;
    int ax, az;
    unsigned int savedIme;
    int count;
    u32 *task;
    struct DmaQueue *queue;
    int ang;
    unsigned char *gb;

    playerId = *(int *)(gState + (0xfa << 1));
    p = Data_a0138;
    base = gSpriteSlots[*(unsigned short *)gBuffer].b >> 5;
    best = -1;
    bestDist = 0x64;
    blend = L9f168[(iwram_3001e40 >> 1) & 0x1f];
    spr = (struct Spr *)(gBuffer + 0x20);
    if (_GetFlag(0x11c) == 0 && (gKeyHeld & 0x300) != 0) {
        a = GetFieldActor(playerId);
        if (a != 0) {
            cx = ((*(int *)(a + 8) - 0x10000000) >> 16) * 240 / 0x1000;
            cz = *(short *)(a + 0x12) * 160 / 0x1000;
        }
    } else {
        ang = L9f188[(gKeyHeld >> 4) & 0xf];
        if (ang != 0xffff) {
            gb = gBuffer;
            v[0] = *(int *)(gb + 4);
            v[1] = 0;
            v[2] = *(int *)(gb + 8);
            vec3_translate(*(int *)(gb + 0x18), ang, v);
            if (v[0] < 0x100000)
                v[0] = 0x100000;
            if (v[0] > 0xef0000)
                v[0] = 0xef0000;
            if (v[2] < 0)
                v[2] = 0;
            if (v[2] > 0x8d0000)
                v[2] = 0x8d0000;
            gb = gBuffer;
            *(int *)(gb + 4) = v[0];
            *(int *)(gb + 8) = v[2];
            if (*(int *)(gb + 0x18) < 0x60000)
                *(int *)(gb + 0x18) += 0x2000;
        } else {
            *(int *)(gBuffer + 0x18) = 0x10000;
        }
        cx = *(short *)(gBuffer + 6);
        cz = *(short *)(gBuffer + 0xa);
    }
    for (i = 0; i <= 0x41; i++) {
        if (i == playerId || i > 0xa) {
            if (i == 0) {
                if (_GetFlag(0x11c) != 0)
                    continue;
                hi = 0;
                kind = 0;
                id = playerId;
            } else {
                id = *p++;
                nm = *p++;
                if (id == 0)
                    continue;
                hi = id >> 16;
                kind = 1;
                if (id == -1)
                    break;
            }
            if (_GetFlag(id) == 0)
                continue;
            a = GetFieldActor(i);
            if (a == 0)
                continue;
            ax = ((*(int *)(a + 8) - 0x10000000) >> 16) * 240 / 0x1000;
            az = *(short *)(a + 0x12) * 160 / 0x1000;
            spr->kind = kind;
            spr->c = base + hi;
            spr->d = ax - 1;
            spr->f4 = az - 1;
            w = ax - cx;
            h = az - cz;
            if (w * w + h * h < bestDist) {
                name = nm;
                best = i;
                bestDist = w * w + h * h;
                bx = ax;
                bz = az;
            }
            if (i == 0 && (iwram_3001e40 & 0xf) > 7)
                continue;
            Func_8003dec(spr, az);
            spr++;
        }
    }
    if (best != -1 && (iwram_3001e40 & 0xf) <= 7) {
        spr->kind = 0;
        spr->c = base + 3;
        spr->d = bx - 2;
        spr->f4 = bz - 2;
        Func_8003dec(spr, bz);
    }
    spr = (struct Spr *)ewram_2010338;
    spr->e = 0;
    spr->d = cx - 0x11;
    spr->f4 = cz + 1;
    Func_8003dec(spr, 0xf6);
    if (*(short *)(gBuffer + 0x12) != best) {
        *(unsigned short *)(gBuffer + 0x12) = best;
        _Func_8016478(*(void **)(gBuffer + 0x1c));
        if (best != -1) {
            if (best == 0)
                name = 0x984;
            else
                name = GetLocationName(name, 1) + 0x99b;
            _Func_8018790(name, &w, &h);
            cx = bx - 1;
            cz = bz - 0xb;
            if (cx + w > 0xf0) {
                cx = 0xe8 - w;
                cz = bz - 0x14;
            }
            if (cz < 0)
                cz = 0;
            _DrawSmallText(name, *(void **)(gBuffer + 0x1c), cx, cz);
        }
    }
    queue = &gDMATaskCount;
    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = 0xfc << 6;
        *task++ = (u32)&REG_BLDCNT;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = ((0x10 - blend) << 8) | blend;
        *task++ = (u32)&REG_BLDALPHA;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}
