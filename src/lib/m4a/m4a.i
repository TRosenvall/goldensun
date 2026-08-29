# 1 "src/lib/m4a/m4a.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 385 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "src/lib/m4a/m4a.c" 2
# 1 "tools/agbcc/include/string.h" 1
# 14 "tools/agbcc/include/string.h"
# 1 "tools/agbcc/include/_ansi.h" 1
# 15 "tools/agbcc/include/_ansi.h"
# 1 "tools/agbcc/include/sys/config.h" 1
# 103 "tools/agbcc/include/sys/config.h"
typedef int __int32_t;
typedef unsigned int __uint32_t;
# 16 "tools/agbcc/include/_ansi.h" 2
# 15 "tools/agbcc/include/string.h" 2


# 1 "tools/agbcc/include/stddef.h" 1





typedef long int ptrdiff_t;



typedef unsigned long int size_t;






typedef int wchar_t;
# 18 "tools/agbcc/include/string.h" 2





void * memchr (const void *, int, size_t);
int memcmp (const void *, const void *, size_t);
void * memcpy (void *, const void *, size_t);
void * memmove (void *, const void *, size_t);
void * memset (void *, int, size_t);
char *strcat (char *, const char *);
char *strchr (const char *, int);
int strcmp (const char *, const char *);
int strcoll (const char *, const char *);
char *strcpy (char *, const char *);
size_t strcspn (const char *, const char *);
char *strerror (int);
size_t strlen (const char *);
char *strncat (char *, const char *, size_t);
int strncmp (const char *, const char *, size_t);
char *strncpy (char *, const char *, size_t);
char *strpbrk (const char *, const char *);
char *strrchr (const char *, int);
size_t strspn (const char *, const char *);
char *strstr (const char *, const char *);


char *strtok (char *, const char *);


size_t strxfrm (char *, const char *, size_t);


char *strtok_r (char *, const char *, char **);

int bcmp (const char *, const char *, size_t);
void bcopy (const char *, char *, size_t);
void bzero (char *, size_t);
int ffs (int);
char *index (const char *, int);
void * memccpy (void *, const void *, int, size_t);
char *rindex (const char *, int);
int strcasecmp (const char *, const char *);
char *strdup (const char *);
int strncasecmp (const char *, const char *, size_t);
char *strsep (char **, const char *);
char *strlwr (char *);
char *strupr (char *);
# 2 "src/lib/m4a/m4a.c" 2
# 1 "include/gba/types.h" 1



typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long int u64;






typedef signed char s8;



typedef short s16;
typedef int s32;
typedef long long int s64;

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

typedef float f32;
typedef double f64;

typedef u8 bool8;
typedef u16 bool16;
typedef u32 bool32;
typedef vu8 vbool8;
typedef vu16 vbool16;
typedef vu32 vbool32;



typedef s32 fx32;
typedef s16 fx16;

typedef struct {
    fx32 x, y;
} vec2_t;

typedef struct {
    fx32 x, y, z;
} vec3_t;

typedef fx32 matrix_t[4][3];
# 3 "src/lib/m4a/m4a.c" 2
# 1 "include/lib/m4a/m4a_internal.h" 1



# 1 "include/gba/gba.h" 1








# 1 "tools/agbcc/include/stdint.h" 1
# 31 "tools/agbcc/include/stdint.h"
# 1 "tools/agbcc/include/limits.h" 1
# 32 "tools/agbcc/include/stdint.h" 2



typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;



typedef signed char int_least8_t;
typedef short int_least16_t;
typedef int int_least32_t;
typedef long long int_least64_t;
typedef unsigned char uint_least8_t;
typedef unsigned short uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long long uint_least64_t;



typedef int int_fast8_t;
typedef int int_fast16_t;
typedef int int_fast32_t;
typedef long long int_fast64_t;
typedef unsigned int uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long long uint_fast64_t;



typedef int intptr_t;
typedef unsigned int uintptr_t;



typedef long long intmax_t;
typedef unsigned long long uintmax_t;
# 10 "include/gba/gba.h" 2
# 1 "include/gba/defines.h" 1
# 11 "include/gba/gba.h" 2
# 1 "include/gba/io.h" 1
# 12 "include/gba/gba.h" 2
# 1 "include/gba/syscall.h" 1
# 13 "include/gba/syscall.h"
void CpuSet(const void *src, void *dest, u32 control);
void CpuFastSet(const void *src, void *dest, u32 control);
# 13 "include/gba/gba.h" 2
# 1 "include/gba/cpuset_macros.h" 1
# 14 "include/gba/gba.h" 2
# 5 "include/lib/m4a/m4a_internal.h" 2
# 106 "include/lib/m4a/m4a_internal.h"
typedef s32 fixed8_24;






struct MP2KTrack;
struct MP2KPlayerState;

typedef void (*MP2KEventNoteFunc)(u8, struct MP2KPlayerState *, struct MP2KTrack *);
typedef void (*MP2KEventFunc)(struct MP2KPlayerState *, struct MP2KTrack *);
typedef void (*CgbSoundFunc)(void);
typedef void (*CgbOscOffFunc)(u8);
typedef u32 (*MidiKeyToCgbFreqFunc)(u8, u8, u8);
typedef void (*ExtVolPitFunc)(void);
typedef void (*MPlayMainFunc)(struct MP2KPlayerState *);

struct MixerSource {
    u8 status;
    u8 type;
    u8 rightVol;
    u8 leftVol;

    union {
        struct {
            u8 attack;
            u8 decay;
            u8 sustain;
            u8 release;
            u8 key;
            u8 envelopeVol;

            u8 envelopeGoal;
            u8 envelopeCtr;

            u8 echoVol;
            u8 echoLen;
            u8 padding1;
            u8 padding2;
            u8 gateTime;
            u8 untransposedKey;
            u8 velocity;
            u8 priority;
            u8 rhythmPan;
            u8 padding3;
            u8 padding4;
            u8 padding5;

            u8 padding6;
            u8 sustainGoal;
            u8 nrx4;
            u8 pan;

            u8 panMask;
            u8 cgbStatus;
            u8 length;
            u8 sweep;

            u32 freq;
        } cgb;
        struct {
            u8 attack;
            u8 decay;
            u8 sustain;
            u8 release;
            u8 key;
            u8 envelopeVol;

            u8 envelopeVolR;
            u8 envelopeVolL;

            u8 echoVol;
            u8 echoLen;
            u8 padding1;
            u8 padding2;
            u8 gateTime;
            u8 untransposedKey;
            u8 velocity;
            u8 priority;
            u8 rhythmPan;
            u8 padding3;
            u8 padding4;
            u8 padding5;

            u32 ct;
            fixed8_24 fw;

            u32 freq;
        } sound;
    } data;

    void *wav;
    void *current;

    struct MP2KTrack *track;
    struct MixerSource *prev;
    struct MixerSource *next;
    u32 padding7;
    u32 blockCount;
};

struct SoundMixerState {


    u32 lockStatus;

    vu8 dmaCounter;


    u8 reverb;
    u8 numChans;
    u8 masterVol;
    u8 freqOption;

    u8 extensionFlags;
    u8 cgbCounter15;
    u8 framesPerDmaCycle;
    u8 maxScanlines;
    u8 gap[3];
    s32 samplesPerFrame;
    s32 sampleRate;

    s32 sampleRateReciprocal;



    struct MixerSource *cgbChans;
    MPlayMainFunc MPlayMainHead;
    struct MP2KPlayerState *musicPlayerHead;
    CgbSoundFunc CgbSound;
    CgbOscOffFunc CgbOscOff;
    MidiKeyToCgbFreqFunc MidiKeyToCgbFreq;
    void **MPlayJumpTable;
    MP2KEventNoteFunc plynote;
    ExtVolPitFunc ExtVolPit;
    void *reserved2;
    void *reserved3;
    void *reversed4;
    void *reserved5;
    struct MixerSource chans[12];

    s8 pcmBuffer[1584 * 2];



};

struct MP2KVoiceGroup {
    u8 type;
    u8 drumKey;
    u8 cgbLength;
    u8 pan_sweep;
    union {
        struct {
            struct WaveData *wav;
            u8 attack;
            u8 decay;
            u8 sustain;
            u8 release;
        } sound;
        struct {
            struct MP2KVoiceGroup *group;
            u8 *keySplitTable;
        } keySplit;
    } data;
};

struct WaveData {
    u16 type;
    u16 status;

    u32 freq;
    u32 loopStart;
    u32 size;
    s8 data[1];
};

struct MP2KSongHeader {
    u8 trackCount;
    u8 blockCount;
    u8 priority;
    u8 reverb;
    struct MP2KVoiceGroup *voicegroup;
    u8 *part[1];
};

