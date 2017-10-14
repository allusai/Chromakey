/*********************************************************************
 File:     chromakey.cpp

 Author:   Sai Allu

 Email address: allu@usc.edu

 Usage: program_name in.bmp background.bmp dist_threshold out1.bmp out2.bmp

 Notes:
 This program performs the chroma key operation on an input 
 using two different methods.

 Method 1 Utilize a user-defined distance threshold from the
          chromakey value as a discriminator

 Method 2 Devise a method that to determine the chromakey mask
          that doesn't require a user-input threshold

********************************************************************/

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "bmplib.h"

using namespace std;

// Prototypes
// IMPORTANT: you must exactly use these input types, function names, and 
// return types. Otherwise the grader can't test them.
void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold);

void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE]);

void replace(bool mask[][SIZE],
	     unsigned char inImage[][SIZE][RGB],
	     unsigned char bgImage[][SIZE][RGB],
	     unsigned char outImage[][SIZE][RGB]);
double magn(double[] );

int main(int argc, char *argv[])
{
  // Image data array
  // Note:  DON'T use the static keyword except where we expressly say so.
  //        It puts the large array in a separate, fixed, area of memory. 
  //        It is bad practice. But useful until we have dynamic allocation.
  static unsigned char inputImage[SIZE][SIZE][RGB];
  static unsigned char bgrndImage[SIZE][SIZE][RGB];
  static unsigned char outputImage[SIZE][SIZE][RGB];
  static bool chromaMask[SIZE][SIZE];

  double threshold;

  if (argc < 6) {
    cerr << "usage: program_name in.bmp background.bmp dist_threshold " 
         << "out1.bmp out2.bmp" << endl;
    return 0;
  }
	
  if (readRGBBMP(argv[1], inputImage)) {
    cerr << "Error reading file: " << argv[1] << endl;
    return 1;
  }

  if (readRGBBMP(argv[2], bgrndImage)) {
    cout << "Error reading file: " << argv[2] << endl;
    return 1;
  }
  
  // Write code to convert the threshold (argv[3])
  //  from string format to a double and assign the 'threshold'
  threshold = atof(argv[3]); //super useful method for ascii to floating point
  cout << threshold << endl;

  // Call Method 1 Function
  //method1(inputImage, chromaMask, threshold);

  // Produce the output by calling replace()
  //replace(chromaMask,inputImage, bgrndImage, outputImage);

  // Write the output image to a file using the filename argv[4]
  //if (writeRGBBMP(argv[4], outputImage)) {
    //cout << "Error writing file: " << argv[4] << endl;
    //exit(1);
 // }	

  // Call Method 2 Function
    method2(inputImage, chromaMask);


  // Produce the output by calling replace()
      replace(chromaMask,inputImage, bgrndImage, outputImage);

  // Write the output image to a file using the filename argv[5]
  if (writeRGBBMP(argv[5], outputImage)) {
    cout << "Error writing file: " << argv[5] << endl;
    exit(1);
  }	

  return 0;
}



