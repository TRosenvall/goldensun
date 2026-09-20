/* LoadGS1CreditsBG -- asm/rom_f0000/rom_f0254_a_a.s (2 functions; the sibling Func_80f0254 is
 * parked at src/non_matching/rom_f0000/80f0254.c, so landing needs a two-way split).
 *
 * NOT MATCHING: 14 differing of 87 encodings, SIZE IDENTICAL -- and 13 of those are instructions,
 * one is a pool word. Candidate below.
 *
 * ================ BLOCKED ON A size.sym ENTRY: _SIZE_80f0024 = 0x230 ================
 *
 * The ROM has `ldr r5, =0x230`. Spelled as the literal, the function COLLAPSES to 76 lines
 * against 82, because gcc builds 0x230 with `mov/lsl` (it is shiftable -- 0x8c << 2, or
 * equivalently 0x23 << 4) and then folds the runtime `lsr r5, #2 / orr r2, r5` count-word build
 * into a single pooled `=0x8400008c`.
 *
 * THE ARITHMETIC IS VERIFIED, not inferred: Func_80f0024 is at 0x080f0024
 * (asm/rom_f0000/rom_f0008.s:13) and Func_80f0254 at 0x080f0254, so the gap is exactly 0x230.
 * That is precisely size.sym's class -- the size of a routine the game copies into RAM and runs
 * there -- and there are four existing _SIZE_ entries on the same footing.
 *
 * NOT ADDED, because the entry would buy a NON-MATCH: 13 encodings still differ with it. Same
 * reasoning that kept _MSG_b24 out in batch 272 and the label.sym entry out of
 * src/non_matching/rom_77000/8078144.c in this one. A build input is worth adding when it
 * COMPLETES a function.
 *
 * IT ALSO RETIRES AN OPEN QUESTION ELSEWHERE. src/non_matching/rom_9000/8012388.c closes with
 * "NEXT: a way to keep a local constant unfolded, which nothing in the notebook currently
 * offers". Its 0x27c is this same class -- a _SIZE_8009e7c, not a fold problem. That park should
 * be re-read with this in mind.
 *
 * ================ THE -fno-gcse MEASUREMENT, ANSWERED IN THE NEGATIVE ================
 *
 * This file's sibling src/rom_f0000/rom_f0254_a_b.c carries GCSE_CFLAGS, which is why the flag was
 * checked here. It is BYTE-FOR-BYTE INERT -- the full instruction streams were diffed rather than
 * the counts, and they are IDENTICAL, on three separate candidates (13, 17 and 14 differing).
 * NO Makefile row is warranted. Recorded because "the neighbour has the flag" is exactly the kind
 * of inference that would otherwise get acted on.
 *
 * THREE LEVERS DID LAND, 67 differing to 14 to 13:
 *   - the park's DMA3_SET / DmaQueue idiom, transferred.
 *   - THE QUEUE POINTER AS AN INLINE'S PARAMETER, so it is materialised before REG_IME. That fixed
 *     the whole tail, 67 to 14.
 *   - `bias, pal, dst` ORDER IN BOTH IF-ARMS. An arm-order MISMATCH makes gcc emit `pal`'s
 *     high-register copy in each arm instead of once at the join, where jump2's cross-jumping
 *     merges it. All eleven arm-order permutations were measured: (bias,dst,pal) and
 *     (bias,pal,dst) are 14; (dst,bias,pal) in both arms is 21; everything else 62-68.
 *   - a named `vu16 *ime` pointer instead of `SET_IO(REG_IME, ...)` takes 14 to 13.
 *
 * REMAINING 13 WITH THE SYMBOL: a `mov r1, r7` / `add r0, r6, r2` adjacent swap at the indirect
 * call (2), an r0/r1 assignment swap across the whole IME/queue block (7+2), and the
 * `add r2, #1` / `stmia` order (2). `count++` before the src store fixes the last pair but flips
 * `file`/`dst` between r6 and r7 (17); declaration order, inline-parameter order and a named `src`
 * local are all inert on it.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

extern struct DmaQueue gDMATaskCount;
extern unsigned char gPtrs[];
extern unsigned char _SIZE_80f0024[];

extern unsigned char *GetFile(int id);
extern unsigned char *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void Func_80f0254(int page);
extern void Func_80f0024(void);

typedef void (*Fn)(void *src, void *dst, u32 bias);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void PushDma(struct DmaQueue *queue, const void *src, void *dest)
{
    vu16 *ime;
    u32 savedIme;
    s32 count;
    u32 *task;

    ime = (vu16 *)REG_ADDR_IME;
    savedIme = *ime;
    SET_IO(*ime, REG_ADDR_IME);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *task++ = (u32)src;
        *(u16 *)queue = count + 1;
        *task++ = (u32)dest;
        *task = 0x84000040;
    }
    SET_IO(*ime, savedIme);
}

void LoadGS1CreditsBG(int id, int page)
{
    unsigned char *file;
    void *dst;
    void *pal;
    u32 bias;
    unsigned char *g;
    unsigned char *scratch;
    Fn fn;
    int n;

    if (id == 0) {
        Func_80f0254(page);
        return;
    }
    file = GetFile(id);
    if (page == 0) {
        bias = 0;
        dst = (void *)(0xc0 << 19);
        pal = (void *)(0xa0 << 19);
    } else {
        bias = 0x80808080;
        pal = (void *)0x5000100;
        dst = (void *)0x6008000;
    }
    n = (int)&_SIZE_80f0024;
    scratch = galloc_iwram(0x31, n);
    DMA3_COPY(Func_80f0024, scratch, n);
    g = gPtrs;
    g += 0xc4;
    fn = *(Fn *)g;
    fn(file + (0x80 << 1), dst, bias);
    gfree(0x31);
    PushDma(&gDMATaskCount, file, pal);
}
