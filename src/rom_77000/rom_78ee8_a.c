/* ResetPCs -- 0x08078ee8, the only function in asm/rom_77000/rom_78ee8_a.s.
 * Whole-file conversion: 284 bytes, 129 encodings and 11 relocations identical.
 *
 * Requires `_MSG_66` (message.sym) -- the base of the eight starting-PC name
 * strings, 0x66..0x6d.  The ROM pools a value gcc would always build with a single
 * `mov`, which is only possible for a symbol; see that entry for the corpus-wide
 * control (zero counterexamples in 4,282 generated .s files).
 *
 * FOUR LEVERS, each measured alone:
 *   - the name copy stores through a `char *`, not a struct `char[15]` member.  A
 *     struct member does NOT give alias set 0 -- the struct's own set governs -- and
 *     `-fno-strict-aliasing` reproduces the same change, which is the proof.
 *   - the copy loop is `while (1)` with THE BOUND TEST FIRST and the zero test last,
 *     plus an explicit peeled store, because expand_end_loop rolls the LAST exit
 *     test into the latch.
 *   - ONE counter serves all three inner loops (81 -> 56 differing).  That makes it
 *     inherit the item loop's call-crossing conflict, which is what excludes r4.
 *   - the item bound is `13U`.  A signed bound lets check_dbra_loop reverse the loop.
 *
 * `p = L7b690;` must be written before the first loop.  `.L7b690` is defined and
 * exported by asm/rom_77000/rom_78ee8_c.s, which is unaffected by this conversion.
 */
struct Unit {
	unsigned char name[15];
	unsigned char pad0f[0x14 - 0x0f];
	unsigned short f14;
	unsigned short f16;
	unsigned char pad18[0xd8 - 0x18];
	unsigned short psy[15];
	unsigned char padf6[0x128 - 0xf6];
	unsigned char pc;
};

struct Base {
	unsigned char pad00[0x96];
	unsigned char lvl;
	unsigned char pad97[1];
	unsigned short items[13];
};

extern struct Unit *GetUnit(int id);
extern void _DecompressString2(int id, unsigned short *dst);
extern struct Base *GetPCBaseStats(unsigned int idx);
extern int GiveItemTo(int id, int item);
extern void EquipItem(int id, int slot);
extern void Func_8079ae8(int id);
extern void SetMinLevel(int id, int lvl);
extern void CalcStats(int id);

extern int _MSG_66;
extern int L7b690[] __asm__(".L7b690");

void ResetPCs(void)
{
	unsigned short buf[16];
	struct Unit *unit;
	struct Base *base;
	int *p;
	char *d;
	int i;
	int j;

	p = L7b690;
	for (i = 0; i <= 7; i++) {
		unit = GetUnit(i);
		_DecompressString2((int)(&_MSG_66) + i, buf);
		d = (char *)unit;
		j = 0;
		d[j] = buf[j];
		if (buf[j] != 0) {
			while (1) {
				if (++j > 13)
					break;
				d[j] = buf[j];
				if (buf[j] == 0)
					break;
			}
		}
		unit->name[14] = 0;
	}
	while (*p != -1) {
		unit = GetUnit(*p);
		if (unit != 0) {
			unit->pc = *p;
			base = GetPCBaseStats(unit->pc);
			for (j = 0; j < 15; j++)
				unit->psy[j] = 0;
			for (j = 0; j < 13U; j++)
				EquipItem(*p, GiveItemTo(*p, base->items[j] & 0x1ff));
			Func_8079ae8(*p);
			unit->f16 = 0x4000;
			unit->f14 = 0x4000;
			SetMinLevel(*p, base->lvl);
			CalcStats(*p);
		}
		p++;
	}
}
