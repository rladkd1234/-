#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

#define BYTE    unsigned char
#define W 512
#define H 512  
BYTE inImg[H][W];
BYTE outImg[H*4][W*4];
float temp[H][W];



void bmpBWrw()
{
	FILE *infile;
	char filename[20] = "Lena.raw";
//	char outfilename[20];
	infile = fopen(filename, "rb");
	fread(inImg, sizeof(char), H*W, infile);
	fclose(infile);
	
	for (int i = 0; i < H; i++)
	for (int j = 0; j < W; j++)
		outImg[4 * i][4 * j] = inImg[i][j];

	for (int i = 0; i < 4 * H; i++)
	for (int j = 0; j < 4 * W; j++){
		if ((i % 4) != 0 || (j % 4) != 0)
			outImg[i][j] = inImg[i / 4][j / 4];

	}
	FILE* fp;

	fp = fopen("6-1 lena.raw", "wb");
	fwrite(outImg, sizeof(char), H*W*16, fp);
	fclose(fp);

	// Image Output
	//sprintf(outfilename, "", filename);

	
	
}

void main()
{
	bmpBWrw();
}