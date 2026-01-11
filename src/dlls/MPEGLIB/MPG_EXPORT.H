#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#define MPEG_Mode_BlackAndWhite 0x0001
#define MPEG_Mode_Fast2X		0x0002
#define MPEG_Mode_Fast4X		0x0004
#define MPEG_Mode_Fast8X		0x0008
#define MPEG_Mode_Fast16X		0x0010
#define MPEG_Mode_Fast32X		0x0020
#define MPEG_Mode_Fast64X		0x0040
#define MPEG_Mode_SuperFast		0x0080
#define MPEG_Mode_BigDots		0x0100
#define MPEG_Mode_UseFloat		0x0200
#define MPEG_Mode_VerticalFlip	0x0400
#define MPEG_Mode_SwapRedBlue   0x0800

typedef struct MPG_IOStruct_ {
	unsigned long	ulFlags; /* Faster */
	unsigned long	SizeX;
	unsigned long	SizeY;
	float			fFrameRate;
	unsigned long	ulCurrentFrame;
	unsigned long	ulNumberOfFrames;
	int				(* Read2048 ) (int);
	void			(* GoStart  ) (void);
	unsigned long	*ulBufferResult;
} MPG_IOStruct;

#ifdef JADEFUSION
extern int  MPEG_INIT(struct MPG_IOStruct_ *p_IO);
#else
extern int  MPEG_INIT(MPG_IOStruct *p_IO);
#endif
extern int  MPEG_FRAME(void);
extern void MPEG_GOTO(int ulNum);
extern int  MPEG_END(void);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
