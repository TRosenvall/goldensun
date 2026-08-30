extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(char *, int);
extern void __WaitFrames(int);
extern void __PlaySound(int);
extern void __Actor_SetSpriteFlags(char *, int);
extern void __Func_8092158(int, int, int);
extern void __vec3_translate(int, int, int *);
extern int __TestCollision(char *, int *);

int OvlFunc_946_2009a44(char *p)
{
	int vec[3];
	int *v;
	unsigned char *f;
	int saved;
	int ang;

	f = (unsigned char *)(p + 0x55);
	saved = *f;
	v = vec;
	v[0] = (*(int *)(p + 8) & 0xfff00000) + (0x80 << 12);
	v[1] = *(int *)(p + 0xc);
	v[2] = (*(int *)(p + 0x10) & 0xfff00000) + (0x80 << 12);
	ang = (*(unsigned short *)(p + 6) + (0x80 << 6)) & (0xc0 << 8);
	__vec3_translate(0x80 << 14, ang, v);
	if (__TestCollision(p, v) == 0) {
		__CutsceneStart();
		__Actor_SetAnim(p, 6);
		__WaitFrames(6);
		__PlaySound(0x98);
		__Actor_SetAnim(p, 7);
		*(int *)(p + 0x30) = 0xc0 << 10;
		*(int *)(p + 0x34) = 0x80 << 10;
		*(int *)(p + 0x28) = 0x80 << 11;
		*f = *f & 0x7e;
		__Actor_SetSpriteFlags(p, 0);
		__Func_8092158(0, *(short *)((char *)v + 2), *(short *)((char *)v + 0xa));
		__Actor_SetAnim(p, 6);
		__Actor_SetSpriteFlags(p, 1);
		*f = saved;
		__CutsceneEnd();
		return 1;
	} else {
		return 0;
	}
}
