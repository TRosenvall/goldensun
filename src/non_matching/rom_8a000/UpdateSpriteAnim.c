/* UpdateSpriteAnim -- 0x0800aa0c, asm/rom_9000/rom_a97c_c.s, 544 instructions.
 *
 * NON-MATCHING, 589 of 674 encodings differ
 *
 * SIZE IS NOT EXACT and COUNT IS NOT EXACT: ref 1640 bytes / 674 encodings,
 * ours 1656 / 682 (+16 bytes, +8 instructions).  The objcmp figure above is
 * therefore SATURATED and cannot rank candidates -- rank by size-and-count
 * first, then by the aligned view.  aligncmp reads
 *     aligned-equal 297 (44.1% of ref), 516 differing/ins/del in 110 hunks
 * and that is the number that moved during this batch (34.1% -> 44.1%).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/UpdateSpriteAnim.c \
 *     asm/rom_9000/rom_a97c_c.s --func UpdateSpriteAnim
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_8a000/UpdateSpriteAnim.c \
 *     asm/rom_9000/rom_a97c_c.s UpdateSpriteAnim
 *
 * SHIMS: none.  tools/shimcount.py reports 0 -- this candidate is PIN-FREE,
 * and no flag row is needed (everything above is plain production -O2).
 *
 * SPLIT SHAPE (inherited from RECON_UpdateSpriteAnim.txt, unchanged by this
 * batch): a TEXT/DATA split is required and stage1.ld names the object TWICE,
 * line 108 `(.text)` -> the .c's object, line 284 `(.rodata)` -> a data-only
 * object.  The rodata block is asm/rom_9000/rom_a97c_c.s:769-786 and NINE
 * labels need `.global` on the data half:
 *     .L1307c .L1308c .L13094 .L1309c .L130ac .L130bc .L130c4 .L130cc .L1310c
 * None is referenced from any other file, so nine is the complete list.  The
 * function's OWN two jump tables (the control-code table at :133 and the
 * font-kind table at :223) are gcc's switch tables for labels INSIDE the
 * function; they come free from the two `switch` statements below and must NOT
 * be exported or split out.  Step 1 of the split is verifiable on its own with
 * `make compare` and should be landed before this .c is installed.
 *
 * THE NAME IS A MISNOMER.  This is a multi-line TEXT LABEL rasteriser, not an
 * animation tick -- see the recon for the three independent confirmations.  The
 * reconstruction below follows the code, not the symbol.
 *
 * WHAT LANDED THIS BATCH, WITH FIGURES.  Start 699 instructions / 1688 bytes /
 * 34.1% aligned; end 682 / 1656 / 44.1%.  Five changes, each measured alone or
 * in a pair:
 *
 *  1. THE INNER GLYPH LOOP MUST BE A `goto` LOOP, NOT A `for(;;)` WITH A BREAK.
 *     Both `while ((s16)line->penx <= 0)` and `for(;;) { if (...) break; }`
 *     reach jump.c's `duplicate_loop_exit_test`, which copies the five-insn
 *     test (`mov rX,#2 / ldrsh / ldrh / cmp / b`) to the loop bottom and leaves
 *     the top copy as a guard.  The ROM has exactly ONE copy, at the top, with
 *     every switch arm closing the loop by an explicit `b .Laa94`.  Writing it
 *     as `top:` + `goto top` emits no loop note at all, so the pass cannot
 *     fire.  Worth 5 instructions; it is the reason the arm bodies below all
 *     end in `goto top` rather than `break`.
 *
 *  2. DO NOT HOIST `line->text` INTO A LOCAL.  A `txt` variable took r7, which
 *     pushed `style` into r8 and cost a `mov r0, r8` in EVERY ONE of the nine
 *     font-kind switch arms.  Reading `line->text[line->pos++]` twice lets CSE
 *     share the one `ldr r1,[r6,#0x10]` the ROM has, and `style` lands in r7 as
 *     `asr r7, r1, #16`.  Worth 9 instructions -- the single largest item.
 *
 *  3. ONE COUNTER FOR FIVE DISJOINT LOOPS.  The ROM keeps r8 for the measure
 *     loop index, the insertion-sort index, the blit index, the outline y and
 *     the merge counter.  The signedness is NOT a contradiction: the measure,
 *     sort and blit compares are `blt`/`bge` because their limits are `int`,
 *     and the outline and merge compares are `bcc`/`bcs` because `ylim` and
 *     `size` are `unsigned int` and the usual arithmetic conversions make the
 *     compare unsigned.  So a single `int i` serves all five.  This plus (5)
 *     took 39.5% -> 44.1% aligned and put `i` back in r8.
 *
 *  4. THE TILE MASK MUST STAY IN SImode.  `ctl->tile = (ctl->tile & ~0x3ff) |
 *     (slot & 0x3ff)` written as one expression is narrowed by combine to
 *     HImode: gcc then pools 0x3ff as an `ldrh` and builds 0xfc00 as
 *     `mov #252 / lsl #8`, and the knock-on even turned `*dirtyp = 0` into a
 *     pooled halfword.  Splitting it into `slot &= 0x3ff; tile = ctl->tile &
 *     ~0x3ff; tile |= slot; ctl->tile = tile;` through an `int` keeps the ROM's
 *     two SImode pool words, 0x3ff and 0xfffffc00, which are now byte-identical.
 *
 *  5. SEPARATE FUNCTION-POINTER LOCALS PER CALL SITE.  One shared `clear` for
 *     the two `Func_80008d4` calls emitted `bl _call_via_r2` at both; two
 *     locals emit `bl _call_via_r3` at both, which is the ROM.  Six of the
 *     seven veneer registers are now correct (objcmp 590 -> 589).
 *
 * MEASURED NEGATIVES -- do not re-spend on these.
 *
 *  * LEVER 3 (the offset as a named local) DOES NOT REACH THE `gPtrs` READ.
 *    The ROM has `ldr r0,=gPtrs / add r0,#0xd4 / ldr r4,[r0]`; gcc folds ours
 *    into one pool word `gPtrs+212`.  BOTH escapes measured BYTE-IDENTICAL to
 *    the unfixed form -- `int k = 0xd4; *(void **)((u8 *)gPtrs + k)` and the
 *    two-statement `int k = 0x35; ... + (k << 2)`.  589/1656/44.1% all three
 *    times.  The fold happens before the pool decision, so the lever that went
 *    three for three in batch 303 is inert at this site.  Note the CONTRAST in
 *    the same function: `iwram_3001e68 + 0xb8` DOES come out as the ROM's
 *    `mov r5,r3 / add r5,#0xb8`, because `hookp` is read TWICE (once for the
 *    guard, once after the DMA) so the address survives as a pseudo.  The
 *    discriminator is the USE COUNT of the derived address, not the spelling.
 *
 *  * INLINING THE `Func_8000d30` CALL AS A CAST EXPRESSION IS A WRONG PROGRAM.
 *    `(*(u8 *(*)(u8 *, u8 *))Func_8000d30)(...)` measures 1648 bytes / 680
 *    instructions -- BETTER than this park on both -- but it emits
 *    `bl Func_8000d30`, a direct THM_CALL, where the ROM has a pool word plus
 *    `bl _call_via_r3`.  A closer size from a changed relocation FORM is a
 *    worse candidate, and this is the third instance of that trap on file.
 *    The candidate is kept at scratch_elev/b306a/c4ab.c if anyone wants the
 *    measurement; it must not be installed.
 *
 *  * THE ROM'S `ldr r1, =0x2c4` IS UNREACHABLE AS WRITTEN.  0x2c4 = 0xb1 << 2
 *    is `thumb_shiftable_const`, so gcc-2.96 ALWAYS splits an SImode one into
 *    `mov r1,#177 / lsl r1,#2` (elevation.md, "The shiftability tell needs a
 *    MODE").  The pool word carries no relocation in the reference, so it is
 *    not the symbol the tell would predict, and the argument is an SImode `int`
 *    so the HImode escape does not apply.  Costs +1 instruction, -4 pool bytes.
 *    Same class as Func_80a5cc0's `ldr r1,=0xaf0`.
 *
 *  * THE PEELED INSERTION SORT DID NOT PAY.  Rewriting the shift loop in the
 *    ROM's literal peeled shape (first compare peeled out, a walking `u16 *d`
 *    for the shift stores) measures 1656 / 683 / 40.7% against this park's
 *    1656 / 682 / 44.1% -- no size or count gain and worse aligned, though it
 *    did drop the hunk count 96 -> 88.  Kept at scratch_elev/b306a/c4d.c.
 *
 * THE BLOCKER, AND WHAT RULES OUT THE ALTERNATIVES.  The +8 instructions are
 * TWO residues, both in loop.c, and NEITHER is a scheduling problem:
 *
 *  A. +7 IN THE INSERTION-SORT PREHEADER -- loop.c's `strength_reduce` plus
 *     invariant motion make TWO extra quantities we do not want.  The ROM's
 *     preheader is `add r3,r4 / add r3,#0x28 / mov r1,#0 / mov r12,r3 /
 *     mov r14,r1` -- one giv for the line-pointer walk (r12, `add r12,-4`) and
 *     one giv for the byte offset k = 2*(nord+1) (r14).  Ours adds a THIRD,
 *     `ip` = &ord[nord], and then HOISTS `&ord[0]` as loop-invariant and spills
 *     it to sp+0, which is also the +4-byte frame (ours 56 vs the ROM's 0x38
 *     after (3); before (3) it was 60).  The reason the ROM has no hoisted base
 *     is visible in the reference: it computes `add r5, sp, #0x30` TWICE, once
 *     in the `nord >= 0` arm and once in the `nord < 0` arm, and neither
 *     dominates the other, so the pseudo has two sets and loop.c's
 *     `scan_loop` refuses it (`set_in_loop != 1`).  We write `ord[m]` and
 *     `ord[m+1]` as one expression form, CSE commons the base to a single set,
 *     and the single set is exactly what makes it movable.  RULED OUT: this is
 *     not sched2 -- sched2 does not create registers, and the extra insns are
 *     `mov ip,r3` / `str r1,[sp]` DEFS, not a reordering.  It is not
 *     allocno_compare either: the quantity count differs, and batch 301's rule
 *     is that a spelling can only move n_refs, live_length and declaration
 *     order, none of which can delete an allocno.
 *
 *  B. +3 AT THE MEASURE-LOOP BOTTOM, AND THE `_call_via_sl` -- loop.c hoists
 *     `ldr rX, =Func_8000d30` out of the blit loop as invariant, into r10/sl.
 *     The ROM materialises it INSIDE the loop (`ldr r3,=Func_8000d30 /
 *     bl _call_via_r3`).  Taking sl for a constant is what rotates the three
 *     high-register values: ours has nlinesp in sl and buf in fp where the ROM
 *     has nlinesp in r11 and buf in r10, and the rotation is why the measure
 *     loop's `continue` tail (`mov r3,sl / ldrb r2,[r3] / b`) no longer
 *     cross-jumps with the fall-through tail, leaving a THIRD copy of the
 *     `i < *nlinesp` test where the ROM has two.  RULED OUT: the two calls are
 *     the pass's own doing, not allocation -- global_alloc was handed a
 *     pseudo that loop.c had already lifted out of the loop, so its live range
 *     spans the calls and it must take a call-saved register.  Declaring the
 *     pointer inside the loop body does not help (block scope is not a
 *     liveness statement), and the cast-expression form that does defeat the
 *     hoist changes the call to a direct one (see the negatives).
 *
 * NEXT ACTION for whoever picks this up: (A) is the whole +7 and it is a
 * TWO-SETS-VS-ONE-SET question, not an ordering one.  Write the two `ord`
 * base computations so CSE cannot common them -- the two arms of the
 * `nord >= 0` test each need their own, textually distinct, route to
 * `&ord[0]`.  If that lands, the frame returns to 0x38, sp+0 frees, and the
 * high-register rotation in (B) may follow from the pressure drop alone.
 */

