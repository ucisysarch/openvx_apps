#include <VX/vx.h>
#include <VX/vxu.h>
#include <stdio.h>
#include <stdlib.h>

#define OCTAVE_NUM 5
#define OCTAVE_LAYERS 5
#define INPUT_IMAGE "mandrill.pgm"

#define MAX_KEYPOINTS_PER_THREE_DOGS 200

//valuables for custom convoution
static const vx_uint32 gaussian5x5scale = 256;
static const vx_uint32 gaussian7x7scale = 1024;
static const vx_uint32 gaussian9x9scale = 65536;
static const vx_uint32 gaussian11x11scale = 2097152;

//make convolution mask using pibonachi
static vx_int16 gaussian5x5[5][5] =
{
	{ 1, 4, 6, 4, 1 },
	{ 4, 16, 24, 16, 4 },
	{ 6, 24, 36, 24, 6 },
	{ 4, 16, 24, 16, 4 },
	{ 1, 4, 6, 4, 1 }
};

static const vx_int16 gaussian7x7[7][7] =
{
	{ 1, 4, 6, 10, 6, 4, 1 },
	{ 4, 16, 24, 40, 24, 16, 4 },
	{ 6, 24, 36, 60, 36, 24, 6 },
	{ 10, 40, 60, 100, 60, 40, 10 },
	{ 6, 24, 36, 60, 36, 24, 6 },
	{ 4, 16, 24, 40, 24, 16, 4 },
	{ 1, 4, 6, 10, 6, 4, 1 },
};

static const vx_int16 gaussian9x9[9][9] =
{
	{ 1, 8, 28, 56, 70, 56, 28, 8, 1 },
	{ 8, 64, 244, 488, 560, 448, 224, 64, 8 },
	{ 28, 244, 784, 1568, 1960, 1568, 784, 224, 28 },
	{ 56, 448, 1568, 3136, 3920, 3136, 1568, 448, 56 },
	{ 70, 560, 1960, 3920, 4900, 3920, 1960, 560, 70 },
	{ 56, 448, 1568, 3136, 3920, 3136, 1568, 448, 56 },
	{ 28, 224, 784, 1568, 1960, 1568, 784, 224, 28 },
	{ 8, 64, 224, 448, 560, 448, 224, 64, 8 },
	{ 1, 8, 28, 56, 70, 56, 28, 8, 1 }
};
static const vx_int16 gaussian11x11[11][11] =
{
	{ 1, 10, 45, 120, 210, 252, 210, 120, 45, 10, 1 },
	{ 10, 100, 450, 1200, 2100, 25200, 2100, 1200, 450, 100, 10 },
	{ 45, 450, 2025, 5400, 9450, 11340, 9450, 5400, 2025, 450, 45 },
	{ 120, 1200, 5400, 14400, 25200, 30240, 25200, 14400, 5400, 1200, 120 },
	{ 210, 2100, 9450, 25200, 44100, 52920, 44100, 25200, 9450, 2100, 210 },
	{ 252, 2520, 11340, 30240, 52920, 63504, 52920, 30240, 11340, 2520, 252 },
	{ 210, 2100, 9450, 25200, 44100, 52920, 44100, 25200, 9450, 2100, 210 },
	{ 120, 1200, 5400, 14400, 25200, 30240, 25200, 14400, 5400, 1200, 120 },
	{ 45, 450, 2025, 5400, 9450, 11340, 9450, 5400, 2025, 450, 45 },
	{ 10, 100, 450, 1200, 2100, 25200, 2100, 1200, 450, 100, 10 },
	{ 1, 10, 45, 120, 210, 252, 210, 120, 45, 10, 1 },
};

