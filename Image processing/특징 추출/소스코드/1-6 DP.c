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

	BYTE *DP = malloc(hInfo.biSizeImage);
	memset(DP, 0, hInfo.biSizeImage);

	double *temp = malloc(hInfo.biSizeImage * sizeof(double));
	memset(temp, 0, hInfo.biSizeImage * sizeof(double));

	int rwsize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth);

	double result = 0;
	int sum = 0;
	int max = 0;
	double Emax = 0;
	double Emin = 10000;
	int hist[256] = { 0, };

	// DP
	for (int i = 2; i < hInfo.biHeight - 3; i++)
	for (int j = 2; j < rwsize - 3; j++){
		max = 0;
		sum = 0;
		for (int k = 0; k < 5; k++){
			for (int e = 0; e < 5; e++){
				sum += Img[(i + k - 2) * rwsize + j + e - 2];
			
				if (Img[(i + k - 2) * rwsize + j + e - 2] > max)
				max = Img[(i + k - 2) * rwsize + j + e - 2];
			}
		}
		// (sum / 25.) == 5x5 window에서 I(m,n)의 maximum intensity
		if (sum == 0)
			result = 0;
		else
			result = (double)((max - Img[i*rwsize + j]) / (float)(sum / 25.));
			
		temp[i*rwsize + j] = result;

		if (result > Emax)
			Emax = result;

		if (result < Emin)
			Emin = result;

	}

	//normalize and histogram
	for (int i = 1; i < hInfo.biHeight - 2; i++)
	for (int j = 1; j < rwsize - 2; j++) {
		temp[i * rwsize + j] = (double)((temp[i * rwsize + j] - Emin) / (Emax - Emin));
			//printf("temp = %f\n", temp[i*rwsize + j]);
		DP[i * rwsize + j] = (BYTE)(temp[i * rwsize + j] * 255. + 0.5);

		//get histogram
		hist[DP[i*rwsize + j]]++;
	}
	//for (int i = 255; i >= 0; i--)
	//{
	//	printf("his[%d] = %d\n", i, hist[i]);
	//}
	int hissum = 0;
	int T = 0;

	for (int i = 255; i > 0; i--)
	{
		hissum += hist[i];
		if (hissum >= hInfo.biSizeImage * 0.2)
		{
			T = i;
			break;
		}
	}

	printf("Emax : %f, Emin : %f\n", Emax, Emin);
	printf("hissum = %d, T = %d\n", hissum, T);

	//Edge rate = 20%인 경계 출력
	for (int i = 0; i < hInfo.biHeight; i++)
	for (int j = 0; j < rwsize; j++) {
		if (DP[i * rwsize + j] >= T)
			DP[i*rwsize + j] = 0;
		else
			DP[i*rwsize + j] = 255;
	}

	sprintf(outfilename, "1-6) DP %s", filename);
	FILE *outfile = fopen(outfilename, "wb");
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
	fwrite(DP, sizeof(char), hInfo.biSizeImage, outfile);
	fclose(outfile);

	free(Img);
	free(DP);
	free(temp);
}

void main()
{
	bmpBWrw();
}
