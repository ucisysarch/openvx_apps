#include <VX/vx.h>
#include <VX/vxu.h>
#include <stdio.h>
#include <stdlib.h>

void put_array_to_vx_image(vx_image img, int* array, int w, int h)
{
	vx_rectangle_t imrect;
	imrect.start_x = imrect.start_y = 0;
	imrect.end_x = w; imrect.end_y = h;
	vx_uint32 plane = 0;
	vx_imagepatch_addressing_t imaddr;
	void* imbaseptr = NULL;

	if (vxAccessImagePatch(img, &imrect, plane, &imaddr, &imbaseptr, VX_READ_AND_WRITE) != VX_SUCCESS)
		printf("access failed\n");

	printf("\n");
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			vx_int16 *ptr2 = (vx_int16*)vxFormatImagePatchAddress2d(imbaseptr, x, y, &imaddr);

			(*ptr2) = array[y*w + x];
			printf("%d ", (*ptr2));
		}

		printf("\n");
	}

	vxCommitImagePatch(img, &imrect, plane, &imaddr, imbaseptr);

}

int main()
{
	vx_context context = vxCreateContext();
	vx_graph graph = vxCreateGraph(context);

	//========================TEST==========================
	printf("[test 1] begin \n");
	int w, h;
	FILE* file1_1 = NULL;
	FILE* file1_2 = NULL;
	FILE* file1_3 = NULL;
	file1_1 = fopen("test5_1.txt", "r");		//<======= put input .txt file here
	file1_2 = fopen("test5_2.txt", "r");
	file1_3 = fopen("test5_3.txt", "r");
	fscanf(file1_1, "%d %d", &w, &h);
	fscanf(file1_2, "%d %d", &w, &h);
	fscanf(file1_3, "%d %d", &w, &h);
	int* arr1_1 = new int[w*h];
	int* arr1_2 = new int[w*h];
	int* arr1_3 = new int[w*h];

	printf("[test 1] declaration complete \n");

	for (int i = 0; i < (w*h); i++)
	{
		fscanf(file1_1, "%d", &arr1_1[i]);
		fscanf(file1_2, "%d", &arr1_2[i]);
		fscanf(file1_3, "%d", &arr1_3[i]);
	}

	printf("[test 1] data to array memcpy complete \n");

	vx_image test1_1 = vxCreateImage(context, w, h, VX_DF_IMAGE_S16);
	vx_image test1_2 = vxCreateImage(context, w, h, VX_DF_IMAGE_S16);
	vx_image test1_3 = vxCreateImage(context, w, h, VX_DF_IMAGE_S16);


	put_array_to_vx_image(test1_1, arr1_1, w, h);
	put_array_to_vx_image(test1_2, arr1_2, w, h);
	put_array_to_vx_image(test1_3, arr1_3, w, h);

	vx_array testarr = vxCreateArray(context, VX_TYPE_COORDINATES2D, 1000);

	/*vx_status final_status = vxVerifyGraph(graph);
	if (final_status == VX_SUCCESS)
	{
		printf("Graph got verified!\n");
		final_status = vxProcessGraph(graph);
	}*/

	vxuFindSiftKeypoint(context, test1_1, test1_2, test1_3, 88, testarr);

	printf("PROCESS GRAPH COMPLETE\n");

	printf("[test 1] array to vx_image memcpy complete \n");

	delete[] arr1_1;
	delete[] arr1_2;
	delete[] arr1_3;
	fclose(file1_1);
	fclose(file1_2);
	fclose(file1_3);

	vxReleaseImage(&test1_1); vxReleaseImage(&test1_2); vxReleaseImage(&test1_2);

}