//initialize custom convolvution.
static vx_convolution vxCreateGaussianConvolution(vx_context context, vx_int16 num)
{
	vx_status status;
	vx_convolution conv = NULL;

	switch (num)
	{
	case 5:
		conv = vxCreateConvolution(context, 5, 5);
		vxAccessConvolutionCoefficients(conv, NULL);
		vxCommitConvolutionCoefficients(conv, (vx_int16*)gaussian5x5);
		status = vxSetConvolutionAttribute(conv, VX_CONVOLUTION_ATTRIBUTE_SCALE, (void *)&gaussian5x5scale, sizeof(vx_uint32));
		break;
	case 7:
		conv = vxCreateConvolution(context, 7, 7);
		vxAccessConvolutionCoefficients(conv, NULL);
		vxCommitConvolutionCoefficients(conv, (vx_int16*)gaussian7x7);
		status = vxSetConvolutionAttribute(conv, VX_CONVOLUTION_ATTRIBUTE_SCALE, (void *)&gaussian7x7scale, sizeof(vx_uint32));
		break;
	case 9:
		conv = vxCreateConvolution(context, 9, 9);
		vxAccessConvolutionCoefficients(conv, NULL);
		vxCommitConvolutionCoefficients(conv, (vx_int16*)gaussian9x9);
		status = vxSetConvolutionAttribute(conv, VX_CONVOLUTION_ATTRIBUTE_SCALE, (void *)&gaussian9x9scale, sizeof(vx_uint32));
		break;
	case 11:
		conv = vxCreateConvolution(context, 11, 11);
		vxAccessConvolutionCoefficients(conv, NULL);
		vxCommitConvolutionCoefficients(conv, (vx_int16*)gaussian11x11);
		status = vxSetConvolutionAttribute(conv, VX_CONVOLUTION_ATTRIBUTE_SCALE, (void *)&gaussian11x11scale, sizeof(vx_uint32));
		break;
	default:
		break;
	}

	if (status != VX_SUCCESS)
	{
		vxReleaseConvolution(&conv);
		return NULL;
	}
	return conv;
}

//Make 'imgname.png' from given vx_image.
void saveimage(char* imgname, vx_image* img)
{
	//write-only
	FILE* outf = fopen(imgname, "wb");

	//pgm file contains width, height value itself.
	vx_uint32 w, h;
	vxQueryImage((*img), VX_IMAGE_ATTRIBUTE_WIDTH, &w, sizeof(w));
	vxQueryImage((*img), VX_IMAGE_ATTRIBUTE_HEIGHT, &h, sizeof(h));
	printf("%s %d %d>>>>\n", imgname, w, h);
	
	//Set patch we are going to access from (0,0) to (width, height). This stands for entire image.
	vx_rectangle_t imrect;
	imrect.start_x = imrect.start_y = 0;
	imrect.end_x = w; imrect.end_y = h;
	vx_uint32 plane = 0;
	vx_imagepatch_addressing_t imaddr;
	void* imbaseptr = NULL;

	if (vxAccessImagePatch((*img), &imrect, plane, &imaddr, &imbaseptr, VX_READ_ONLY) != VX_SUCCESS)
		printf("access failed\n");


	//write width, height, 255(max value for per pixel) to PGM file.
	fprintf(outf, "P5\n%d %d\n255\n", w, h);

	//write each pixel value as BINARY on file.
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			vx_uint8 *ptr2 = (vx_uint8 *)vxFormatImagePatchAddress2d(imbaseptr, x, y, &imaddr);

			fprintf(outf, "%c", (*ptr2));
		}

	}

	vxCommitImagePatch((*img), &imrect, plane, &imaddr, imbaseptr);

	fclose(outf);
}


