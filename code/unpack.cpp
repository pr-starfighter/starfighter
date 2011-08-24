/*
Copyright (C) 2003 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "unpack.h"

/*
Searches the pak file for the required data. When
it is found, the data is read into a character buffer.
*/
void unpack(char *file, signed char fileType)
{
	unsigned char *packBuffer;
	char packFilename[60];
	int packFSize;

	FILE *pak;
	FILE *fp = NULL; // music has to be read-written-read!
	char musicFilename[PATH_MAX];

	strcpy(packFilename, "");

	pak = fopen(PACKLOCATION, "rb");
	if (pak == NULL)
	{
		printf("Couldn't access the Project: Starfighter data file!\n");
		exit(1);
	}
	fseek(pak, 4, SEEK_SET);

	while (true)
	{
		if (!fread(packFilename, 1, 56, pak))
		{
			fclose(pak);
			if (fileType != PAK_FONT)
			{
				showErrorAndExit(0, file);
			}
			exit(1);
			break;
		}

		fread(&packFSize, 4, 1, pak);
		packFSize = SDL_SwapLE32(packFSize);

		if (strcmp(packFilename, file) == 0)
		{
			if ((fileType == PAK_MOD) || (fileType == PAK_S3M))
			{
				if (fileType == PAK_MOD)
				{
					sprintf(musicFilename, "%smusic.mod", engine.userHomeDirectory);
					fp = fopen(musicFilename, "wb");
				}
				else
				{
					sprintf(musicFilename, "%smusic.s3m", engine.userHomeDirectory);
					fp = fopen(musicFilename, "wb");
				}

				if (fp == NULL)
					showErrorAndExit(1, "");
			}
			packBuffer = (unsigned char*) malloc(packFSize);
			fread(packBuffer, 1, packFSize, pak);
			if ((fileType == PAK_MOD) || (fileType == PAK_S3M))
			{
				fwrite(packBuffer, 1, packFSize, fp);
				fclose(fp);
			}
			break;
		}
		else
		{
			fseek(pak, packFSize, SEEK_CUR);
		}
	}

	if ((fileType != PAK_MOD) && (fileType != PAK_S3M))
		engine.sdlrw = SDL_RWFromMem(packBuffer, packFSize);

	//printf("Extracted: %s\n", file);

	fclose(pak);
}

/*
Search the data package for the required file.
When it is found, return the location.
*/
int locateDataInPak(char *file, signed char required)
{
	//printf("Looking for %s...", file);

	char packFilename[60];
	int packFSize;
	int location = 0;

	FILE *pak;

	strcpy(packFilename, "");

	pak = fopen(PACKLOCATION, "rb");
	if (pak == NULL)
	{
		printf("Couldn't access the Project: Starfighter data file!\n");
		exit(1);
	}
	fseek(pak, 4, SEEK_SET);

	while (true)
	{
		if (!fread(packFilename, 1, 56, pak))
		{
			fclose(pak);
			if (required)
			{
				showErrorAndExit(0, file);
				exit(1);
			}
			break;
		}
		fread(&packFSize, 4, 1, pak);
		packFSize = SDL_SwapLE32(packFSize);

		if (strcmp(packFilename, file) == 0)
		{
			location = ftell(pak);
			fclose(pak);

			//printf("found it!\n");

			return location;
		}
		else
		{
			fseek(pak, packFSize, SEEK_CUR);
		}
	}

	//printf("not found (skipping)\n");

	return -1; // we only get this if it isn't required
}