#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct TextLine {
    /* 0x00 */ u16 unk00;
    /* 0x02 */ s16 penx;
    /* 0x04 */ u8 kind;
    /* 0x05 */ u8 pal;
    /* 0x06 */ u8 order;
    /* 0x07 */ u8 mode;
    /* 0x08 */ u8 **glyphs;
    /* 0x0c */ u32 unk0c;
    /* 0x10 */ u8 *text;
    /* 0x14 */ u8 pos;
    /* 0x15 */ u8 back;
    /* 0x16 */ u8 measured;
    /* 0x17 */ u8 pending;
};

struct Label {
    /* 0x00 */ u8 pad00[8];
    /* 0x08 */ u16 tile;
    /* 0x0a */ u8 pad0a[0x12];
    /* 0x1c */ u8 fmt;
    /* 0x1d */ u8 pad1d[3];
    /* 0x20 */ u8 width;
    /* 0x21 */ u8 height;
    /* 0x22 */ u8 pad22[2];
    /* 0x24 */ u8 style;
    /* 0x25 */ u8 dirty;
    /* 0x26 */ u8 opts;
    /* 0x27 */ u8 nlines;
    /* 0x28 */ struct TextLine *lines[4];
};

extern void *galloc_iwram(s32 tag, s32 size);
extern void gfree(s32 tag);
extern void *Func_8004938(s32 size);
extern void free(void *p);
extern void Func_8009bb8(void);
extern u8 Data_8009d9c[];
extern void *gPtrs[];
extern void *iwram_3001e68[];
extern u8 *DecompressLZ(u8 *src, u8 *dst);
extern u8 *DecompressSpriteLZ(u8 *src, u8 *dst);
extern void SpriteLayer_SetAnim(struct TextLine *line, int anim);
extern int UploadSpriteGFX(int fmt, int size, int arg);
extern void Func_80008d4(void);
extern void Func_8000d30(void);

