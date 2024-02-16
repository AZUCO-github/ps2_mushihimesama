///////////////////////////////////////////////////////////////////////////////
//	AFS converter
//		TIM2 format
//			041204
//				by	AZUCO
///////////////////////////////////////////////////////////////////////////////
#include	<windows.h>
#include	<stdio.h>

extern	HANDLE	NonOverWriteCreateFile(LPCSTR);
extern	void	decode_palette_8bit(unsigned char *);
extern	void	decode_palette_4bit(unsigned char *);
extern	void	extend_palette_h2t(unsigned char *);

		bool	is_tim2(unsigned char *);
		void	decode_tim2(char *,unsigned long,unsigned char *);
		void	decode_tim2_xxxx(unsigned char *,char *);
		void	decode_tim2_0000(unsigned char *,char *);
		void	decode_tim2_0040(unsigned char *,char *);

void	decode_tim2(char *name,unsigned long ssrc,unsigned char *src)
{
/*
	switch(*(unsigned short *)(src+0x14)){
		case	0x0040:	//16p 1byte=2dot
			decode_tim2_0040(src,name);	break;
		case	0x0000:	//high c 2byte=1dot
			decode_tim2_0000(src,name);	break;
		default:
			decode_tim2_xxxx(src,name);	break;
	}
*/
/*
	switch(*(unsigned char *)(src+0x6)){
		case	0x01:	//256p
			decode_tim2_xxxx(src,name);	break;
		default:
			printf("TIM2 +06 =%02X\n",*(unsigned char *)(src+0x06));
			decode_tim2_xxxx(src,name);	break;
	}
*/

	//パレットサイズを見ての判定
	switch(*(unsigned long *)(src+0x14)){
		case	0x00000400:	//256p
			decode_tim2_xxxx(src,name);	break;
		case	0x00000040:	//16p
			decode_tim2_0040(src,name);	break;
		case	0x00000000:	//16p
			decode_tim2_0000(src,name);	break;
		default:
			printf("TIM2 +0x84 =%04X\n",*(unsigned long *)(src+0x84));
			decode_tim2_xxxx(src,name);	break;
	}
}



void	decode_tim2_xxxx(unsigned char *src,char *name)
{	unsigned long		tul1,tul2;
	unsigned long		sx,sy,sz;
	unsigned char		pal[0x400];
	char				tfn[0x100];
	HANDLE				hbmp;
	BITMAPFILEHEADER	bf;
	BITMAPINFOHEADER	bi;


//printf("type=%02X\n",*(src+0x05));
	if(*(src+0x05)==0){	sx=*(unsigned short*)(src+0x24);
						sy=*(unsigned short*)(src+0x26);	}
	else{				sx=*(unsigned short*)(src+0x94);
						sy=*(unsigned short*)(src+0x96);	}
						sz=sx*sy;
	if(sx==0)	return;
	if(sy==0)	return;

	sprintf(tfn,"%s.bmp",name);
	hbmp=NonOverWriteCreateFile((LPCSTR)tfn);
	if(hbmp==(HANDLE)-1){
		printf("\tError\tcan't create bmp file\n");
		exit(-1);	}



	*((char *)&bf.bfType+0)='B';
	*((char *)&bf.bfType+1)='M';
	bf.bfSize=			sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x400+						//palette
						sz;					//image
	bf.bfOffBits=		sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x400;						//palette

	bi.biSize=			sizeof(BITMAPINFOHEADER);
	bi.biWidth=			sx;
	bi.biHeight=		sy;
	bi.biPlanes=		1;
	bi.biBitCount=		8;
	bi.biCompression=	0;
	bi.biSizeImage=		0;
	bi.biXPelsPerMeter=	0;
	bi.biYPelsPerMeter=	0;
	bi.biClrUsed=		0;
	bi.biClrImportant=	0;

	WriteFile(hbmp,&bf,sizeof(BITMAPFILEHEADER),&tul1,NULL);
	WriteFile(hbmp,&bi,sizeof(BITMAPINFOHEADER),&tul1,NULL);

	if(*(src+0x05)==0){
		memcpy(pal,src+0x40+sz,0x400);
		if(*(src+0x15)!=0x04)
			extend_palette_h2t(pal);		//high color expression pal data
			decode_palette_8bit(pal);
		WriteFile(hbmp,pal,256*4,&tul1,NULL);

		for(tul2=(sz-sx);tul2<=sz;tul2-=sx)	//ベタなのでひっくり返すだけで良い
			WriteFile(hbmp,src+0x40+tul2,sx,&tul1,NULL);	}
	else{
		memcpy(pal,src+0x100+sz,0x400);
		if(*(src+0x85)!=0x04)				//これで正しいのかは不明
			extend_palette_h2t(pal);		//high color expression pal data
			decode_palette_8bit(pal);
		WriteFile(hbmp,pal,256*4,&tul1,NULL);

		for(tul2=(sz-sx);tul2<=sz;tul2-=sx)
			WriteFile(hbmp,src+0x100+tul2,sx,&tul1,NULL);	}
	CloseHandle(hbmp);
}



