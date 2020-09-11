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
	//BYTE *Img = new BYTE[hInfo.biSizeImage];//for c++
	int rwsize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth);

	BYTE *Img = malloc(hInfo.biSizeImage);//for c
	fread(Img, sizeof(char), hInfo.biSizeImage, infile);
	fclose(infile);

	BYTE *NL = malloc(hInfo.biSizeImage);
	memset(NL, 0, hInfo.biSizeImage);


	int *temp = malloc(hInfo.biSizeImage * sizeof(int));
	memset(temp, 0, hInfo.biSizeImage * sizeof(int));

	int max = 0;
	int min = 0;

	// Nonlinear Laplacian
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


		//Zero-crossing detection

	for (int i = 2; i < hInfo.biHeight - 3; i++)
		for (int j = 2; j < rwsize - 3; j++)
		{
			if (((temp[i*rwsize + j] * temp[i * rwsize + (j + 1)]) < 0) || ((temp[i*rwsize + j] * temp[(i + 1) * rwsize + j] < 0)))
				NL[i*rwsize + j] = 0;
			else
				NL[i*rwsize + j] = 255;
		}

	sprintf(outfilename, "1-3) NL %s", filename);
	FILE *outfile = fopen(outfilename, "wb");
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
	fwrite(NL, sizeof(char), hInfo.biSizeImage, outfile);
	fclose(outfile);

	free(Img);
	free(NL);
	free(temp);

}

void main()
{
	bmpBWrw();
}
