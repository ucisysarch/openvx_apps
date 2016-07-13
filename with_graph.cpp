#include <VX/vx.h>
#include <VX/vxu.h>
#include <stdio.h>
#include <stdlib.h>

#define OCTAVE_NUM 5
#define OCTAVE_LAYERS 5
#define INPUT_IMAGE "mandrill.pgm"

//img�� �����͸� ������, imgname�̶�� �̸����� pgm�� �����ϴ� ���
//Make 'imgname.png' from given vx_image.
void saveimage(char* imgname, vx_image* img)
{
	//write-only
	FILE* outf = fopen(imgname, "wb");
	//vx_uint8* bytes = (vx_uint8*)malloc(sizeof(vx_uint8)*w*h);

	//vx_image* img�� ����� �̹����� width, height�� ���´�.
	//pgm file contains width, height value itself.
	vx_uint32 w, h;
	vxQueryImage((*img), VX_IMAGE_ATTRIBUTE_WIDTH, &w, sizeof(w));
	vxQueryImage((*img), VX_IMAGE_ATTRIBUTE_HEIGHT, &h, sizeof(h));
	printf("%s %d %d>>>>\n", imgname, w, h);

	//������ ����. ���⼱ �̹��� ������ ������ ���̹Ƿ� (0, 0)���� (width, height)������ ������ �����Ѵ�.
	//Set patch we are going to access from (0,0) to (width, height). This stands for entire image.
	vx_rectangle_t imrect;
	imrect.start_x = imrect.start_y = 0;
	imrect.end_x = w; imrect.end_y = h;
	vx_uint32 plane = 0;
	vx_imagepatch_addressing_t imaddr;
	void* imbaseptr = NULL;

	if (vxAccessImagePatch((*img), &imrect, plane, &imaddr, &imbaseptr, VX_READ_AND_WRITE) != VX_SUCCESS)
		printf("access failed\n");


	//������ pgm ���� ���κп� �� �̹����� width�� height, value �ִ밪(����̹Ƿ� 255)�� �����Ѵ�.
	//write width, height, 255(max value for per pixel) to PGM file.
	fprintf(outf, "P5\n%d %d\n255\n", w, h);

	//�� �̹��� �ȼ��� ����, �׸��� ���Ͽ� �� �ȼ����� �����Ѵ�.
	//write each pixel value as BINARY on file.
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			vx_uint8 *ptr2 = (vx_uint8 *)vxFormatImagePatchAddress2d(imbaseptr, x, y, &imaddr);

			fprintf(outf, "%c", (*ptr2));
		}

	}

	fclose(outf);
}


