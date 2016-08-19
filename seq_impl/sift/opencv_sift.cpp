#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <cstdlib>

#define OCTAVE_NUM 5
#define OCTAVE_LAYERS 5


#define SIFT_IMG_BORDER 8

using namespace std;
using namespace cv;

void findMaximas(Mat& prev, Mat& curr, Mat& next, int o, std::vector<KeyPoint>& keypts);
void verifyKeypoints(Mat& mag, std::vector<KeyPoint>& keypts, std::vector<KeyPoint>& verified_keypts);


int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Usage: opencv_sift input_pgm_file\n");
        return 0;
    }
   
	// == read external image
	Mat origImg = imread(argv[1], 0);
	//Mat img_orientation;

	int img_w, img_h;
	img_w = origImg.cols;
	img_h = origImg.rows;

	Mat dx_grad;
	Mat dy_grad;
	Mat img_magnitude;

	// == calculate gradients magnitude
	Mat sobel_x = (Mat_<int>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
	Mat sobel_y = (Mat_<int>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	
	filter2D(origImg, dx_grad, dx_grad.depth(), sobel_x);
	filter2D(origImg, dy_grad, dy_grad.depth(), sobel_y);
	
	//printf("a pixel %d\n", dx_grad.at<uchar>(10, 10));
	dx_grad.convertTo(dx_grad, CV_32F);
	dy_grad.convertTo(dy_grad, CV_32F);
	//printf("a pixel %f\n", dx_grad.at<float>(10 ,10));

	/*imshow("dx", dx_grad);
	waitKey(0);
	imshow("dy", dy_grad);
	waitKey(0);*/

	//img_magnitude = Mat(img_h, img_w, CV_32FC1);
	magnitude(dx_grad, dy_grad, img_magnitude);

	img_magnitude.convertTo(img_magnitude, CV_32S);

	/*printf("CV_32F[%d], CV_64F[%d]\n", CV_32F, CV_64F);
	printf("ori depth[%d], channel[%d] type[%d]\n", origImg.depth(), origImg.channels(), origImg.type());
	printf("dx depth[%d], channel[%d] type[%d]\n", dx_grad.depth(), dx_grad.channels(), dx_grad.type());
	printf("dy depth[%d], channel[%d] type[%d]\n", dy_grad.depth(), dy_grad.channels(), dy_grad.type());
	printf("mag depth[%d], channel[%d] type[%d]\n", img_magnitude.depth(), img_magnitude.channels(), img_magnitude.type());
*/
	
	//magnitude(dx_grad, dy_grad, img_magnitude);
	// == make gaussian pyramid
	Mat gau_pyra[OCTAVE_NUM][OCTAVE_LAYERS];//pyramid

	//resize each image to set first image in each octave
	gau_pyra[0][0] = origImg;
	for (int i = 1; i < OCTAVE_NUM; i++)
	{
		int w = gau_pyra[i - 1][0].cols;
		int h = gau_pyra[i - 1][0].rows;

		resize(gau_pyra[i - 1][0], gau_pyra[i][0], Size(w / 2, h / 2));
	}

	Mat gaussianKernels[OCTAVE_LAYERS - 1];
	double sigmas[OCTAVE_LAYERS-1];
	sigmas[0] = 1.6;
	for (int i = 1; i < OCTAVE_LAYERS-1; i++)
		sigmas[i] = sigmas[i - 1] * pow(2.0, 1.3);


	for (int i = 0; i < OCTAVE_LAYERS-1; i++)
		gaussianKernels[i] = getGaussianKernel(5, sigmas[i]);

	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 1; j <= (OCTAVE_LAYERS-1); j++)
		{
			filter2D(gau_pyra[i][j - 1], gau_pyra[i][j], gau_pyra[i][j - 1].depth(), gaussianKernels[j-1]);
		}
	}

	// == make DOG pyramid and process abs
	Mat dog_pyra[OCTAVE_NUM][OCTAVE_LAYERS - 1];
	
	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 0; j < (OCTAVE_LAYERS - 1); j++)
		{
			absdiff(gau_pyra[i][j], gau_pyra[i][j + 1], dog_pyra[i][j]);
			dog_pyra[i][j].convertTo(dog_pyra[i][j], CV_8UC1);
		}
	}
	

	// == find maxima from 3 DOG imgs
	std::vector<KeyPoint> found_kpts;
	found_kpts.clear();
	std::vector<KeyPoint> verified_kpts;
	verified_kpts.clear();


	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 0; j < OCTAVE_LAYERS - 3; j++)
		{
			findMaximas(dog_pyra[i][j], dog_pyra[i][j + 1], dog_pyra[i][j + 2], i, found_kpts);
		}
	}
	printf("%d keypoints were detected\n", found_kpts.size());

	verifyKeypoints(img_magnitude, found_kpts, verified_kpts);
	printf("%d keypoints were verified\n", verified_kpts.size());

    cv::Mat outImage;
    drawKeypoints(origImg, verified_kpts, outImage);

    namedWindow( "Image with keypoints", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Image with keypoints", outImage );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}

void findMaximas(Mat& prev, Mat& curr, Mat& next, int o, std::vector<KeyPoint>& keypts)
{
	int w, h;
	w = curr.cols;
	h = curr.rows;

	//printf("w %d, h %d\n", w, h);

	for (int i = SIFT_IMG_BORDER; i < h - SIFT_IMG_BORDER; i++)
	{
		for (int j = SIFT_IMG_BORDER; j < w - SIFT_IMG_BORDER; j++)
		{
			//printf("i[%d]j[%d]\n", i, j);
			unsigned char curr_pixel = curr.at<uchar>(i, j);
			//printf("curr_pixel %d\n", curr_pixel);

			unsigned char curr_neighbors[8] =
			{
				curr.at<uchar>(i, j + 1), curr.at<uchar>(i + 1, j + 1), curr.at<uchar>(i + 1, j), curr.at<uchar>(i+1, j-1),
				curr.at<uchar>(i, j - 1), curr.at<uchar>(i - 1, j - 1), curr.at<uchar>(i - 1, j), curr.at<uchar>(i-1 , j+1)
			};
			unsigned char prev_neighbors[9] = 
			{
				prev.at<uchar>(i, j), prev.at<uchar>(i, j + 1), prev.at<uchar>(i + 1, j + 1), prev.at<uchar>(i + 1, j), prev.at<uchar>(i + 1, j - 1),
				prev.at<uchar>(i, j - 1), prev.at<uchar>(i - 1, j - 1), prev.at<uchar>(i - 1, j), prev.at<uchar>(i - 1, j + 1)
			};
			unsigned char next_neighbors[9] =
			{
				next.at<uchar>(i, j), next.at<uchar>(i, j + 1), next.at<uchar>(i + 1, j + 1), next.at<uchar>(i + 1, j), next.at<uchar>(i + 1, j - 1),
				next.at<uchar>(i, j - 1), next.at<uchar>(i - 1, j - 1), next.at<uchar>(i - 1, j), next.at<uchar>(i - 1, j + 1)
			};


			//if maxima
			if (curr_pixel >= curr_neighbors[0] && curr_pixel >= curr_neighbors[1] && curr_pixel >= curr_neighbors[2] &&
				curr_pixel >= curr_neighbors[3] && curr_pixel >= curr_neighbors[4] && curr_pixel >= curr_neighbors[5] &&
				curr_pixel >= curr_neighbors[6] && curr_pixel >= curr_neighbors[7] &&
				curr_pixel >= prev_neighbors[0] && curr_pixel >= prev_neighbors[1] && curr_pixel >= prev_neighbors[2] &&
				curr_pixel >= prev_neighbors[3] && curr_pixel >= prev_neighbors[4] && curr_pixel >= prev_neighbors[5] &&
				curr_pixel >= prev_neighbors[6] && curr_pixel >= prev_neighbors[7] && curr_pixel >= prev_neighbors[8] &&
				curr_pixel >= next_neighbors[0] && curr_pixel >= next_neighbors[1] && curr_pixel >= next_neighbors[2] &&
				curr_pixel >= next_neighbors[3] && curr_pixel >= next_neighbors[4] && curr_pixel >= next_neighbors[5] &&
				curr_pixel >= next_neighbors[6] && curr_pixel >= next_neighbors[7] && curr_pixel >= next_neighbors[8])
			{
				
				float dxx = (curr_neighbors[2]) + (curr_neighbors[6]) - 2 * curr_pixel;
				float dyy = (curr_neighbors[0]) + (curr_neighbors[4]) - 2 * curr_pixel;
				float dxy = ((curr_neighbors[1]) - (curr_neighbors[3]) - (curr_neighbors[5]) - (curr_neighbors[7])) / 4.0;

				float tr = dxx + dyy;
				float det = dxx * dyy - dxy * dxy;

				//is edge?
				if (det > 0 && tr * tr / det < (10 + 1.0)*(10 + 1.0) / 10)
				{
					KeyPoint keypt;
					keypt.pt.x = j; keypt.pt.y = i; keypt.octave = o;
					keypt.pt.x *= (1 << o);
					keypt.pt.y *= (1 << o);
					keypts.push_back(keypt);
				}

			}

		}
	}
}


void verifyKeypoints(Mat& mag, std::vector<KeyPoint>& keypts, std::vector<KeyPoint>& verified_keypts)
{
	for (int i = 0; i < keypts.size(); i++)
	{
		KeyPoint a_pt = keypts.at(i);
		int pt_x = a_pt.pt.x;
		int pt_y = a_pt.pt.y;

		if (mag.at<int>(pt_y, pt_x) >= 70 )
			verified_keypts.push_back(a_pt);
	}
}
