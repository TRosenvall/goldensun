extern unsigned char gScript_881__0200cbe4[];

extern char *__CreateActor(int kind);
extern int __CheckPartyItem(int item);
extern int __CheckItem(int slot, int item);
extern void __Actor_SetScript(char *actor, unsigned char *script);
extern int __galloc_iwram(int id, int size);
extern void __LoadItemIcon(int item);
extern void __UploadSpriteGFX(int tile, int count, int src);
extern void __gfree(int id);
extern void __PlaySound(int id);
extern void __Func_808f140(char *actor, int a);
extern void __Func_8078948(int a, int b);
extern void __GiveItemTo(int slot, int item);
extern void __DeleteActor(char *actor);
extern void __MapActor_SetAnim(int id, int anim);

int OvlFunc_896_200c260(int item)
{
	char *actor;
	char *s;
	int slot;
	int r;
	char *buf;
	int mask;

	buf = 0;
	actor = __CreateActor(0x16);
	slot = __CheckPartyItem(0xe0);
	r = __CheckItem(slot, 0xe0);
	if (actor == 0)
		return slot;
	__Actor_SetScript(actor, gScript_881__0200cbe4);
	s = *(char **)(actor + 0x50);
	s[0x26] = (int)buf;
	s[0x27] = (int)buf;
	mask = -0x21;
	s[5] = s[5] & mask;
	s[9] = s[9] & 0xf;
	*(int *)(actor + 0x28) = 0xa0 << 10;
	*(int *)(actor + 0x48) = 0x80 << 7;
	buf = (char *)__galloc_iwram(0x11, 0xc1 << 3);
	__LoadItemIcon(item);
	__UploadSpriteGFX(*(unsigned char *)(s + 0x1c), 0x80, (int)((0x80 << 3) + buf));
	__gfree(0x11);
	__PlaySound(0x53);
	__Func_808f140(actor, 3);
	__Func_8078948(slot, r);
	__GiveItemTo(slot, item);
	__DeleteActor(actor);
	__MapActor_SetAnim(0, 1);
	return slot;
}
