
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
		return (char*)"";
	
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
		return (char*)"";
	
	return (fname+i+1);
}
//==============================================================================



char* 	FATFS_GetFilenameFromPath(char *path)
{
	if (path == NULL)
		return NULL;

	int16_t i = strlen(path) - 1;
	while (path[i] != 0 && path[i] != '/' && i >= 0)
		i--;

	return (path + i + 1);
}
//==============================================================================



char*		FATFS_GetFilenameFromPathUTF(char *path)
{
	if (path == NULL)
		return NULL;

	int16_t i = strlen(path) - 1;
	if (path[i] >= 0x80)
		i--;
	while (path[i] != 0 && path[i] != '/' && i >= 0)
	{
		i--;
		if (path[i] >= 0x80)
			i--;
	}

	return (path + i + 1);
}
//==============================================================================



void		FATFS_GetPathFromFilename(char *fname, char *path)
{
	if (fname == NULL || path == NULL)
		return;

	int16_t i = strlen(fname) - 1;
	while (fname[i] != 0 && fname[i] != '/' && i >= 0)
		i--;

	if (i < 0)
	{
		path[0] = 0;
		return;
	}
	if (i == 0)
	{
		path[0] = '/';
		path[1] = 0;
	}
	strncpy(path, fname, i);
	path[i] = 0;
	return;
}
//==============================================================================



void		FATFS_GetPathFromFilenameUTF(char *fname, char *path)
{
	if (fname == NULL || path == NULL)
		return;

	int16_t i = strlen(fname) - 1;
	if (path[i] >= 0x80)
		i--;
	while (fname[i] != 0 && fname[i] != '/' && i >= 0)
	{
		i--;
		if (path[i] >= 0x80)
			i--;
	}

	if (i < 0)
	{
		path[0] = 0;
		return;
	}
	if (i == 0)
	{
		path[0] = '/';
		path[1] = 0;
	}
	strncpy(path, fname, i);
	path[i] = 0;
	return;
}
//==============================================================================



void FATFS_DelFileExtension(char *fname)
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




char*		strupper_utf(char *src)
{
	uint16_t	uchar;
	char		*string = src;
	while (*string != 0)
	{
		if (*string > 96 && *string < 123)
		{
			*string -= 32;
		}
		else if (*string >= 0x7F)
		{
			// cyrillic UTF8
			uchar = *string;
			uchar <<= 8;
			uchar +=  *(string + 1);
			if (uchar > 0xD0AF && uchar < 0xD0C0)
			{
				uchar -= 0x20;
			}
			else if (uchar > 0xD17F && uchar < 0xD190)
			{
				uchar -= 0xE0;
			}
			
			*string = uchar >> 8;
			string++;
			*string = uchar & 0xFF;
		}
		string++;
	}
	
	return src;
}
//==============================================================================




