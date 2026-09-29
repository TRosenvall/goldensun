#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef int (*ClearFn)(void *dst, s32 len);
typedef int (*FillFn)(void *dst, u32 size, u32 value);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern int *iwram_3001eec[];
extern Part gBuffer[];
extern unsigned char ewram_2010c56[], ewram_2011156[], ewram_2012a56[];
extern unsigned char ewram_2012d80[], ewram_2013c56[], ewram_2014000[];
extern unsigned char ewram_20146e4[], ewram_2014b00[], ewram_2015980[];
extern unsigned char ewram_20158d2[], ewram_20106e8[];
extern unsigned short Data_ede48[];
extern unsigned char Data_ede9f[], Data_edea5[], Data_edeab[];
extern unsigned short Data_edeb2[], Data_edebe[], Data_edeca[], Data_eded0[];
extern unsigned char Leedd0[] __asm__(".Leedd0");
extern unsigned char Leedd4[] __asm__(".Leedd4");
extern unsigned char Leede2[] __asm__(".Leede2");
extern unsigned short Leedea[] __asm__(".Leedea");
extern void *gPtrs[];
extern void *iwram_3001f0c;

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void BuildDraw2DFuncs(int a, DrawFn *out);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void MatrixRoll(int a);
extern void MatrixPitch(int a);
extern void MatrixYaw(int a);
extern int  Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80e38b8(void *p, int a, int b);
extern void Func_80e46f0(int id);
extern void Func_80df9d0(void *a, void *b, int c, int d);
extern int *_GetBattleActor(int id);
extern int  _Func_80b8530(int id);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern void _Actor_SetAnim(void *actor, int anim);
extern void _Actor_SetAnimSpeed(void *actor, int speed);
extern void _SetBattleActorKnockback(int id, int a);
extern int  Random(void);
extern int  sin(int a);
extern int  cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80cd4b4(void);
extern void Func_80cd52c(void);
extern void Func_80cdb24(int a);
extern void Func_80dbb9c(void);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);
extern int  Func_80008d4(void *dst, s32 len);
extern int  Func_80008d8(void *dst, u32 size, u32 value);
extern void BaseAnim_ParticleSpray(void *context, int subanim);
extern void BaseAnim_Nova(void *context, int subanim);
extern void Anim_MysticFlame(void *context);
extern void Anim_Impair(void *context);
