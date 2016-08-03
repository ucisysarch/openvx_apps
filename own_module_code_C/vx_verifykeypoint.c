/*
 * Copyright (c) 2012-2014 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */


#include <VX/vx.h>
#include <VX/vxu.h>
#include <VX/vx_helper.h>
#include <VX/vx_lib_extras.h>
#include <VX/vx_lib_debug.h>
#include <vx_internal.h>

//#include <stdlib.h>
//#include <stdio.h>

static vx_status VX_CALLBACK vxVerifyKeypointKernel(vx_node node, vx_reference *parameters, vx_uint32 num)
{
    if (num == 7)
    {
		//parameters
		vx_array before_arr = (vx_array)parameters[0];
		vx_image mag = (vx_image)parameters[1];
		vx_scalar img_width = (vx_scalar)parameters[2];
		vx_scalar img_height = (vx_scalar)parameters[3];
		vx_scalar maximum = (vx_scalar)parameters[4];
		vx_array after_arr = (vx_array)parameters[5];
		vx_image output_image = (vx_image)parameters[6];


		//maximum keypoints can be detected for single call of this node amount to 200
		int keyptCnt = 0;

		//FILE* fff = NULL;
		//fff = fopen("www2.txt", "a+");

		//magnitude threshold
		int MAGNITUDE_THRESHOLD = 70;

		//maximum keypoints
		vx_int32 MAXIMUM_KEYPOINTS = 0;

		//for access to array
		vx_size i, j, stride = 0ul;
		void* base = 0;
		vx_coordinates2d_t foundKey;

		//for access to magnitude image
		vx_rectangle_t imrect;
		vx_uint32 plane = 0;
		vx_imagepatch_addressing_t imaddr;
		void* imbaseptr = NULL;

		//for writing to output vx image
		vx_rectangle_t output_imrect;
		vx_uint32 output_plane = 0;
		vx_imagepatch_addressing_t output_imaddr;
		void* output_imbaseptr = NULL;

		//array data
		vx_size num_items;
		vx_status st;

		vx_uint32 w, h;	//images' width&height

		//get width and height from image
		vxQueryImage(mag, VX_IMAGE_ATTRIBUTE_WIDTH, &w, sizeof(w));
		vxQueryImage(mag, VX_IMAGE_ATTRIBUTE_HEIGHT, &h, sizeof(h));

		//get length of array and permission to access array
		vxQueryArray(before_arr, VX_ARRAY_ATTRIBUTE_NUMITEMS, &num_items, sizeof(num_items));

		st = vxAccessArrayRange(before_arr, (vx_size)0, (vx_size)num_items, &stride, (void**)&base, VX_READ_ONLY);
		if (st != VX_SUCCESS) return VX_SUCCESS;

		//setting patch as entire size of image and get permission to access image
		imrect.start_x = imrect.start_y = 0;
		imrect.end_x = w; imrect.end_y = h;
		st = vxAccessImagePatch(mag, &imrect, plane, &imaddr, &imbaseptr, VX_READ_ONLY);
		//if (st != VX_SUCCESS) fprintf(fff, "ACCESS IMAGE FAILED [1]\n");

		//setting patch for output vx_image for keypoint.
		output_imrect.start_x = output_imrect.start_y = 0;
		output_imrect.end_x = w; output_imrect.end_y = h;
		st = vxAccessImagePatch(output_image, &output_imrect, output_plane, &output_imaddr, &output_imbaseptr, VX_READ_AND_WRITE);
		//if (st != VX_SUCCESS) fprintf(fff, "ACCESS IMAGE FAILED [2]\n");

		vx_int32 wwval, hhval;
		vxAccessScalarValue(img_width, &wwval);
		vxAccessScalarValue(img_height, &hhval);

		vxAccessScalarValue(maximum, &MAXIMUM_KEYPOINTS);

		//fprintf(fff, "w %d, h %d, max %d\n", wwval, hhval, (int)MAXIMUM_KEYPOINTS);
		
		//writing keypoint to image
		/*
		for (i = 0; i < hhval; i++)
		{
			for (j = 0; j < wwval; j++)
			{
				vx_uint8* outputpixel = (vx_uint8*)vxFormatImagePatchAddress2d(output_imbaseptr, j, i, &output_imaddr);
				(*outputpixel) = (vx_uint8)0;
			}
		}
		*/
		


		for (i = 0; i < num_items; i++)
		{
			vx_coordinates2d_t* xp = &vxArrayItem(vx_coordinates2d_t, base, i, stride);

			vx_int16* currpixel = (vx_int16 *)vxFormatImagePatchAddress2d(imbaseptr, xp->x, xp->y, &imaddr);
			
			if ((*currpixel) >= MAGNITUDE_THRESHOLD)
			{

				

				//fprintf(fff, "%d %d\n", xp->x, xp->y);

				//add to after verification array
				foundKey.x = (vx_uint32)xp->x;
				foundKey.y = (vx_uint32)xp->y;
				vxAddArrayItems(after_arr, 1, &foundKey, 0);

				vx_uint8* outputpixel = (vx_uint8*)vxFormatImagePatchAddress2d(output_imbaseptr, xp->x, xp->y, &output_imaddr);
				(*outputpixel) = (vx_uint8)255;

				keyptCnt++;

				if (keyptCnt >= ((int)MAXIMUM_KEYPOINTS)) break;
			}

			if (keyptCnt >= ((int)MAXIMUM_KEYPOINTS)) break;
		}
		vxCommitArrayRange(before_arr, 0, num_items, base);
		vxCommitImagePatch(mag, &imrect, plane, &imaddr, imbaseptr);
		vxCommitImagePatch(output_image, &output_imrect, output_plane, &output_imaddr, output_imbaseptr);

		/////////////////////////////////
		//writing keypoint to image
		/*
		FILE* fff2;
		fff2 = fopen("outim.pgm", "wb");

		fprintf(fff2, "P5\n%d %d\n255\n", wwval, hhval);
		for (i = 0; i < hhval; i++)
		{
			for (j = 0; j < wwval; j++)
			{
				vx_uint8* outputpixel = (vx_uint8*)vxFormatImagePatchAddress2d(output_imbaseptr, j, i, &output_imaddr);
				fprintf(fff2, "%c", (*outputpixel));
			}
		}

		fclose(fff2);
		/////////////////////////////////


		fclose(fff);
		*/

		return VX_SUCCESS;
    }
    
	return VX_ERROR_INVALID_PARAMETERS;
}

