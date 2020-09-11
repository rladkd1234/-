#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#define BYTE    unsigned char

void bmpBWrw()
{
	FILE *infile;
	char filename[20] = "hand.bmp";
	char outfilename[30];

	if ((infile = fopen(filename, "rb")) == NULL) {
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
	//BYTE *Img = new BYTE[hInfo.biSizeImage];//for c++
	BYTE *Img = malloc(hInfo.biSizeImage);//for c
	fread(Img, sizeof(char), hInfo.biSizeImage, infile);
	fclose(infile);

	BYTE *outImg = malloc(hInfo.biSizeImage);
	memset(outImg, 0, hInfo.biSizeImage);

	int rwsize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth);

	// Otsu algorithm

	int GM = 0; //Global Mean
	int M1 = 0;
	int M2 = 0;
	int hist[256] = { 0, };
	int hissum[256] = { 0, };
	float cdf[256] = { 0, };
	float Var[256] = { 0, };
	float max = 0;
	int maxT = 0;
	//printf("H * w = %d, %d   rw = %d\n", hInfo.biWidth, hInfo.biHeight, rwsize);

	//Get histogram 
	for (int i = 0; i < hInfo.biHeight; i++)
	for (int j = 0; j < hInfo.biWidth; j++)
	{
		hist[Img[i * rwsize + j]]++;
	}

	//Get hissum, cdf and M
	for (int i = 0; i < 256; i++)
	{
		hissum[i] = hist[i];
		GM += hist[i] * i;
	}

	for (int i = 1; i < 256; i++)
	{
		hissum[i] += hissum[i - 1];
	}

	for (int i = 0; i < 256; i++)
	{
		cdf[i] = (float)hissum[i] / hissum[255];
	}

	GM /= (int)hissum[255];

	// get T
	for (int T = 0; T < 256; T++)
	{
		M1 = 0;
		M2 = 0;
		for (int i = 0; i < 256; i++)
		{
			if (i <= T)
			{
				M1 += hist[i] * i;
			}
			else
			{
				M2 += hist[i] * i;
			}
		}

		if (hissum[T] == 0)
			M1 = 0;
		else
			M1 /= (int)hissum[T];

		if ((hissum[255] - hissum[T]) == 0)
			M2 = 0;
		else
			M2 /= (int)(hissum[255] - hissum[T]);

		Var[T] = (float)(cdf[T] * (M1 - GM) * (M1 - GM) + (1 - cdf[T]) * (M2 - GM) * (M2 - GM));
		
	//	printf("Var[%d] = %lf\n", T, Var[T]);
		if (max <= Var[T]){
			max = Var[T];
			maxT = T;
		}
		//printf("hissum[] = %d\n", hissum[255]);
		//printf("M: %d \n", M);
		//printf("M1 : %d, M2 : %d\n", M1, M2);
		//printf("%d] max = %f \n",T, Var[T]);
	}

	printf("Otsu threshold = %d\n", maxT);

	for (int i = 0; i < hInfo.biHeight; i++)
	for (int j = 0; j < hInfo.biWidth; j++)
	{
		//Get histogram 
		if (Img[i * rwsize + j] > maxT)
			Img[i * rwsize + j] = 255;
		else
			Img[i * rwsize + j] = 0;

	}
	sprintf(outfilename, "Otsu th %s", filename);
	FILE *outfile = fopen(outfilename, "wb");
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
	fwrite(Img, sizeof(char), hInfo.biSizeImage, outfile);
	fclose(outfile);

	free(Img);

}

void main()
{
	bmpBWrw();
}
