extern volatile unsigned int gKeyHeld;
extern unsigned char *iwram_3001ebc;

extern int Func_8092c40(int slot, int style);
extern void WaitFrames(int n);
extern int Func_8092ba8(int slot);
extern int Func_808d394(int id);
extern void _Func_8019e48(int id);
extern void _Func_8019a54(void);
extern int _Func_8017394(int h);

void ActorMessage(int slot, int style)
{
	unsigned char *p;
	int h;
	int id;
	int m;
	unsigned int n;

	p = iwram_3001ebc;
	h = Func_8092c40(slot, style);
	WaitFrames(1);
	n = 0;
	id = Func_8092ba8(slot);
	if (slot <= 7) {
		m = slot & 0xfff;
		if (Func_808d394(m) == 0)
			id = m;
	}
	_Func_8019e48(id);
	if (*(int *)(p + 0x1cc) == 0) {
		while (_Func_8017394(h) == 0) {
			WaitFrames(1);
			n++;
			if (n > 0x258
			    || ((gKeyHeld & 4) != 0 && (gKeyHeld & 0x100) != 0
				&& (gKeyHeld & 0x200) != 0 && (gKeyHeld & 1) != 0))
				_Func_8019a54();
		}
	}
	WaitFrames(1);
}
