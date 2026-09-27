/* Func_8021cb8 -- asm/rom_15000/rom_20198_c_c_c_c_c.s   (PARK, fresh target)
 *
 * LoadIconAssetScratch: decompress icon (idx) of file 0xF1, remap every pixel
 * through map[256], allocating new palette slots from the counter at map+0x100
 * (copying OBJ palette 0x5000200[c] to BG palette 0x5000000[n]), then DMA the
 * 0x400-byte result to 0x6004000 + slot*64.
 *
 * 14 encodings of 92 differ (objcmp: "ENCODINGS differ in 14 place(s) (ref 92,
 * ours 92)").  Same size, same schedule, same control flow; the residue is ONE
 * register pair in the remap loop.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/8021cb8.c asm/rom_15000/rom_20198_c_c_c_c_c.s --func Func_8021cb8
 *
 * BLOCKER: the ROM holds the looked-up byte in TWO pseudos -- the output (r2)
 * and a copy that is compared and then overwritten by *cnt (r3):
 *     rom  ldrb r2,[r7,r4] / mov r3,r2 / cmp r3,#0xff ... ldr r3,[r0] / strb r3,[r7,r4]
 *          ... (count > 0x3f)  mov r2,r3 / strb r2,[r5]
 * In the >0x3f path the output is the FULL-WORD *cnt register (low byte stored),
 * not a reload of map[c] and not a narrowed byte load.  Every spelling tried
 * either coalesces the two into one register (int variables) or makes gcc narrow
 * the *cnt load to "ldrb r2,[r0]" (unsigned char v) -- this park is the latter.
 *
 * WHAT WORKED (reusable):
 *   * The loop is a goto loop with the counter test at the bottom
 *     ("i++; if (i < 0x400) goto top;").  A for/do loop lets loop.c hoist
 *     0x100 / 0x5000000 / 0x5000200 into callee-saved regs (108 vs 92 lines);
 *     the ROM rebuilds all three inside the body.
 *   * "map[c] = v = *cnt;" with "else o = v;" reproduces the ROM's else path
 *     reusing the stored register: the else output is *cnt, not 0xff.
 *
 * INERT / WORSE (all measured): v/o as int/int (coalesced, 64 differ);
 * int v / uchar o (zero-extend, 32); "v = map[c]; o = v;" and
 * "o = map[c]; v = o;" copies (coalesced); reloading map[c] in both arms
 * (jump2 merges them, reuse lost); "v = map[c] = *cnt" (zero-extend); an
 * explicit "else v = 0xff" (constant, not the register).
 */
extern int _FILE_f1;
#define FILE_f1 ((int)&_FILE_f1)

extern unsigned char *galloc_iwram(int tag, int size);
extern unsigned char *GetFile(int id);
extern void DecompressLZ1(unsigned char *src, void *dst);
extern unsigned char *Func_8004938(unsigned int size);
extern void free(void *p);
extern void gfree(int tag);

#define REG_DMA3SAD (*(volatile unsigned int *)0x040000D4)

static inline void DMA3_SET(const void *src, void *dst, unsigned int cnt) {
    register volatile unsigned int *_base __asm__("r3") = &REG_DMA3SAD;
    register const void *_src  __asm__("r0") = src;
    register void *_dst  __asm__("r1") = dst;
    register unsigned int _cnt  __asm__("r2") = cnt;
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        :
        : "r" (_base), "r" (_src), "r" (_dst), "r" (_cnt)
        : "memory", "r0"
    );
}

void Func_8021cb8(unsigned char *map, int idx, int slot)
{
    unsigned char *buf;
    unsigned char *file;
    unsigned char *out;
    unsigned char *dst;
    int i;
    unsigned char c;
    unsigned char v;
    unsigned char o;

    buf = galloc_iwram(0x11, 0xc1 << 3);
    file = GetFile(FILE_f1);
    *(unsigned char **)(buf + 0x604) = file + ((unsigned short *)file)[idx];
    DecompressLZ1(*(unsigned char **)(buf + 0x604), buf);
    out = Func_8004938(0x80 << 3);
    dst = out;
    i = 0;
top:
    {
        c = *buf++;
        o = map[c];
        if (o == 0xff) {
            int *cnt = (int *)(map + 0x100);
            map[c] = v = *cnt;
            if (*cnt <= 0x3f) {
                ((unsigned short *)0x5000000)[*cnt] = ((unsigned short *)0x5000200)[c];
                (*cnt)++;
                o = map[c];
            } else
                o = v;
        }
        *dst++ = o;
    }
    i++;
    if (i < 0x400)
        goto top;
    DMA3_SET(out, (void *)(0x6004000 + (slot << 6)), 0x84000100);
    free(out);
    gfree(0x11);
}