static vx_status VX_CALLBACK vxVerifyKeypointInputValidator(vx_node node, vx_uint32 index)
{
	vx_status status = VX_ERROR_INVALID_PARAMETERS;
	if (index == 1)
	{
		vx_image input = 0;
		vx_parameter param = vxGetParameterByIndex(node, index);

		vxQueryParameter(param, VX_PARAMETER_ATTRIBUTE_REF, &input, sizeof(input));
		if (input)
		{
			vx_df_image format = 0;
			vxQueryImage(input, VX_IMAGE_ATTRIBUTE_FORMAT, &format, sizeof(format));
			if ((format == VX_DF_IMAGE_U8)
				|| (format == VX_DF_IMAGE_S16)
#if defined(EXPERIMENTAL_USE_S16)
				|| (format == VX_DF_IMAGE_U16)
				|| (format == VX_DF_IMAGE_U32)
				|| (format == VX_DF_IMAGE_S32)
#endif
				)
			{
				status = VX_SUCCESS;
			}
			vxReleaseImage(&input);
		}
		vxReleaseParameter(&param);
	}
	
	if (index == 0)
	{
		vx_parameter param = vxGetParameterByIndex(node, index);
		if (param)
		{
			vx_array arr = 0;
			vxQueryParameter(param, VX_PARAMETER_ATTRIBUTE_REF, &arr, sizeof(arr));
			if (arr)
			{
				vx_enum item_type = 0;
				vxQueryArray(arr, VX_ARRAY_ATTRIBUTE_ITEMTYPE, &item_type, sizeof(item_type));
				if (item_type == VX_TYPE_COORDINATES2D)
				{
					status = VX_SUCCESS;
				}
				vxReleaseArray(&arr);
			}
			vxReleaseParameter(&param);
		}
	}

	if (index == 4 || index == 3 || index == 2)
	{

		vx_parameter param = vxGetParameterByIndex(node, index);
		vx_scalar scalar = 0;
		status = vxQueryParameter(param, VX_PARAMETER_ATTRIBUTE_REF, &scalar, sizeof(scalar));


		vx_enum type = 0;
		vxQueryScalar(scalar, VX_SCALAR_ATTRIBUTE_TYPE, &type, sizeof(type));
		if (type == VX_TYPE_INT32)
		{
			vx_int32 o = 0;
			vxAccessScalarValue(scalar, &o);
			if (o >= 0)
			{
				status = VX_SUCCESS;
			}
			else
			{
				status = VX_ERROR_INVALID_VALUE;
			}
		}
	}
	
	return status;
}

