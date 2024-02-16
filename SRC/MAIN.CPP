/******************************************************************************
	PS2 虫姫さま
		prog. by	AZUCO
******************************************************************************/
#include	<windows.h>
#include	<stdio.h>
#include	"Shlwapi.h"

extern	void	decode_pdm(unsigned char *,char *);

	unsigned char	*tbl=0;



void	main(void)
{	char			file[_MAX_PATH];
	char			*tfpa;
	char			*tfpb;
	HANDLE			hndl;
	unsigned char	*buff=0;
	unsigned long	size;
	char			drive[MAX_PATH];
	char			dir[MAX_PATH];
	char			fname[MAX_PATH];
	char			ext[MAX_PATH];
	unsigned long	tmp;

	printf("PS2 虫姫さま file cutter\n");
	printf("\t\tProg. By AZUCO\n\n");



	strcpy(file,(const char *)GetCommandLine());				//コマンドラインをコピーする
	tfpa=strrchr(file,' ');		//一番最後のスペース以降をファイル名と見なす
	tfpa++;

	tfpb=strrchr(tfpa,'"');		//ダブルクオートありの場合
	if(tfpb!=NULL)	*tfpb=0;

	tfpb=strrchr(tfpa,'>');		//リダイレクトの場合
	if(tfpb!=NULL)	*tfpb=0;



	tfpb=&file[0];
loop:
	if(*tfpa=='\\'){	*(tfpb+0)='\\';
						*(tfpb+1)='\\';
						tfpa++;
						tfpb+=2;
						goto loop;		}
	if(*tfpa>=0x20){	*tfpb=*tfpa;
						tfpa++;
						tfpb++;
						goto loop;		}
						*tfpb=0;



	hndl=CreateFile(file,
					GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hndl==(HANDLE)-1){
		printf("rekotan <data file>\n");
		printf("\tError\tsource file not found\n");
		exit(-1);	}

	_splitpath(file,drive,dir,fname,ext);

	if(strcmp(fname,"CA013R")==0){
		if(strcmp(ext,".TBL")==0){
			size=GetFileSize(hndl,NULL);
			tbl=new unsigned char[size+4];
			ReadFile(hndl,tbl,size,&tmp,NULL);
			CloseHandle(hndl);
			tmp=size;

			PathRemoveExtension(file);
			PathAddExtension(file,".PDM");
//			tfpa=strrchr(file,'.');			tfpa++;
//			*tfpa='P';						tfpa++;
//			*tfpa='D';						tfpa++;
//			*tfpa='M';						tfpa++;

			hndl=CreateFile(file,
					GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hndl==(HANDLE)-1){
				printf("rekotan <data file>\n");
				printf("\tError\tsource file not found\n");
				exit(-1);	}

			size=GetFileSize(hndl,NULL);
			*(unsigned long *)(tbl+tmp)=size;
			buff=new unsigned char[size];
			ReadFile(hndl,buff,size,&tmp,NULL);
			CloseHandle(hndl);
			decode_pdm(buff,file);
		}

		if(strcmp(ext,".PDM")==0){
			size=GetFileSize(hndl,NULL);
			buff=new unsigned char[size];
			ReadFile(hndl,buff,size,&tmp,NULL);
			CloseHandle(hndl);
			tmp=size;

			PathRemoveExtension(file);
			PathAddExtension(file,".TBL");
//			tfpa=strrchr(file,'.');			tfpa++;
//			*tfpa='T';						tfpa++;
//			*tfpa='B';						tfpa++;
//			*tfpa='L';						tfpa++;

			hndl=CreateFile(file,
					GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hndl==(HANDLE)-1){
				printf("rekotan <data file>\n");
				printf("\tError\tsource file not found\n");
				exit(-1);	}

			size=GetFileSize(hndl,NULL);
			tbl=new unsigned char[size+4];
			ReadFile(hndl,tbl,size,&tmp,NULL);
			CloseHandle(hndl);
			*(unsigned long *)(tbl+size)=tmp;
			decode_pdm(buff,file);
		}
	}

	if(buff!=0)	delete[]	buff;
	if(tbl!=0)	delete[]	tbl;
}
