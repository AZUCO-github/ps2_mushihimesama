#include	<windows.h>
#include	<stdio.h>
#include	"le.h"

		void	decode_bgm(				unsigned char *,
										char *			);

extern	void	decode_adpcm(			unsigned char *,
										short *,
										long *,
										long *			);
extern	HANDLE	NonOverWriteCreateFile(LPCSTR);



void			decode_bgm(	unsigned char *org,
							char *fname			)
{
	typedef	struct	{
		char	hriff[4];
		long	totalsize;

		char	hwave[8];
		long	hdrsize;
		short	format;
		short	channel;
		long	freq;
		long	byte_per_sec;
		short	blocksize;
		short	bits;

		char	hdata[4];
		long	datasize;
	}	WAVHDR;

	unsigned long	TUL1;
	long			TSL1;
	char			name[MAX_PATH];

	unsigned long	s,w;
									//真のサンプル数
	unsigned long	sample_cnt		=ler4(org+0x4c)>>4;
	unsigned long	sample_max;
									//インターリーブ長
	unsigned long	interleave_len	=*(unsigned long *)(org+0x10);
	unsigned char	*srl=NULL;
	unsigned char	*srr=NULL;
	short			*wav=NULL;
	short			*wal=NULL;
	short			*war=NULL;
	long			ppl,pl,ppr,pr;
	WAVHDR			wavhdr;
	HANDLE			hwav;

	sample_max=sample_cnt*14*2;
	sprintf(wavhdr.hriff,	"RIFF");
	sprintf(wavhdr.hwave,	"WAVEfmt ");
	sprintf(wavhdr.hdata,	"data");
			wavhdr.hdrsize		=0x10;
			wavhdr.format		=1;		//PCM
			wavhdr.bits			=16;
			wavhdr.channel		=2;
			wavhdr.freq			=ler4(org+0x50);
			wavhdr.blocksize	=wavhdr.channel<<1;
			wavhdr.byte_per_sec =wavhdr.freq*wavhdr.blocksize;
			wavhdr.datasize		=wavhdr.blocksize*sample_max;
			wavhdr.totalsize	=wavhdr.datasize+sizeof(wavhdr)-8;



	srl=new unsigned char[sample_cnt*16+interleave_len];	//left
	srr=new unsigned char[sample_cnt*16+interleave_len];	//right
	wav=new short[sample_max*wavhdr.channel*2];
	wal=new short[sample_max*2];
	war=new short[sample_max*2];



	for(TUL1=0,	TSL1=sample_cnt*16;
				TSL1>0;
		TUL1++,	TSL1-=interleave_len){	//インターリーブ解除
		memcpy(srl+TUL1*interleave_len,
			org+0x800+TUL1*interleave_len*2,
			interleave_len);
		memcpy(srr+TUL1*interleave_len,
			org+0x800+TUL1*interleave_len*2+interleave_len,
			interleave_len);
	}



	TUL1=sample_cnt;						//Ｌチャネルデコード
	s=0;	w=0;	ppl=0;	pl=0;
	while(TUL1) {
		decode_adpcm(srl+s,(short *)(wal+w),&ppl,&pl);
		s+=0x10;
		w+=14*2;
		TUL1--;
	}



	TUL1=sample_cnt;						//Ｒチャネル
	s=0;	w=0;	ppr=0;	pr=0;
	while(TUL1) {
		decode_adpcm(srr+s,(short *)(war+w),&ppr,&pr);
		s+=0x10;
		w+=14*2;
		TUL1--;
	}



	for(TUL1=0;TUL1<sample_max;TUL1++){		//WAVインターリーブ
		wav[TUL1*2]		=wal[TUL1];
		wav[TUL1*2+1]	=war[TUL1];	}



	sprintf(name,"%s.wav",fname);			//ファイルオープン
	hwav=NonOverWriteCreateFile((LPCSTR)name);
	if(hwav==(HANDLE)-1){
		printf("\tError\tcan't create wav file\n");
		exit(-1);									}

	WriteFile(hwav,&wavhdr,sizeof(WAVHDR),&TUL1,NULL);
	WriteFile(hwav,wav,sample_max*2*wavhdr.channel,&TUL1,NULL);

	CloseHandle(hwav);
	if(war!=NULL)	delete[]	war;
	if(wal!=NULL)	delete[]	wal;
	if(wav!=NULL)	delete[]	wav;
	if(srr!=NULL)	delete[]	srr;
	if(srl!=NULL)	delete[]	srl;
	return;
}
