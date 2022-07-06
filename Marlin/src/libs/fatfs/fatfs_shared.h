#ifndef FAT_FS_SHARED_H
#define FAT_FS_SHARED_H

#include "ff.h"


#define FF_DEBUG


#define     DISK_SD     "0"
#define     DISK_FLADH  "1"

extern FATFS FATFS_sd;


char*		FATFS_GetFileExtension(char *fname);
char*		FATFS_GetFileExtensionUTF(char *fname);
void		FATFS_DelFileExtension(char *fname);
void		FATFS_DelFileExtensionUTF(char *fname);
char*		FATFS_GetPrevDir(char *fname);


#endif