/* Entry point. */
int main(int argc, char* argv[])
{
	//pgm file contains width, height value itself.
	int width;
	int height;

	FILE* in = fopen(INPUT_IMAGE, "rb");

	// Declare pointer of vx_uint8 as amount of width*height.
	vx_uint8* bytes;

	//Read width and height
	printf("before scan!\n");
	fscanf(in, "%*[^\n]\n%d %d\n%*[^\n]\n", &width, &height);
	printf("after scan! W(%d) H(%d)\n", width, height);

	bytes = (vx_uint8*)malloc(sizeof(vx_uint8)*width*height);

	//Fill bytes with pixel values
	for (int y = 0; y<height; y++)
		for (int x = 0; x<width; x++)
			fscanf(in, "%c", &bytes[(y*width) + x]);

	

	//Image reference for loading external image onto vximage
	vx_imagepatch_addressing_t addrs[] = {
		{
			width,
			height,
			sizeof(vx_uint8),
			width * sizeof(vx_uint8),
			VX_SCALE_UNITY,
			VX_SCALE_UNITY,
			1,
			1
		}
	};

	//wrap bytes as void*
	void* data[] = { bytes };


	//Create context.
	vx_context context = vxCreateContext();

	//finaly external image are set into vx_image. Now we can handle this as vx_image.
	vx_image image = vxCreateImageFromHandle(
		context,
		VX_DF_IMAGE_U8,
		addrs,
		data,
		VX_IMPORT_TYPE_HOST);


	//convolution values for gaussian blur
	vx_convolution conv5x5 = vxCreateGaussianConvolution(context, 5);
	vx_convolution conv7x7 = vxCreateGaussianConvolution(context, 7);
	vx_convolution conv9x9 = vxCreateGaussianConvolution(context, 9);
	vx_convolution conv11x11 = vxCreateGaussianConvolution(context, 11);

	//Create graph for context declared above
	vx_graph graph = vxCreateGraph(context);

	//=========================================================================================================
	//========================================== CREATING VIRTUAL IMAGES ======================================
	//=========================================================================================================

	//X gradient and Y gradient and magnitude of original images. Must be signed 16 bits images
	vx_image x_grad = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_S16);
	vx_image y_grad = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_S16);
	vx_image mag = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_S16);
	//vx_image ori = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_U8);	//phase function's output should be u8 type.


	//for test gradient image! soon will be eliminated
	//vx_image x_grad_test = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_U8);
	//vx_image y_grad_test = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_U8);
	//vx_image mag_test = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_U8);
	

	//Gaussian Pyramid
	vx_image gau_pyra[OCTAVE_NUM*OCTAVE_LAYERS];
	for (int i = 0; i < (OCTAVE_LAYERS*OCTAVE_NUM); i++)
		gau_pyra[i] = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_U8);


	//DOG Pyramid. Subtraction will cause negative output value so that we should prepare signed bit image.
	vx_image DOG_pyra[OCTAVE_NUM*(OCTAVE_LAYERS - 1)];
	for (int i = 0; i < ((OCTAVE_LAYERS - 1)*OCTAVE_NUM); i++)
		DOG_pyra[i] = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_U8);


	//Create pyramid for basis of gaussian pyramid we're going to build. Only U8 is allowed.
	vx_pyramid pyra = vxCreateVirtualPyramid(graph, OCTAVE_NUM, VX_SCALE_PYRAMID_HALF, width, height, VX_DF_IMAGE_U8);


	//vx_scalar for converting depth (U8 -> S16)
	vx_int32 zero1 = 0;
	vx_int32 zero2 = 0;


	vx_scalar scalar1 = vxCreateScalar(context, VX_TYPE_INT32, (void *)&zero1);
	vx_scalar scalar2 = vxCreateScalar(context, VX_TYPE_INT32, (void *)&zero2);

	//this is for output of DOG pyramid images for test.
	/*
	vx_image onedog[OCTAVE_NUM*(OCTAVE_LAYERS - 1)];
	for (int i = 0; i < ((OCTAVE_LAYERS - 1)*OCTAVE_NUM); i++)
		onedog[i] = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_U8);
	*/


	//several vx_array has itself responsiblity for one node function
	//containing keypoints
	vx_array keypt_arr[OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2)];
	vx_array verified_keypt_arr[OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2)];
	vx_image keypoint_img[OCTAVE_NUM*(OCTAVE_LAYERS - 1 - 2)];
	//vx_array descrs[OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2)];

	for (int i = 0; i < (OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2)); i++)
	{
		keypt_arr[i] = vxCreateVirtualArray(graph, VX_TYPE_COORDINATES2D, (5 * MAX_KEYPOINTS_PER_THREE_DOGS));
		verified_keypt_arr[i] = vxCreateVirtualArray(graph, VX_TYPE_COORDINATES2D, MAX_KEYPOINTS_PER_THREE_DOGS);
		keypoint_img[i] = vxCreateVirtualImage(graph, width, height, VX_DF_IMAGE_U8);
		//descrs[i] = vxCreateVirtualArray(graph, VX_TYPE_FLOAT32, 50000);
	}





	//================================================================================================
	//========================================== CREATING NODES ======================================
	//================================================================================================


	if (vxSobel3x3Node(graph, image, x_grad, y_grad) == 0) printf("ERROR SOBEL NODE\n");
	else printf("GETTING GRADIENTS BY SOBEL COMPLETE!\n");

	if (vxMagnitudeNode(graph, x_grad, y_grad, mag) == 0) printf("ERROR MAGNITUDE NODE\n");
	else printf("GETTING MAGNITUDE FROM GRADIENTS COMPLETE!\n");

	// [!] phase function doesn't return radian value(0 ~ 2*PHI or 0 ~ 6.28). Rather they'll be mapped into u8 value(0 ~ 255)
	// < 0 ~ 6.28 => 0 ~ 255 >
	/*
	if (vxPhaseNode(graph, x_grad, y_grad, ori) == 0) printf("ERROR PHASE NODE\n");
	else printf("GETTING ORIENTATION FROM GRADIENTS COMPLETE!\n");
	*/

	if (vxGaussianPyramidNode(graph, image, pyra) == 0) printf("ERROR GAUSSIANPYRAMID\n");
	else printf("GAUSSIAN PYRAMID COMPLETE!\n");

	//===== Building Gaussian pyramid =====//
	//
	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		//ex)	gau_pyra[0] (1st layer of octave 0) : original image
		//		gau_pyra[5] (1st layer of octave 1) : half sized of gau_pyra[0]
		//		gau_pyra[10] (1st layer of octave 2) : half sized of gau_pyra[5]
		//		gau_pyra[15] (1st layer of octave 3) : half sized of gau_pyra[10]
		//		gau_pyra[20] (1st layer of octave 4) : half sized of gau_pyra[15]
		if ((gau_pyra[i*OCTAVE_LAYERS] = vxGetPyramidLevel(pyra, i)) == 0) printf("WRONG INDEXING\n");


		//next layer is blurred image from previous one.
		//ex)	gau_pyra[0] = Original x1 size image		--octave 0
		//		gau_pyra[1] = BLUR( gau_pyra[0] )
		//		gau_pyra[2] = BLUR( gau_pyra[1] )
		//		gau_pyra[3] = BLUR( gau_pyra[2] )
		//		gau_pyra[4] = BLUR( gau_pyra[3] )
		//
		//		gau_pyra[5] = Original x0.5 size image		--octave 1
		//		gau_pyra[6] = BLUR( gau_pyra[5] )	
		//		gau_pyra[7] = BLUR( gau_pyra[6] )
		//		gau_pyra[8] = BLUR( gau_pyra[7] )
		//		gau_pyra[9] = BLUR( gau_pyra[8] )
		//
		for (int j = 1; j < OCTAVE_LAYERS; j++)
		{
			vx_image tempimage = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_S16);

			if (j<2)
				vxConvolveNode(graph, gau_pyra[(i*OCTAVE_LAYERS) + (j - 1)], conv5x5, tempimage);
			else if (j<3)
				vxConvolveNode(graph, gau_pyra[(i*OCTAVE_LAYERS) + (j - 1)], conv7x7, tempimage);
			else
				vxConvolveNode(graph, gau_pyra[(i*OCTAVE_LAYERS) + (j - 1)], conv9x9, tempimage);

			vxConvertDepthNode(graph, tempimage, gau_pyra[(i*OCTAVE_LAYERS) + j], VX_CONVERT_POLICY_WRAP, scalar1);

			
		}

	}


	//===== SUBTRACT TWO GAUSSIAN IMAGES WITH ABSDIFF TO BUILD DOG PYRAMID =====//
	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 0; j < OCTAVE_LAYERS - 1; j++)
		{
			//DOG[i] = GAU[i] - GAU[i+1]

			if ((vxAbsDiffNode(graph, gau_pyra[(i*OCTAVE_LAYERS) + j], gau_pyra[(i*OCTAVE_LAYERS) + (j + 1)], 
				DOG_pyra[(i*(OCTAVE_LAYERS - 1)) + j])) == 0) {
				printf("subtraction failed\n");
				return 0;
			}
		}
	}



	//to print out gradient images, soon will be eliminated
	
	//vxConvertDepthNode(graph, x_grad, x_grad_test, VX_CONVERT_POLICY_WRAP, scalar1);
	//vxConvertDepthNode(graph, y_grad, y_grad_test, VX_CONVERT_POLICY_WRAP, scalar1);
	//vxConvertDepthNode(graph, mag, mag_test, VX_CONVERT_POLICY_WRAP, scalar1);
	


	//own module
	printf("befroe SIFTNODE\n");
	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 0; j < (OCTAVE_LAYERS - 1 - 2); j++)
		{
			printf("DOG [%d] [%d] [%d] (octave %d) => keypt_arr[%d]\n",
				(i*(OCTAVE_LAYERS - 1)) + j, (i*(OCTAVE_LAYERS - 1)) + j + 1, (i*(OCTAVE_LAYERS - 1)) + j + 2, i,
				(i*(OCTAVE_LAYERS - 1-2)) + j);

			
			//find keypoints from 3 DOG images
			if( (vxFindSiftKeypointNode(graph, DOG_pyra[(i*(OCTAVE_LAYERS - 1)) + j], DOG_pyra[(i*(OCTAVE_LAYERS - 1)) + j + 1], DOG_pyra[(i*(OCTAVE_LAYERS - 1)) + j + 2], i, 
				(vx_int32)(MAX_KEYPOINTS_PER_THREE_DOGS * 5), keypt_arr[(i*(OCTAVE_LAYERS - 1 - 2) + j)])) == 0)
				printf("FINDSIFTKEYPOINT NODE FAILED\n");
			
			
			//verify(reduce) keypoints from keypoints found in 3 DOG images
			if ((vxVerifyKeypointNode(graph, keypt_arr[(i*(OCTAVE_LAYERS - 1 - 2) + j)], mag, width, height, (vx_int32)MAX_KEYPOINTS_PER_THREE_DOGS, verified_keypt_arr[(i*(OCTAVE_LAYERS - 1 - 2) + j)], keypoint_img[(i*(OCTAVE_LAYERS - 1 - 2)) + j])) == 0)
				printf("VERIFYKEYPOINT NODE FAILED\n");

			
			//make descriptor from verified keypoints above
			/*
			if (vxCalcSiftGradientNode(graph, ori, mag, verified_keypt_arr[(i*(OCTAVE_LAYERS - 1 - 2) + j)], descrs[(i*(OCTAVE_LAYERS - 1 - 2) + j)]))
				printf("CALCSIFTGRADIENT NODE FAILED\n");
				*/

		}
	}


	

	printf("after SIFTNODE\n");

	// Running graph we created.
	vx_status final_status = vxVerifyGraph(graph);
	if (final_status == VX_SUCCESS)
	{
		printf("Graph got verified!\n");
		final_status = vxProcessGraph(graph);
	}

	printf("PROCESS GRAPH COMPLETE\n");




	//checking a keypoint array for debugging
	/*
	vx_size num_items;
	vx_size item_size;
	vxQueryArray(verified_keypt_arr[0], VX_ARRAY_ATTRIBUTE_NUMITEMS, &num_items, sizeof(num_items));
	vxQueryArray(verified_keypt_arr[0], VX_ARRAY_ATTRIBUTE_ITEMSIZE, &item_size, sizeof(item_size));
	printf("[ %d %d ]\n", num_items, item_size);
	*/
	//vxQueryArray(new_keypt_arr[0], VX_ARRAY_ATTRIBUTE_NUMITEMS, &num_items, sizeof(num_items));
	//vxQueryArray(new_keypt_arr[0], VX_ARRAY_ATTRIBUTE_ITEMSIZE, &item_size, sizeof(item_size));
	//printf("[ %d %d ]\n", num_items, item_size);


	//check own node
	/*
	

	vx_size i, stride = 0ul;
	void* base = 0;

	
	vx_size num_items;
	vx_size item_size;
	vxQueryArray(keypt_arr[7], VX_ARRAY_ATTRIBUTE_NUMITEMS, &num_items, sizeof(num_items));
	vxQueryArray(keypt_arr[7], VX_ARRAY_ATTRIBUTE_ITEMSIZE, &item_size, sizeof(item_size));
	printf("<%d %d>\n", num_items, item_size);
	printf("before access\n");
	vx_status st = vxAccessArrayRange(keypt_arr[7], (vx_size)0, (vx_size)num_items, &stride, (void**)&base, VX_READ_ONLY);
	printf("%d, VX_SUCCESS %d\n", (int)st, (int)VX_SUCCESS);
	printf("after access\n");
	for (i = 0; i < num_items; i++)
	{
		vx_coordinates2d_t* xp = &vxArrayItem(vx_coordinates2d_t, base, i, stride);
		printf("x : %d ", xp->x);
		printf("y : %d\n", xp->y);
	}
	printf("before commit\n");
	vxCommitArrayRange(keypt_arr[7], 0, num_items, base);
	printf("after commit\n");

	*/
	
	


	//=========saving images for checking purpose===========

	//saveimage("x_grad.pgm", &x_grad_test);
	//saveimage("y_grad.pgm", &y_grad_test);
	//saveimage("mag.pgm", &mag_test);

	
	//save Gaussian Pyramid images as pgm
	/*
	for (int i = 0; i < OCTAVE_NUM; i++)
		for (int j = 0; j < OCTAVE_LAYERS; j++)
		{
			char ingn[64] = { 0, };
			sprintf(ingn, "%d-%d.pgm", i, j);
			saveimage(ingn, &gau_pyra[(i*OCTAVE_LAYERS) + j]);
		}
	*/
	
	
	//save keypoint images as pgm
	
	/*
	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 0; j < OCTAVE_LAYERS - 1; j++) {
			char ingn[64] = { 0, };
			sprintf(ingn, "KEYP_%d-%d.pgm", i, j);
			saveimage(ingn, &keypoint_img[(i*(OCTAVE_LAYERS - 1)) + j]);
			//saveimage(ingn, &onedog[(i*(OCTAVE_LAYERS - 1)) + j]);
		}
	}
	*/
	
	//save DOG Pyramid images as pgm

	/*
	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 0; j < OCTAVE_LAYERS - 1; j++) {
			char ingn[64] = { 0, };
			sprintf(ingn, "DOG_%d-%d.pgm", i, j);
			saveimage(ingn, &DOG_pyra[(i*(OCTAVE_LAYERS - 1)) + j]);
			//saveimage(ingn, &onedog[(i*(OCTAVE_LAYERS - 1)) + j]);
		}
	}
	*/

	fclose(in);



	//release data strutures created
	vxReleaseScalar(&scalar1);
	vxReleaseScalar(&scalar2);

	for (int i = 0; i < OCTAVE_LAYERS*OCTAVE_NUM; i++)
		vxReleaseImage(&gau_pyra[i]);
	
	for (int i = 0; i < (OCTAVE_LAYERS - 1 - 2)*OCTAVE_NUM; i++)
	{
		vxReleaseArray(&keypt_arr[i]);
		vxReleaseImage(&keypoint_img[i]);
		vxReleaseArray(&verified_keypt_arr[i]);
		//vxReleaseArray(&descrs[i]);

		//vxReleaseArray(&new_keypt_arr[i]);
	}


	for (int i = 0; i < (OCTAVE_LAYERS - 1)*OCTAVE_NUM; i++)
	{
		vxReleaseImage(&DOG_pyra[i]);
		//vxReleaseImage(&keypoint_img[i]);
		//vxReleaseImage(&onedog[i]);
	}

	vxReleaseImage(&x_grad);
	vxReleaseImage(&y_grad);
	vxReleaseImage(&mag);
	//vxReleaseImage(&ori);
	//vxReleaseImage(&x_grad_test);
	//vxReleaseImage(&y_grad_test);
	//vxReleaseImage(&mag_test);

	vxReleasePyramid(&pyra);
	vxReleaseGraph(&graph);
	vxReleaseContext(&context);

	int num;
	scanf("%d", &num);
}