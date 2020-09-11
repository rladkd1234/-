#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#define BYTE    unsigned char

int labelhist[10000];

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

	int *label = malloc(hInfo.biSizeImage * sizeof(int));
	memset(label, 0, (hInfo.biSizeImage * sizeof(int)));

	int *sort = malloc(hInfo.biSizeImage * sizeof(int));
	memset(sort, 0, (hInfo.biSizeImage * sizeof(int)));


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
	}

	for (int i = 1; i < 256; i++)
	{
		//	printf("hist[%d] = %d\n", i, hist[i]);
		hissum[i] += hissum[i - 1];
	}

	// ------------------------------
	while (T[flag - 1] != T[flag]){

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

	int num = 1;
	int renum = 0;
	int count = 0;
	int histmax = 0;
	int histsec = 0;


	// 1.Labeling(관심화소에 숫자할당)

	for (int i = 0; i < hInfo.biHeight; i++)
	for (int j = 0; j < hInfo.biWidth; j++)
	{
		if (Img[i * rwsize + j] != 0) // 검은게 아니면
		{
			label[i * rwsize + j] = num++; // label 붙힘	
		}
	}

	//printf("num = %d\n", num);

	// 2. 2-Pass algorithm

	for (int pass = 0; pass < 5; pass++) //  2-pass를 한번만하면 전이가 잘 일어나지 않음.
	{
	// (1) Top -Down
	for (int i = 1; i < hInfo.biHeight - 1; i++)
	for (int j = 1; j < hInfo.biWidth - 1; j++)
	{
		renum = label[i * rwsize + j];

		if ((label[i * rwsize + j] != 0) && Img[i * rwsize + j] == Img[i * rwsize + (j + 1)]
			&& (label[i * rwsize + j] != label[i * rwsize + (j + 1)])) // (1)번
		{
			if (renum > label[i * rwsize + (j + 1)])
				renum = label[i * rwsize + (j + 1)];
			label[i * rwsize + j] = renum;
			label[i * rwsize + (j + 1)] = renum;
		}
		 if ((label[i * rwsize + j] != 0) && Img[i * rwsize + j] == Img[(i + 1) * rwsize + (j + 1)]
			&& (label[i * rwsize + j] != label[(i + 1) * rwsize + (j + 1)])) // (2)번
		{
			if (renum > label[(i + 1) * rwsize + (j + 1)])
				renum = label[(i + 1) * rwsize + (j + 1)];
			label[i * rwsize + j] = renum;
			label[(i + 1) * rwsize + (j + 1)] = renum;
		}
		 if ((label[i * rwsize + j] != 0) && Img[i * rwsize + j] == Img[(i + 1) * rwsize + j]
			&& (label[i * rwsize + j] != label[(i + 1) * rwsize + j])) // (3)번
		{
			if (renum > label[(i + 1) * rwsize + j])
				renum = label[(i + 1) * rwsize + j];
			label[i * rwsize + j] = renum;
			label[(i + 1) * rwsize + j] = renum;
		}
		 if ((label[i * rwsize + j] != 0) && Img[i * rwsize + j] == Img[(i + 1) * rwsize + (j - 1)]
			&& (label[i * rwsize + j] != label[(i + 1) * rwsize + (j - 1)])) // (4)번
		{
			if (renum > label[(i + 1) * rwsize + (j - 1)])
				renum = label[(i + 1) * rwsize + (j - 1)];
			label[i * rwsize + j] = renum;
			label[(i + 1) * rwsize + (j - 1)] = renum;
		}
	}

	//Bottom-up

	for (int i = hInfo.biHeight - 1; i > 1; i--)
	for (int j = 2; j <= hInfo.biHeight - 1; j++)
	{
		renum = label[i * rwsize - j];

		if ((label[i * rwsize - j] != 0) && Img[i * rwsize - j] == Img[i * rwsize - (j + 1)]
			&& (label[i * rwsize - j] != label[i * rwsize - (j + 1)])) // (5)번
		{
			if (renum > label[i * rwsize - (j + 1)])
				renum = label[i * rwsize - (j + 1)];
			label[i * rwsize - j] = renum;
			label[i * rwsize - (j + 1)] = renum;
		}
		 if ((label[i * rwsize - j] != 0) && Img[i * rwsize - j] == Img[(i - 1) * rwsize - (j + 1)]
			&& (label[i * rwsize - j] != label[(i - 1) * rwsize - (j + 1)])) // (6)번
		{
			if (renum > label[(i - 1) * rwsize - (j + 1)])
				renum = label[(i - 1) * rwsize - (j + 1)];
			label[i * rwsize - j] = renum;
			label[(i - 1) * rwsize - (j + 1)] = renum;
		}
		 if ((label[i * rwsize - j] != 0) && Img[i * rwsize - j] == Img[(i - 1) * rwsize - j]
			&& (label[i * rwsize - j] != label[(i - 1) * rwsize - j])) // (7)번
		{
			if (renum > label[(i - 1) * rwsize - j])
				renum = label[(i - 1) * rwsize - j];
			label[i * rwsize - j] = renum;
			label[(i - 1) * rwsize - j] = renum;
		}
		if ((label[i * rwsize - j] != 0) && Img[i * rwsize - j] == Img[(i - 1) * rwsize - (j - 1)]
			&& (label[i * rwsize - j] != label[(i - 1) * rwsize - (j - 1)])) // (8)번
		{
			if (renum > label[(i - 1) * rwsize - (j - 1)])
				renum = label[(i - 1) * rwsize - (j - 1)];
			label[i * rwsize - j] = renum;
			label[(i - 1) * rwsize - (j - 1)] = renum;
		}
		///	printf("Label[%d] = %d \n", i * rwsize + j, label[i * rwsize + j]);
	}
	
	}

	// 3. Renumbering

	//sort배열에 0이아닌 label(2-pass되어서 번져있는)들의 값들이 들어감
	for (int i = 0; i < hInfo.biHeight; i++)
	for (int j = 0; j < hInfo.biWidth; j++)
	{
		if (label[i * rwsize + j] != 0){
			sort[count] = label[i * rwsize + j];
			count++;
		}
	}

	printf("count = %d\n", count);
	
	// 선택 정렬
	int sorttemp = 0;
	int jnum = 0;
	for (int i = 0; i < count; i++)
	{
		jnum = i;
		while (sort[jnum] > sort[jnum + 1]){
			sorttemp = sort[jnum];
			sort[jnum] = sort[jnum + 1];
			sort[jnum + 1] = sorttemp;
			jnum--;
		}
	}

	// sort에 정렬 + 중복되는 수 제거후 값만
	for (int i = 0; i < count; i++)
	{
		for (int j = i + 1; j < count; j++)
		{
			if (sort[i] == sort[j]){
				for (int k = j; k < count; k++){
					sort[k] = sort[k + 1];
				}
				count--; j--;
			}
		}
	}

	//for (int i = 0; i < count; i++)
	//	printf("sort[%d]= %d\n", i ,sort[i]);

	// 라벨 수를 순서대로 표기
	for (int i = 0; i < hInfo.biHeight; i++)
	for (int j = 0; j < hInfo.biWidth; j++)
	{
		for (int k = 0; k <= count; k++)
		{
			if (label[i * rwsize + j] == sort[k])
				label[i * rwsize + j] = k;
		}
	}

	//라벨의 히스토그램
	for (int i = 0; i < hInfo.biHeight; i++)
	for (int j = 0; j < hInfo.biWidth; j++)
	{
		if (label[i * rwsize + j] != 0)
			labelhist[label[i * rwsize + j]]++;
	}

	//-------------------------------------------

	/*for (int i = 0; i <= count; i++)
	{
		printf("labelhist[%d] = %d\n", i, labelhist[i]);
	}*/

	//-------------------------------------------

	 // 영역중 제일 큰 값
		for (int i = 0; i <= count; i++)
		{
			if (histmax < labelhist[i])
				histmax = labelhist[i];
		}

		// 영역중 두번째 큰 값
		for (int i = 0; i <= count; i++)
		{
			if (histsec < labelhist[i] && labelhist[i] != histmax)
				histsec = labelhist[i];
		}

		printf("histmax = %d \n", histmax);
		printf("histsec = %d \n", histsec);


		// 가장 큰 2개의 영역을 제외하고는 없앰
		for (int i = 0; i < hInfo.biHeight; i++)
		for (int j = 0; j < hInfo.biWidth; j++)
		{
			if (labelhist[label[i * rwsize + j]] >= histsec)
				Img[i * rwsize + j] = 255;
			else
				Img[i * rwsize + j] = 0;
		}


	sprintf(outfilename, "Select %s", filename);
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
