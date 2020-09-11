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

	// Itertative threshold

	float M1 = 0;
	float M2 = 0;
	int temp1 = 0;
	int temp2 = 0;
	int flag = 1;
	int T[100] = { 0, };
	int hist[256] = { 0, };
	int hissum[256] = { 0, };
	int sum = 0;

	//printf("H * w = %d, %d   rw = %d\n", hInfo.biWidth, hInfo.biHeight, rwsize);
	// 4개의 귀퉁이를 배경으로 봄
	for (int i = 0; i < hInfo.biHeight; i++)
	for (int j = 0; j < hInfo.biWidth; j++)
	{
		//Get histogram 
		hist[Img[i * rwsize + j]]++;

		if ((i * rwsize + j != 0) || (i * rwsize + j != rwsize - 1) || (i * rwsize + j != hInfo.biSizeImage - rwsize) || (i * rwsize + j != hInfo.biSizeImage - 1))
		M1 += Img[i * rwsize + j];
	}
	
	M1 /= (hInfo.biSizeImage - 4); // 배경
	M2 = (float)(Img[0] + Img[rwsize - 1] + Img[hInfo.biSizeImage - rwsize] + Img[hInfo.biSizeImage - 1]) / 4; // 전경
	//printf("M1 = %f M2 = %f \n", M1, M2);
	T[1] = (int)((M1 + M2) / 2); // 128로 상관없다고 하심. 
	//printf("T : %d \n", T[flag]);


	//Get hissum
	for (int i = 0; i < 256; i++)
	{
		hissum[i] = hist[i];
		sum += hist[i];
	}

	for (int i = 1; i < 256; i++)
	{
	//	printf("hist[%d] = %d\n", i, hist[i]);
		hissum[i] += hissum[i - 1];
	}
	
	// ------------------------------
	while (T[flag-1] != T[flag]){
	
	//	printf("T[%d] = %d\n", flag, T[flag]);
		
		for (int i = 0; i < 256; i++)
		{
		if (i <= T[flag]){
			temp1 += hist[i] * i;
			//sum += hist[i];
		
		}
		else 
			temp2 += hist[i] * i;
			//printf("hist[%d] = %d\n", i, hist[i]);

			//printf("hist[%d] = %d\n", i, hist[i]);
		}

		M1 = (float)temp1 / hissum[T[flag]];
		M2 = (float)temp2 / (hInfo.biSizeImage - hissum[T[flag]]);
		
		//printf("temp1 : %d, temp2 : %d\n", temp1, temp2);
	//	printf("M1 = %f M2 = %f \n", M1, M2);
		
		temp1 = 0;
		temp2 = 0;
		//printf("sum = %d, hissum[T] = %d\n", sum, hissum[T]);
		//printf("M1 = %f M2 = %f \n", M1, M2);
		flag++;

		T[flag] = (int)(M1 + M2) / 2;

	}
	
	printf("Iterative threshold = %d\n", T[flag]);

	for (int i = 0; i < hInfo.biHeight; i++)
	for (int j = 0; j < hInfo.biWidth; j++)
	{
		//Get histogram 
		
		if (Img[i * rwsize + j] > T[flag])
			Img[i * rwsize + j] = 255;
			//printf("Img= %d \n", Img[i * rwsize + j]);
		else
			Img[i * rwsize + j] = 0;
	}

	sprintf(outfilename, "Iterative th %s", filename);
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