struct MP2KTrack {
    u8 status;
    u8 wait;
    u8 patternLevel;
    u8 repeatCount;
    u8 gateTime;
    u8 key;
    u8 velocity;
    u8 runningStatus;
    s8 keyShiftCalculated;
    u8 pitchCalculated;
    s8 keyShift;
    s8 keyShiftPublic;
    s8 tune;
    u8 pitchPublic;
    s8 bend;
    u8 bendRange;
    u8 volRightCalculated;
    u8 volLeftCalculated;
    u8 vol;
    u8 volPublic;
    s8 pan;
    s8 panPublic;
    s8 modCalculated;
    u8 modDepth;
    u8 modType;
    u8 lfoSpeed;
    u8 lfoSpeedCounter;
    u8 lfoDelay;
    u8 lfoDelayCounter;
    u8 priority;
    u8 echoVolume;
    u8 echoLength;

    struct MixerSource *chan;
    struct MP2KVoiceGroup voicegroup;

    u8 gap[10];
    u16 unk_3A;
    u32 unk_3C;
    u8 *cmdPtr;
    u8 *patternStack[3];
};

struct MP2KPlayerState {
    struct MP2KSongHeader *songHeader;
    u32 status;
    u8 trackCount;
    u8 priority;
    u8 cmd;
    bool8 checkSongPriority;
    u32 clock;
    u8 padding[8];
    u8 *memAccArea;
    u16 tempoRawBPM;
    u16 tempoScale;
    u16 tempoInterval;
    u16 tempoCounter;
    u16 fadeInterval;
    u16 fadeCounter;
    u16 fadeOV;
    struct MP2KTrack *tracks;
    struct MP2KVoiceGroup *voicegroup;
    u32 lockStatus;
    MPlayMainFunc nextPlayerFunc;
    struct MP2KPlayerState *nextPlayer;
};

struct MusicPlayer {
    struct MP2KPlayerState *info;
    struct MP2KTrack *track;
    u8 numTracks;
    u16 unk_A;
};

struct Song {
    struct MP2KSongHeader *header;
    u16 ms;
    u16 me;
};

typedef void (*XcmdFunc)(struct MP2KPlayerState *, struct MP2KTrack *);

extern char SoundMainRAM[];
extern u8 gMPlayMemAccArea[];
extern void *gMPlayJumpTable[];
extern struct MixerSource gCgbChans[];

extern const struct MusicPlayer gMPlayTable[4];
extern const struct Song gSongTable[];
extern const XcmdFunc gXcmdTable[];

extern const u8 gClockTable[];
extern const u8 gScaleTable[];
extern const u32 gFreqTable[];
extern const u16 gPcmSamplesPerVBlankTable[];
extern void *const gMPlayJumpTableTemplate[];

extern const u8 gCgbScaleTable[];
extern const s16 gCgbFreqTable[];
extern const u8 gNoiseTable[];
extern const u8 gCgb3Vol[];


extern char gNumMusicPlayers[];
extern char gMaxLines[];


u32 MidiKeyToFreq(struct WaveData *wav, u8 key, u8 fineAdjust);
u32 umul3232H32(u32 multiplier, u32 multiplicand);
void SoundMain(void);
void SoundMainBTM(void *ptr);
void TrackStop(struct MP2KPlayerState *player, struct MP2KTrack *track);
void MP2KPlayerMain(struct MP2KPlayerState *);

void ClearChain(struct MixerSource *chan);
void MP2KClearChain(struct MixerSource *chan);

void MPlayContinue(struct MP2KPlayerState *mplayInfo);
void MPlayStart(struct MP2KPlayerState *mplayInfo, struct MP2KSongHeader *songHeader);
void MPlayStop(struct MP2KPlayerState *mplayInfo);
void FadeOutBody(struct MP2KPlayerState *mplayInfo);
void TrkVolPitSet(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track);
void MPlayFadeOut(struct MP2KPlayerState *mplayInfo, u16 speed);
void Clear64byte(void *addr);
void SoundInit(struct SoundMixerState *soundInfo);
void MPlayExtender(struct MixerSource *cgbChans);
void m4aSoundMode(u32 mode);
void MPlayOpen(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *tracks, u8 trackCount);
void CgbSound(void);
void CgbOscOff(u8);
void CgbModVol(struct MixerSource *chan);
u32 MidiKeyToCgbFreq(u8, u8, u8);
void MPlayJumpTableCopy(void **mplayJumpTable);
void SampleFreqSet(u32 freq);
void m4aSoundVSyncOn(void);
void m4aSoundVSyncOff(void);

void m4aMPlayTempoControl(struct MP2KPlayerState *mplayInfo, u16 tempo);
void m4aMPlayVolumeControl(struct MP2KPlayerState *mplayInfo, u16 trackBits, u16 volume);
void m4aMPlayPitchControl(struct MP2KPlayerState *mplayInfo, u16 trackBits, s16 pitch);
void m4aMPlayPanpotControl(struct MP2KPlayerState *mplayInfo, u16 trackBits, s8 pan);
void ClearModM(struct MP2KTrack *track);
void m4aMPlayModDepthSet(struct MP2KPlayerState *mplayInfo, u16 trackBits, u8 modDepth);
void m4aMPlayLFOSpeedSet(struct MP2KPlayerState *mplayInfo, u16 trackBits, u8 lfoSpeed);


