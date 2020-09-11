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
	char filename[20] = "Ctest.bmp";
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

	int *Ixsquare = malloc(hInfo.biSizeImage * sizeof(int));
	memset(Ixsquare, 0, hInfo.biSizeImage * sizeof(int));

	int *Iysquare = malloc(hInfo.biSizeImage * sizeof(int));
	memset(Iysquare, 0, hInfo.biSizeImage * sizeof(int));

	int *IxIysquare = malloc(hInfo.biSizeImage * sizeof(int));
	memset(IxIysquare, 0, hInfo.biSizeImage * sizeof(int));

	float *IxStemp = malloc(hInfo.biSizeImage * sizeof(float));
	memset(IxStemp, 0, hInfo.biSizeImage * sizeof(float));

	float *IyStemp = malloc(hInfo.biSizeImage * sizeof(float));
	memset(IyStemp, 0, hInfo.biSizeImage * sizeof(float));

	float *IxIyStemp = malloc(hInfo.biSizeImage * sizeof(float));
	memset(IxIyStemp, 0, hInfo.biSizeImage * sizeof(float));

	float *CRF = malloc(hInfo.biSizeImage * sizeof(float));
	memset(CRF, 0, hInfo.biSizeImage * sizeof(float));

	BYTE *outImg = malloc(hInfo.biSizeImage);
	memset(outImg, 0, hInfo.biSizeImage);

	int rwsize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth);


	int maskX[3][3] = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };
	int maskY[3][3] = { { -1, -2, -1 }, { 0, 0, 0, }, { 1, 2, 1 } };

	int Xsum = 0;
	int Ysum = 0;

	float Ix= 0;
	float Iy = 0;
	float IxIy = 0;

	//Sobel operator
	for (int i = 1; i < hInfo.biHeight - 2; i++){
		for (int j = 1; j < rwsize - 2; j++) {
			Xsum = 0;
			Ysum = 0;

			for (int k = 0; k < 3; k++){
				for (int e = 0; e < 3; e++)
				{
					//Xsum += Img[i + k][j + e] * maskX[k][e];
					//Ysum += Img[i + k][j + e] * maskY[k][e];

					Xsum += Img[(i + k - 1)*rwsize + (j + e - 1)] * maskX[k][e]; //   
					Ysum += Img[(i + k - 1)*rwsize + (j + e - 1)] * maskY[k][e];
				
				}
			}

			Ixsquare[i * rwsize + j] = Xsum * Xsum;
			Iysquare[i * rwsize + j] = Ysum * Ysum;
			IxIysquare[i * rwsize + j] = abs(Xsum) * abs(Ysum);
		
		
		}
	}
	
	// 5x5 AverageFilter
	for (int i = 2; i < hInfo.biHeight - 3; i++){
		for (int j = 2; j < rwsize - 3; j++){

			for (int k = 0; k < 5; k++){
				for (int e = 0; e < 5; e++){
					IxStemp[i * rwsize + j] += (float)Ixsquare[(i + k - 2) * rwsize + j + e - 2];
					IyStemp[i * rwsize + j] += (float)Iysquare[(i + k - 2) * rwsize + j + e - 2];
					IxIyStemp[i * rwsize + j] += (float)IxIysquare[(i + k - 2) * rwsize + j + e - 2];
				
				}
			}
			IxStemp[i * rwsize + j] /= 25.;
			IyStemp[i * rwsize + j] /= 25.;
			IxIyStemp[i * rwsize + j] /= 25.;
			//printf("IxStemp = %f \n", IxStemp[i * rwsize +  j]);
		}
	}

	//Harris Corner Response Function 
	
	for (int i = 2; i < hInfo.biHeight - 3; i++){
		for (int j = 2; j < rwsize - 3; j++){
			Ix = IxStemp[i * rwsize + j];
			Iy = IyStemp[i * rwsize + j];
			IxIy = IxIyStemp[i * rwsize + j];
			
			CRF[i * rwsize + j] = (Ix* Iy) - IxIy * IxIy - (float)(0.05*((Ix + Iy) * (Ix + Iy)));
		}
	}

	// CRF > 0.01 ÀÌ¸י Corner

		for(int i = 0; i < hInfo.biHeight; i++){
		for (int j = 0; j < rwsize; j++){
			if (CRF[i * rwsize + j] > 0.01)
				outImg[i * rwsize + j] = 255;
			else
				outImg[i * rwsize + j] = 0;
		}
	}




	sprintf(outfilename, "2) Harris corner %s", filename);
	FILE *outfile = fopen(outfilename, "wb");
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
	fwrite(outImg, sizeof(char), hInfo.biSizeImage, outfile);
	fclose(outfile);

	free(Img);
	free(outImg);
	free(Ixsquare);
	free(Iysquare);
	free(IxIysquare);
	free(IxStemp);
	free(IyStemp);
	free(IxIyStemp);
	free(CRF);
}

void main()
{
	bmpBWrw();
}