void	decode_tim2_0040(unsigned char *src,char *name)
{	unsigned long		tul1,tul2;
	unsigned long		sx,sy,sz;
	char				tfn[0x100];
	HANDLE				hbmp;
	BITMAPFILEHEADER	bf;
	BITMAPINFOHEADER	bi;

//printf("type=%02X\n",*(src+0x05));
	if(*(src+0x05)==0){	sx=*(unsigned short*)(src+0x24);
						sy=*(unsigned short*)(src+0x26);	}
	else{				sx=*(unsigned short*)(src+0x94);
						sy=*(unsigned short*)(src+0x96);	}
						sz=sx*sy/2;					//1byte=2dot
	if(sx==0)	return;
	if(sy==0)	return;

	sprintf(tfn,"%s.bmp",name);
	hbmp=NonOverWriteCreateFile((LPCSTR)tfn);
	if(hbmp==(HANDLE)-1){
		printf("\tError\tcan't create bmp file\n");
		exit(-1);	}



	*((char *)&bf.bfType+0)='B';
	*((char *)&bf.bfType+1)='M';
	bf.bfSize=			sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x40+						//palette
						sz;							//image
	bf.bfOffBits=		sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x40;						//palette

	bi.biSize=			sizeof(BITMAPINFOHEADER);
	bi.biWidth=			sx;
	bi.biHeight=		sy;
	bi.biPlanes=		1;
	bi.biBitCount=		4;
	bi.biCompression=	0;
	bi.biSizeImage=		0;
	bi.biXPelsPerMeter=	0;
	bi.biYPelsPerMeter=	0;
	bi.biClrUsed=		0;
	bi.biClrImportant=	0;

	WriteFile(hbmp,&bf,sizeof(BITMAPFILEHEADER),&tul1,NULL);
	WriteFile(hbmp,&bi,sizeof(BITMAPINFOHEADER),&tul1,NULL);

	sx=sx>>1;										//1byte=2dot

	if(*(src+0x05)==0){
		for(tul2=0;tul2<=sz;tul2++){
			tul1=	*(src+0x40+tul2);
					*(src+0x40+tul2)=	((tul1&0x0f)<<4)|((tul1&0xf0)>>4);	}
		decode_palette_4bit(src+0x40+sz);
		WriteFile(hbmp,src+0x40+sz,16*4,&tul1,NULL);
		for(tul2=(sz-sx*2);tul2<=sz;tul2-=sx*2){
			WriteFile(hbmp,src+0x40+tul2+sx*1,sx,&tul1,NULL);
			WriteFile(hbmp,src+0x40+tul2+sx*0,sx,&tul1,NULL);	}	}
	else{
		for(tul2=0;tul2<=sz;tul2++){
			tul1=	*(src+0x100+tul2);
					*(src+0x100+tul2)=	((tul1&0x0f)<<4)|((tul1&0xf0)>>4);	}
		decode_palette_4bit(src+0x100+sz);
		WriteFile(hbmp,src+0x100+sz,16*4,&tul1,NULL);
		for(tul2=(sz-sx*2);tul2<=sz;tul2-=sx*2){
			WriteFile(hbmp,src+0x100+tul2+sx*1,sx,&tul1,NULL);
			WriteFile(hbmp,src+0x100+tul2+sx*0,sx,&tul1,NULL);	}	}

	CloseHandle(hbmp);
}



void	decode_tim2_0000(unsigned char *src,char *name)
{	unsigned long		tul1,tul2;
	unsigned long		sx,sy,sz,sp;
	unsigned long		ax,ay,az,ap;
	char				tfn[0x100];
	unsigned char		*bmp;
	HANDLE				hbmp;
	BITMAPFILEHEADER	bf;
	BITMAPINFOHEADER	bi;

//printf("type=%02X\n",*(src+0x05));
	if(*(src+0x05)==0){	sx=*(unsigned short*)(src+0x24);
						sy=*(unsigned short*)(src+0x26);	}
	else{				sx=*(unsigned short*)(src+0x94);
						sy=*(unsigned short*)(src+0x96);	}
						sz=sx*sy*4;					//true color
	if(sx==0)	return;
	if(sy==0)	return;

	sprintf(tfn,"%s.bmp",name);
	hbmp=NonOverWriteCreateFile((LPCSTR)tfn);
	if(hbmp==(HANDLE)-1){
		printf("\tError\tcan't create bmp file\n");
		exit(-1);	}



	*((char *)&bf.bfType+0)='B';
	*((char *)&bf.bfType+1)='M';
	bf.bfSize=			sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						sz;							//image
	bf.bfOffBits=		sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER);

	bi.biSize=			sizeof(BITMAPINFOHEADER);
	bi.biWidth=			sx;
	bi.biHeight=		sy;
	bi.biPlanes=		1;
	bi.biBitCount=		32;
	bi.biCompression=	0;
	bi.biSizeImage=		0;
	bi.biXPelsPerMeter=	0;
	bi.biYPelsPerMeter=	0;
	bi.biClrUsed=		0;
	bi.biClrImportant=	0;

	WriteFile(hbmp,&bf,sizeof(BITMAPFILEHEADER),&tul1,NULL);
	WriteFile(hbmp,&bi,sizeof(BITMAPINFOHEADER),&tul1,NULL);

	if(*(src+0x05)==0)	src+=0x40;
	else				src+=0x100;
	bmp=new unsigned char[sz];

	for(ay=0;ay<sy;ay++){
		for(ax=0;ax<sx;ax++){
			ap=(ay*sx+ax)*4;
			sp=(ay*sx+ax)*2;
			bmp[ap+0]=	 (src[sp+1]&0x7c)<<1;
			bmp[ap+1]=	((src[sp+1]&0x03)<<6)|
						((src[sp+0]&0xe0)>>2);
			bmp[ap+2]=	 (src[sp+0]&0x1f)<<3;
			bmp[ap+3]=	0;
		}
	}

	for(tul2=(sz-sx*4);tul2<=sz;tul2-=sx*4)
		WriteFile(hbmp,bmp+tul2,sx*4,&tul1,NULL);
	CloseHandle(hbmp);
}



bool	is_tim2(unsigned char *src)
{
	if(	(src[0]=='T')&&
		(src[1]=='I')&&
		(src[2]=='M')&&
		(src[3]=='2')	)	return(TRUE);
							return(FALSE);	}

