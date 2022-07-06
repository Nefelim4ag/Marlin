
#include "../../MarlinCore.h"

#include "fatfs_shared.h"


FATFS FATFS_sd;



char*		FATFS_GetFileExtension(char *fname)
{
	if (fname == NULL)
		return NULL;
	
	int16_t	i = strlen(fname) - 1;
	while (fname[i] != 0 && fname[i] != '.' && i >= 0)
		i--;
	if (i < 0)
		return "";
	
	return (fname+i+1);
}
//==============================================================================




char*		FATFS_GetFileExtensionUTF(char *fname)
{
	if (fname == NULL)
		return NULL;
	
	int16_t	i = strlen(fname) - 1;
	if (fname[i] >= 0x80)
		i--;
	while (fname[i] != 0 && fname[i] != '.' && i >= 0)
	{
		i--;
		if (fname[i] >= 0x80)
			i--;
	}
	if (i < 0)
		return "";
	
	return (fname+i+1);
}
//==============================================================================




void		FATFS_DelFileExtension(char *fname)
{
	if (fname == NULL)
		return;
	
	char	*ffname = FATFS_GetFileExtension(fname);
	if (*ffname != 0)
	{
		ffname--;
		*ffname = 0;
	}
	return;
}
//==============================================================================




void		FATFS_DelFileExtensionUTF(char *fname)
{
	if (fname == NULL)
		return;
	
	char	*ffname = FATFS_GetFileExtensionUTF(fname);
	if (*ffname != 0)
	{
		ffname--;
		if (*ffname >= 0x80)
			ffname--;
		*ffname = 0;
	}
	
	return;
}
//==============================================================================




char*		FATFS_GetPrevDir(char *fname)
{
	if (fname == NULL)
		return NULL;
	
	int16_t	i = strlen(fname) - 1;
	while (fname[i] != 0 && fname[i] != '/' && i >= 0)
		i--;
	if (i < 0)
		return (char*)"";
	
	return (fname+i+1);
}
//==============================================================================




char*		FATFS_GetPrevDirUTF(char *fname)
{
	if (fname == NULL)
		return NULL;
	
	int16_t	i = strlen(fname) - 1;
	if (fname[i] >= 0x80)
		i--;
	while (fname[i] != 0 && fname[i] != '/' && i >= 0)
	{
		i--;
		if (fname[i] >= 0x80)
			i--;
	}
	if (i < 0)
		return (char*)"";
	
	return (fname+i+1);
}
//==============================================================================




