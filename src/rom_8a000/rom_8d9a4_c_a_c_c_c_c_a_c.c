/* Cluster Func_808ef70..Func_808ef70 extracted from
 * goldensun/asm/rom_8a000/rom_8d9a4_c_a_c_c_c_c_a.s.
 *
 * Total .text for this TU = 344 bytes (= 0x158). Never attempted before batch 278.
 * No pins, no flags. 144 instructions, 7 candidates.
 *
 * A PARK CARRIED THE RECORD LAYOUT, AND THIS FUNCTION THEN ESCAPED THE BLOCKER THAT PARK NAMES.
 * src/non_matching/overlays/200b6d0.c supplied `struct Sprite` with the `b0:2, b2:2, b4:4`
 * bitfields at offset 9 and the `struct Actor` shape with f50/f6c. That park is the one whose
 * one-instruction residue is a `find_equiv_reg` call-result copy, priced out across 25
 * spellings. HERE THE COPY SURVIVES, because the ROM's `ret = 0` between _CreateActor and
 * _Actor_SetScript kills r0's equivalence -- and writing the function as THREE PLAIN `return`s
 * rather than a single-exit `ret` local is what produces that. 30 -> 16.
 *
 * So the recorded class statement holds and now has its discriminator: a one-instruction residue
 * at a call-result copy is a pressure symptom, and whether it is reachable depends on whether
 * anything writes r0 between the call and the copy.
 *
 * BOTH NARROW MASKS ARE BITFIELD STORES, WHICH IS WHAT KEEPS THEM SImode -- the same lever that
 * landed OvlFunc_common1_1608 in this batch, and a second independent instance of it:
 *   `s->c5 = 0;` gives the ROM's `sub r3, #0x21` (derived off the live zero), where a plain
 *   `s->f05 &= ~0x20;` truncates to `mov r3, #0xdf`.
 *   `unsigned short f08 : 10;` gives `ldr r3, =0xfffffc00` + `and r0, #0x3ff`, where a
 *   hand-rolled mask on a u16 field truncates to `mov r3, #0xfc / lsl r3, #8`.
 *
 * AND THE SINGLE STORE `(v & 0xf) & ~0xc | 4` IS TWO ADJACENT BITFIELD WRITES TO ONE BYTE, NOT
 * ONE EXPRESSION. `s->b12 = 0; s->b10 = 1;` with `u16 f08:10, b10:2, b12:4` emits exactly one
 * ldrb/strb pair carrying both ANDs. An intermediate `int u` reproduces both ANDs but leaves
 * `mov r1,#0xd / neg` hoisted (16); `unsigned char u` is 4; the bitfield pair is 0.
 *
 * Naming the index (`idx = slot * 4 + 0x14`) gives the ROM's `add r0,#0x14 / ldr r7,[r6,r0]`
 * rather than `add r0,r6 / ldr r7,[r0,#0x14]` -- the batch-277 "name the INDEX, not the address"
 * rule.
 *
 * Measured inert: statement order for base/idx (16); the pointer form of the global (16).
 * Worse: the byte-9 read before `s->c5 = 0` (30).
 */
struct Sprite {
    unsigned char pad00[5];
    unsigned char c0 : 5;
    unsigned char c5 : 1;
    unsigned char c6 : 2;
    unsigned char pad06[2];
    unsigned short f08 : 10;
    unsigned short b10 : 2;
    unsigned short b12 : 4;
    unsigned char pad0a[0x1c - 0x0a];
    unsigned char f1c;
    unsigned char pad1d[0x26 - 0x1d];
    unsigned char f26;
    unsigned char f27;
};

struct Actor {
    void *f00;
    unsigned char pad04[2];
    unsigned short f06;
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x28 - 0x14];
    int f28;
    unsigned char pad2c[0x48 - 0x2c];
    int f48;
    unsigned char pad4c[0x50 - 0x4c];
    struct Sprite *f50;
    unsigned char pad54[0x6c - 0x54];
    void *f6c;
};

extern unsigned char iwram_3001ebc[];
extern unsigned char L9e87c[] __asm__(".L9e87c");
extern unsigned char L9e6c0[] __asm__(".L9e6c0");

extern void vec3_translate(int dist, int angle, int *v);
extern void _DeleteActor(struct Actor *a);
extern void WaitFrames(int n);
extern struct Actor *_CreateActor(int id, int x, int y, int z);
extern void _Actor_SetScript(struct Actor *a, unsigned char *s);
extern void *galloc_iwram(int index, unsigned int size);
extern void _LoadItemIcon(int item);
extern int UploadSpriteGFX(int slot, int n, void *src);
extern void gfree(int index);
extern void Func_808f28c(void);
extern void Func_808eee4(void);

struct Actor *Func_808ef70(int slot, int item);

struct Actor *Func_808ef70(int slot, int item)
{
    struct Actor *obj;
    struct Actor *a;
    struct Sprite *s;
    unsigned char *base;
    unsigned char *buf;
    int v[3];
    int x;
    int z;
    int i;
    int u;
    int g;
    int idx;

    base = *(unsigned char **)iwram_3001ebc;
    idx = slot * 4 + 0x14;
    obj = *(struct Actor **)(base + idx);
    if (obj == 0)
        return 0;
    v[0] = obj->f08;
    v[1] = obj->f0c;
    v[2] = obj->f10;
    vec3_translate(0x80 << 13, obj->f06, v);
    x = (v[0] & 0xfff00000) + (0x80 << 12);
    z = (v[2] & 0xfff00000) + (0x80 << 12);
    if (*(short *)(base + 0xcb8) != 0) {
        a = *(struct Actor **)(iwram_3001ebc - 0x58);
        for (i = 0x3f; i >= 0; i--) {
            if (a->f00 != 0) {
                if (a->f6c == Func_808f28c)
                    _DeleteActor(a);
                if (a->f00 == L9e87c)
                    _DeleteActor(a);
            }
            a++;
        }
    }
    WaitFrames(3);
    obj = _CreateActor(0x16, x, 0x80 << 13, z);
    if (obj == 0)
        return 0;
    _Actor_SetScript(obj, L9e6c0);
    s = obj->f50;
    s->f26 = 0;
    s->f27 = 0;
    s->c5 = 0;
    s->b12 = 0;
    s->b10 = 1;
    obj->f28 = 0x80 << 10;
    obj->f48 = 0x80 << 7;
    buf = galloc_iwram(0x11, 0xc1 << 3);
    _LoadItemIcon(item);
    buf += 0x80 << 3;
    g = UploadSpriteGFX(s->f1c, 0x80, buf);
    s->f08 = g;
    gfree(0x11);
    obj->f6c = Func_808eee4;
    return obj;
}