void MP2K_event_fine(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_goto(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_patt(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_pend(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_rept(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_memacc(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_prio(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_tempo(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_keysh(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_voice(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_vol(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_pan(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_bend(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_bendr(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_lfos(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_lfodl(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_mod(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_modt(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_tune(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_port(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xcmd(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_endtie(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_nxx(u8 clock, struct MP2KPlayerState *, struct MP2KTrack *);


void MP2K_event_xxx(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xwave(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xtype(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xatta(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xdeca(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xsust(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xrele(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xiecv(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xiecl(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xleng(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xswee(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xcmd_0C(struct MP2KPlayerState *, struct MP2KTrack *);
void MP2K_event_xcmd_0D(struct MP2KPlayerState *, struct MP2KTrack *);
# 4 "src/lib/m4a/m4a.c" 2

# 1 "tools/agbcc/include/stdio.h" 1
# 40 "tools/agbcc/include/stdio.h"
# 1 "tools/agbcc/include/stdarg.h" 1








typedef void *__gnuc_va_list;
# 31 "tools/agbcc/include/stdarg.h"
typedef __gnuc_va_list va_list;
# 41 "tools/agbcc/include/stdio.h" 2







# 1 "tools/agbcc/include/sys/reent.h" 1
# 14 "tools/agbcc/include/sys/reent.h"
# 1 "tools/agbcc/include/time.h" 1
# 21 "tools/agbcc/include/time.h"
# 1 "tools/agbcc/include/machine/time.h" 1
# 22 "tools/agbcc/include/time.h" 2
# 33 "tools/agbcc/include/time.h"
# 1 "tools/agbcc/include/machine/types.h" 1
# 34 "tools/agbcc/include/time.h" 2


typedef unsigned long clock_t;




typedef long time_t;



struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

clock_t clock (void);
double difftime (time_t _time2, time_t _time1);
time_t mktime (struct tm *_timeptr);
time_t time (time_t *_timer);

char *asctime (const struct tm *_tblock);
char *ctime (const time_t *_time);
struct tm *gmtime (const time_t *_timer);
struct tm *localtime (const time_t *_timer);

size_t strftime (char *_s, size_t _maxsize, const char *_fmt, const struct tm *_t);

char *asctime_r (const struct tm *, char *);
char *ctime_r (const time_t *, char *);
struct tm *gmtime_r (const time_t *, struct tm *);
struct tm *localtime_r (const time_t *, struct tm *);
# 15 "tools/agbcc/include/sys/reent.h" 2







typedef unsigned int ULong;
# 31 "tools/agbcc/include/sys/reent.h"
struct _glue
{
  struct _glue *_next;
  int _niobs;
  struct __sFILE *_iobs;
};

struct _Bigint
{
  struct _Bigint *_next;
  int _k, _maxwds, _sign, _wds;
  ULong _x[1];
};







struct _atexit {
 struct _atexit *_next;
 int _ind;
 void (*_fns[32])(void);
};
# 64 "tools/agbcc/include/sys/reent.h"
struct __sbuf {
 unsigned char *_base;
 int _size;
};






typedef long _fpos_t;
# 102 "tools/agbcc/include/sys/reent.h"
struct __sFILE {
  unsigned char *_p;
  int _r;
  int _w;
  short _flags;
  short _file;
  struct __sbuf _bf;
  int _lbfsize;


  void * _cookie;

  int (*_read) (void * _cookie, char *_buf, int _n);
  int (*_write) (void * _cookie, const char *_buf, int _n);
  _fpos_t (*_seek) (void * _cookie, _fpos_t _offset, int _whence);
  int (*_close) (void * _cookie);


  struct __sbuf _ub;
  unsigned char *_up;
  int _ur;


  unsigned char _ubuf[3];
  unsigned char _nbuf[1];


  struct __sbuf _lb;


  int _blksize;
  int _offset;

  struct _reent *_data;
};
# 146 "tools/agbcc/include/sys/reent.h"
struct _reent
{

  int _errno;




  struct __sFILE *_stdin, *_stdout, *_stderr;

  int _inc;
  char _emergency[25];

  int _current_category;
  const char *_current_locale;

  int __sdidinit;

  void (*__cleanup) (struct _reent *);


  struct _Bigint *_result;
  int _result_k;
  struct _Bigint *_p5s;
  struct _Bigint **_freelist;


  int _cvtlen;
  char *_cvtbuf;

  union
    {
      struct
        {
          unsigned int _rand_next;
          char * _strtok_last;
          char _asctime_buf[26];
          struct tm _localtime_buf;
          int _gamma_signgam;
        } _reent;



      struct
        {

          unsigned char * _nextf[30];
          unsigned int _nmalloc[30];
        } _unused;
    } _new;


  struct _atexit *_atexit;
  struct _atexit _atexit0;


  void (**(_sig_func))(int);




  struct _glue __sglue;
  struct __sFILE __sf[3];
};
# 225 "tools/agbcc/include/sys/reent.h"
extern struct _reent *_impure_ptr ;

void _reclaim_reent (struct _reent *);
# 49 "tools/agbcc/include/stdio.h" 2

typedef _fpos_t fpos_t;

typedef struct __sFILE FILE;
# 128 "tools/agbcc/include/stdio.h"
int remove (const char *);
int rename (const char *, const char *);

char * tempnam (const char *, const char *);
FILE * tmpfile (void);
char * tmpnam (char *);
int fclose (FILE *);
int fflush (FILE *);
FILE * freopen (const char *, const char *, FILE *);
void setbuf (FILE *, char *);
int setvbuf (FILE *, char *, int, size_t);
int fprintf (FILE *, const char *, ...);
int fscanf (FILE *, const char *, ...);
int printf (const char *, ...);
int scanf (const char *, ...);
int sscanf (const char *, const char *, ...);
int vfprintf (FILE *, const char *, __gnuc_va_list);
int vprintf (const char *, __gnuc_va_list);
int vsprintf (char *, const char *, __gnuc_va_list);
int fgetc (FILE *);
char * fgets (char *, int, FILE *);
int fputc (int, FILE *);
int fputs (const char *, FILE *);
int getc (FILE *);
int getchar (void);
char * gets (char *);
int putc (int, FILE *);
int putchar (int);
int puts (const char *);
int ungetc (int, FILE *);
size_t fread (void *, size_t _size, size_t _n, FILE *);
size_t fwrite (const void * , size_t _size, size_t _n, FILE *);
int fgetpos (FILE *, fpos_t *);
int fseek (FILE *, long, int);
int fsetpos (FILE *, const fpos_t *);
long ftell ( FILE *);
void rewind (FILE *);
void clearerr (FILE *);
int feof (FILE *);
int ferror (FILE *);
void perror (const char *);

FILE * fopen (const char *_name, const char *_type);
int sprintf (char *, const char *, ...);


int vfiprintf (FILE *, const char *, __gnuc_va_list);
int iprintf (const char *, ...);
int fiprintf (FILE *, const char *, ...);
int siprintf (char *, const char *, ...);
# 186 "tools/agbcc/include/stdio.h"
FILE * fdopen (int, const char *);

int fileno (FILE *);
int getw (FILE *);
int pclose (FILE *);
FILE * popen (const char *, const char *);
int putw (int, FILE *);
void setbuffer (FILE *, char *, int);
int setlinebuf (FILE *);






FILE * _fdopen_r (struct _reent *, int, const char *);
FILE * _fopen_r (struct _reent *, const char *, const char *);
int _getchar_r (struct _reent *);
char * _gets_r (struct _reent *, char *);
int _iprintf_r (struct _reent *, const char *, ...);
int _mkstemp_r (struct _reent *, char *);
char * _mktemp_r (struct _reent *, char *);
void _perror_r (struct _reent *, const char *);
int _printf_r (struct _reent *, const char *, ...);
int _putchar_r (struct _reent *, int);
int _puts_r (struct _reent *, const char *);
int _remove_r (struct _reent *, const char *);
int _rename_r (struct _reent *, const char *_old, const char *_new);

int _scanf_r (struct _reent *, const char *, ...);
int _sprintf_r (struct _reent *, char *, const char *, ...);
char * _tempnam_r (struct _reent *, const char *, const char *);
FILE * _tmpfile_r (struct _reent *);
char * _tmpnam_r (struct _reent *, char *);
int _vfprintf_r (struct _reent *, FILE *, const char *, __gnuc_va_list);
int _vprintf_r (struct _reent *, const char *, __gnuc_va_list);
int _vsprintf_r (struct _reent *, char *, const char *, __gnuc_va_list);





int __srget (FILE *);
int __swbuf (int, FILE *);






FILE *funopen (const void * _cookie, int (*readfn)(void * _cookie, char *_buf, int _n), int (*writefn)(void * _cookie, const char *_buf, int _n), fpos_t (*seekfn)(void * _cookie, fpos_t _off, int _whence), int (*closefn)(void * _cookie));
# 6 "src/lib/m4a/m4a.c" 2
# 16 "src/lib/m4a/m4a.c"
extern char SoundMainRAM_Buffer[0x400];

extern struct MP2KTrack gMPlayTrack_BGM[16];
extern struct MP2KTrack gMPlayTrack_SE1[16];
extern struct MP2KTrack gMPlayTrack_SE2[16];
extern struct MP2KTrack gMPlayTrack_SE3[16];

extern struct SoundMixerState gSoundInfo;
extern void *gMPlayJumpTable[36];
extern struct MixerSource gCgbChans[4];

extern struct MP2KPlayerState gMPlayInfo_BGM;
extern struct MP2KPlayerState gMPlayInfo_SE1;
extern struct MP2KPlayerState gMPlayInfo_SE2;
extern u8 gMPlayMemAccArea[4 * sizeof(uintptr_t)];
extern struct MP2KPlayerState gMPlayInfo_SE3;

static void MP2K_event_null(void);

u32 MidiKeyToFreq(struct WaveData *wav, u8 key, u8 fineAdjust)
{
    u32 val1;
    u32 val2;
    u32 fineAdjustShifted = fineAdjust << 24;

    if (key > 178) {
        key = 178;
        fineAdjustShifted = 255 << 24;
    }

    val1 = gScaleTable[key];
    val1 = gFreqTable[val1 & 0xF] >> (val1 >> 4);

    val2 = gScaleTable[key + 1];
    val2 = gFreqTable[val2 & 0xF] >> (val2 >> 4);

    return umul3232H32(wav->freq, val1 + umul3232H32(val2 - val1, fineAdjustShifted));
}

__attribute__((unused)) static void UnusedFunc(void) { }

void MPlayContinue(struct MP2KPlayerState *mplayInfo)
{
    if (mplayInfo->lockStatus == 0x68736D53) {
        mplayInfo->lockStatus++;
        mplayInfo->status &= ~0x80000000;
        mplayInfo->lockStatus = 0x68736D53;
    }
}

void MPlayFadeOut(struct MP2KPlayerState *mplayInfo, u16 speed)
{
    if (mplayInfo->lockStatus == 0x68736D53) {
        mplayInfo->lockStatus++;
        mplayInfo->fadeCounter = speed;
        mplayInfo->fadeInterval = speed;
        mplayInfo->fadeOV = (64 << 2);
        mplayInfo->lockStatus = 0x68736D53;
    }
}

void m4aSoundInit(void)
{
    s32 i;

    CpuSet((void *)((s32)SoundMainRAM & ~1), SoundMainRAM_Buffer, 0x04000000 | ((sizeof(SoundMainRAM_Buffer))/(32/8) & 0x1FFFFF));


    SoundInit(&gSoundInfo);
    MPlayExtender(gCgbChans);
    m4aSoundMode((0x00900000 | 0x00070000 | (15 << 12) | (8 << 8)));

    for (i = 0; i < ((u16)gNumMusicPlayers); i++) {
        struct MP2KPlayerState *mplayInfo = gMPlayTable[i].info;
        MPlayOpen(mplayInfo, gMPlayTable[i].track, gMPlayTable[i].numTracks);
        mplayInfo->checkSongPriority = gMPlayTable[i].unk_A;
        mplayInfo->memAccArea = gMPlayMemAccArea;
    }
}

void m4aSoundMain(void) { SoundMain(); }

void m4aSongNumStart(u16 n)
{
    const struct MusicPlayer *mplayTable = gMPlayTable;
    const struct Song *songTable = gSongTable;
    const struct Song *song = &songTable[n];
    const struct MusicPlayer *mplay = &mplayTable[song->ms];

    MPlayStart(mplay->info, song->header);
}

void m4aSongNumStartOrChange(u16 n)
{
    const struct MusicPlayer *mplayTable = gMPlayTable;
    const struct Song *songTable = gSongTable;
    const struct Song *song = &songTable[n];
    const struct MusicPlayer *mplay = &mplayTable[song->ms];

    if (mplay->info->songHeader != song->header) {
        MPlayStart(mplay->info, song->header);
    } else {
        if ((mplay->info->status & 0x0000ffff) == 0 || (mplay->info->status & 0x80000000)) {
            MPlayStart(mplay->info, song->header);
        }
    }
}

void m4aSongNumStartOrContinue(u16 n)
{
    const struct MusicPlayer *mplayTable = gMPlayTable;
    const struct Song *songTable = gSongTable;
    const struct Song *song = &songTable[n];
    const struct MusicPlayer *mplay = &mplayTable[song->ms];

    if (mplay->info->songHeader != song->header)
        MPlayStart(mplay->info, song->header);
    else if ((mplay->info->status & 0x0000ffff) == 0)
        MPlayStart(mplay->info, song->header);
    else if (mplay->info->status & 0x80000000)
        MPlayContinue(mplay->info);
}

void m4aSongNumStop(u16 n)
{
    const struct MusicPlayer *mplayTable = gMPlayTable;
    const struct Song *songTable = gSongTable;
    const struct Song *song = &songTable[n];
    const struct MusicPlayer *mplay = &mplayTable[song->ms];

    if (mplay->info->songHeader == song->header)
        MPlayStop(mplay->info);
}

void m4aSongNumContinue(u16 n)
{
    const struct MusicPlayer *mplayTable = gMPlayTable;
    const struct Song *songTable = gSongTable;
    const struct Song *song = &songTable[n];
    const struct MusicPlayer *mplay = &mplayTable[song->ms];

    if (mplay->info->songHeader == song->header)
        MPlayContinue(mplay->info);
}

void m4aMPlayAllStop(void)
{
    s32 i;

    for (i = 0; i < ((u16)gNumMusicPlayers); i++)
        MPlayStop(gMPlayTable[i].info);
}

void m4aMPlayContinue(struct MP2KPlayerState *mplayInfo) { MPlayContinue(mplayInfo); }

void m4aMPlayAllContinue(void)
{
    s32 i;

    for (i = 0; i < ((u16)gNumMusicPlayers); i++)
        MPlayContinue(gMPlayTable[i].info);
}

void m4aMPlayFadeOut(struct MP2KPlayerState *mplayInfo, u16 speed) { MPlayFadeOut(mplayInfo, speed); }

void m4aMPlayFadeOutTemporarily(struct MP2KPlayerState *mplayInfo, u16 speed)
{
    if (mplayInfo->lockStatus == 0x68736D53) {
        mplayInfo->lockStatus++;
        mplayInfo->fadeCounter = speed;
        mplayInfo->fadeInterval = speed;
        mplayInfo->fadeOV = (64 << 2) | 0x0001;
        mplayInfo->lockStatus = 0x68736D53;
    }
}

void m4aMPlayFadeIn(struct MP2KPlayerState *mplayInfo, u16 speed)
{
    if (mplayInfo->lockStatus == 0x68736D53) {
        mplayInfo->lockStatus++;
        mplayInfo->fadeCounter = speed;
        mplayInfo->fadeInterval = speed;
        mplayInfo->fadeOV = (0 << 2) | 0x0002;
        mplayInfo->status &= ~0x80000000;
        mplayInfo->lockStatus = 0x68736D53;
    }
}

void m4aMPlayImmInit(struct MP2KPlayerState *mplayInfo)
{
    s32 trackCount = mplayInfo->trackCount;
    struct MP2KTrack *track = mplayInfo->tracks;

    while (trackCount > 0) {
        if (track->status & 0x80) {
            if (track->status & 0x40) {
                Clear64byte(track);
                track->status = 0x80;
                track->bendRange = 2;
                track->volPublic = 64;
                track->lfoSpeed = 22;
                track->voicegroup.type = 1;
            }
        }

        trackCount--;
        track++;
    }
}

void MPlayExtender(struct MixerSource *cgbChans)
{
    struct SoundMixerState *soundInfo;
    u32 lockStatus;

    (*(vu16 *)(0x4000000 + 0x84)) = 0x0080 | 0x0008 | 0x0004 | 0x0002 | 0x0001;
    (*(vu16 *)(0x4000000 + 0x80)) = 0;
    (*(vu8 *)(0x4000000 + 0x63)) = 0x8;
    (*(vu8 *)(0x4000000 + 0x69)) = 0x8;
    (*(vu8 *)(0x4000000 + 0x79)) = 0x8;
    (*(vu8 *)(0x4000000 + 0x65)) = 0x80;
    (*(vu8 *)(0x4000000 + 0x6d)) = 0x80;
    (*(vu8 *)(0x4000000 + 0x7d)) = 0x80;
    (*(vu8 *)(0x4000000 + 0x70)) = 0;
    (*(vu8 *)(0x4000000 + 0x80)) = 0x77;
# 249 "src/lib/m4a/m4a.c"
    soundInfo = (*(struct SoundMixerState **)0x3007FF0);

    lockStatus = soundInfo->lockStatus;

    if (lockStatus != 0x68736D53)
        return;

    soundInfo->lockStatus++;

    gMPlayJumpTable[8] = MP2K_event_memacc;
    gMPlayJumpTable[17] = MP2K_event_lfos;
    gMPlayJumpTable[19] = MP2K_event_mod;
    gMPlayJumpTable[28] = MP2K_event_xcmd;
    gMPlayJumpTable[29] = MP2K_event_endtie;
    gMPlayJumpTable[30] = SampleFreqSet;
    gMPlayJumpTable[31] = TrackStop;
    gMPlayJumpTable[32] = FadeOutBody;
    gMPlayJumpTable[33] = TrkVolPitSet;

    soundInfo->cgbChans = cgbChans;
    soundInfo->CgbSound = CgbSound;
    soundInfo->CgbOscOff = CgbOscOff;
    soundInfo->MidiKeyToCgbFreq = MidiKeyToCgbFreq;
    soundInfo->maxScanlines = ((u32)gMaxLines);

    { vu32 tmp = (vu32)(0); CpuSet((void *)&tmp, cgbChans, 0x04000000 | 0x01000000 | ((sizeof(struct MixerSource) * 4)/(32/8) & 0x1FFFFF)); };

    cgbChans[0].type = 1;
    cgbChans[0].data.cgb.panMask = 0x11;
    cgbChans[1].type = 2;
    cgbChans[1].data.cgb.panMask = 0x22;
    cgbChans[2].type = 3;
    cgbChans[2].data.cgb.panMask = 0x44;
    cgbChans[3].type = 4;
    cgbChans[3].data.cgb.panMask = 0x88;

    soundInfo->lockStatus = lockStatus;
}


void MusicPlayerJumpTableCopy(void) { asm("swi 0x2A"); }


void ClearChain(struct MixerSource *chan)
{
    void (*func)(void *) = *(&gMPlayJumpTable[34]);
    func(chan);
}

void Clear64byte(void *x)
{
    void (*func)(void *) = *(&gMPlayJumpTable[35]);
    func(x);
}

void SoundInit(struct SoundMixerState *soundInfo)
{
    soundInfo->lockStatus = 0;

    if ((*(vu32 *)(0x4000000 + 0xc4)) & (0x0200 << 16))
        (*(vu32 *)(0x4000000 + 0xc4)) = ((0x8000 | 0x0000 | 0x0400 | 0x0000 | 0x0040) << 16) | 4;
    if ((*(vu32 *)(0x4000000 + 0xd0)) & (0x0200 << 16))
        (*(vu32 *)(0x4000000 + 0xd0)) = ((0x8000 | 0x0000 | 0x0400 | 0x0000 | 0x0040) << 16) | 4;
    (*(vu16 *)(0x4000000 + 0xc6)) = 0x0400;
    (*(vu16 *)(0x4000000 + 0xd2)) = 0x0400;
    (*(vu16 *)(0x4000000 + 0x84)) = 0x0080 | 0x0008 | 0x0004 | 0x0002 | 0x0001;
    (*(vu16 *)(0x4000000 + 0x82)) = 0x8000 | 0x0000 | 0x2000 | 0x0800 | 0x0000
        | 0x0100 | 0x000E;
    (*(vu8 *)(0x4000000 + 0x89)) = ((*(vu8 *)(0x4000000 + 0x89)) & 0x3F) | 0x40;
    (*(vu32 *)(0x4000000 + 0xbc)) = (intptr_t)soundInfo->pcmBuffer;
    (*(vu32 *)(0x4000000 + 0xc0)) = (intptr_t)&(*(vu32 *)(0x4000000 + 0xa0));
    (*(vu32 *)(0x4000000 + 0xc8)) = (intptr_t)soundInfo->pcmBuffer + 1584;
    (*(vu32 *)(0x4000000 + 0xcc)) = (intptr_t)&(*(vu32 *)(0x4000000 + 0xa4));


    (*(struct SoundMixerState **)0x3007FF0) = soundInfo;
    { vu32 tmp = (vu32)(0); CpuSet((void *)&tmp, soundInfo, 0x04000000 | 0x01000000 | ((sizeof(struct SoundMixerState))/(32/8) & 0x1FFFFF)); };

    soundInfo->numChans = 8;
    soundInfo->masterVol = 15;
    soundInfo->plynote = MP2K_event_nxx;
    soundInfo->CgbSound = MP2K_event_null;
    soundInfo->CgbOscOff = (CgbOscOffFunc)MP2K_event_null;
    soundInfo->MidiKeyToCgbFreq = (MidiKeyToCgbFreqFunc)MP2K_event_null;
    soundInfo->ExtVolPit = (ExtVolPitFunc)MP2K_event_null;

    MPlayJumpTableCopy((void **)gMPlayJumpTable);

    soundInfo->MPlayJumpTable = gMPlayJumpTable;


    SampleFreqSet(0x00040000);




    soundInfo->lockStatus = 0x68736D53;
}

void SampleFreqSet(u32 freq)
{
    struct SoundMixerState *soundInfo = (*(struct SoundMixerState **)0x3007FF0);

    freq = (freq & 0xF0000) >> 16;
    soundInfo->freqOption = freq;


    soundInfo->samplesPerFrame = gPcmSamplesPerVBlankTable[freq - 1];
    soundInfo->framesPerDmaCycle = 1584 / soundInfo->samplesPerFrame;


    soundInfo->sampleRate = (597275 * soundInfo->samplesPerFrame + 5000) / 10000;


    soundInfo->sampleRateReciprocal = (0x1000000 / soundInfo->sampleRate + 1) >> 1;
# 372 "src/lib/m4a/m4a.c"
    (*(vu16 *)(0x4000000 + 0x102)) = 0;


    (*(vu16 *)(0x4000000 + 0x100)) = -(280896 / soundInfo->samplesPerFrame);

    m4aSoundVSyncOn();

    while (*(vu8 *)(0x4000000 + 0x6) == 159)
        ;

    while (*(vu8 *)(0x4000000 + 0x6) != 159)
        ;

    (*(vu16 *)(0x4000000 + 0x102)) = 0x80 | 0x00;
}

void m4aSoundMode(u32 mode)
{
    struct SoundMixerState *soundInfo = (*(struct SoundMixerState **)0x3007FF0);
    u32 temp;

    if (soundInfo->lockStatus != 0x68736D53)
        return;

    soundInfo->lockStatus++;

    temp = mode & (0x00000080 | 0x0000007F);

    if (temp)
        soundInfo->reverb = temp & 0x0000007F;

    temp = mode & 0x00000F00;

    if (temp) {
        struct MixerSource *chan;

        soundInfo->numChans = temp >> 8;

        temp = 12;
        chan = &soundInfo->chans[0];

        while (temp != 0) {
            chan->status = 0;
            temp--;
            chan++;
        }
    }

    temp = mode & 0x0000F000;

    if (temp)
        soundInfo->masterVol = temp >> 12;

    temp = mode & 0x00B00000;

    if (temp) {
        temp = (temp & 0x300000) >> 14;
        (*(vu8 *)(0x4000000 + 0x89)) = ((*(vu8 *)(0x4000000 + 0x89)) & 0x3F) | temp;
    }

    temp = mode & 0x000F0000;

    if (temp) {
        m4aSoundVSyncOff();
        SampleFreqSet(temp);
    }

    soundInfo->lockStatus = 0x68736D53;
}

void SoundClear(void)
{
    struct SoundMixerState *soundInfo = (*(struct SoundMixerState **)0x3007FF0);
    s32 i;
    struct MixerSource *chan;

    if (soundInfo->lockStatus != 0x68736D53)
        return;

    soundInfo->lockStatus++;

    i = 12;
    chan = &soundInfo->chans[0];

    while (i > 0) {
        chan->status = 0;
        i--;
        chan = (void *)((intptr_t)chan + sizeof(struct MixerSource));
    }

    chan = soundInfo->cgbChans;

    if (chan) {
        i = 1;

        while (i <= 4) {
            soundInfo->CgbOscOff(i);
            chan->status = 0;
            i++;
            chan = (void *)((intptr_t)chan + sizeof(struct MixerSource));
        }
    }

    soundInfo->lockStatus = 0x68736D53;
}

void m4aSoundVSyncOff(void)
{
    struct SoundMixerState *soundInfo = (*(struct SoundMixerState **)0x3007FF0);

    if (soundInfo->lockStatus >= 0x68736D53 && soundInfo->lockStatus <= 0x68736D53 + 1) {
        soundInfo->lockStatus += 10;

        if ((*(vu32 *)(0x4000000 + 0xc4)) & (0x0200 << 16))
            (*(vu32 *)(0x4000000 + 0xc4)) = ((0x8000 | 0x0000 | 0x0400 | 0x0000 | 0x0040) << 16) | 4;

        if ((*(vu32 *)(0x4000000 + 0xd0)) & (0x0200 << 16))
            (*(vu32 *)(0x4000000 + 0xd0)) = ((0x8000 | 0x0000 | 0x0400 | 0x0000 | 0x0040) << 16) | 4;

        (*(vu16 *)(0x4000000 + 0xc6)) = 0x0400;
        (*(vu16 *)(0x4000000 + 0xd2)) = 0x0400;

        { vu32 tmp = (vu32)(0); CpuSet((void *)&tmp, soundInfo->pcmBuffer, 0x04000000 | 0x01000000 | ((sizeof(soundInfo->pcmBuffer))/(32/8) & 0x1FFFFF)); };
    }
}

void m4aSoundVSyncOn(void)
{
    struct SoundMixerState *soundInfo = (*(struct SoundMixerState **)0x3007FF0);
    u32 lockStatus = soundInfo->lockStatus;

    if (lockStatus == 0x68736D53)
        return;

    (*(vu16 *)(0x4000000 + 0xc6)) = 0x8000 | 0x3000 | 0x0400 | 0x0200;
    (*(vu16 *)(0x4000000 + 0xd2)) = 0x8000 | 0x3000 | 0x0400 | 0x0200;

    soundInfo->dmaCounter = 0;
    soundInfo->lockStatus = lockStatus - 10;
}

void MPlayOpen(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *tracks, u8 trackCount)
{
    struct SoundMixerState *soundInfo;

    if (trackCount == 0)
        return;

    if (trackCount > 16)
        trackCount = 16;

    soundInfo = (*(struct SoundMixerState **)0x3007FF0);

    if (soundInfo->lockStatus != 0x68736D53)
        return;

    soundInfo->lockStatus++;

    Clear64byte(mplayInfo);

    mplayInfo->tracks = tracks;
    mplayInfo->trackCount = trackCount;
    mplayInfo->status = 0x80000000;

    while (trackCount != 0) {
        tracks->status = 0;
        trackCount--;
        tracks++;
    }



    if (soundInfo->MPlayMainHead != ((void *)0)) {
        mplayInfo->nextPlayerFunc = soundInfo->MPlayMainHead;
        mplayInfo->nextPlayer = soundInfo->musicPlayerHead;

        soundInfo->MPlayMainHead = ((void *)0);
    }

    soundInfo->musicPlayerHead = mplayInfo;
    soundInfo->MPlayMainHead = MP2KPlayerMain;

    soundInfo->lockStatus = 0x68736D53;
    mplayInfo->lockStatus = 0x68736D53;
}

void MPlayStart(struct MP2KPlayerState *mplayInfo, struct MP2KSongHeader *songHeader)
{
    s32 i;
    u8 checkSongPriority;
    struct MP2KTrack *track;

    if (mplayInfo->lockStatus != 0x68736D53)
        return;

    checkSongPriority = mplayInfo->checkSongPriority;

    if (!checkSongPriority
        || ((!mplayInfo->songHeader || !(mplayInfo->tracks[0].status & 0x40))
            && ((mplayInfo->status & 0x0000ffff) == 0 || (mplayInfo->status & 0x80000000)))
        || (mplayInfo->priority <= songHeader->priority)) {
        mplayInfo->lockStatus++;
        mplayInfo->status = 0;
        mplayInfo->songHeader = songHeader;
        mplayInfo->voicegroup = songHeader->voicegroup;
        mplayInfo->priority = songHeader->priority;
        mplayInfo->clock = 0;
        mplayInfo->tempoRawBPM = 150;
        mplayInfo->tempoInterval = 150;
        mplayInfo->tempoScale = 0x100;
        mplayInfo->tempoCounter = 0;
        mplayInfo->fadeInterval = 0;

        i = 0;
        track = mplayInfo->tracks;

        while (i < songHeader->trackCount && i < mplayInfo->trackCount) {
            TrackStop(mplayInfo, track);
            track->status = 0x80 | 0x40;
            track->chan = 0;
            track->cmdPtr = songHeader->part[i];
            i++;
            track++;
        }

        while (i < mplayInfo->trackCount) {
            TrackStop(mplayInfo, track);
            track->status = 0;
            i++;
            track++;
        }

        if (songHeader->reverb & 0x00000080)
            m4aSoundMode(songHeader->reverb);

        mplayInfo->lockStatus = 0x68736D53;
    }
}

void MPlayStop(struct MP2KPlayerState *mplayInfo)
{
    s32 i;
    struct MP2KTrack *track;

    if (mplayInfo->lockStatus != 0x68736D53)
        return;

    mplayInfo->lockStatus++;
    mplayInfo->status |= 0x80000000;

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;

    while (i > 0) {
        TrackStop(mplayInfo, track);
        i--;
        track++;
    }

    mplayInfo->lockStatus = 0x68736D53;
}

void FadeOutBody(struct MP2KPlayerState *mplayInfo)
{
    s32 i;
    struct MP2KTrack *track;
    u16 fadeOV;

    if (mplayInfo->fadeInterval == 0)
        return;
    if (--mplayInfo->fadeCounter != 0)
        return;

    mplayInfo->fadeCounter = mplayInfo->fadeInterval;

    if (mplayInfo->fadeOV & 0x0002) {
        if ((u16)(mplayInfo->fadeOV += (4 << 2)) >= (64 << 2)) {
            mplayInfo->fadeOV = (64 << 2);
            mplayInfo->fadeInterval = 0;
        }
    } else {
        if ((s16)(mplayInfo->fadeOV -= (4 << 2)) <= 0) {
            i = mplayInfo->trackCount;
            track = mplayInfo->tracks;

            while (i > 0) {
                u32 val;

                TrackStop(mplayInfo, track);

                val = 0x0001;
                fadeOV = mplayInfo->fadeOV;
                val &= fadeOV;

                if (!val)
                    track->status = 0;

                i--;
                track++;
            }

            if (mplayInfo->fadeOV & 0x0001)
                mplayInfo->status |= 0x80000000;
            else
                mplayInfo->status = 0x80000000;

            mplayInfo->fadeInterval = 0;
            return;
        }
    }

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;

    while (i > 0) {
        if (track->status & 0x80) {
            fadeOV = mplayInfo->fadeOV;

            track->volPublic = (fadeOV >> 2);
            track->status |= 0x03;
        }

        i--;
        track++;
    }
}

void TrkVolPitSet(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    if (track->status & 0x01) {
        s32 x;
        s32 y;

        x = (u32)(track->vol * track->volPublic) >> 5;

        if (track->modType == 1)
            x = (u32)(x * (track->modCalculated + 128)) >> 7;

        y = 2 * track->pan + track->panPublic;

        if (track->modType == 2)
            y += track->modCalculated;

        if (y < -128)
            y = -128;
        else if (y > 127)
            y = 127;

        track->volRightCalculated = (u32)((y + 128) * x) >> 8;
        track->volLeftCalculated = (u32)((127 - y) * x) >> 8;
    }

    if (track->status & 0x04) {
        s32 bend = track->bend * track->bendRange;
        s32 x = (track->tune + bend) * 4 + (track->keyShift << 8) + (track->keyShiftPublic << 8) + track->pitchPublic;

        if (track->modType == 0)
            x += 16 * track->modCalculated;

        track->keyShiftCalculated = x >> 8;
        track->pitchCalculated = x;
    }

    track->status &= ~(0x04 | 0x01);
}

u32 MidiKeyToCgbFreq(u8 chanNum, u8 key, u8 fineAdjust)
{
    if (chanNum == 4) {
        if (key <= 20) {
            key = 0;
        } else {
            key -= 21;
            if (key > 59)
                key = 59;
        }

        return gNoiseTable[key];
    } else {
        s32 val1;
        s32 val2;

        if (key <= 35) {
            fineAdjust = 0;
            key = 0;
        } else {
            key -= 36;
            if (key > 130) {
                key = 130;
                fineAdjust = 255;
            }
        }

        val1 = gCgbScaleTable[key];
        val1 = gCgbFreqTable[val1 & 0xF] >> (val1 >> 4);

        val2 = gCgbScaleTable[key + 1];
        val2 = gCgbFreqTable[val2 & 0xF] >> (val2 >> 4);

        return val1 + ((fineAdjust * (val2 - val1)) >> 8) + 2048;
    }
}

void CgbOscOff(u8 chanNum)
{
    switch (chanNum) {
        case 1:
            (*(vu8 *)(0x4000000 + 0x63)) = 8;
            (*(vu8 *)(0x4000000 + 0x65)) = 0x80;
            break;
        case 2:
            (*(vu8 *)(0x4000000 + 0x69)) = 8;
            (*(vu8 *)(0x4000000 + 0x6d)) = 0x80;
            break;
        case 3:
            (*(vu8 *)(0x4000000 + 0x70)) = 0;
            break;
        default:
            (*(vu8 *)(0x4000000 + 0x79)) = 8;
            (*(vu8 *)(0x4000000 + 0x7d)) = 0x80;
    }




}

static inline int CgbPan(struct MixerSource *chan)
{
    u32 rightVol = chan->rightVol;
    u32 leftVol = chan->leftVol;

    if ((rightVol = (u8)rightVol) >= (leftVol = (u8)leftVol)) {
        if (rightVol / 2 >= leftVol) {
            chan->data.cgb.pan = 0x0F;
            return 1;
        }
    } else {
        if (leftVol / 2 >= rightVol) {
            chan->data.cgb.pan = 0xF0;
            return 1;
        }
    }

    return 0;
}

void CgbModVol(struct MixerSource *chan)
{
    if (!CgbPan(chan)) {
        chan->data.cgb.pan = 0xFF;
        chan->data.cgb.envelopeGoal = (u32)(chan->rightVol + chan->leftVol) / 16;
    } else {
        chan->data.cgb.envelopeGoal = (u32)(chan->rightVol + chan->leftVol) / 16;

        if (chan->data.cgb.envelopeGoal > 15)
            chan->data.cgb.envelopeGoal = 15;
    }

    chan->data.cgb.sustainGoal = (chan->data.cgb.envelopeGoal * chan->data.cgb.sustain + 15) >> 4;
    chan->data.cgb.pan &= chan->data.cgb.panMask;
}

void CgbSound(void)
{
    s32 ch;
    struct MixerSource *channels;
    s32 prevC15;
    struct SoundMixerState *soundInfo = (*(struct SoundMixerState **)0x3007FF0);
    vu8 *nrx0ptr;
    vu8 *nrx1ptr;
    vu8 *nrx2ptr;
    vu8 *nrx3ptr;
    vu8 *nrx4ptr;
    s32 envelopeStepTimeAndDir;


    int mask = 0xff;

    if (soundInfo->cgbCounter15)
        soundInfo->cgbCounter15--;
    else
        soundInfo->cgbCounter15 = 14;

    for (ch = 1, channels = soundInfo->cgbChans; ch <= 4; ch++, channels++) {
        if (!(channels->status & (0x80 | 0x40 | 0x04 | 0x03)))
            continue;


        switch (ch) {
            case 1:
                nrx0ptr = &(*(vu8 *)(0x4000000 + 0x60));
                nrx1ptr = &(*(vu8 *)(0x4000000 + 0x62));
                nrx2ptr = &(*(vu8 *)(0x4000000 + 0x63));
                nrx3ptr = &(*(vu8 *)(0x4000000 + 0x64));
                nrx4ptr = &(*(vu8 *)(0x4000000 + 0x65));
                break;
            case 2:
                nrx0ptr = (vu8 *)((0x4000000 + 0x60) + 1);
                nrx1ptr = &(*(vu8 *)(0x4000000 + 0x68));
                nrx2ptr = &(*(vu8 *)(0x4000000 + 0x69));
                nrx3ptr = &(*(vu8 *)(0x4000000 + 0x6c));
                nrx4ptr = &(*(vu8 *)(0x4000000 + 0x6d));
                break;
            case 3:
                nrx0ptr = &(*(vu8 *)(0x4000000 + 0x70));
                nrx1ptr = &(*(vu8 *)(0x4000000 + 0x72));
                nrx2ptr = &(*(vu8 *)(0x4000000 + 0x73));
                nrx3ptr = &(*(vu8 *)(0x4000000 + 0x74));
                nrx4ptr = &(*(vu8 *)(0x4000000 + 0x75));
                break;
            default:
                nrx0ptr = (vu8 *)((0x4000000 + 0x70) + 1);
                nrx1ptr = &(*(vu8 *)(0x4000000 + 0x78));
                nrx2ptr = &(*(vu8 *)(0x4000000 + 0x79));
                nrx3ptr = &(*(vu8 *)(0x4000000 + 0x7c));
                nrx4ptr = &(*(vu8 *)(0x4000000 + 0x7d));
                break;
        }

        prevC15 = soundInfo->cgbCounter15;
        envelopeStepTimeAndDir = *nrx2ptr;


        if (channels->status & 0x80) {
            if (!(channels->status & 0x40)) {
                channels->status = 0x03;
                channels->data.cgb.cgbStatus = 0x02 | 0x01;
                CgbModVol(channels);
                switch (ch) {
                    case 1:
                        *nrx0ptr = channels->data.cgb.sweep;




                    case 2:
                        *nrx1ptr = ((intptr_t)channels->wav << 6) + channels->data.cgb.length;
                        goto init_env_step_time_dir;
                    case 3:
                        if (channels->wav != channels->current) {
                            *nrx0ptr = 0x40;
                            (*(vu32 *)(0x4000000 + 0x90)) = ((u32 *)channels->wav)[0];
                            (*(vu32 *)(0x4000000 + 0x94)) = ((u32 *)channels->wav)[1];
                            (*(vu32 *)(0x4000000 + 0x98)) = ((u32 *)channels->wav)[2];
                            (*(vu32 *)(0x4000000 + 0x9c)) = ((u32 *)channels->wav)[3];
                            channels->current = channels->wav;



                        }
                        *nrx0ptr = 0;
                        *nrx1ptr = channels->data.cgb.length;
                        if (channels->data.cgb.length)
                            channels->data.cgb.nrx4 = 0xC0;
                        else
                            channels->data.cgb.nrx4 = 0x80;
                        break;
                    default:
                        *nrx1ptr = channels->data.cgb.length;
                        *nrx3ptr = (intptr_t)channels->wav << 3;
                    init_env_step_time_dir:
                        envelopeStepTimeAndDir = channels->data.cgb.attack + 0x08;
                        if (channels->data.cgb.length)
                            channels->data.cgb.nrx4 = 0x40;
                        else
                            channels->data.cgb.nrx4 = 0x00;
                        break;
                }



                channels->data.cgb.envelopeCtr = channels->data.cgb.attack;
                if ((s8)(channels->data.cgb.attack & mask)) {
                    channels->data.cgb.envelopeVol = 0;
                    goto envelope_step_complete;
                } else {

                    goto envelope_decay_start;
                }
            } else {
                goto oscillator_off;
            }
        } else if (channels->status & 0x04) {
            channels->data.cgb.echoLen--;
            if ((s8)(channels->data.cgb.echoLen & mask) <= 0) {
            oscillator_off:
                CgbOscOff(ch);
                channels->status = 0;
                goto channel_complete;
            }
            goto envelope_complete;
        } else if ((channels->status & 0x40) && (channels->status & 0x03)) {
            channels->status &= ~0x03;
            channels->data.cgb.envelopeCtr = channels->data.cgb.release;
            if ((s8)(channels->data.cgb.release & mask)) {
                channels->data.cgb.cgbStatus |= 0x01;
                if (ch != 3)
                    envelopeStepTimeAndDir = channels->data.cgb.release | 0x00;
                goto envelope_step_complete;
            } else {
                goto envelope_pseudoecho_start;
            }
        } else {
        envelope_step_repeat:
            if (channels->data.cgb.envelopeCtr == 0) {
                if (ch == 3)
                    channels->data.cgb.cgbStatus |= 0x01;

                CgbModVol(channels);
                if ((channels->status & 0x03) == 0x00) {
                    channels->data.cgb.envelopeVol--;
                    if ((s8)(channels->data.cgb.envelopeVol & mask) <= 0) {
                    envelope_pseudoecho_start:
                        channels->data.cgb.envelopeVol = ((channels->data.cgb.envelopeGoal * channels->data.cgb.echoVol) + 0xFF) >> 8;
                        if (channels->data.cgb.envelopeVol) {
                            channels->status |= 0x04;
                            channels->data.cgb.cgbStatus |= 0x01;
                            if (ch != 3)
                                envelopeStepTimeAndDir = 0 | 0x08;
                            goto envelope_complete;
                        } else {
                            goto oscillator_off;
                        }
                    } else {
                        channels->data.cgb.envelopeCtr = channels->data.cgb.release;
                    }
                } else if ((channels->status & 0x03) == 0x01) {
                envelope_sustain:
                    channels->data.cgb.envelopeVol = channels->data.cgb.sustainGoal;
                    channels->data.cgb.envelopeCtr = 7;
                } else if ((channels->status & 0x03) == 0x02) {
                    int envelopeVol, sustainGoal;

                    channels->data.cgb.envelopeVol--;
                    envelopeVol = (s8)(channels->data.cgb.envelopeVol & mask);
                    sustainGoal = (s8)(channels->data.cgb.sustainGoal);
                    if (envelopeVol <= sustainGoal) {
                    envelope_sustain_start:
                        if (channels->data.cgb.sustain == 0) {
                            channels->status &= ~0x03;
                            goto envelope_pseudoecho_start;
                        } else {
                            channels->status--;
                            channels->data.cgb.cgbStatus |= 0x01;
                            if (ch != 3)
                                envelopeStepTimeAndDir = 0 | 0x08;
                            goto envelope_sustain;
                        }
                    } else {
                        channels->data.cgb.envelopeCtr = channels->data.cgb.decay;
                    }
                } else {
                    channels->data.cgb.envelopeVol++;
                    if ((u8)(channels->data.cgb.envelopeVol & mask) >= channels->data.cgb.envelopeGoal) {
                    envelope_decay_start:
                        channels->status--;
                        channels->data.cgb.envelopeCtr = channels->data.cgb.decay;
                        if ((u8)(channels->data.cgb.envelopeCtr & mask)) {
                            channels->data.cgb.cgbStatus |= 0x01;
                            channels->data.cgb.envelopeVol = channels->data.cgb.envelopeGoal;
                            if (ch != 3)
                                envelopeStepTimeAndDir = channels->data.cgb.decay | 0x00;
                        } else {
                            goto envelope_sustain_start;
                        }
                    } else {
                        channels->data.cgb.envelopeCtr = channels->data.cgb.attack;
                    }
                }
            }
        }

    envelope_step_complete:


        channels->data.cgb.envelopeCtr--;
        if (prevC15 == 0) {
            prevC15--;
            goto envelope_step_repeat;
        }

    envelope_complete:

        if (channels->data.cgb.cgbStatus & 0x02) {
            if (ch < 4 && (channels->type & 0x08)) {
                int dac_pwm_rate = (*(vu8 *)(0x4000000 + 0x89));

                if (dac_pwm_rate < 0x40)
                    channels->data.cgb.freq = (channels->data.cgb.freq + 2) & 0x7fc;
                else if (dac_pwm_rate < 0x80)
                    channels->data.cgb.freq = (channels->data.cgb.freq + 1) & 0x7fe;
            }

            if (ch != 4)
                *nrx3ptr = channels->data.cgb.freq;
            else
                *nrx3ptr = (*nrx3ptr & 0x08) | channels->data.cgb.freq;
            channels->data.cgb.nrx4 = (channels->data.cgb.nrx4 & 0xC0) + (*((u8 *)(&channels->data.cgb.freq) + 1));
            *nrx4ptr = (s8)(channels->data.cgb.nrx4 & mask);
        }


        if (channels->data.cgb.cgbStatus & 0x01) {
            (*(vu8 *)(0x4000000 + 0x81)) = ((*(vu8 *)(0x4000000 + 0x81)) & ~channels->data.cgb.panMask) | channels->data.cgb.pan;
            if (ch == 3) {
                *nrx2ptr = gCgb3Vol[channels->data.cgb.envelopeVol];
                if (channels->data.cgb.nrx4 & 0x80) {
                    *nrx0ptr = 0x80;
                    *nrx4ptr = channels->data.cgb.nrx4;
                    channels->data.cgb.nrx4 &= 0x7f;
                }
            } else {
                u32 envMask = 0xF;
                *nrx2ptr = (envelopeStepTimeAndDir & envMask) + (channels->data.cgb.envelopeVol << 4);
                *nrx4ptr = channels->data.cgb.nrx4 | 0x80;
                if (ch == 1 && !(*nrx0ptr & 0x08))
                    *nrx4ptr = channels->data.cgb.nrx4 | 0x80;
            }





        }

    channel_complete:
        channels->data.cgb.cgbStatus = 0;
    }
}

void m4aMPlayTempoControl(struct MP2KPlayerState *mplayInfo, u16 tempo)
{
    if (mplayInfo->lockStatus == 0x68736D53) {
        mplayInfo->lockStatus++;
        mplayInfo->tempoScale = tempo;
        mplayInfo->tempoInterval = (mplayInfo->tempoRawBPM * mplayInfo->tempoScale) >> 8;
        mplayInfo->lockStatus = 0x68736D53;
    }
}

void m4aMPlayVolumeControl(struct MP2KPlayerState *mplayInfo, u16 trackBits, u16 volume)
{
    s32 i;
    u32 bit;
    struct MP2KTrack *track;

    if (mplayInfo->lockStatus != 0x68736D53)
        return;

    mplayInfo->lockStatus++;

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;
    bit = 1;

    while (i > 0) {
        if (trackBits & bit) {
            if (track->status & 0x80) {
                track->volPublic = volume / 4;
                track->status |= 0x03;
            }
        }

        i--;
        track++;
        bit <<= 1;
    }

    mplayInfo->lockStatus = 0x68736D53;
}

void m4aMPlayPitchControl(struct MP2KPlayerState *mplayInfo, u16 trackBits, s16 pitch)
{
    s32 i;
    u32 bit;
    struct MP2KTrack *track;

    if (mplayInfo->lockStatus != 0x68736D53)
        return;

    mplayInfo->lockStatus++;

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;
    bit = 1;

    while (i > 0) {
        if (trackBits & bit) {
            if (track->status & 0x80) {
                track->keyShiftPublic = pitch >> 8;
                track->pitchPublic = pitch;
                track->status |= 0x0C;
            }
        }

        i--;
        track++;
        bit <<= 1;
    }

    mplayInfo->lockStatus = 0x68736D53;
}

void m4aMPlayPanpotControl(struct MP2KPlayerState *mplayInfo, u16 trackBits, s8 pan)
{
    s32 i;
    u32 bit;
    struct MP2KTrack *track;

    if (mplayInfo->lockStatus != 0x68736D53)
        return;

    mplayInfo->lockStatus++;

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;
    bit = 1;

    while (i > 0) {
        if (trackBits & bit) {
            if (track->status & 0x80) {
                track->panPublic = pan;
                track->status |= 0x03;
            }
        }

        i--;
        track++;
        bit <<= 1;
    }

    mplayInfo->lockStatus = 0x68736D53;
}

void ClearModM(struct MP2KTrack *track)
{
    track->lfoSpeedCounter = 0;
    track->modCalculated = 0;

    if (track->modType == 0)
        track->status |= 0x0C;
    else
        track->status |= 0x03;
}

void m4aMPlayModDepthSet(struct MP2KPlayerState *mplayInfo, u16 trackBits, u8 modDepth)
{
    s32 i;
    u32 bit;
    struct MP2KTrack *track;

    if (mplayInfo->lockStatus != 0x68736D53)
        return;

    mplayInfo->lockStatus++;

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;
    bit = 1;

    while (i > 0) {
        if (trackBits & bit) {
            if (track->status & 0x80) {
                track->modDepth = modDepth;

                if (!track->modDepth)
                    ClearModM(track);
            }
        }

        i--;
        track++;
        bit <<= 1;
    }

    mplayInfo->lockStatus = 0x68736D53;
}

void m4aMPlayLFOSpeedSet(struct MP2KPlayerState *mplayInfo, u16 trackBits, u8 lfoSpeed)
{
    s32 i;
    u32 bit;
    struct MP2KTrack *track;

    if (mplayInfo->lockStatus != 0x68736D53)
        return;

    mplayInfo->lockStatus++;

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;
    bit = 1;

    while (i > 0) {
        if (trackBits & bit) {
            if (track->status & 0x80) {
                track->lfoSpeed = lfoSpeed;

                if (!track->lfoSpeed)
                    ClearModM(track);
            }
        }

        i--;
        track++;
        bit <<= 1;
    }

    mplayInfo->lockStatus = 0x68736D53;
}







void MP2K_event_memacc(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    u32 op;
    u8 *addr;
    u8 data;

    op = *track->cmdPtr;
    track->cmdPtr++;

    addr = mplayInfo->memAccArea + *track->cmdPtr;
    track->cmdPtr++;

    data = *track->cmdPtr;
    track->cmdPtr++;

    switch (op) {
        case 0:
            *addr = data;
            return;
        case 1:
            *addr += data;
            return;
        case 2:
            *addr -= data;
            return;
        case 3:
            *addr = mplayInfo->memAccArea[data];
            return;
        case 4:
            *addr += mplayInfo->memAccArea[data];
            return;
        case 5:
            *addr -= mplayInfo->memAccArea[data];
            return;
        case 6:
            if (*addr == data) goto cond_true; else goto cond_false;
            return;
        case 7:
            if (*addr != data) goto cond_true; else goto cond_false;
            return;
        case 8:
            if (*addr > data) goto cond_true; else goto cond_false;
            return;
        case 9:
            if (*addr >= data) goto cond_true; else goto cond_false;
            return;
        case 10:
            if (*addr <= data) goto cond_true; else goto cond_false;
            return;
        case 11:
            if (*addr < data) goto cond_true; else goto cond_false;
            return;
        case 12:
            if (*addr == mplayInfo->memAccArea[data]) goto cond_true; else goto cond_false;
            return;
        case 13:
            if (*addr != mplayInfo->memAccArea[data]) goto cond_true; else goto cond_false;
            return;
        case 14:
            if (*addr > mplayInfo->memAccArea[data]) goto cond_true; else goto cond_false;
            return;
        case 15:
            if (*addr >= mplayInfo->memAccArea[data]) goto cond_true; else goto cond_false;
            return;
        case 16:
            if (*addr <= mplayInfo->memAccArea[data]) goto cond_true; else goto cond_false;
            return;
        case 17:
            if (*addr < mplayInfo->memAccArea[data]) goto cond_true; else goto cond_false;
            return;
        default:
            return;
    }

cond_true : {
    {
        void (*func)(struct MP2KPlayerState *, struct MP2KTrack *) = *(&gMPlayJumpTable[1]);
        func(mplayInfo, track);
    }
    return;
}

cond_false:
    track->cmdPtr += 4;
}

void MP2K_event_xcmd(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    u32 n = *track->cmdPtr;
    track->cmdPtr++;

    gXcmdTable[n](mplayInfo, track);
}

void MP2K_event_xxx(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    void (*func)(struct MP2KPlayerState *, struct MP2KTrack *) = *(&gMPlayJumpTable[0]);
    func(mplayInfo, track);
}

void MP2K_event_xwave(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    union {
        u8 *a;
        u8 d[sizeof(uintptr_t)];
    } u;

    u.d[0] = *(track->cmdPtr + 0);
    u.d[1] = *(track->cmdPtr + 1);
    u.d[2] = *(track->cmdPtr + 2);
    u.d[3] = *(track->cmdPtr + 3);






    track->voicegroup.data.sound.wav = (struct WaveData *)u.a;
    track->cmdPtr += sizeof(uintptr_t);
}

void MP2K_event_xtype(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    track->voicegroup.type = *track->cmdPtr;
    track->cmdPtr++;
}

void MP2K_event_xatta(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    track->voicegroup.data.sound.attack = *track->cmdPtr;
    track->cmdPtr++;
}

void MP2K_event_xdeca(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    track->voicegroup.data.sound.decay = *track->cmdPtr;
    track->cmdPtr++;
}

void MP2K_event_xsust(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    track->voicegroup.data.sound.sustain = *track->cmdPtr;
    track->cmdPtr++;
}

void MP2K_event_xrele(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    track->voicegroup.data.sound.release = *track->cmdPtr;
    track->cmdPtr++;
}

void MP2K_event_xiecv(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    track->echoVolume = *track->cmdPtr;
    track->cmdPtr++;
}

void MP2K_event_xiecl(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    track->echoLength = *track->cmdPtr;
    track->cmdPtr++;
}

void MP2K_event_xleng(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    track->voicegroup.cgbLength = *track->cmdPtr;
    track->cmdPtr++;
}

void MP2K_event_xswee(struct MP2KPlayerState *mplayInfo, struct MP2KTrack *track)
{
    track->voicegroup.pan_sweep = *track->cmdPtr;
    track->cmdPtr++;
}

void MP2K_event_null(void) { }
