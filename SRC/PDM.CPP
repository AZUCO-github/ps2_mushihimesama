#include	<windows.h>
#include	<stdio.h>

extern	HANDLE	NonOverWriteCreateFile(LPCSTR);
		void	decode_pdm(unsigned char *,char *);
extern	void	decode_bgm(				unsigned char *,
										char *			);
extern	void	decode_se(				unsigned char *,
										char *			);
extern	void	decode_tim2(char *,unsigned long,unsigned char *);


extern	unsigned char	*tbl;



		void	decode_pdm(unsigned char *pdm,char *file)
{
	char			fname[_MAX_PATH];
	unsigned long	IMX=*(unsigned short *)(tbl+0x0a);
	unsigned long	IOF=*(unsigned short *)(tbl+0x08);

	unsigned long	INO;
	unsigned long	IAD;
	unsigned long	IAP;
	unsigned long	ILN;
	HANDLE			hndl;
	unsigned long	tmp;

	for(INO=0;INO<IMX;INO++){
		IAP=*(unsigned long *)(tbl+IOF+INO*8);
		ILN=*(unsigned long *)(tbl+IOF+INO*8+4);
		IAD=*(unsigned long *)(tbl+IAP);

		if(	(pdm[IAD+0]=='T')&&
			(pdm[IAD+1]=='I')&&
			(pdm[IAD+2]=='M')&&
			(pdm[IAD+3]=='2')	)	{	sprintf(fname,"%08d.TM2",INO);
										decode_tim2(fname,ILN,pdm+IAD);
		}

		if(	(pdm[IAD+0]=='S')&&
			(pdm[IAD+1]=='E')&&
			(pdm[IAD+2]=='0')&&
			(pdm[IAD+3]=='0')	)	{	sprintf(fname,"%08d.SE",INO);
										decode_se(pdm+IAD,fname);
		}

		if(	(pdm[IAD+0]=='B')&&
			(pdm[IAD+1]=='G')&&
			(pdm[IAD+2]=='0')&&
			(pdm[IAD+3]=='0')	)	{	sprintf(fname,"%08d.BGM",INO);
										decode_bgm(pdm+IAD,fname);
		}
	}
}
