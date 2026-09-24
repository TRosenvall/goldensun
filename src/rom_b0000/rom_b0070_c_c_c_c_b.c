/* InnHeal  --  0x080b3398, first of four in asm/rom_b0000/rom_b0070_c_c_c_c.s.
 * EXACT: 172 bytes, 72 encodings and 14 relocations identical (objcmp, 3 runs).
 * No pins, no barriers, no shim, no new .sym entry.
 *
 * TWO THINGS ARE LOAD-BEARING, and the drop ladder says nothing else is.
 *
 * 1. THE PARTY-SLOT HALFWORD IS READ TWICE, NOT CACHED.  `_GetUnit(list[i])`
 *    and `_UpdateStatBarPercent(list[i])` are two textual reads of the same
 *    `short` and the ROM has two `ldrsh`.  Caching it in `id` is 66 of 72 and
 *    four bytes short -- the same tell rom_a1000/rom_a5534_a_c_c.c landed on.
 * 2. THE `for` FORM.  `cmp r7,#0 / ble` over a countdown counter is what
 *    check_dbra_loop makes of `for (i = 0; i < n; i++)`.  The explicit
 *    `i = 0; if (n > 0) do {} while (i < n)` guard is 54 of 72.
 *
 * INERT, all three measured EXACT: the struct-typed unit against four hand
 * casts off an `unsigned char *`; `0x1c0`/`0x1c8` written flat instead of
 * `0xe0 << 1`/`0xe4 << 1`; and `price = -price;` split out of the _AddCoins
 * argument list.  The `0xe0 << 1` spelling is kept only because it is the
 * bank's convention -- it is not doing any work here.
 *
 * Note `ldrsh` is register-offset-only in Thumb, so gcc materialises the
 * loop's byte offset itself.  No `off = 0` scaffold: `list[i]` is enough.
 */
struct Unit {
	unsigned char pad00[0x34];
	unsigned short maxhp;
	unsigned short maxpp;
	short hp;
	unsigned short pp;
};

extern unsigned char *iwram_3001ebc;

extern int _Func_80796c4(void *buf);
extern void _AddCoins(int amount);
extern struct Unit *_GetUnit(int id);
extern void _UpdateStatBarPercent(int id);
extern void WaitFrames(int n);
extern void _MapTransitionOut(void);
extern void _MapTransitionIn(void);
extern void _WaitMapTransition(void);
extern void _PlaySound(int sfx);
extern void Func_80b04c4(void);

void InnHeal(int price)
{
	short list[8];
	struct Unit *u;
	int n;
	int i;

	n = _Func_80796c4(list);
	_AddCoins(-price);
	for (i = 0; i < n; i++) {
		u = _GetUnit(list[i]);
		if (u->hp != 0) {
			u->hp = u->maxhp;
			u->pp = u->maxpp;
			_UpdateStatBarPercent(list[i]);
		}
	}
	*(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
	*(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x3c;
	WaitFrames(0x14);
	_MapTransitionOut();
	_WaitMapTransition();
	_PlaySound(0x56);
	Func_80b04c4();
	WaitFrames(0xa);
	_MapTransitionIn();
	_WaitMapTransition();
	WaitFrames(0x1e);
	*(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
}