extern u8 L1307c[] __asm__(".L1307c");
extern u8 L1308c[] __asm__(".L1308c");
extern u8 L13094[] __asm__(".L13094");
extern u8 L1309c[] __asm__(".L1309c");
extern u8 L130ac[] __asm__(".L130ac");
extern u8 L130bc[] __asm__(".L130bc");
extern u8 L130c4[] __asm__(".L130c4");
extern u8 L130cc[] __asm__(".L130cc");
extern u8 L1310c[] __asm__(".L1310c");

int UpdateSpriteAnim(struct Label *ctl, s16 style)
{
    void **hookp;
    void (*blit)(u8 *, u8 *, int);
    void (*clear)(void *, unsigned int);
    void (*clear2)(void *, unsigned int);
    u8 *g;
    u8 *nlinesp;
    u8 *dirtyp;
    u8 *wp;
    u8 *hp;
    u8 *buf;
    u8 *out;
    u8 *p;
    u8 *q;
    struct TextLine *line;
    int special;
    int resident;
    int i;
    int w;
    int m;
    int a;
    unsigned int mv;
    unsigned int px;
    int nord;
    int cA;
    int cB;
    int slot;
    int tile;
    unsigned int size;
    unsigned int x;
    unsigned int wv;
    unsigned int hv;
    unsigned int xlim;
    unsigned int ylim;
    u16 ord[4];

    special = 0;
    g = (u8 *)iwram_3001e68[0];
    resident = 1;
    hookp = (void **)((u8 *)iwram_3001e68 + 0xb8);
    blit = (void (*)(u8 *, u8 *, int))*hookp;
    if (blit == 0) {
        void *arena = galloc_iwram(0x34, 0x2c4);
        DMA3_COPY((void *)Func_8009bb8, arena,
                  (u32)(Data_8009d9c - (u8 *)Func_8009bb8));
        blit = (void (*)(u8 *, u8 *, int))*hookp;
        resident = 0;
    }

    dirtyp = &ctl->dirty;
    nlinesp = &ctl->nlines;
    for (i = 0; i < *nlinesp; i++) {
        line = ctl->lines[i];
        if (line == 0)
            continue;
        if (line->text == 0)
            continue;
      top:
        px = (u16)line->penx;
        if (line->penx > 0)
            goto measured;
        w = line->text[line->pos++];
        a = line->text[line->pos++];
        switch (w) {
        case 0xfe:
            SpriteLayer_SetAnim(line, a);
            ctl->style = a;
            goto top;
        case 0xfd:
            line->pos = a;
            goto top;
        case 0xf5:
            line->penx = (u16)line->penx + (a << 4);
            goto top;
        case 0xf1:
            line->pos -= 2;
            goto pending;
        case 0xf0:
            line->kind = a;
            goto top;
        case 0xff:
            line->pending = 0xff;
            w = 0xff;
            line->penx = (u16)line->penx + (a << 4);
            goto dispatch;
        case 0xef:
            line->pending = 0xff;
            line->text = 0;
            (*nlinesp)--;
            w = 0xff;
            goto dispatch;
        case 0xf2:
        case 0xf3:
        case 0xf4:
        case 0xf6:
        case 0xf7:
        case 0xf8:
        case 0xf9:
        case 0xfa:
        case 0xfc:
            goto top;
        case 0xfb:
        default:
            line->pending = w;
            line->penx = (u16)line->penx + (a << 4);
            goto dispatch;
        }
      measured:
        line->penx = px - line->back;
      pending:
        w = line->pending;
      dispatch:
        switch (line->kind) {
        case 1:
            mv = L1307c[(u16)style >> 13];
            break;
        case 2:
        case 20:
            mv = L13094[(u16)style >> 13];
            break;
        case 22:
            mv = L1308c[(u16)style >> 13];
            break;
        case 3:
            mv = L1309c[(u16)style >> 12];
            break;
        case 4:
            mv = L130cc[(u16)style >> 10];
            break;
        case 5:
            mv = L130ac[(u16)style >> 12];
            break;
        case 6:
            mv = L1310c[(u16)style >> 10];
            break;
        case 8:
            mv = L130bc[(unsigned int)(((u16)style << 16) + 0x10000000) >> 29];
            break;
        case 88:
            mv = L130c4[(unsigned int)(((u16)style << 16) + 0x10000000) >> 29];
            break;
        default:
            mv = 0;
            break;
        }
        w += mv & 7;
        if (i == 0 && (mv >> 7) != 0)
            special = 1;
        if (line->measured != w) {
            line->measured = w;
            *dirtyp = 1;
        }
    }

    if (*dirtyp != 0) {
        wp = &ctl->width;
        hp = &ctl->height;
        size = *wp * *hp;
        buf = Func_8004938(size);
        clear = (void (*)(void *, unsigned int))Func_80008d4;
        clear(buf, size);

        nord = -1;
        for (i = *nlinesp - 1; i >= 0; i--) {
            unsigned int key;
            line = ctl->lines[i];
            if (line == 0)
                continue;
            if (line->glyphs == 0)
                continue;
            if (line->measured == 0xff)
                continue;
            if (line->order > 3)
                continue;
            key = (line->order << 8) | i;
            m = nord;
            while (m >= 0 && ord[m] > key) {
                ord[m + 1] = ord[m];
                m--;
            }
            ord[m + 1] = key;
            nord++;
        }
        nord++;
        for (i = 0; i < nord; i++) {
            line = ctl->lines[(u8)ord[i]];
            if (line->mode == 1) {
                DecompressLZ(line->glyphs[line->measured], buf);
            } else if (line->mode == 3) {
                if (line->pal != 0) {
                    u8 *tmp = Func_8004938(0x80 << 3);
                    blit(DecompressSpriteLZ(line->glyphs[line->measured], tmp),
                         buf, line->pal);
                    free(tmp);
                } else {
                    u8 *(*expand)(u8 *, u8 *) = (u8 *(*)(u8 *, u8 *))Func_8000d30;
                    u8 *r = expand(line->glyphs[line->measured], buf);
                    if (r != 0)
                        blit(r, buf, 0);
                }
            } else {
                blit(line->glyphs[line->measured], buf, line->pal);
            }
        }

        if ((ctl->opts & 2) != 0) {
            out = Func_8004938(size);
            cA = g[6];
            wv = *wp;
            hv = *hp;
            cB = g[7];
            clear2 = (void (*)(void *, unsigned int))Func_80008d4;
            clear2(out, size);
            p = buf + wv + 1;
            q = out + wv + 1;
            ylim = hv - 1;
            xlim = wv - 1;
            for (i = 1; i < ylim; i++) {
                for (x = 1; x < xlim; x++) {
                    if (p[-1] != 0 && p[1] != 0 && p[-(int)wv] != 0
                        && p[wv] != 0)
                        *q = 1;
                    p++;
                    q++;
                }
                p += 2;
                q += 2;
            }
            p = buf;
            q = out;
            for (i = 0; i < size; i++) {
                if (*q != 0)
                    *p = cB;
                else if (*p != 0)
                    *p = cA;
                p++;
                q++;
            }
            free(out);
        }

        slot = UploadSpriteGFX(ctl->fmt, size, 0);
        {
            void (*upload)(u8 *, int, int, int);
            upload = (void (*)(u8 *, int, int, int))
                     *(void **)((u8 *)gPtrs + 0xd4);
            upload(buf, *wp, *hp, 0x6010000 + (slot << 5));
        }
        slot &= 0x3ff;
        tile = ctl->tile & ~0x3ff;
        tile |= slot;
        ctl->tile = tile;
        *dirtyp = 0;
        *(u16 *)g += size;
        free(buf);
    }

    if (resident == 0)
        gfree(0x34);
    return special;
}
