#include	<windows.h>
#include	<stdio.h>
#include	"le.h"

		void	decode_se(				unsigned char *,
										char *			);

extern	void	decode_adpcm(			unsigned char *,
										short *,
										long *,
										long *			);
extern	HANDLE	NonOverWriteCreateFile(LPCSTR);



void			decode_se(	unsigned char *org,
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
	char			name[MAX_PATH];

	unsigned long	sample_cnt;
	unsigned long	sample_max;
	unsigned long	s,w;
	short			*wav=NULL;
	long			ppl,pl;
	WAVHDR			wavhdr;
	HANDLE			hwav;

	unsigned long	IDO				=*(unsigned long *)(org+0x04);
	unsigned long	IMX				=*(unsigned long *)(org+0x08);
	unsigned long	IAD;
	unsigned long	INO;

	for(INO=0;INO<IMX;INO++){
		IAD=IDO+ *(unsigned long *)(org+0x0c+INO*8);
									//真のサンプル数

		sample_cnt=ler4(org+IAD+0x0c)>>4;
		sample_max=sample_cnt*14*2;

		sprintf(wavhdr.hriff,	"RIFF");
		sprintf(wavhdr.hwave,	"WAVEfmt ");
		sprintf(wavhdr.hdata,	"data");
				wavhdr.hdrsize		=0x10;
				wavhdr.format		=1;		//PCM
				wavhdr.bits			=16;
				wavhdr.channel		=1;
				wavhdr.freq			=ler4(org+IAD+0x10);
				wavhdr.blocksize	=wavhdr.channel<<1;
				wavhdr.byte_per_sec =wavhdr.freq*wavhdr.blocksize;
				wavhdr.datasize		=wavhdr.blocksize*sample_max;
				wavhdr.totalsize	=wavhdr.datasize+sizeof(wavhdr)-8;

		wav=new short[sample_max*2];

		TUL1=sample_cnt;						//Ｌチャネルデコード
		s=0;	w=0;	ppl=0;	pl=0;
		while(TUL1) {
			decode_adpcm(org+IAD+0x30+s,(short *)(wav+w),&ppl,&pl);
			s+=0x10;
			w+=14*2;
			TUL1--;
		}

		sprintf(name,"%s-%08X.wav",fname,INO);			//ファイルオープン
		hwav=NonOverWriteCreateFile((LPCSTR)name);
		if(hwav==(HANDLE)-1){
			printf("\tError\tcan't create wav file\n");
			exit(-1);									}

		WriteFile(hwav,&wavhdr,sizeof(WAVHDR),&TUL1,NULL);
		WriteFile(hwav,wav,sample_max*2*wavhdr.channel,&TUL1,NULL);

		CloseHandle(hwav);
		if(wav!=NULL)	delete[]	wav;
	}
	return;
}