// Use user-provided threshold for chroma-key distance
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold)
{
	//Find the average chromakey color by using top 20x20 pixels
	double chromakey[] = {0,0,0}; //[R,G,B]
	int sums[] = {0,0,0}; //[Rt,Gt,Bt]

	for(int i = 0; i < 20; i++) //Go through each row
	{
		for(int j = 0; j < 20; j++) //Go through each column
		{
			//Add the color value of pixel into the sums array
			sums[0] += inImage[i][j][0];
			sums[1] += inImage[i][j][1];
			sums[2] += inImage[i][j][2];
			//Look at each color channel
		}
	}

	for(int k = 0; k < 3; k++)
	{
		chromakey[k] = sums[k] / 400.0; //Average the pixel colors
	}

	//cout << "Average red is: " << chromakey[0] << endl;
	//cout << "Average blue is: " << chromakey[1] << endl;
	//cout << "Average green is: " << chromakey[2] << endl;

	/* Now loop over each pixel in the inputImage, find its
	"distance" to the chromakey average pixel. If the distance
	is less than the threshold, set the corresponding mask pixel
	to 0. */

	//Initialize the mask
	for(int i = 0; i < SIZE; i++)
		for(int j = 0; j < SIZE; j++)
			mask[i][j] = 1;

	for(int i = 0; i < SIZE; i++)
	{
		for(int j = 0; j < SIZE; j++)
		{
			//The pixel you are looking at
			double pixel[] = {inImage[i][j][0],
				inImage[i][j][1],inImage[i][j][2]};

			//Find the distance
			double dist = sqrt( pow(pixel[0]-chromakey[0],2) +
			pow(pixel[1]-chromakey[1],2) + pow(pixel[2]-chromakey[2],2) );

			//If the distance is below the threshold, set the
			//corresponding mask pixel to 0
			if(dist < threshold)
				mask[i][j] = 0;
		}
	}

	/* Check if the mask was built properly 
	for(int i = 1; i < 2; i++)
	{
		for(int j = 0; j < SIZE; j++)
		{
			cout << mask[i][j] << " ";
		}
		cout << endl;
	} */



}

double magn(double pixel[])
{
	return sqrt( pow(pixel[0],2) + pow(pixel[1],2) + pow(pixel[2],2) );
}

// Devise a method to automatically come up with a threshold
//  for the chroma key determination
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE])  
{
	/*Observation: When the threshold was too low for the 
	method1 trials, there were random green globs in the 
	output image. If we assume that the pixels near the borders
	will be background, we can find the range of green values
	by finding the max and min green pixels. */

	//Determine the threshold by finding the biggest distance difference
	double thr;
	double minPixel[] = {255,255,255};
	double maxPixel[] = {0,0,0};
	double pixel[] = {0,0,0};

	/*Find the max and min pixels in the left 50 vertical lines
	of the image */
	for(int i = 0; i < SIZE; i++)
	{
		for(int j = 0; j < 50; j++)
		{
			pixel[0] = inImage[i][j][0]; 
			pixel[1] = inImage[i][j][1]; 
			pixel[2] = inImage[i][j][2]; 

			if(magn(pixel) > magn(maxPixel))
			{
				//Set maxPixel to that pixel
				for(int k = 0; k < 3; k++)
					maxPixel[k] = inImage[i][j][k];
			}

			if(magn(pixel) < magn(minPixel))
			{
				//Set minPixel to that pixel
				for(int k = 0; k < 3; k++)
					minPixel[k] = inImage[i][j][k];
			}

		}
	}

	//cout << maxPixel[0] << " " << maxPixel[1] << " " << maxPixel[2]; 

	thr = sqrt( pow(maxPixel[0]-minPixel[0],2) +
			pow(maxPixel[1]-minPixel[1],2) + pow(maxPixel[2]-minPixel[2],2) );

	method1(inImage, mask, thr);


}

// If mask[i][j] = 1 use the input image pixel for the output image
// Else if mask[i][j] = 0 use the background image pixel
void replace(bool mask[SIZE][SIZE],
	     unsigned char inImage[SIZE][SIZE][RGB],
	     unsigned char bgImage[SIZE][SIZE][RGB],
	     unsigned char outImage[SIZE][SIZE][RGB])
{
  // Create the output image using the mask to determine
  //  whether to use the pixel from the Input or Background image
	
	for(int i = 0; i < SIZE; i++)
	{
		for(int j = 0; j < SIZE; j++)
		{
			if(mask[i][j] == 1)
			{
				outImage[i][j][0] = inImage[i][j][0];
				outImage[i][j][1] = inImage[i][j][1];
				outImage[i][j][2] = inImage[i][j][2];
			}
			else
			{
				outImage[i][j][0] = bgImage[i][j][0];
				outImage[i][j][1] = bgImage[i][j][1];
				outImage[i][j][2] = bgImage[i][j][2];

			}
		}
	}


}
