/* Cluster OvlFunc_919_2008200..OvlFunc_919_2008200 -- the whole of
 * goldensun/asm/overlays/rom_7a67d8/ovl_30_c_a_c_a.s, confirmed data-free by
 * split_s.py.
 *
 * Total .text for this TU = 108 bytes.
 *
 * The previous park was right about everything it claimed, and its
 * offset-clobber head is kept verbatim. Its remaining 18 differing lines were
 * ENTIRELY the tail's pointer locals, not the head's register swap: replacing
 * the three local-pointer halfword writes with struct-field writes fixed the
 * head's register roles as a side effect. A residue in one place can be
 * downstream of a spelling somewhere else entirely.
 */
#include "gba/types.h"
#include "gba/io.h"

struct S {
    unsigned char pad00[0x52a];
    unsigned short f52a;
    unsigned char pad52c[8];
    unsigned short f534;
    unsigned short f536;
};

extern void *iwram_3001ebc[];
extern void __Func_808fe38(int n);
extern void OvlFunc_919_20082e0(void);

int OvlFunc_919_2008200(void)
{
    char *p;
    int *d;
    int off;

    off = 0xe0 << 1;
    p = (char *)iwram_3001ebc[0];
    d = (int *)(p + off);
    off -= 0xc0;
    *d = off;
    __Func_808fe38(9);
    REG_BLDCNT = 0x3f42;
    REG_BLDALPHA = 0xc04;
    ((struct S *)iwram_3001ebc[4])->f534 = 0x3f3f;
    ((struct S *)iwram_3001ebc[4])->f536 = 0x1f;
    ((struct S *)iwram_3001ebc[4])->f52a = 0xa;
    OvlFunc_919_20082e0();
    return 0;
}
