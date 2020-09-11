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

	//getch();
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

	BYTE *outImg = malloc(hInfo.biSizeImage);//for c
	memset(outImg, 255, hInfo.biSizeImage);

	int *temp = malloc(hInfo.biSizeImage * sizeof(int)); // Local varience
	memset(temp, 0, hInfo.biSizeImage * sizeof(int));

	double *LV = malloc(hInfo.biSizeImage * sizeof(double)); // Local varience
	memset(LV, 0, hInfo.biSizeImage * sizeof(double));

	double Lm = 0; // Local mean
	double LVM = 0;

	// Get Img 5x5 Local Variance
	for (int i = 2; i < hInfo.biHeight - 3; i++)
	for (int j = 2; j < rwsize - 3; j++)
	{
		Lm = 0;

		for (int k = 0; k < 5; k++)
		{
			for (int e = 0; e < 5; e++)
			{
				Lm += Img[(i + k - 2) * rwsize + j + e - 2];
			}
		}

		Lm /= 25;
		//	printf("OgLm : %f\n", OgLm);
		for (int k = 0; k < 5; k++){
			for (int e = 0; e < 5; e++)
			{
				LV[i * rwsize + j] += ((Img[(i + k - 2)* rwsize + j + e - 2] - Lm)) * ((Img[(i + k - 2) * rwsize + j + e - 2] - Lm));
				//printf("(Img - Lm) : %f\n", (Img[i * rwsize + j] - Lm));
				//printf("(Img - Lm)^2 : %f\n", (Img[i * rwsize + j] - Lm) * (Img[i * rwsize + j] - Lm));
				//printf("OgLVar : %f\n", OgLVar);
			}
		}

		LV[i * rwsize + j]/= 25; // Local varience

	//	printf("OgLV : %f\n", OgLV[i*rwsize + j]);

	}

	for (int i = 2; i < hInfo.biHeight - 3; i++)
	for (int j = 2; j < rwsize - 3; j++)
	{
		LVM+= LV[(i - 2) * rwsize + j - 2];
	}
		//printf("ogLVmean = %f \n", OgLVMean[i * rwsize + j]);
	
	//Threshold가 될 Mean of Local Variance
	LVM /= (hInfo.biHeight - 5 + 1) * (rwsize - 5 + 1);
	printf("LVmean = %f \n", LVM);
	
	// Nonlinear Laplacian
	int max = 0;
	int min = 0;

	for (int i = 2; i < hInfo.biHeight - 3; i++)
	for (int j = 2; j < rwsize - 3; j++)
	{
		max = Img[(i - 2) * rwsize + j - 2];
		min = Img[(i - 2) * rwsize + j - 2];
		for (int k = 0; k < 5; k++)
		for (int e = 0; e < 5; e++){
			if (Img[(i + k - 2) * rwsize + j + e - 2] >= max)
				max = Img[(i + k - 2) * rwsize + j + e - 2];
			if (Img[(i + k - 2) * rwsize + j + e - 2] < min)
				min = Img[(i + k - 2) * rwsize + j + e - 2];
		}
		temp[i*rwsize + j] = max + min - Img[i*rwsize + j] - Img[i*rwsize + j];
	}

	//Zero-crossing
	for (int i = 2; i < hInfo.biHeight - 3; i++)
	for (int j = 2; j < rwsize - 3; j++)
	{
		
		if (((temp[i * rwsize + j] * temp[i * rwsize + (j + 1)]) <  0) || ((temp[i * rwsize + j] * temp[(i + 1) * rwsize + j]) < 0))
			//printf("OgLV = %f  // OgLVMean = %f \n ", OgLV[i*rwsize + j], OgLVMean[i*rwsize + j]);
			outImg[i*rwsize + j] = 0; // Edge
		else
			outImg[i*rwsize + j] = 255; // Non Edge
	}	

	//Threshold
	for (int i = 2; i < hInfo.biHeight - 3; i++)
	for (int j = 2; j < rwsize - 3; j++)
	{
		if (outImg[i * rwsize + j] == 0) // 현재 Zero-Crossing 된 Edge
		{
			if (LV[i * rwsize + j] < LVM)
				outImg[i * rwsize + j] = 255;
		}
	}


	//------

	sprintf(outfilename, "1-4) 수정 %s", filename);
	FILE *outfile = fopen(outfilename, "wb");
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
	fwrite(outImg, sizeof(char), hInfo.biSizeImage, outfile);
	fclose(outfile);

	free(Img);
	free(temp);
	free(LV);

}

void main()
{
	bmpBWrw();
}