static vx_status VX_CALLBACK vxVerifyKeypointOutputValidator(vx_node node, vx_uint32 index, vx_meta_format_t *ptr)
{

    vx_status status = VX_ERROR_INVALID_PARAMETERS;
	if (index == 5)
	{
		vx_parameter param = vxGetParameterByIndex(node, 4);

		if (param)
		{
			vx_scalar maximum;
			vx_int32 max_keypoint_num;
			vxQueryParameter(param, VX_PARAMETER_ATTRIBUTE_REF, &maximum, sizeof(maximum));

			vxAccessScalarValue(maximum, &max_keypoint_num);

			ptr->type = VX_TYPE_ARRAY;
			ptr->dim.array.item_type = VX_TYPE_COORDINATES2D;
			ptr->dim.array.capacity = max_keypoint_num;
			//no defined capacity requirement
			status = VX_SUCCESS;

			vxReleaseScalar(&maximum);
			
		}
		vxReleaseParameter(&param);

		
	}
	if (index == 6)
    {
		
		vx_parameter param[2] = {
			vxGetParameterByIndex(node, 2),
			vxGetParameterByIndex(node, 3),
		};

		if (param[0] && param[1])
		{
			vx_int32 img_w;
			vx_int32 img_h;
			vx_scalar w_scalar;
			vx_scalar h_scalar;
			vxQueryParameter(param[0], VX_PARAMETER_ATTRIBUTE_REF, &w_scalar, sizeof(w_scalar));
			vxQueryParameter(param[1], VX_PARAMETER_ATTRIBUTE_REF, &h_scalar, sizeof(h_scalar));

			vxAccessScalarValue(w_scalar, &img_w);
			vxAccessScalarValue(h_scalar, &img_h);


			ptr->type = VX_TYPE_IMAGE;
			ptr->dim.image.format = VX_DF_IMAGE_U8;
			ptr->dim.image.width = img_w;
			ptr->dim.image.height = img_h;
			status = VX_SUCCESS;


			vxReleaseScalar(&w_scalar);
			vxReleaseScalar(&h_scalar);
		}

		vxReleaseParameter(&param[0]);
		vxReleaseParameter(&param[1]);
    }

    return status;
	
}


static vx_param_description_t verifykeypoint_kernel_params[] = {
	{ VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED },
	{ VX_OUTPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_OPTIONAL },
	{VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_OPTIONAL }, 
};

vx_kernel_description_t verifykeypoint_kernel = {
	VX_KERNEL_VERIFYKEYPOINT,
	"org.khronos.openvx.nothing",
	vxVerifyKeypointKernel,
	verifykeypoint_kernel_params, dimof(verifykeypoint_kernel_params),
	vxVerifyKeypointInputValidator,
	vxVerifyKeypointOutputValidator,
	NULL,
	NULL,
};