/* Entry point. */
int main(int argc, char* argv[])
{
	//vx_image* img�� ����� �̹����� width, height�� ���´�.
	//pgm file contains width, height value itself.
	int width;
	int height;

	//���� ����
	FILE* in = fopen(INPUT_IMAGE, "rb");
	// uint8 ������ �����, ����*���θ�ŭ �Ҵ����Ͽ�.
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



	//���α׷� ���������� ������ context�� �����Ѵ�.
	//Create context.
	vx_context context = vxCreateContext();

	//�о���� ��ݻ input �̹����� ���������� �����Ͽ� vx_image�� �����.
	//finaly external image are set into vx_image. Now we can handle this as vx_image.
	vx_image image = vxCreateImageFromHandle(
		context,
		VX_DF_IMAGE_U8,
		addrs,
		data,
		VX_IMPORT_TYPE_HOST);

	//�� context�� ��̿�� graph�� �����
	//Create graph for context declared above
	vx_graph graph = vxCreateGraph(context);





	//=========================================================================================================
	//========================================== CREATING VIRTUAL IMAGES ======================================
	//=========================================================================================================


	//����þ� �Ƕ�̵�
	//Gaussian Pyramid
	vx_image gau_pyra[OCTAVE_NUM*OCTAVE_LAYERS];
	for (int i = 0; i < (OCTAVE_LAYERS*OCTAVE_NUM); i++)
		gau_pyra[i] = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_U8);

	//�� ����þ� �Ƕ�̵��� �̹����� ��� 16bit signed�� �ٲپ� ���� �̹����� �����Ѵ�
	//Temporary image to contain converted U8 Gaussian pyramid into as S16.
	//ex) DOG[a] = tem[a] - tem[b]
	vx_image tem1[OCTAVE_NUM*(OCTAVE_LAYERS - 1)];
	for (int i = 0; i < ((OCTAVE_LAYERS - 1)*OCTAVE_NUM); i++)
		tem1[i] = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_S16);

	vx_image tem2[OCTAVE_NUM*(OCTAVE_LAYERS - 1)];
	for (int i = 0; i < ((OCTAVE_LAYERS - 1)*OCTAVE_NUM); i++)
		tem2[i] = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_S16);


	//DOG �Ƕ�̵�. subtract�� �ϴٺ��� ������ ���� ��쵵 ������, SIGNED 16bit�� �̿��ϵ��� �Ѵ�.
	//DOG Pyramid. Subtraction will cause negative output value so that we should prepare signed bit image.
	vx_image DOG_pyra[OCTAVE_NUM*(OCTAVE_LAYERS - 1)];
	for (int i = 0; i < ((OCTAVE_LAYERS - 1)*OCTAVE_NUM); i++)
		DOG_pyra[i] = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_S16);


	// - OPENVX 1.0 ��μ� Unsigned 8bit�� ����
	//Create pyramid for basis of gaussian pyramid we're going to build. Only U8 is allowed.
	vx_pyramid pyra = vxCreateVirtualPyramid(graph, OCTAVE_NUM, VX_SCALE_PYRAMID_HALF, width, height, VX_DF_IMAGE_U8);


	//vx_scalar for converting depth (U8 -> S16)
	vx_int32 zero1 = 0;
	vx_int32 zero2 = 0;

	vx_scalar scalar1 = vxCreateScalar(context, VX_TYPE_INT32, (void *)&zero1);
	vx_scalar scalar2 = vxCreateScalar(context, VX_TYPE_INT32, (void *)&zero2);

	//this is for output of DOG pyramid images for test.
	vx_image onedog[OCTAVE_NUM*(OCTAVE_LAYERS - 1)];
	for (int i = 0; i < ((OCTAVE_LAYERS - 1)*OCTAVE_NUM); i++)
		onedog[i] = vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_U8);

	vx_array keypointarr = vxCreateVirtualArray(graph, VX_TYPE_COORDINATES2D, (vx_size)1000);


	//================================================================================================
	//========================================== CREATING NODES ======================================
	//================================================================================================

	if (vxGaussianPyramidNode(graph, image, pyra) == 0) printf("ERROR GAUSSIANPYRAMID\n");
	else printf("GAUSSIAN PYRAMID COMPLETE!\n");

	//===== Building Gaussian pyramid =====//
	//gau_pyra �迭�� �� �Ƕ�̵��� �������� vxGetPyramidLevel�� �̿��� �̹����� �ҷ��´�.
	//
	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		//�ϴ� gau_pyra[i*OCTAVE_LAYERS]���� �� ��Ÿ�긦 ��ǥ�ϴ� ù��° �̹����� ��ġ.(�������� �پ��� ���)
		//ex)	gau_pyra[0] (1st layer of octave 0) : original image
		//		gau_pyra[5] (1st layer of octave 1) : half sized of gau_pyra[0]
		//		gau_pyra[10] (1st layer of octave 2) : half sized of gau_pyra[5]
		//		gau_pyra[15] (1st layer of octave 3) : half sized of gau_pyra[10]
		//		gau_pyra[20] (1st layer of octave 4) : half sized of gau_pyra[15]
		if ((gau_pyra[i*OCTAVE_LAYERS] = vxGetPyramidLevel(pyra, i)) == 0) printf("WRONG INDEXING\n");

		//�� ��, ���� LAYER���� ���� �̹����� blur ����� ���̴�.
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
			vxGaussian3x3Node(graph, gau_pyra[(i*OCTAVE_LAYERS) + (j - 1)], gau_pyra[(i*OCTAVE_LAYERS) + j]);
		}

	}

	//===== DOG�� ���� tem ����� =====//
	//===== CONVERTING U8 GAUSSIAN PYRAMID IMAGES TO S16 FOR SUBTRACTION =====//
	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 0; j < OCTAVE_LAYERS - 1; j++)
		{
			if ((vxConvertDepthNode(graph, gau_pyra[(i*OCTAVE_LAYERS) + j], tem1[(i*(OCTAVE_LAYERS - 1)) + j], VX_CONVERT_POLICY_SATURATE, scalar1)) == 0) {
				printf("convert tem1 failed\n");
				return 0;
			}

			if ((vxConvertDepthNode(graph, gau_pyra[(i*OCTAVE_LAYERS) + (j + 1)], tem2[(i*(OCTAVE_LAYERS - 1)) + j], VX_CONVERT_POLICY_SATURATE, scalar2)) == 0) {
				printf("convert tem2 failed\n");
				return 0;
			}
		}
	}


	//===== DOG �Ƕ�̵� ����� =====//
	//===== GENERATING DOG PYRAMID USING SUBTRACTION =====//
	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 0; j < OCTAVE_LAYERS - 1; j++)
		{
			//DOG[i]�� GAU[i] - GAU[i+1]�̴�.

			if ((vxSubtractNode(graph, tem1[(i*(OCTAVE_LAYERS - 1)) + j], tem2[(i*(OCTAVE_LAYERS - 1)) + j],
				VX_CONVERT_POLICY_WRAP, DOG_pyra[(i*(OCTAVE_LAYERS - 1)) + j])) == 0) {
				printf("subtraction failed\n");
				return 0;
			}

		}
	}
	printf("DOG COMPLETE\n");

	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 0; j < OCTAVE_LAYERS - 1; j++) {
			if ((vxConvertDepthNode(graph, DOG_pyra[(i*(OCTAVE_LAYERS - 1)) + j], onedog[(i*(OCTAVE_LAYERS - 1)) + j],
				VX_CONVERT_POLICY_WRAP, scalar1)) == 0)
				printf("-_-;;");
		}
	}


	

	//own module
	printf("befroe SIFTNODE\n");
	vxFindSiftKeypointNode(graph, onedog[0], onedog[1], onedog[2], keypointarr);
	printf("after SIFTNODE\n");
	     
	//�׷����� ������ ��̿�ߴٸ�, ���� �׷����� PROCESS���Ѽ� �������Ѿ� �Ѵ�.
	//�̹����� �����ϰڴٸ� ���� �� ������ ��ģ ��, �̹��� ó���� �Ǹ� �׶� �����ϵ��� �Ѵ�.
	// Running graph we created.
	vx_status final_status = vxVerifyGraph(graph);
	if (final_status == VX_SUCCESS)
	{
		printf("Graph got verified!\n");
		final_status = vxProcessGraph(graph);
	}

	printf("PROCESS GRAPH COMPLETE\n");


	

	//own node�� �� ���ư��� üũ��


	vx_size i, stride = 0ul;
	void* base = 0;

	
	vx_size num_items;
	vx_size item_size;
	vxQueryArray(keypointarr, VX_ARRAY_ATTRIBUTE_NUMITEMS, &num_items, sizeof(num_items));
	vxQueryArray(keypointarr, VX_ARRAY_ATTRIBUTE_ITEMSIZE, &item_size, sizeof(item_size));
	printf("<%d %d>\n", num_items, item_size);



	printf("before access\n");
	vx_status st = vxAccessArrayRange(keypointarr, (vx_size)0, (vx_size)3, &stride, (void**)&base, VX_READ_ONLY);
	printf("%d, VX_SUCCESS %d\n", (int)st, (int)VX_SUCCESS);
	printf("after access\n");
	for (i = 0; i < 3; i++)
	{
		vx_coordinates2d_t* xp = &vxArrayItem(vx_coordinates2d_t, base, i, stride);
		printf("x : %d ", xp->x);
		printf("y : %d\n", xp->y);
	}
	printf("before commit\n");
	vxCommitArrayRange(keypointarr, 0, 1000, base);
	printf("after commit\n");

	////�̹��� ����
	//=========saving images for checking purpose===========

	/*

	//����þ� �Ƕ�̵� �̹��� ����
	//save Gaussian Pyramid images as pgm
	for (int i = 0; i < OCTAVE_NUM; i++)
		for (int j = 0; j < OCTAVE_LAYERS; j++)
		{
			char ingn[64] = { 0, };
			sprintf(ingn, "%d-%d.pgm", i, j);
			saveimage(ingn, &gau_pyra[(i*OCTAVE_LAYERS) + j]);
		}

	printf("GAUSSIAN �̹��� ����Ϸ�\n");

	//DOG �Ƕ�̵� �̹��� ����
	//save DOG Pyramid images as pgm


	for (int i = 0; i < OCTAVE_NUM; i++)
	{
		for (int j = 0; j < OCTAVE_LAYERS - 1; j++) {
			char ingn[64] = { 0, };
			sprintf(ingn, "DOG_%d-%d.pgm", i, j);
			vx_image* temar = (vx_image*)DOGREF;
			saveimage(ingn, &temar[(i*(OCTAVE_LAYERS - 1)) + j]);
			//saveimage(ingn, &onedog[(i*(OCTAVE_LAYERS - 1)) + j]);
		}
	}

	*/

	fclose(in);



	//��ȯ
	//release data strutures created


	vxReleaseScalar(&scalar1);
	vxReleaseScalar(&scalar2);

	for (int i = 0; i < OCTAVE_LAYERS*OCTAVE_NUM; i++)
		vxReleaseImage(&gau_pyra[i]);
	for (int i = 0; i < (OCTAVE_LAYERS - 1)*OCTAVE_NUM; i++)
	{
		vxReleaseImage(&tem1[i]);
		vxReleaseImage(&tem2[i]);
		vxReleaseImage(&DOG_pyra[i]);
		vxReleaseImage(&onedog[i]);
	}

	vxReleaseGraph(&graph);
	vxReleaseContext(&context);

	int num;
	scanf("%d", &num);
}