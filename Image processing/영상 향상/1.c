#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

#define WIDTHBYTES(bits) (((bits)+31)/32*4)  // width를 4bytes의 배수로 만드는 수식 (32bit가 1덩어리 되게하기위해) 1.xx가 되는데 거기에 사이즈 곱해짐.
#define BYTE    unsigned char
#define H 880  
#define W 880
BYTE inImg[H][W];

void bmpBWrw()
{
	FILE *infile;
	char filename[20] = "chest.bmp";
	char outfilename[20];

	if ((infile = fopen(filename, "rb")) == NULL) {
		printf("No Image File");
		return;
	}

	// BMP Header Information
	BITMAPFILEHEADER hf;
	BITMAPINFOHEADER hInfo;
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, infile);//14 bytes
	if (hf.bfType != 0x4D42) exit(1);//"BM"
	/*
	printf("bfType : 0x%x\n",hf.bfType);
	printf("bfSize : %ldBytes\n", hf.bfSize);
	printf("bfReserved1 : %x\n", hf.bfReserved1);
	printf("bfReserved2 : %x\n", hf.bfReserved2);
	printf("bfOffBits : %ldBytes = 14 + 40 + %ld(Pallete size)\n\n", hf.bfOffBits, hf.bfOffBits-54);

	_getch();
	*/

	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, infile);//40 bytes
	if (hInfo.biBitCount != 8) { printf("Bad File Format!!"); return; }//BW

	printf("biSize : %ldBytes\n", hInfo.biSize);
	printf("Image Size : %ld x %ld -> %ld x %ld\n", hInfo.biWidth, hInfo.biHeight, WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth), hInfo.biHeight);
	printf("biPlanes : %d\n", hInfo.biPlanes);
	printf("biBitCount : %d\n", hInfo.biBitCount);
	printf("biCompression : %ld\n", hInfo.biCompression);
	printf("biSizeImage : %ldBytes\n", hInfo.biSizeImage);
	printf("biXPelsPerMeter : %ld\n", hInfo.biXPelsPerMeter);
	printf("biYPelsPerMeter : %ld\n", hInfo.biYPelsPerMeter);
	printf("biClrUsed : %ld\n", hInfo.biClrUsed);
	printf("biClrImportant : %ld\n\n", hInfo.biClrImportant);

	// BMP Pallete

	RGBQUAD hRGB[256];
	//printf("sizeofRGBQUAD : %d\n", sizeof(RGBQUAD));
	//_getch();
	fread(hRGB, sizeof(RGBQUAD), 256, infile);//read color pallete 

	//for (int i = 0; i < (int)pow(2, hInfo.biBitCount); i++) {
	//	printf("[%3d] B:%3d G:%3d R:%3d Res:%3d\n",i, hRGB[i].rgbBlue, hRGB[i].rgbGreen, hRGB[i].rgbRed, hRGB[i].rgbReserved);
	//_getch();
	//}

	fread(inImg, sizeof(char), hInfo.biSizeImage, infile);
	fclose(infile);

	//int rwsize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth); // 공백을 포함한 사이즈 (width로 넘어가면 안되므로)

	

	float cdf[256] = { 0, };
	int hist[256] = { 0, };


	for (int i = 0; i <hInfo.biHeight; i++)
	for (int j = 0; j < hInfo.biWidth; j++)
	{
		hist[inImg[i][j]]++;
	}

	//Call CDF
	for (int u = 0; u < 256; u++)
	{
		cdf[u] = hist[u] / (float)(H*W);
	}
	for (int u = 0; u < 256; u++)
	{
		if (u != 0)
			cdf[u] += cdf[u - 1];
		printf("cdf[%d] : %f\n", u, cdf[u]);
	}
	//process = Histogram
	for (int x = 0; x < hInfo.biHeight; x++)
	for (int y = 0; y < hInfo.biWidth; y++)
		inImg[x][y] = (int)(cdf[inImg[x][y]] * 255 + 0.5);
	// Image Output
	sprintf(outfilename, "1 %s", filename);
	FILE* outfile = fopen(outfilename, "wb");
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
	fwrite(inImg, sizeof(char), hInfo.biSizeImage, outfile);
	fclose(outfile);

}

void main()
{
	bmpBWrw();

}