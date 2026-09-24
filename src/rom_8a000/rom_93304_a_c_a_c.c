typedef unsigned char u8;
typedef unsigned short u16;

struct Sprite {
	u8 pad00[9];
	u8 b9;
	u8 pad0a[0x26 - 0xa];
	u8 b26;
};

struct Actor {
	u8 pad00[8];
	int f08;
	int f0c;
	int f10;
	u8 pad14[0x50 - 0x14];
	struct Sprite *f50;
	u8 pad54[1];
	u8 f55;
	u8 pad56[0x64 - 0x56];
	u16 f64;
	u16 f66;
	void *f68;
	void *f6c;
};

extern u8 L9fc2c[] __asm__(".L9fc2c");
extern u8 L9fd38[] __asm__(".L9fd38");

extern void _PlaySound(int id);
extern struct Actor *GetFieldActor(int slot);
extern struct Actor *_CreateActor(int kind, int x, int y, int z);
extern void _DeleteActor(struct Actor *a);
extern void _Actor_SetScript(struct Actor *a, u8 *script);
extern void _Actor_SetAnim(struct Actor *a, int anim);
extern int Update_EmoteBubble(struct Actor *a);
extern void CutsceneWait(int frames);

void MapActor_Emote(int slot, int packed, int wait)
{
	struct Actor *a;
	struct Actor *e;
	struct Sprite *s;
	int b1;
	int t1;
	int t2;
	int b2;
	int v2;

	if ((packed & 0xff) == 6)
		_PlaySound(0x6e);
	a = GetFieldActor(slot);
	if (a != 0) {
		e = _CreateActor(0x15, a->f08, a->f0c, a->f10);
		if (e != 0) {
			_Actor_SetScript(e, L9fc2c);
			_Actor_SetAnim(e, packed & 0xf);
			e->f55 = 0;
			e->f64 = 0;
			e->f66 = slot;
			s = e->f50;
			e->f6c = Update_EmoteBubble;
			s->b26 = 0;
			e->f68 = a;
			if (packed & 0x100) {
				b1 = s->b9;
				t1 = ~0xc;
				t1 &= b1;
				t1 |= 4;
				s->b9 = t1;
			} else {
				t2 = 0xc;
				t2 &= a->f50->b9;
				b2 = s->b9;
				v2 = ~0xc;
				v2 &= b2;
				v2 |= t2;
				s->b9 = v2;
			}
		}
		CutsceneWait(wait);
	}
}

void MapActor_Surprise(int slot, int mode)
{
	struct Actor *a;
	struct Actor *e;
	struct Sprite *s;
	int b1;
	int t1;
	int t2;
	int b2;
	int v2;

	a = GetFieldActor(slot);
	e = 0;
	s = 0;
	if (a == 0)
		return;
	if ((mode & 3) != 0) {
		if ((mode & 3) == 2 || a->f68 == 0)
			e = _CreateActor(0xd1, a->f08, a->f0c, a->f10);
	} else {
		e = a->f68;
		if (e == 0)
			return;
		_DeleteActor(e);
		a->f68 = s;
		return;
	}
	if (e == 0)
		return;
	switch (mode & 3) {
	case 1:
		_Actor_SetAnim(e, 1);
		a->f68 = e;
		e->f64 = 1;
		break;
	case 2:
		_Actor_SetAnim(e, 2);
		_Actor_SetScript(e, L9fd38);
		e->f64 = 1;
		break;
	}
	e->f66 = slot;
	e->f55 = 0;
	s = e->f50;
	e->f6c = Update_EmoteBubble;
	s->b26 = 0;
	e->f68 = a;
	if (mode & 0x100) {
		b1 = s->b9;
		t1 = ~0xc;
		t1 &= b1;
		t1 |= 4;
		s->b9 = t1;
	} else {
		t2 = 0xc;
		t2 &= a->f50->b9;
		b2 = s->b9;
		v2 = ~0xc;
		v2 &= b2;
		v2 |= t2;
		s->b9 = v2;
	}
}
