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
BYTE outImg[H * 4][W * 4];
float temp[H][W];
float temp2[H][W];
//float temp2[H][W];


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

	for (int i = 0; i < H - 1; i++)
	for (int j = 0; j < W - 1; j++)
	{
		temp[i][j] = (float)(inImg[i][j + 1] - inImg[i][j]); // 수평차
		temp2[i][j] = (float)(inImg[i + 1][j] - inImg[i][j]); // 수직 차
	}

	for (int i = 0; i < 4 * H; i+=4 )
	for (int j = 0; j < 4 * W; j+=4 ){
			outImg[i][j + 1] = (BYTE)((outImg[i][j] + (temp[i / 4][j / 4] / 4) * 1. ) + 0.5);
			outImg[i][j + 2] = (BYTE)((outImg[i][j] + (temp[i / 4][j / 4] / 4) * 2.) + 0.5);
			outImg[i][j + 3] = (BYTE)((outImg[i][j] + (temp[i / 4][j / 4] / 4) * 3.) + 0.5);
	}

	for (int i = 0; i < 4 * H; i+=4)
		for (int j = 0; j < 4 * W; j++){
				outImg[i + 1][j] = (BYTE)((outImg[i][j] + (temp2[i / 4][j / 4] / 4) * 1.) + 0.5);
				outImg[i + 2][j] = (BYTE)((outImg[i][j] + (temp2[i / 4][j / 4] / 4) * 2.) + 0.5);
				outImg[i + 3][j] = (BYTE)((outImg[i][j] + (temp2[i / 4][j / 4] / 4) * 3.) + 0.5);
		}

	FILE* fp;

	fp = fopen("6-2 lena.raw", "wb");
	fwrite(outImg, sizeof(char), H*W * 16, fp);
	fclose(fp);


}

void main()
{
	bmpBWrw();
}