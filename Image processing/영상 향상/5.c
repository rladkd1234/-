#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

#define WIDTHBYTES(bits) (((bits)+31)/32*4)  // width를 4bytes의 배수로 만드는 수식 (32bit가 1덩어리 되게하기위해) 1.xx가 되는데 거기에 사이즈 곱해짐.
#define BYTE    unsigned char
#define W 512
#define H 512  
BYTE inImg[H][W];
BYTE outImg[H][W];
float temp[H][W];
float temp2[H][W];
float integralImg[H][W];


int median(BYTE N[], int nSize)
{
	int hold;
	int k;
	k = nSize / 2;   // 마스크의 중앙 위치 계산

	for (int loop = 1; loop<nSize; loop++) {
		for (int i = 0; i<nSize - 1; i++) {
			if (N[i] > N[i + 1]) {
				hold = N[i];
				N[i] = N[i + 1];
				N[i + 1] = hold;
			}
		}
	}

	return N[k];   // 중앙값을 반환
}
void bmpBWrw()
{
	FILE *infile;
	char filename[20] = "Pentagon.bmp";
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
	/*
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
	*/
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


	float window = (float)1 / 25;

	for (int i = 0; i < H; i++)
	for (int j = 0; j < W; j++)
	{
		temp2[i][j] = (float)inImg[i][j];
	}

	for (int i = 0; i < H; i++)
	for (int j = 0; j < W; j++){
		if (j == 0) integralImg[i][j] = inImg[i][j];
		else integralImg[i][j] = integralImg[i][j - 1] + inImg[i][j];
	}

	for (int j = 0; j < W; j++)
	for (int i = 1; i < H; i++)
		integralImg[i][j] += integralImg[i - 1][j];

	int totalsum = (int)integralImg[H - 1][W - 1];
	printf("totalsum = %d \n", totalsum);

	for (int i = 3; i < (H - 2); i++)
	for (int j = 3; j < (W - 2); j++)
		temp[i][j] = window * (integralImg[i + 2][j + 2] + integralImg[i - 3][j - 3] - integralImg[i + 2][j - 3] - integralImg[i - 3][j + 2]);

	printf("temp[][] = %f\n", temp[20][20]);

	for (int i = 3; i < H - 2; i++)
	for (int j = 3; j < W - 2; j++)
	{
		if ((temp2[i][j] + (float)(0.3 * (temp2[i][j] - temp[i][j])) >= 255))
			temp2[i][j] = 255.;

		else if ((temp2[i][j] + (float)(0.3 * (temp2[i][j] - temp[i][j])) <= 0))
			temp2[i][j] = 0.;
		
		else temp2[i][j] = temp2[i][j] + (float)(0.3 * (temp2[i][j] - temp[i][j]));
	}
	printf("temp2[][] = %f\n", (0.3 * (temp2[20][20] - temp[20][20])));

	for (int i = 0; i < H; i++)
	for (int j = 0; j < W; j++)
	{
		outImg[i][j] = (BYTE)(temp2[i][j] + 0.5);
	}


	/*
	for (int k = -10; k <= 10; k++)
	for (int l = -10; l <= 10; l++)
	printf("widnows[%d][%d] : %f\n", k+10, l+10,(windows[k+10][l+10]));
	*/


	/*
	for (int i = 10; i <= (H - 10); i++)
	for (int j = 10; j <= (W - 10); j++)
	for (int k = -10; k <= 10; k++)
	for (int l = -10; l <= 10; l++)
	temp[i][j] += (windows[k + 10][l + 10] * (float)inImg[i + k][j + l]);


	for (int i = 0; i < H; i++)
	for (int j = 0; j < W; j++)
	{
	outImg[i][j] = (BYTE)(temp[i][j] + 0.5);
	}
	*/
	//process = Histogram



	// Image Output
	sprintf(outfilename, "5 %s", filename);
	FILE* outfile = fopen(outfilename, "wb");
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
	fwrite(outImg, sizeof(char), hInfo.biSizeImage, outfile);
	fclose(outfile);

}

void main()
{
	bmpBWrw();
}