// BMP24toILI565.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "dirent.h"

FILE *rgb24file;
FILE *rgb16file;
int bmpWidth, bmpHeight;   // W+H in pixels
UINT16  bmpDepth;              // Bit depth (currently must be 24)
UINT32 bmpImageoffset;        // Start of image data in file
UINT32 rowSize;               // Not always = bmpWidth; may have padding
boolean  goodBmp = false;       // Set to true on valid header parse
boolean  flip    = true;        // BMP is stored bottom-to-top
UINT16 w, h, row, col;
UINT8  r, g, b;
UINT32 pos = 0, startTime;
bool  first = true;
DIR* dir;
struct dirent *ent;


//inline UINT16 read16(FILE *file)
//{
//	UINT16 word;
//	fread(&word, 2, 1, file);
//	return word;
//}

//inline UINT16 read32(FILE *file)
//{
//	UINT32 dword;
//	fread(&dword, 4, 1, file);
//	return dword;
//}

UINT16 read16(FILE *file) {
	UINT16 word;
	fread(&(((UINT8 *)&word)[0]), 1, 1, file);
	fread(&(((UINT8 *)&word)[1]), 1, 1, file);
	//((UINT8 *)&word)[0] = f.read(); // LSB
	//((UINT8 *)&word)[1] = f.read(); // MSB
	return word;
}
//
UINT32 read32(FILE *file) {
	UINT32 dword;
	fread(&(((UINT8 *)&dword)[0]), 1, 1, file);
	fread(&(((UINT8 *)&dword)[1]), 1, 1, file);
	fread(&(((UINT8 *)&dword)[2]), 1, 1, file);
	fread(&(((UINT8 *)&dword)[3]), 1, 1, file);
	return dword;
}

inline UINT16 to565(UINT8 r, UINT8 g, UINT8 b) 
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	//return ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
}

int convertImage(char* filename)
{
	try
	{
		printf("\n");
		printf("Converting %s...\n", filename);
		rgb24file = fopen(filename, "rb");
		if (rgb24file == NULL)
		{
			printf("Could not find file %s \n", filename);
			return -1;
		}

		fseek(rgb24file, 0, SEEK_SET);

		if(read16(rgb24file) != 0x4D42)  // BMP signature
		{
			printf("Not a BMP file.\n");
			return -1;
		}

		printf("File size: %u\n", read32(rgb24file));
		(void)read32(rgb24file); // Read & ignore creator bytes
		bmpImageoffset = read32(rgb24file); // Start of image data
		printf("Image Offset: %d\n", bmpImageoffset);
		// Read DIB header
		printf("Header size: %d\n", read32(rgb24file));
		bmpWidth  = read32(rgb24file);
		bmpHeight = read32(rgb24file);

		if(read16(rgb24file) != 1) // # planes -- must be '1'
		{
			printf("Number of planes must be 1\n");
			return -1;
		}

		bmpDepth = read16(rgb24file); // bits per pixel
		printf("Bit Depth: %d\n", bmpDepth);

		if(bmpDepth != 24)
		{ 
			printf("Image is not in 24bit\n");
			return -1;
		}

		if(read32(rgb24file) != 0)  // 0 = uncompressed
		{
			printf("BMP must be in uncompressed format\n");
			return -1;
		}

		goodBmp = true; // Supported BMP format -- proceed!
		printf("Image size: %dx%d\n", bmpWidth, bmpHeight);

		char outFilename[255];
		sprintf(outFilename, "%s", filename);
		sprintf(outFilename+strlen(filename)-3, "565");

		printf("%s created\n", outFilename);
		rgb16file = fopen(outFilename, "wb");

		if(rgb16file == NULL)
		{
			printf("Could not create file %s\n", outFilename);
			return -1;
		}

		UINT8 header[54];
		fseek(rgb24file, 0, SEEK_SET);
		fread(header, 1, 54, rgb24file);

		header[0x1C] = 16;	// updage bit depth to 16 bpp

		fwrite(header, 1, 54, rgb16file);

		// BMP rows are padded (if needed) to 4-byte boundary
		rowSize = (bmpWidth * 3 + 3) & ~3;

		// If bmpHeight is negative, image is in top-down order.
		// This is not canon but has been observed in the wild.
		if(bmpHeight < 0) {
			bmpHeight = -bmpHeight;
			flip      = false;
		}

		UINT8 inRGB[3], outRGB[2];

		for (row=0; row<bmpHeight; row++) { // For each scanline...

			if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
				pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
			else     // Bitmap is stored top-to-bottom
				pos = bmpImageoffset + row * rowSize;

			fseek(rgb24file, pos, SEEK_SET);

			for(UINT16 c=0; c<3*bmpWidth; c+=3)
			{
				fread(inRGB, 1, 3, rgb24file);
				UINT16 iliColor = to565(inRGB[2], inRGB[1], inRGB[0]);
				outRGB[0] = iliColor >> 8;
				outRGB[1] = iliColor & 0xFF;
				fwrite(outRGB, 1, 2, rgb16file);
			}
		}
	}
	catch(...)
	{
		printf("Error converting file\n");
	}

	_fcloseall();
}

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		if ((dir = opendir (".")) != NULL) {
			/* print all the files and directories within directory */
			int bmpFilesFound = 0;
			while ((ent = readdir(dir)) != NULL) {
				if(strncmp(ent->d_name + ent->d_namlen - 4, ".bmp", 4) == 0)
				{
					convertImage(ent->d_name);
					bmpFilesFound++;
				}
			}
			closedir (dir);
			if(bmpFilesFound == 0)
			{
				printf("\n");
				printf("No .bmp files found.\n");
			}
		} else {
			/* could not open directory */
			perror ("");
			return EXIT_FAILURE;
		}
	}
	else
	{
		convertImage(argv[1]);
	}


}
