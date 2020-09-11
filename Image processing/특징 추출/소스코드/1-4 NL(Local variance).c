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
	char outfilename[30];

	if ((infile = fopen("1-3) NL Lena.bmp", "rb")) == NULL) {
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
	int rwsize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth);

	BYTE *Img = malloc(hInfo.biSizeImage);//for c
	fread(Img, sizeof(char), hInfo.biSizeImage, infile);
	fclose(infile);

	double *temp = malloc(hInfo.biSizeImage * sizeof(double)); // Local varience
	memset(temp, 0, hInfo.biSizeImage * sizeof(double));

	/*BYTE *NL = malloc(hInfo.biSizeImage);
	memset(NL, 0, hInfo.biSizeImage);*/

	double *LV = malloc(hInfo.biSizeImage * sizeof(double)); // Local varience
	memset(LV, 0, hInfo.biSizeImage * sizeof(double));

	//double *WV = malloc(hInfo.biSizeImage * sizeof(double)); // Whole varience
	//memset(WV, 0, hInfo.biSizeImage * sizeof(double));

	double Lm = 0; // Local mean
	double LVar = 0;
	double LVmean = 0;

	for (int i = 2; i < hInfo.biHeight - 3; i++)
	for (int j = 2; j < rwsize - 3; j++)
	{
		Lm = 0;
		LVar = 0;

		for (int k = 0; k < 5; k++)
		{
		for (int e = 0; e < 5; e++)
		{
			Lm += Img[(i + k - 2) * rwsize + j + e - 2];
		}
		}

		Lm /= 25; 
		//printf("Lm : %f\n", Lm);
		for (int k = 0; k < 5; k++){
			for (int e = 0; e < 5; e++)
			{
				LVar += ((Img[i * rwsize + j] - Lm)) * ((Img[i * rwsize + j] - Lm));
				//printf("(Img - Lm) : %f\n", (Img[i * rwsize + j] - Lm));
				//printf("(Img - Lm)^2 : %f\n", (Img[i * rwsize + j] - Lm) * (Img[i * rwsize + j] - Lm));
				//printf("LVar : %f\n", LVar);
			}
		}
	
		LV[i * rwsize + j] = (LVar / 25); // Local varience
		//printf("LV : %f\n", LV[i*rwsize + j]);
		/*if (Img[i * rwsize + j] < LV[i *rwsize + j])
			Img[i * rwsize + j] = 0;*/
		//else
			//Img[i * rwsize + j] = 255;
	
	}

	for (int i = 2; i < hInfo.biHeight - 3; i++)
	for (int j = 2; j < rwsize - 3; j++)
	{
		LVmean = 0;
		for (int k = 0; k < 5; k++)
		{
			for (int e = 0; e < 5; e++)
			{
				LVmean += LV[(i + k - 2) * rwsize + j + e - 2];
			}
		}
		LVmean /= 25;
		//printf("LV : %f\n", LV[i*rwsize + j]);
		//printf("LVmean = %f\n", LVmean);
		if (LV[i * rwsize + j] < LVmean * 1.2)  // LVmean에 1.2 가중치줌
			Img[i * rwsize + j] = 255; // Theshold보다 작으면 Edge 아닌걸로 
	}

	sprintf(outfilename, "1-4) NL LocalVariance.bmp", filename);
	FILE *outfile = fopen(outfilename, "wb");
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
	fwrite(Img, sizeof(char), hInfo.biSizeImage, outfile);
	fclose(outfile);

	free(Img);
	free(temp);
	free(LV);

}

void main()
{
	bmpBWrw();
}
