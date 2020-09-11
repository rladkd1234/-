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

	int rwsize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth);

	// Kapur algorithm
	double P1[256] = { 0, };
	double P2[256] = { 0, };
	int hist[256] = { 0, };
	int hissum[256] = { 0, };
	double H1 = 0;
	double H2 = 0;
	double max = 0;
	double sum = 0;
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
	}

	for (int i = 1; i < 256; i++)
	{
		hissum[i] += hissum[i - 1];
	}
	

	// get T
	for (int T = 0; T < 256; T++)
	{
		H1 = 0;
		H2 = 0;

		for (int i = 0; i < 256; i++)
		{	
				P1[i] = (double)hist[i]; 
				P2[i] = (double)hist[i];
		}

		for (int i = 0; i < 256; i++)
		{
			if (i <= T)
			{
				if (hissum[T] == 0)
					P1[i] = 0;
				else
					P1[i] /= (double)hissum[T];
				//printf("P1[%d]= %f \n", i, P1[i]);
				
				if (P1[i] == 0)
					continue;
				else
					H1 -= (double) P1[i] * (log(P1[i]) / log10(2.));

			}
			else
		{
				if ((hissum[255] - hissum[T]) == 0)
					P2[i] = 0;
				else
					P2[i] /= (double)(hissum[255] - hissum[T]);
				//printf("hissum255 - hissumT = %lf\n", (double)(hissum[255] - hissum[T]));
				//printf("P2[%d]= %f \n", i, P2[i]);
				
				if (P2[i] == 0)
					continue;
				else
					H2 -= (double)P2[i] * (log(P2[i]) / log10(2.));
			}
			
		}
		
		sum = H1 + H2;

	//	printf("%d sum = %lf \n", T, sum);
		if (max <= sum ){
			max = sum;
			maxT = T;
		}

	}
	
	printf("Kapur threshold = %d\n", maxT);

	for (int i = 0; i < hInfo.biHeight; i++)
	for (int j = 0; j < hInfo.biWidth; j++)
	{
		//Get histogram 
		if (Img[i * rwsize + j] > maxT)
			Img[i * rwsize + j] = 255;
		else
			Img[i * rwsize + j] = 0;

	}

	sprintf(outfilename, "Kapur th %s", filename);
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
