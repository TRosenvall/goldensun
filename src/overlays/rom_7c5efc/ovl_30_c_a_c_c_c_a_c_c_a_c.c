// fakematch
/* OvlFunc_941_2008828  --  0x02008828   (draft v1)
 */
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __SetCameraTarget(int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_800fe9c(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern int  __Func_8091c7c(int a, int b);
extern void OvlFunc_941_20091b8(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_941_2008828(void)
{
    register int m __asm__("r5");

    __CutsceneStart();  /*S0*/
    { PIN3; q0 = 1; q1 = 0xc8 << 16; q2 = 0x88 << 18;
      __MapActor_SetPos(q0, q1, q2); }  /*S1*/
    { PIN3; q0 = 0; q1 = 0xb8 << 16; q2 = 0x88 << 18;
      __MapActor_SetPos(q0, q1, q2); }  /*S2*/
    { PIN3; q0 = 3; q1 = 0xa8 << 16; q2 = 0x88 << 18;
      __MapActor_SetPos(q0, q1, q2); }  /*S3*/
    { PIN3; q0 = 2; q1 = 0xd4 << 16; q2 = 0x84 << 18;
      __MapActor_SetPos(q0, q1, q2); }  /*S4*/
    { PIN3; q0 = 0xd; q1 = 0xc8 << 16; q2 = 0x80 << 18;
      __MapActor_SetPos(q0, q1, q2); }  /*S5*/
    { PIN3; q0 = 0xc; q1 = 0xa8 << 16; q2 = 0x80 << 18;
      __MapActor_SetPos(q0, q1, q2); }  /*S6*/
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S7*/
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S8*/
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S9*/
    { PIN3; q0 = 2; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S10*/
    { PIN3; q0 = 0xd; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S11*/
    __Func_8092adc(0xc, 0, 0);  /*S12*/
    __MapActor_SetAnim(0xc, 1);  /*S13*/
    __SetCameraTarget(0, 0);  /*S14*/
    __WaitFrames(1);  /*S15*/
    __Func_800fe9c();  /*S16*/
    __WaitFrames(1);  /*S17*/
    __MapTransitionIn();  /*S18*/
    __WaitMapTransition();  /*S19*/
    __CutsceneWait(0x14);  /*S20*/
    __MapActor_DoAnim(2, 3);  /*S21*/
    __CutsceneWait(0x1e);  /*S22*/
    { PIN3; q1 = 0x81; q2 = 0x46; q1 <<= 1; q0 = 0xc;
      __MapActor_Emote(q0, q1, q2); }  /*S23*/
    do { m = 0x2516; } while (0);
    __MessageID(m);  /*S24*/
    __ActorMessage(0xc, 0);  /*S25*/
    __MapActor_DoAnim(0xd, 3);  /*S26*/
    __MessageID(m + 1);  /*S27*/
    __ActorMessage(0xd, 0);  /*S28*/
    __Func_809280c(2, 0xd, 0);  /*S29*/
    __CutsceneWait(0x1e);  /*S30*/
    __Func_80925cc(2, 1);  /*S31*/
    __CutsceneWait(0x28);  /*S32*/
    __MessageID(m + 2);  /*S33*/
    { PIN2; q0 = 0x4002; q1 = 0;
      __ActorMessage(q0, q1); }  /*S34*/
    __Func_809280c(1, 2, 0);  /*S35*/
    __CutsceneWait(0xa);  /*S36*/
    __MapActor_Emote(1, 0x107, 0x50);  /*S37*/
    __MessageID(m + 3);  /*S38*/
    { PIN2; q0 = 0x4001; q1 = 0;
      __ActorMessage(q0, q1); }  /*S39*/
    { PIN3; q0 = 2; q1 = 0x80 << 1; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }  /*S40*/
    __Func_809280c(2, 1, 0);  /*S41*/
    __MessageID(m + 4);  /*S42*/
    { PIN2; q0 = 0x4002; q1 = 0;
      __ActorMessage(q0, q1); }  /*S43*/
    { PIN3; q0 = 0xd; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S44*/
    __CutsceneWait(0x3c);  /*S45*/
    { PIN3; q0 = 0xd; q1 = 0xa0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S46*/
    __CutsceneWait(0x3c);  /*S47*/
    __MessageID(m + 5);  /*S48*/
    { PIN2; q0 = 0x400d; q1 = 0;
      __ActorMessage(q0, q1); }  /*S49*/
    __CutsceneWait(0x1e);  /*S50*/
    __MapActor_SetAnim(0, 3);  /*S51*/
    __MapActor_SetAnim(3, 3);  /*S52*/
    __MapActor_SetAnim(1, 3);  /*S53*/
    __CutsceneWait(0x78);  /*S54*/
    { PIN3; q0 = 0xd; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S55*/
    __CutsceneWait(0x1e);  /*S56*/
    { PIN3; q0 = 0xd; q1 = 0x84 << 1; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }  /*S57*/
    __MessageID(m + 6);  /*S58*/
    { PIN2; q0 = 0x400d; q1 = 0;
      __ActorMessage(q0, q1); }  /*S59*/
    __Func_809280c(2, 0xd, 0);  /*S60*/
    { PIN3; q0 = 2; q1 = 0x84 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }  /*S61*/
    __MapActor_DoAnim(2, 3);  /*S62*/
    __CutsceneWait(0x1e);  /*S63*/
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }  /*S64*/
    __Func_809280c(3, 0xc, 0);  /*S65*/
    __MessageID(m + 7);  /*S66*/
    { PIN2; q0 = 0x4003; q1 = 0;
      __ActorMessage(q0, q1); }  /*S67*/
    { PIN3; q0 = 0xd; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }  /*S68*/
    __Func_809280c(0xd, 0xc, 0);  /*S69*/
    __MessageID(m + 8);  /*S70*/
    { PIN2; q0 = 0x400d; q1 = 0;
      __ActorMessage(q0, q1); }  /*S71*/
    { PIN3; q0 = 0xc; q1 = 0x81 << 1; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }  /*S72*/
    __MessageID(m + 9);  /*S73*/
    __ActorMessage(0xc, 0);  /*S74*/
    __Func_80925cc(0xc, 1);  /*S75*/
    __CutsceneWait(0x14);  /*S76*/
    __MessageID(m + 0xa);  /*S77*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S78*/
    __Func_8092848(0xd, 2, 0);  /*S79*/
    __CutsceneWait(0x3c);  /*S80*/
    __Func_809280c(0xd, 0xc, 0);  /*S81*/
    __Func_809280c(2, 0xc, 0);  /*S82*/
    __MapActor_DoAnim(0xc, 4);  /*S83*/
    __CutsceneWait(0x1e);  /*S84*/
    __MessageID(m + 0xb);  /*S85*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S86*/
    __Func_80925cc(1, 1);  /*S87*/
    __CutsceneWait(0x14);  /*S88*/
    __Func_809280c(1, 0xc, 0);  /*S89*/
    __MessageID(m + 0xc);  /*S90*/
    { PIN2; q0 = 0x4001; q1 = 0;
      __ActorMessage(q0, q1); }  /*S91*/
    __Func_809280c(0xc, 1, 0);  /*S92*/
    __CutsceneWait(0x14);  /*S93*/
    __MessageID(m + 0xd);  /*S94*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S95*/
    __Func_809280c(3, 0xc, 0);  /*S96*/
    __MapActor_Emote(3, 0x101, 0x46);  /*S97*/
    __MessageID(m + 0xe);  /*S98*/
    { PIN2; q0 = 0x4003; q1 = 0;
      __ActorMessage(q0, q1); }  /*S99*/
    __MapActor_DoAnim(0xc, 3);  /*S100*/
    __CutsceneWait(0x28);  /*S101*/
    __MessageID(m + 0xf);  /*S102*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S103*/
    { PIN3; q0 = 0xd; q1 = 0x80 << 1; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }  /*S104*/
    __MessageID(m + 0x10);  /*S105*/
    { PIN2; q0 = 0x400d; q1 = 0;
      __ActorMessage(q0, q1); }  /*S106*/
    __Func_809280c(0xc, 0xd, 0);  /*S107*/
    __MapActor_DoAnim(0xc, 3);  /*S108*/
    __CutsceneWait(0x1e);  /*S109*/
    __MessageID(m + 0x11);  /*S110*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S111*/
    __MapActor_DoAnim(0xd, 3);  /*S112*/
    __CutsceneWait(0x1e);  /*S113*/
    __MessageID(m + 0x12);  /*S114*/
    { PIN2; q0 = 0x400d; q1 = 0;
      __ActorMessage(q0, q1); }  /*S115*/
    { PIN3; q0 = 0xc; q1 = 0x84 << 1; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }  /*S116*/
    __Func_809259c(2, 1);  /*S117*/
    __CutsceneWait(0x1e);  /*S118*/
    __MessageID(m + 0x13);  /*S119*/
    { PIN2; q0 = 0x4002; q1 = 0;
      __ActorMessage(q0, q1); }  /*S120*/
    __Func_809280c(0xc, 2, 0);  /*S121*/
    __MapActor_DoAnim(0xc, 3);  /*S122*/
    __CutsceneWait(0x1e);  /*S123*/
    __MessageID(m + 0x14);  /*S124*/
    { PIN2; q0 = 0x4002; q1 = 0;
      __ActorMessage(q0, q1); }  /*S125*/
    __Func_80925cc(1, 1);  /*S126*/
    __MessageID(m + 0x15);  /*S127*/
    { PIN2; q0 = 0x4001; q1 = 0;
      __ActorMessage(q0, q1); }  /*S128*/
    { PIN3; q0 = 0xc; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S129*/
    __MapActor_DoAnim(0xc, 4);  /*S130*/
    __MessageID(m + 0x16);  /*S131*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S132*/
    __Func_80925cc(3, 1);  /*S133*/
    __MessageID(m + 0x17);  /*S134*/
    { PIN2; q0 = 0x4003; q1 = 0;
      __ActorMessage(q0, q1); }  /*S135*/
    __Func_809280c(0xc, 3, 0);  /*S136*/
    { PIN3; q0 = 0xc; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }  /*S137*/
    __MessageID(m + 0x18);  /*S138*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S139*/
    __Func_8092848(2, 0xd, 0);  /*S140*/
    __CutsceneWait(0x14);  /*S141*/
    __MapActor_DoAnim(2, 3);  /*S142*/
    __CutsceneWait(0x1e);  /*S143*/
    __MessageID(m + 0x19);  /*S144*/
    { PIN2; q0 = 0x4002; q1 = 0;
      __ActorMessage(q0, q1); }  /*S145*/
    __MapActor_DoAnim(0xd, 3);  /*S146*/
    __Func_809280c(0xd, 0xc, 0);  /*S147*/
    __Func_809280c(2, 0xc, 0);  /*S148*/
    __CutsceneWait(0x14);  /*S149*/
    __MessageID(m + 0x1a);  /*S150*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S151*/
    { PIN2; q0 = 0xc; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }  /*S152*/
    __MessageID(m + 0x1b);  /*S153*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S154*/
    __Func_809280c(1, 0xc, 0);  /*S155*/
    __MessageID(m + 0x1c);  /*S156*/
    { PIN2; q0 = 0x4001; q1 = 0;
      __ActorMessage(q0, q1); }  /*S157*/
    { PIN3; q0 = 0xc; q1 = 0x105; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }  /*S158*/
    __MapActor_DoAnim(3, 4);  /*S159*/
    __MessageID(m + 0x1d);  /*S160*/
    { PIN2; q0 = 0x4003; q1 = 0;
      __ActorMessage(q0, q1); }  /*S161*/
    __Func_80925cc(0xc, 1);  /*S162*/
    __MessageID(m + 0x1e);  /*S163*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S164*/
    __Func_80925cc(0xc, 1);  /*S165*/
    __MessageID(m + 0x1f);  /*S166*/
    __ActorMessage(0x400d, 0);  /*S167*/
    __CutsceneWait(0x1e);  /*S168*/
    __Func_809280c(0xc, 0xd, 0);  /*S169*/
    __CutsceneWait(0x3c);  /*S170*/
    __MapActor_DoAnim(0xc, 4);  /*S171*/
    __MessageID(m + 0x20);  /*S172*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S173*/
    __MapActor_DoAnim(0xd, 3);  /*S174*/
    __CutsceneWait(0x1e);  /*S175*/
    { PIN3; q0 = 0xc; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S176*/
    { PIN3; q0 = 0xc; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }  /*S177*/
    { PIN3; q0 = 0xc; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }  /*S178*/
    { PIN3; q0 = 0xc; q1 = 0x90; q2 = 0x84 << 2;
      __Func_809218c(q0, q1, q2); }  /*S179*/
    __MapActor_WaitMovement(0xc);  /*S180*/
    { PIN3; q0 = 0xc; q1 = 0xa8; q2 = 0x8c << 2;
      __Func_809218c(q0, q1, q2); }  /*S181*/
    __MapActor_WaitMovement(0xc);  /*S182*/
    __CutsceneWait(0x3c);  /*S183*/
    { PIN3; q0 = 0xc; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S184*/
    __CutsceneWait(0x28);  /*S185*/
    __Func_8092adc(0xc, 0xa0 << 7, 0);  /*S186*/
    __CutsceneWait(0x28);  /*S187*/
    { PIN3; q0 = 0xc; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S188*/
    __CutsceneWait(0x28);  /*S189*/
    { PIN3; q0 = 0xc; q1 = 0x90; q2 = 0x84 << 2;
      __Func_809218c(q0, q1, q2); }  /*S190*/
    __MapActor_WaitMovement(0xc);  /*S191*/
    __Func_809218c(0xc, 0xa8, 0xf4 << 1);  /*S192*/
    __MessageID(m + 0x21);  /*S193*/
    { PIN2; q0 = 0x4002; q1 = 0;
      __ActorMessage(q0, q1); }  /*S194*/
    __MapActor_WaitMovement(0xc);  /*S195*/
    { PIN3; q0 = 0xc; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S196*/
    __MapActor_SetAnim(0xc, 1);  /*S197*/
    __MapActor_DoAnim(0xc, 4);  /*S198*/
    __CutsceneWait(0x14);  /*S199*/
    __MessageID(m + 0x22);  /*S200*/
    __ActorMessage(0xc, 0);  /*S201*/
    __CutsceneWait(0x14);  /*S202*/
    __Func_8092848(1, 0, 0);  /*S203*/
    __CutsceneWait(0x3c);  /*S204*/
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S205*/
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S206*/
    { PIN3; q0 = 0xc; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }  /*S207*/
    __MessageID(m + 0x23);  /*S208*/
    __ActorMessage(0xc, 0);  /*S209*/
    __Func_80925cc(3, 1);  /*S210*/
    __MessageID(m + 0x24);  /*S211*/
    __ActorMessage(0x4003, 0);  /*S212*/
    { PIN3; q0 = 2; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }  /*S213*/
    __MessageID(m + 0x25);  /*S214*/
    __ActorMessage(0x4002, 0);  /*S215*/
    __Func_809280c(1, 0, 0);  /*S216*/
    __MapActor_Emote(2, 0x81 << 1, 0x28);  /*S217*/
    __MessageID(m + 0x26);  /*S218*/
    { PIN2; q1 = 0; q0 = 0x4001;
      __Func_8092c40(q0, q1); }  /*S219*/
    if (__Func_8091c7c(0, 0) == 0) {
    __MessageID(m + 0x27);  /*S221*/
    __ActorMessage(0x400c, 0);  /*S222*/
    } else {
    __MessageID(m + 0x28);  /*S223*/
    __ActorMessage(0x400c, 0);  /*S224*/
    }
    { PIN3; q0 = 0xc; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }  /*S225*/
    __Func_809280c(0, 0xc, 0);  /*S226*/
    __Func_809280c(1, 0xc, 0);  /*S227*/
    __Func_809280c(3, 0xc, 0);  /*S228*/
    __Func_809280c(2, 0xc, 0);  /*S229*/
    __Func_809280c(0xd, 0xc, 0);  /*S230*/
    { PIN3; q2 = 0x3c; q1 = 0x101; q0 = 0xd;
      __MapActor_Emote(q0, q1, q2); }  /*S231*/
    do { m = 0x253f; } while (0);
    __MessageID(m);  /*S232*/
    __ActorMessage(0x400d, 0);  /*S233*/
    __CutsceneWait(0x14);  /*S234*/
    __MapActor_Emote(0xc, 0x84 << 1, 0x3c);  /*S235*/
    __MessageID(m + 1);  /*S236*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S237*/
    __MapActor_Emote(3, 0x101, 0x3c);  /*S238*/
    __MessageID(m + 2);  /*S239*/
    __ActorMessage(0x4003, 0);  /*S240*/
    __Func_809280c(0xc, 3, 0);  /*S241*/
    __CutsceneWait(0xa);  /*S242*/
    __MapActor_DoAnim(0xc, 3);  /*S243*/
    __Func_8092adc(0xc, 0xc0 << 6, 0);  /*S244*/
    __MessageID(m + 3);  /*S245*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S246*/
    __Func_809280c(2, 0xc, 0);  /*S247*/
    __MessageID(m + 4);  /*S248*/
    __ActorMessage(0x4002, 0);  /*S249*/
    __CutsceneWait(0xa);  /*S250*/
    __MapActor_DoAnim(0xc, 4);  /*S251*/
    __MessageID(m + 5);  /*S252*/
    { PIN2; q0 = 0x400c; q1 = 0;
      __ActorMessage(q0, q1); }  /*S253*/
    __Func_8092848(0, 1, 0);  /*S254*/
    __CutsceneWait(0x3c);  /*S255*/
    __Func_809280c(0, 0xc, 0);  /*S256*/
    __Func_809280c(1, 0xc, 0);  /*S257*/
    __MessageID(m + 6);  /*S258*/
    __ActorMessage(0x4001, 0);  /*S259*/
    __Func_809280c(0xc, 1, 0);  /*S260*/
    __MapActor_DoAnim(0xc, 3);  /*S261*/
    m += 7;
    __CutsceneWait(0x14);  /*S262*/
    __MessageID(m);  /*S263*/
    __ActorMessage(0x400c, 0);  /*S264*/
    __Func_8092848(0xd, 2, 0);  /*S265*/
    __CutsceneWait(0x3c);  /*S266*/
    __MapActor_SetAnim(0xd, 3);  /*S267*/
    __MapActor_SetAnim(2, 3);  /*S268*/
    __CutsceneWait(0x3c);  /*S269*/
    __MapActor_SetAnim(0, 3);  /*S270*/
    __MapActor_SetAnim(1, 3);  /*S271*/
    __MapActor_SetAnim(3, 3);  /*S272*/
    __Func_809280c(0xc, 0, 0);  /*S273*/
    __MapActor_SetAnim(0xc, 3);  /*S274*/
    __CutsceneWait(0x3c);  /*S275*/
    OvlFunc_941_20091b8();  /*S276*/

}
