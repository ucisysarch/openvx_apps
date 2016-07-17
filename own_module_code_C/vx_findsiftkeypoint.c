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

/*!
 * \file
 * \brief The Filter Kernels.
 * \author Erik Rainey <erik.rainey@gmail.com>
 */

#include <VX/vx.h>
#include <VX/vxu.h>
#include <VX/vx_helper.h>
#include <VX/vx_lib_extras.h>
#include <VX/vx_lib_debug.h>
#include <vx_internal.h>

#include <stdlib.h>
#include <stdio.h>

static vx_status VX_CALLBACK vxFindSiftKeypointKernel(vx_node node, vx_reference *parameters, vx_uint32 num)
{
    if (num == 5)
    {
		//parameters
		vx_image prev = (vx_image)parameters[0];
		vx_image curr = (vx_image)parameters[1];
		vx_image next = (vx_image)parameters[2];
		vx_scalar octave = (vx_scalar)parameters[3];
		vx_array arr = (vx_array)parameters[4];

		vx_coordinates2d_t foundKey;

		//patch for access vx_image curr
		vx_rectangle_t curr_imrect;
		vx_uint32 curr_plane = 0;
		vx_imagepatch_addressing_t curr_imaddr;
		void* curr_imbaseptr = NULL;
		//patch for access vx_image prev
		vx_rectangle_t prev_imrect;
		vx_uint32 prev_plane = 0;
		vx_imagepatch_addressing_t prev_imaddr;
		void* prev_imbaseptr = NULL;
		//patch for access vx_image next
		vx_rectangle_t next_imrect;
		vx_uint32 next_plane = 0;
		vx_imagepatch_addressing_t next_imaddr;
		void* next_imbaseptr = NULL;


		//vx_reference DOG_pyramid_ref = (vx_reference)parameters[0];
		//vx_image*  DOG_pyramid = (vx_image*)DOG_pyramid_ref;
		//vx_array keypoints = (vx_array)parameters[1];

		FILE* fff = NULL;
		fff = fopen("www.txt", "a+");

		//access to 'curr' vx_image
		int SIFT_IMG_BORDER = 5;
		//int VALUE_THRESHOLD = 50;
		int r, c;
		vx_int32 o;


		//=================BEGIN. assume we're using same size of vx_images as paramters
		vx_uint32 w, h;	//images' width&height
		vxQueryImage(curr, VX_IMAGE_ATTRIBUTE_WIDTH, &w, sizeof(w));
		vxQueryImage(curr, VX_IMAGE_ATTRIBUTE_HEIGHT, &h, sizeof(h));

		//setting patch as entire size of image
		curr_imrect.start_x = curr_imrect.start_y = 0;
		curr_imrect.end_x = w; curr_imrect.end_y = h;
		prev_imrect.start_x = prev_imrect.start_y = 0;
		prev_imrect.end_x = w; prev_imrect.end_y = h;
		next_imrect.start_x = next_imrect.start_y = 0;
		next_imrect.end_x = w; next_imrect.end_y = h;


		vxAccessScalarValue(octave, &o);

		fprintf(fff, "w : %d, h : %d\n", w, h);

		//allowing access to current vx_image layer
		if (vxAccessImagePatch(curr, &curr_imrect, curr_plane, &curr_imaddr, &curr_imbaseptr, VX_READ_ONLY) != VX_SUCCESS)
			fprintf(fff, "current layer access failed\n");
		//allowing access to previous vx_image layer
		if (vxAccessImagePatch(prev, &prev_imrect, prev_plane, &prev_imaddr, &prev_imbaseptr, VX_READ_ONLY) != VX_SUCCESS)
			fprintf(fff, "previous layer access failed\n");
		//allowing access to next vx_image layer
		if (vxAccessImagePatch(next, &next_imrect, next_plane, &next_imaddr, &next_imbaseptr, VX_READ_ONLY) != VX_SUCCESS)
			fprintf(fff, "next layer access failed\n");

		//fprintf(fff, "< ");

		//pixel access loop
		for (int y = SIFT_IMG_BORDER; y < h - SIFT_IMG_BORDER; y++)
		{
			for (int x = SIFT_IMG_BORDER; x < w - SIFT_IMG_BORDER; x++)
			{

				//currpixel : pixel we're looking as candidate. This one will be compared to neighboring..
				vx_int16* currpixel = (vx_int16 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x, y, &curr_imaddr);
				//fprintf(fff, "%d ", (*currpixel));

				//neighboring 8 pixels on same layer(vx_image curr)
				vx_int16* curr_neighbors[8] = {
					(vx_int16 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x + 1, y, &curr_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x + 1, y + 1, &curr_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x, y + 1, &curr_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x - 1, y + 1, &curr_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x - 1, y, &curr_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x - 1, y - 1, &curr_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x, y - 1, &curr_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x + 1, y - 1, &curr_imaddr)
				};
				//9 pixels on lower layer(previous layer)
				vx_int16 *prev_neighbors[9] = {
					(vx_int16 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x, y, &prev_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x + 1, y, &prev_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x + 1, y + 1, &prev_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x, y + 1, &prev_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x - 1, y + 1, &prev_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x - 1, y, &prev_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x - 1, y - 1, &prev_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x, y - 1, &prev_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x + 1, y - 1, &prev_imaddr)
				};
				//9 pixels on upper layer(next layer)
				vx_int16 *next_neighbors[9] = {
					(vx_int16 *)vxFormatImagePatchAddress2d(next_imbaseptr, x, y, &next_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(next_imbaseptr, x + 1, y, &next_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(next_imbaseptr, x + 1, y + 1, &next_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(next_imbaseptr, x, y + 1, &next_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(next_imbaseptr, x - 1, y + 1, &next_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(next_imbaseptr, x - 1, y, &next_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(next_imbaseptr, x - 1, y - 1, &next_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(next_imbaseptr, x, y - 1, &next_imaddr),
					(vx_int16 *)vxFormatImagePatchAddress2d(next_imbaseptr, x + 1, y - 1, &next_imaddr)
				};


				if (
					(((*currpixel) >= (*curr_neighbors[0])) && ((*currpixel) >= (*curr_neighbors[1])) && ((*currpixel) >= (*curr_neighbors[2])) && ((*currpixel) >= (*curr_neighbors[3]))
					&& ((*currpixel) >= (*curr_neighbors[4])) && ((*currpixel) >= (*curr_neighbors[5])) && ((*currpixel) >= (*curr_neighbors[6])) && ((*currpixel) >= (*curr_neighbors[7]))
					&& ((*currpixel) >= (*prev_neighbors[0])) && ((*currpixel) >= (*prev_neighbors[1])) && ((*currpixel) >= (*prev_neighbors[2])) && ((*currpixel) >= (*prev_neighbors[3]))
					&& ((*currpixel) >= (*prev_neighbors[4])) && ((*currpixel) >= (*prev_neighbors[5])) && ((*currpixel) >= (*prev_neighbors[6])) && ((*currpixel) >= (*prev_neighbors[7]))
					&& ((*currpixel) >= (*prev_neighbors[8])) && ((*currpixel) >= (*next_neighbors[0])) && ((*currpixel) >= (*next_neighbors[1])) && ((*currpixel) >= (*next_neighbors[2]))
					&& ((*currpixel) >= (*next_neighbors[3])) && ((*currpixel) >= (*next_neighbors[4])) && ((*currpixel) >= (*next_neighbors[5])) && ((*currpixel) >= (*next_neighbors[6]))
					&& ((*currpixel) >= (*next_neighbors[7])) && ((*currpixel) >= (*next_neighbors[8]))) ||
					(((*currpixel) <= (*curr_neighbors[0])) && ((*currpixel) <= (*curr_neighbors[1])) && ((*currpixel) <= (*curr_neighbors[2])) && ((*currpixel) <= (*curr_neighbors[3]))
					&& ((*currpixel) <= (*curr_neighbors[4])) && ((*currpixel) <= (*curr_neighbors[5])) && ((*currpixel) <= (*curr_neighbors[6])) && ((*currpixel) <= (*curr_neighbors[7]))
					&& ((*currpixel) <= (*prev_neighbors[0])) && ((*currpixel) <= (*prev_neighbors[1])) && ((*currpixel) <= (*prev_neighbors[2])) && ((*currpixel) <= (*prev_neighbors[3]))
					&& ((*currpixel) <= (*prev_neighbors[4])) && ((*currpixel) <= (*prev_neighbors[5])) && ((*currpixel) <= (*prev_neighbors[6])) && ((*currpixel) <= (*prev_neighbors[7]))
					&& ((*currpixel) <= (*prev_neighbors[8])) && ((*currpixel) <= (*next_neighbors[0])) && ((*currpixel) <= (*next_neighbors[1])) && ((*currpixel) <= (*next_neighbors[2]))
					&& ((*currpixel) <= (*next_neighbors[3])) && ((*currpixel) <= (*next_neighbors[4])) && ((*currpixel) <= (*next_neighbors[5])) && ((*currpixel) <= (*next_neighbors[6]))
					&& ((*currpixel) <= (*next_neighbors[7])) && ((*currpixel) <= (*next_neighbors[8])))
					)
				{
					//if we found maxima/minima, save the position
					r = y; c = x;
					r = r*(1 << o);
					c = c*(1 << o);
					foundKey.x = (vx_uint32)c;
					foundKey.y = (vx_uint32)r;
					vxAddArrayItems(arr, 1, &foundKey, 0);

					fprintf(fff, "[%d %d]\n", c, r);
				}
				
				//fprintf(fff, "%c", (*ptr2));
			}

		}

		//fprintf(fff, ">");

		//commit current layer
		vxCommitImagePatch(curr, &curr_imrect, curr_plane, &curr_imaddr, curr_imbaseptr);
		//commit previous layer
		vxCommitImagePatch(prev, &prev_imrect, prev_plane, &prev_imaddr, prev_imbaseptr);
		//commit next layer
		vxCommitImagePatch(next, &next_imrect, next_plane, &next_imaddr, next_imbaseptr);

		//vxAccessScalarValue(octave, &o);
		//fprintf(fff, "received %d as octave\n", o);

		fclose(fff);
		//=================END

	

		/*
		a1.x = (vx_uint32)111;
		a1.y = (vx_uint32)333;
		a2.x = (vx_uint32)2222;
		a2.y = (vx_uint32)4444;
		a3.x = (vx_uint32)o;
		a3.y = (vx_uint32)o;

		vxAddArrayItems(arr, 1, &a1, 0);
		vxAddArrayItems(arr, 1, &a2, 0);
		vxAddArrayItems(arr, 1, &a3, 0);
		*/

		//기존 모듈 같았더라면 이 위치에 return vxFindSiftKeypoint(vx_image*, vx_array) 가 들어갈 것이다.

		return VX_SUCCESS;
    }
    
	return VX_ERROR_INVALID_PARAMETERS;
}

static vx_status VX_CALLBACK vxFindSiftKeypointInputValidator(vx_node node, vx_uint32 index)
{
	vx_status status = VX_ERROR_INVALID_PARAMETERS;
	if (index == 0 | index == 1 | index == 2)
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
	if (index == 3)
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

static vx_status VX_CALLBACK vxFindSiftKeypointOutputValidator(vx_node node, vx_uint32 index, vx_meta_format_t *ptr)
{

    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    if (index == 4)
    {
        ptr->type = VX_TYPE_ARRAY;
		ptr->dim.array.item_type = VX_TYPE_COORDINATES2D;
        ptr->dim.array.capacity = 1000; 
		//no defined capacity requirement
        status = VX_SUCCESS;
    }

    return status;
	
}


static vx_param_description_t findsiftkeypoint_kernel_params[] = {
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED },
	{VX_OUTPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_OPTIONAL }, 
};

vx_kernel_description_t findsiftkeypoint_kernel = {
	VX_KERNEL_FINDSIFTKEYPOINT,
	"org.khronos.openvx.nothing",
	vxFindSiftKeypointKernel,
	findsiftkeypoint_kernel_params, dimof(findsiftkeypoint_kernel_params),
	vxFindSiftKeypointInputValidator,
	vxFindSiftKeypointOutputValidator,
	NULL,
	NULL,
};

