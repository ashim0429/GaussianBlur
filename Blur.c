/*Ashim Baral
GaussianBlur*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lodepng.h"

unsigned char *outputImg;
unsigned char *img;
unsigned x, y;

//ccreating variable for multithreading
struct variables
{
	unsigned int start;
	unsigned int end;
};

//Multithreaded function is created to implement GaussBlur
void *Blur(void *var)
{
	int i, j;
	int n = 0;
	struct variables *value = (struct variables *)var;
	unsigned int Start = value->start;
	unsigned int endLimit = value->end;

	//Identifying pixel by going through the image.
	for (i = Start; i < endLimit; i += 4)
	{
		
		if ((i % (x * 4)) == 0)
		{

			//taking average for the TOP LEFT CORNER pixel of the image
			if (i == 0)
			{
				n = 4;
				for (j = 0; j < 3; j++)
				{
					*(outputImg + i + j) = (img[i + j] + img[i + 4 + j] + img[4 * x + j] + img[4 * x + 4 + j]) / n;
				}
				*(outputImg + i + 3) = img[i + 3];
			}

			//taking average for the BOTTOM LEFT CORNER pixel of the image
			else if (i == (x * 4 * (y - 1)))
			{
				n = 4;
				for (j = 0; j < 3; j++)
				{
					*(outputImg + i + j) = (img[i + j] + img[i + 4 + j] + img[i - (4 * x) + j] + img[i - (4 * x) + 4 + j]) / n;
				}
				*(outputImg + i + 3) = img[i + 3];
			}

			//taking average for the LEFT COLUMN pixels of the image 
			else
			{
				n = 6;
				for (j = 0; j < 3; j++)
				{
					*(outputImg + i + j) = (img[i + j] + img[i + 4 + j] + img[(4 * x) + i + j] + img[(4 * x) + i + 4 + j] + img[i - (4 * x) + j] + img[i - (4 * x) + 4 + j]) / n;
				}
				*(outputImg + i + 3) = img[i + 3];
			}
		}

		//right
		else if ((i % (x * 4)) == (x * 4) - 4)
		{

			//taking average for the TOP RIGHT CORNER pixel of the image
			if (i == ((x * 4) - 4))
			{
				n = 4;
				for (j = 0; j < 3; j++)
				{
					*(outputImg + i + j) = (img[i + j] + img[i - 4 + j] + img[(4 * x) + i + j] + img[(4 * x) + i - 4 + j]) / n;
				}
				*(outputImg + i + 3) = img[i + 3];
			}

			//taking average for the BOTTOM RIGHT CORNER pixel of the image
			else if (i == ((x * y * 4) - 4))
			{
				n = 4;
				for (j = 0; j < 3; j++)
				{
					*(outputImg + i + j) = (img[i + j] + img[i - 4 + j] + img[i - (4 * x) + j] + img[i - (4 * x) - 4 + j]) / n;
				}
				*(outputImg + i + 3) = img[i + 3];
			}

			//taking average for the RIGHT COLUMN pixels of the image
			else
			{
				n = 6;
				for (j = 0; j < 3; j++)
				{
					*(outputImg + i + j) = (img[i + j] + img[i - 4 + j] + img[(4 * x) + i + j] + img[(4 * x) + i - 4 + j] + img[i - (4 * x) + j] + img[i - (4 * x) - 4] + j) / n;
				}
				*(outputImg + i + 3) = img[i + 3];
			}
		}

		//taking average for the TOP ROW pixels of the image
		else if (i > 0 && i < ((x * 4) - 4))
		{
			n = 6;
			for (j = 0; j < 3; j++)
			{
				*(outputImg + i + j) = (img[i + j] + img[i - 4 + j] + img[i + 4 + j] + img[(4 * x) + i + j] + img[(4 * x) + i + 4 + j] + img[(4 * x) + i - 4 + j]) / n;
			}
			*(outputImg + i + 3) = img[i + 3];
		}

		//taking average for the BOTTOM ROW pixels of the image
		else if (i > (x * 4 * (y - 1)) && i < ((x * y * 4) - 4))
		{
			n = 6;
			for (j = 0; j < 3; j++)
			{
				*(outputImg + i + j) = (img[i + j] + img[i - 4 + j] + img[i + 4 + j] + img[i - (4 * x) + j] + img[i - (4 * x) + 4 + j] + img[i - (4 * x) - 4 + j]) / n;
			}
			*(outputImg + i + 3) = img[i + 3];
		}

		//taking average for Centre pixels of the image
		else
		{
			n = 9;
			for (j = 0; j < 3; j++)
			{
				*(outputImg + i + j) = (img[i + j] + img[i + 4 + j] + img[i - 4 + j] + img[(4 * x) + i + j] + img[(4 * x) + i + 4 + j] + img[(4 * x) + i - 4 + j] + img[i - (4 * x) + j] + img[i - (4 * x) + 4 + j] + img[i - (4 * x) - 4 + j]) / n;
			}
			*(outputImg + i + 3) = img[i + 3];
		}
	}
}

void main()
{
	int i, j;
	unsigned err;
	char filename[20];
	char optFile[20];
	//reading an image file
	printf("Enter input file name in .png: \n");
	scanf("%s", filename);

	//error msg if any error occurs in encoding the blurred image
	err = lodepng_decode32_file(&img, &x, &y, filename);
	if (err)
	{
		printf("Error decoding img %u : %s\n", err, lodepng_error_text(err));
	}
	else
	{
		printf("The height of img is: %d pixels and width of img is: %d pixels.\n\n", y, x);
	}

	printf("Enter output file name: (Example: picture2.png)\n");
	scanf("%s", filename);

	//creating the image file as output after done with blur.
	outputImg = (char *)malloc(x * y * 4 * sizeof(char));

	//Slicing for equal division of tasks among tye threads
	long iterations = x * y * 4;
	int threads;

	printf("Enter the number of threads: ");
	scanf("%d", &threads);

	//declaring slicedValue to slice the iterations to run on different threads
	int sliceList[threads];
	
	
	int remainder = iterations % threads;
	
	//for loop to add slices in the sliced values excluding the remainder, if any
	for (i = 0; i < threads; i++)
	{
		sliceList[i] = iterations / threads;
	}
	
	//for loop to add the remainder iterations to the slices evenly, if any
	for (j = 0; j < remainder; j++)
	{
		sliceList[j] += 1;
	}
	
	//arrays to store the start points and end list for each thread
	int startList[threads], endList[threads];

	int l;
	//for loop to allocate the start and end points for each thread
	for (l = 0; l < threads; l++)
	{
		if (l == 0)
			//start point for first thread
			startList[l] = 0;
		else
			//start points of other threads
			startList[l] = endList[l - 1] + 1;
		//end lists of threads
		endList[l] = startList[l] + sliceList[l] - 1;
	}
	//initializing the structure
	struct variables arr1[threads];

	int k;
	for (k = 0; k < threads; k++)
	{
		//putting the start points and end points in the structure to pass to the threads as parameter
		arr1[k].start = startList[k];
		arr1[k].end = endList[k];
	}

	pthread_t threadId[threads];

	int m;
	
	for (m = 0; m < threads; m++)
	{
		//creating threads for waiting to ensure the image gets blurred
		pthread_create(&threadId[m], NULL, Blur, &arr1[m]);
	}

	int nn;
	for (nn = 0; nn < threads; nn++)
	{
		//joining threads for waiting to ensure that all threads are completed
		pthread_join(threadId[nn], NULL);
	}

	unsigned char *png;
	size_t pngsize;
	//encoding the blurred image in a new file
	err = lodepng_encode32(&png, &pngsize, outputImg, x, y);

	if (!err)
	{
		//saving the blurred image file
		lodepng_save_file(png, pngsize, filename);
		printf("Encoding successful!");
	}

	//freeing the memory allocated by the outputImg array.
	free(outputImg);
	free(img);
}
