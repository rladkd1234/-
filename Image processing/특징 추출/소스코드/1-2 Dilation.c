#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#define BYTE    unsigned char

//BYTE Img[H][W];
//BYTE Img2[H][W];
//BYTE NG[H][W];


void bmpBWrw()
{
	FILE *infile;
	char filename[20] = "Lena.bmp";
	char outfilename[30];

	if ((infile = fopen("Lena.bmp", "rb")) == NULL) {
		printf("No Image File");
		return;
	}

	// BMP Header Information
	BITMAPFILEHEADER hf;
	BITMAPINFOHEADER hInfo;
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, infile);//14 bytes

	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, infile);//40 bytes
	if (hInfo.biBitCount != 8) { printf("Bad File Format!!"); return; }//BW

	//	getch();
	// BMP Pallete
	RGBQUAD hRGB[256];
	//getch();
	fread(hRGB, sizeof(RGBQUAD), 256, infile);//read color pallete



	// Memory

	BYTE *Img = malloc(hInfo.biSizeImage);//for c
	fread(Img, sizeof(char), hInfo.biSizeImage, infile);
	fclose(infile);

	BYTE *NG = malloc(hInfo.biSizeImage);
	memset(NG, 0, hInfo.biSizeImage);

	float *temp = malloc(hInfo.biSizeImage * sizeof(float));
	memset(temp, 0, hInfo.biSizeImage * sizeof(float));

	int rwsize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth);

	float result = 0;
	int max = 0;
	int hist[256] = { 0, };
	int Emax = 0;
	int Emin = 10000;

	// Dilation (Vally에 관심있을 때)
	for (int i = 2; i < hInfo.biHeight - 3; i++)
	for (int j = 2; j < rwsize - 3; j++) {
		//for (int i = 2; i < H - 3; i++)
		//for (int j = 2; j < W - 3; j++){
		max = 0;

		for (int k = 0; k < 5; k++) {
			for (int e = 0; e < 5; e++) {
				if (Img[(i + k - 2) * rwsize + j + e - 2] > max)
					max = Img[(i + k - 2) * rwsize + j + e - 2];
			}
			//if (Img[i + k - 2][j + e - 2]> max)
			//max = Img[i + k][j + e];
			//NG[i][j] = max - Img[i][j];
		}
		result = (float)(max - Img[i*rwsize + j]);

		temp[i*rwsize + j] = result;

		if (result > Emax)
			Emax = (int)result;

		if (result < Emin)
			Emin = (int)result;
	}


	for (int i = 1; i < hInfo.biHeight - 2; i++)
	for (int j = 1; j < rwsize - 2; j++) {
		//normalize and histogram
		temp[i * rwsize + j] = (float)((temp[i * rwsize + j] - Emin) / (Emax - Emin));
		NG[i * rwsize + j] = (BYTE)(temp[i * rwsize + j] * 255. + 0.5);
		//get histogram
		hist[NG[i*rwsize + j]]++;
	}

	//get threshold
	int hissum = 0;
	int T = 0;

	for (int i = 255; i >= 0; i--)
	{
		hissum += hist[i];
		if (hissum >= hInfo.biSizeImage * 0.2)
		{
			T = i;
			break;
		}
	}

	printf("Emax : %d, Emin : %d\n", Emax, Emin);
	printf("hissum = %d, T = %d\n", hissum, T);

	for (int i = 0; i < hInfo.biHeight; i++)
	for (int j = 0; j < rwsize; j++) {
		//Edge rate = 20%인 경계 출력
		if (NG[i * rwsize + j] >= T)
			NG[i*rwsize + j] = 0;
		else
			NG[i*rwsize + j] = 255;
	}


	sprintf(outfilename, "1-2) Dilation %s", filename);
	FILE *outfile = fopen(outfilename, "wb");
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
	fwrite(NG, sizeof(char), hInfo.biSizeImage, outfile);
	fclose(outfile);

	free(Img);
	free(NG);
	free(temp);
}

void main()
{
	bmpBWrw();
}
