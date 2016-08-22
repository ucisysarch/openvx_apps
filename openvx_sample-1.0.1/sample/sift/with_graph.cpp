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


//record image's keypoints and descriptor
void recordImageStatus(vx_array* keypt_arr, vx_array* descr_arr)
{

	int na;
	scanf("%d", &na);

	printf("recording status of image..\n");

	vx_size num_items[(OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2))];
	vx_size key_num_items[(OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2))];

	FILE* recordFile;
	char recordFilename[64] = { 0, };
	sprintf(recordFilename, "record\\%s_status.txt", INPUT_IMAGE);
	if ((recordFile = fopen(recordFilename, "w")) == NULL)
	{
		printf("can't write to recordFile..\n");
		exit(1);
	}

	//every status on one file
	for (int k = 0; k < (OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2)); k++)
	{
		vxQueryArray(keypt_arr[k], VX_ARRAY_ATTRIBUTE_NUMITEMS, &num_items[k], sizeof(num_items[k]));
		vxQueryArray(descr_arr[k], VX_ARRAY_ATTRIBUTE_NUMITEMS, &key_num_items[k], sizeof(key_num_items[k]));

		printf("<keypoint array [%d]>\n", k);
		printf("found keypoints : %d\n", num_items[k]);
		printf("<descriptor array [%d]\n", k);
		printf("descriptor values : %d (%d * 128 == %d)\n\n", key_num_items[k], num_items[k], (128 * num_items[k]));
		fprintf(recordFile, "<keypoint array [%d]>\n", k);
		fprintf(recordFile, "found keypoints : %d\n", num_items[k]);
		fprintf(recordFile, "<descriptor array [%d]\n", k);
		fprintf(recordFile, "descriptor values : %d (%d * 128 == %d)\n\n", key_num_items[k], num_items[k], (128 * num_items[k]));
	}

	for (int k = 0; k < (OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2)); k++)
	{
		FILE* record_keypt = NULL;
		FILE* record_descr = NULL;

		char recordKeyptFilename[64] = { 0, };
		char recordDescrFilename[64] = { 0, };

		sprintf(recordKeyptFilename, "record\\%s_keypt%d.txt", INPUT_IMAGE, (k + 1));
		sprintf(recordDescrFilename, "record\\%s_descr%d.txt", INPUT_IMAGE, (k + 1));

		if ((record_keypt = fopen(recordKeyptFilename, "w")) == NULL)
			exit(1);
		if ((record_descr = fopen(recordDescrFilename, "w")) == NULL)
			exit(1);




		//keypoints
		fprintf(record_keypt, "%d\n", num_items[k]);
		if (num_items[k] > 0)
		{
			//access to keypoint array
			vx_size kpt_stride = 0ul;
			void* kpt_base = 0;
			vxAccessArrayRange(keypt_arr[k], (vx_size)0, (vx_size)num_items[k], &kpt_stride, (void**)&kpt_base, VX_READ_ONLY);
			vx_int32 kpt_x, kpt_y;

			//write keypoint elements
			for (int i = 0; i < num_items[k]; i++)
			{
				vx_coordinates2d_t* xp = &vxArrayItem(vx_coordinates2d_t, kpt_base, i, kpt_stride);
				kpt_x = xp->x; kpt_y = xp->y;
				fprintf(record_keypt, "%d %d\n", kpt_x, kpt_y);
			}

			vxCommitArrayRange(keypt_arr[k], 0, num_items[k], kpt_base);
		}

		//descriptor
		fprintf(record_descr, "%d\n", key_num_items[k]);
		if (key_num_items[k] > 0)
		{

			//access to descriptor array
			vx_size dsc_stride = 0ul;
			void* dsc_base = 0;
			vxAccessArrayRange(descr_arr[k], (vx_size)0, (vx_size)key_num_items[k], &dsc_stride, (void**)&dsc_base, VX_READ_ONLY);


			//write descriptor elements
			for (int i = 0; i < key_num_items[k]; i++)
			{
				if ((i != 0) && (i % 128 == 0)) fprintf(record_descr, "\n");

				vx_float32* xp = &vxArrayItem(vx_float32, dsc_base, i, dsc_stride);
				fprintf(record_descr, "%f ", (*xp));
			}

			vxCommitArrayRange(descr_arr[k], 0, key_num_items[k], dsc_base);
		}

		fclose(record_keypt);
		fclose(record_descr);
	}

	//access each keypoint array and descriptor array write every elements

	fclose(recordFile);
	printf("recording complete.\n");

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
	//printf("before scan!\n");
	fscanf(in, "%*[^\n]\n%d %d\n%*[^\n]\n", &width, &height);
	//printf("after scan! W(%d) H(%d)\n", width, height);

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
	vx_convolution conv1 = vxCreateGaussianConvolution(context, 1);
	vx_convolution conv2 = vxCreateGaussianConvolution(context, 2);
	vx_convolution conv3 = vxCreateGaussianConvolution(context, 3);
	vx_convolution conv4 = vxCreateGaussianConvolution(context, 4);

	//Create graph for context declared above
	vx_graph graph = vxCreateGraph(context);

	//=========================================================================================================
	//========================================== CREATING VIRTUAL IMAGES ======================================
	//=========================================================================================================

	//X gradient and Y gradient and magnitude of original images. Must be signed 16 bits images
	vx_image x_grad = vxCreateImage(context, width, height, VX_DF_IMAGE_S16);
	vx_image y_grad = vxCreateImage(context, width, height, VX_DF_IMAGE_S16);
	vx_image mag = vxCreateImage(context, width, height, VX_DF_IMAGE_S16);
	vx_image ori = vxCreateImage(context, width, height, VX_DF_IMAGE_U8);	//phase function's output should be u8 type.


	//for test gradient image! soon will be eliminated
	//vx_image x_grad_test = vxCreateImage(context, width, height, VX_DF_IMAGE_U8);
	//vx_image y_grad_test = vxCreateImage(context, width, height, VX_DF_IMAGE_U8);
	//vx_image mag_test = vxCreateImage(context, width, height, VX_DF_IMAGE_U8);
	//vx_image ori_test = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_U8);

	//Gaussian Pyramid
	vx_image gau_pyra[OCTAVE_NUM*OCTAVE_LAYERS];
	int nw = width;
	int nh = height;
	for (int i = 0; i < (OCTAVE_NUM); i++)
	{
		for (int j = 0; j < OCTAVE_LAYERS; j++)
			gau_pyra[i*OCTAVE_LAYERS + j] = vxCreateImage(context, nw, nh, VX_DF_IMAGE_U8);

		if (nw % 2 != 0) nw = (nw / 2) + 1;
		else nw /= 2;
		if (nh % 2 != 0) nh = (nh / 2) + 1;
		else nh /= 2;
	}


	//DOG Pyramid. Subtraction will cause negative output value so that we should prepare signed bit image.
	vx_image DOG_pyra[OCTAVE_NUM*(OCTAVE_LAYERS - 1)];
	nw = width; nh = height;
	for (int i = 0; i < (OCTAVE_NUM); i++)
	{
		for (int j = 0; j < OCTAVE_LAYERS - 1; j++)
			DOG_pyra[i*(OCTAVE_LAYERS - 1) + j] = vxCreateImage(context, nw, nh, VX_DF_IMAGE_U8);

		if (nw % 2 != 0) nw = (nw / 2) + 1;
		else nw /= 2;
		if (nh % 2 != 0) nh = (nh / 2) + 1;
		else nh /= 2;
	}


	//Create pyramid for basis of gaussian pyramid we're going to build. Only U8 is allowed.
	vx_pyramid pyra = vxCreateVirtualPyramid(graph, OCTAVE_NUM, VX_SCALE_PYRAMID_HALF, width, height, VX_DF_IMAGE_U8);

	//vx_scalar for converting depth (U8 -> S16)
	vx_int32 zero1 = 0;
	vx_int32 zero2 = 0;

	vx_scalar scalar1 = vxCreateScalar(context, VX_TYPE_INT32, (void *)&zero1);
	vx_scalar scalar2 = vxCreateScalar(context, VX_TYPE_INT32, (void *)&zero2);


	//several vx_array has itself responsiblity for one node function
	//containing keypoints
	vx_array keypt_arr[OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2)];
	//vx_array verified_keypt_arr[OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2)];
	//vx_image keypoint_img[OCTAVE_NUM*(OCTAVE_LAYERS - 1 - 2)];
	vx_array descrs[OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2)];

	for (int i = 0; i < (OCTAVE_NUM * (OCTAVE_LAYERS - 1 - 2)); i++)
	{
		keypt_arr[i] = vxCreateArray(context, VX_TYPE_COORDINATES2D, (MAX_KEYPOINTS_PER_THREE_DOGS));
		descrs[i] = vxCreateArray(context, VX_TYPE_FLOAT32, 30000);
	}





	//================================================================================================
	//========================================== CREATING NODES ======================================
	//================================================================================================


	if (vxSobel3x3Node(graph, image, x_grad, y_grad) == 0) printf("ERROR SOBEL NODE\n");

	if (vxMagnitudeNode(graph, x_grad, y_grad, mag) == 0) printf("ERROR MAGNITUDE NODE\n");

	// [!] phase function doesn't return radian value(0 ~ 2*PHI or 0 ~ 6.28). Rather they'll be mapped into u8 value(0 ~ 255)
	// < 0 ~ 6.28 => 0 ~ 255 >
	if (vxPhaseNode(graph, x_grad, y_grad, ori) == 0) printf("ERROR PHASE NODE\n");

	if (vxGaussianPyramidNode(graph, image, pyra) == 0) printf("ERROR GAUSSIANPYRAMID\n");

	//===== Building Gaussian pyramid =====//
	nw = width; nh = height;
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
			vx_image tempimage = vxCreateVirtualImage(graph, nw, nh, VX_DF_IMAGE_S16);

			if (j<2)
				vxConvolveNode(graph, gau_pyra[(i*OCTAVE_LAYERS) + (j - 1)], conv1, tempimage);
			else if (j<3)
				vxConvolveNode(graph, gau_pyra[(i*OCTAVE_LAYERS) + (j - 1)], conv2, tempimage);
			else if (j<4)
				vxConvolveNode(graph, gau_pyra[(i*OCTAVE_LAYERS) + (j - 1)], conv3, tempimage);
			else if (j<5)
				vxConvolveNode(graph, gau_pyra[(i*OCTAVE_LAYERS) + (j - 1)], conv4, tempimage);

			vxConvertDepthNode(graph, tempimage, gau_pyra[(i*OCTAVE_LAYERS) + j], VX_CONVERT_POLICY_WRAP, scalar1);
		}

		if (nw % 2 != 0) nw = (nw / 2) + 1;
		else nw /= 2;
		if (nh % 2 != 0) nh = (nh / 2) + 1;
		else nh /= 2;
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



	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 0; j < (OCTAVE_LAYERS - 1 - 2); j++)
		{
			//printf("DOG [%d] [%d] [%d] (octave %d) => keypt_arr[%d]\n",
			//(i*(OCTAVE_LAYERS - 1)) + j, (i*(OCTAVE_LAYERS - 1)) + j + 1, (i*(OCTAVE_LAYERS - 1)) + j + 2, i,
			//(i*(OCTAVE_LAYERS - 1-2)) + j);


			//find keypoints from 3 DOG images
			if ((vxFindSiftKeypointNode(graph, mag, DOG_pyra[(i*(OCTAVE_LAYERS - 1)) + j], DOG_pyra[(i*(OCTAVE_LAYERS - 1)) + j + 1], DOG_pyra[(i*(OCTAVE_LAYERS - 1)) + j + 2], i,
				(vx_int32)(MAX_KEYPOINTS_PER_THREE_DOGS), keypt_arr[(i*(OCTAVE_LAYERS - 1 - 2) + j)])) == 0)
				printf("FINDSIFTKEYPOINT NODE FAILED\n");

			//make descriptor from verified keypoints above
			if (vxCalcSiftGradientNode(graph, ori, mag, keypt_arr[(i*(OCTAVE_LAYERS - 1 - 2) + j)], descrs[(i*(OCTAVE_LAYERS - 1 - 2) + j)]) == 0)
				printf("CALCSIFTGRADIENT NODE FAILED\n");

		}
	}



	// Running graph we created.
	vx_status final_status = vxVerifyGraph(graph);
	if (final_status == VX_SUCCESS)
	{
		//printf("Graph got verified!\n");
		final_status = vxProcessGraph(graph);
	}

	//printf("PROCESS GRAPH COMPLETE\n");


	//=========saving images for checking purpose===========


	fclose(in);

	//release data strutures created
	vxReleaseScalar(&scalar1);
	vxReleaseScalar(&scalar2);

	for (int i = 0; i < OCTAVE_LAYERS*OCTAVE_NUM; i++)
		vxReleaseImage(&gau_pyra[i]);

	for (int i = 0; i < (OCTAVE_LAYERS - 1 - 2)*OCTAVE_NUM; i++)
	{
		vxReleaseArray(&keypt_arr[i]);
		vxReleaseArray(&descrs[i]);
	}


	for (int i = 0; i < (OCTAVE_LAYERS - 1)*OCTAVE_NUM; i++)
	{
		vxReleaseImage(&DOG_pyra[i]);
	}

	vxReleaseImage(&x_grad);
	vxReleaseImage(&y_grad);
	vxReleaseImage(&mag);

	vxReleasePyramid(&pyra);
	vxReleaseGraph(&graph);
	vxReleaseContext(&context);

	//scanf("%d", &num);
}
