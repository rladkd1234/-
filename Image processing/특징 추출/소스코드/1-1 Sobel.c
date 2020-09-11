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
	char filename[20] = "Lena.bmp";
	char outfilename[20];

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

	float *temp = malloc(hInfo.biSizeImage * sizeof(float));
	memset(temp, 0, hInfo.biSizeImage * sizeof(float));

	int rwsize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth);
	

	int maskX[3][3] = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };
	int maskY[3][3] = { { -1, -2, -1 }, { 0, 0, 0, }, { 1, 2, 1 } };
	int hist[256] = {0,};

	int Xsum = 0;
	int Ysum = 0;
	int sum = 0;
	int Emax, Emin;
	//double temp = 0;

	Emax = 0;
	Emin = 10000;
		
	//Sobel operator
	for (int i = 1; i < hInfo.biHeight - 2; i++){
		for (int j = 1; j < rwsize - 2; j++) {

			/*	if (Img[(i * rwsize) + j] > Fmax)
					Fmax = Img[(i * rwsize) + j];

					if (Img[(i * rwsize + j)] < Fmin)
					Fmin = Img[(i * rwsize) + j];*/

			for (int k = 0; k < 3; k++){
				for (int e = 0; e < 3; e++)
				{
					//Xsum += Img[i + k][j + e] * maskX[k][e];
					//Ysum += Img[i + k][j + e] * maskY[k][e];

					Xsum += Img[(i + k - 1)*rwsize + (j + e - 1)] * maskX[k][e]; //   
					Ysum += Img[(i + k - 1)*rwsize + (j + e - 1)] * maskY[k][e];
				}
			}

			sum = (abs(Xsum) + abs(Ysum));

			Xsum = 0;
			Ysum = 0;

			temp[i * rwsize + j] = (float)sum;

			if (sum > Emax)
				Emax = sum;

			if (sum < Emin)
				Emin = sum;

			//outImg[i * rwsize + j] = sum;
		}

	}

		//Edge Decision for gradient operator
		for (int i = 1; i < hInfo.biHeight - 2; i++)
			for (int j = 1; j < rwsize - 2; j++) {
				//normalize and histogram
				temp[i * rwsize + j] = (float)((temp[i * rwsize + j] - Emin) / (Emax - Emin));
				outImg[i * rwsize + j] = (BYTE)(temp[i * rwsize + j] * 255. + 0.5);
				//get histogram
				hist[outImg[i*rwsize + j]]++;

			}

		//get threshold
		int T = 0;
		int hissum = 0;

		for (int i = 255; i >= 0; i--)
		{
			hissum += hist[i];
			if (hissum > (hInfo.biSizeImage * 0.2))
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
				if (outImg[i * rwsize + j] >= T)
					outImg[i*rwsize + j] = 0;
				else
					outImg[i*rwsize + j] = 255;
			}


	sprintf(outfilename, "1-1) Sobel %s", filename);
	FILE *outfile = fopen(outfilename, "wb");
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
	fwrite(outImg, sizeof(char), hInfo.biSizeImage, outfile);
	fclose(outfile);

	free(Img);
	free(outImg);
	free(temp);
}

void main()
{
	bmpBWrw();
}
