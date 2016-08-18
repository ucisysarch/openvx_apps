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

static vx_status VX_CALLBACK vxFindSiftKeypointKernel(vx_node node, vx_reference *parameters, vx_uint32 num)
{
	if (num == 7)
	{
		//parameters
		vx_image mag = (vx_image)parameters[0];
		vx_image prev = (vx_image)parameters[1];
		vx_image curr = (vx_image)parameters[2];
		vx_image next = (vx_image)parameters[3];
		vx_scalar octave = (vx_scalar)parameters[4];
		vx_scalar maximum = (vx_scalar)parameters[5];
		vx_array arr = (vx_array)parameters[6];

		vx_coordinates2d_t foundKey;

		//maximum keypoints can be detected for single call of this node amount to 1000
		int keyptCnt = 0;

		//width and height of original image
		vx_uint32 ori_width, ori_height;	//images' width&height

		//get width and height from image
		vxQueryImage(mag, VX_IMAGE_WIDTH, &ori_width, sizeof(ori_width));
		vxQueryImage(mag, VX_IMAGE_HEIGHT, &ori_height, sizeof(ori_height));
		
		//constants from original openCV SIFT code
		/*
		vx_int32 SIFT_FIXPT_SCALE = 48;
		vx_float32 img_scale = 1.f / (255 * SIFT_FIXPT_SCALE);
		vx_float32 deriv_scale = img_scale*0.5f;				//img_scale * 0.5
		vx_float32 second_deriv_scale = img_scale;				//img_scale
		vx_float32 cross_deriv_scale = img_scale*0.25f;			//img_scale * 0.25

		//solution of matrix equation <xi, xr, xc>
		//we have to find out those values
		vx_float32 xi = 0.0f;
		vx_float32 xr = 0.0f;
		vx_float32 xc = 0.0f;

		//3d dD vector, right side of matrix equation.
		vx_float32 dD[3] = { 0.0f, };

		//3*3 Hessian Matrix, left side of matrix equation
		vx_float33 Hess[3][3] = { .0f, };
		*/

		//patch for access vx_image curr
		vx_rectangle_t curr_imrect;
		vx_uint32 curr_plane = 0;
		vx_imagepatch_addressing_t curr_imaddr;
		void* curr_imbaseptr = NULL;
		vx_map_id curr_img_id;

		//patch for access vx_image prev
		vx_rectangle_t prev_imrect;
		vx_uint32 prev_plane = 0;
		vx_imagepatch_addressing_t prev_imaddr;
		void* prev_imbaseptr = NULL;
		vx_map_id prev_img_id;
		
		//patch for access vx_image next
		vx_rectangle_t next_imrect;
		vx_uint32 next_plane = 0;
		vx_imagepatch_addressing_t next_imaddr;
		void* next_imbaseptr = NULL;
		vx_map_id next_img_id;

		//patch for access vx_image magnitude
		vx_rectangle_t mag_imrect;
		vx_uint32 mag_plane = 0;
		vx_imagepatch_addressing_t mag_imaddr;
		void* mag_imbaseptr = NULL;
		vx_map_id mag_img_id;


		//vx_reference DOG_pyramid_ref = (vx_reference)parameters[0];
		//vx_image*  DOG_pyramid = (vx_image*)DOG_pyramid_ref;
		//vx_array keypoints = (vx_array)parameters[1];

		//FILE* fff = NULL;
		//fff = fopen("www.txt", "a+");

		//access to 'curr' vx_image
		int SIFT_IMG_BORDER = 8;
		int VALUE_THRESHOLD = 85;
		int r, c;
		vx_int32 o;

		//maximum keypoints
		vx_int32 MAXIMUM_KEYPOINTS = 0;


		//=================BEGIN. assume we're using same size of vx_images as paramters
		vx_uint32 w, h;	//DOG images' width&height
		vxQueryImage(curr, VX_IMAGE_WIDTH, &w, sizeof(w));
		vxQueryImage(curr, VX_IMAGE_HEIGHT, &h, sizeof(h));

		//setting patch as entire size of image
		curr_imrect.start_x = curr_imrect.start_y = 0;
		curr_imrect.end_x = w; curr_imrect.end_y = h;
		prev_imrect.start_x = prev_imrect.start_y = 0;
		prev_imrect.end_x = w; prev_imrect.end_y = h;
		next_imrect.start_x = next_imrect.start_y = 0;
		next_imrect.end_x = w; next_imrect.end_y = h;
		mag_imrect.start_x = mag_imrect.start_y = 0;
		mag_imrect.end_x = ori_width; mag_imrect.end_y = ori_height;


		vxCopyScalar(octave, &o, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
		vxCopyScalar(maximum, &MAXIMUM_KEYPOINTS, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);

		//fprintf(fff, "w : %d, h : %d\n, max : %d", w, h, (int)MAXIMUM_KEYPOINTS);

		//allowing access to current vx_image layer
		vxMapImagePatch(curr, &curr_imrect, curr_plane, &curr_img_id,
			&curr_imaddr, &curr_imbaseptr, VX_READ_ONLY, VX_MEMORY_TYPE_NONE, VX_NOGAP_X);
		
		vxMapImagePatch(prev, &prev_imrect, prev_plane, &prev_img_id,
			&prev_imaddr, &prev_imbaseptr, VX_READ_ONLY, VX_MEMORY_TYPE_NONE, VX_NOGAP_X);

		vxMapImagePatch(next, &next_imrect, next_plane, &next_img_id,
			&next_imaddr, &next_imbaseptr, VX_READ_ONLY, VX_MEMORY_TYPE_NONE, VX_NOGAP_X);

		vxMapImagePatch(mag, &mag_imrect, mag_plane, &mag_img_id,
			&mag_imaddr, &mag_imbaseptr, VX_READ_ONLY, VX_MEMORY_TYPE_NONE, VX_NOGAP_X);


		//fprintf(fff, "< ");

		//pixel access loop
		for (int y = SIFT_IMG_BORDER; y < (int)(h - SIFT_IMG_BORDER); y++)
		{
			for (int x = SIFT_IMG_BORDER; x < (int)(w - SIFT_IMG_BORDER); x++)
			{

				//currpixel : pixel we're looking as candidate. This one will be compared to neighboring..
				vx_uint8* currpixel = (vx_uint8 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x, y, &curr_imaddr);
				//fprintf(fff, "%d ", (*currpixel));

				//neighboring 8 pixels on same layer(vx_image curr)
				vx_uint8* curr_neighbors[8] = {
					(vx_uint8 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x + 1, y, &curr_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x + 1, y + 1, &curr_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x, y + 1, &curr_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x - 1, y + 1, &curr_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x - 1, y, &curr_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x - 1, y - 1, &curr_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x, y - 1, &curr_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(curr_imbaseptr, x + 1, y - 1, &curr_imaddr)
				};
				//9 pixels on lower layer(previous layer)
				vx_uint8 *prev_neighbors[9] = {
					(vx_uint8 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x, y, &prev_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x + 1, y, &prev_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x + 1, y + 1, &prev_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x, y + 1, &prev_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x - 1, y + 1, &prev_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x - 1, y, &prev_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x - 1, y - 1, &prev_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x, y - 1, &prev_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(prev_imbaseptr, x + 1, y - 1, &prev_imaddr)
				};
				//9 pixels on upper layer(next layer)
				vx_uint8 *next_neighbors[9] = {
					(vx_uint8 *)vxFormatImagePatchAddress2d(next_imbaseptr, x, y, &next_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(next_imbaseptr, x + 1, y, &next_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(next_imbaseptr, x + 1, y + 1, &next_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(next_imbaseptr, x, y + 1, &next_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(next_imbaseptr, x - 1, y + 1, &next_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(next_imbaseptr, x - 1, y, &next_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(next_imbaseptr, x - 1, y - 1, &next_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(next_imbaseptr, x, y - 1, &next_imaddr),
					(vx_uint8 *)vxFormatImagePatchAddress2d(next_imbaseptr, x + 1, y - 1, &next_imaddr)
				};


				if (
					((*currpixel) >= (*curr_neighbors[0])) && ((*currpixel) >= (*curr_neighbors[1])) && ((*currpixel) >= (*curr_neighbors[2])) && ((*currpixel) >= (*curr_neighbors[3]))
					&& ((*currpixel) >= (*curr_neighbors[4])) && ((*currpixel) >= (*curr_neighbors[5])) && ((*currpixel) >= (*curr_neighbors[6])) && ((*currpixel) >= (*curr_neighbors[7]))
					&& ((*currpixel) >= (*prev_neighbors[0])) && ((*currpixel) >= (*prev_neighbors[1])) && ((*currpixel) >= (*prev_neighbors[2])) && ((*currpixel) >= (*prev_neighbors[3]))
					&& ((*currpixel) >= (*prev_neighbors[4])) && ((*currpixel) >= (*prev_neighbors[5])) && ((*currpixel) >= (*prev_neighbors[6])) && ((*currpixel) >= (*prev_neighbors[7]))
					&& ((*currpixel) >= (*prev_neighbors[8])) && ((*currpixel) >= (*next_neighbors[0])) && ((*currpixel) >= (*next_neighbors[1])) && ((*currpixel) >= (*next_neighbors[2]))
					&& ((*currpixel) >= (*next_neighbors[3])) && ((*currpixel) >= (*next_neighbors[4])) && ((*currpixel) >= (*next_neighbors[5])) && ((*currpixel) >= (*next_neighbors[6]))
					&& ((*currpixel) >= (*next_neighbors[7])) && ((*currpixel) >= (*next_neighbors[8]))
					)
				{
					//if we found maxima/minima, save the position

					//@@ check if the pixel's magnitude is bigger than threshold or not
					vx_uint16* pixel_mag = (vx_uint16 *)vxFormatImagePatchAddress2d(mag_imbaseptr, x, y, &mag_imaddr);

					//is bigger than threshold magnitude?
					if ((*pixel_mag) < VALUE_THRESHOLD)
						continue;

					//@@ and next, check if the corresponding pixel is in edge or not
					vx_int16 d = *currpixel;
					vx_float32 dxx = (vx_float32)((*curr_neighbors[2]) + (*curr_neighbors[6]) - 2 * d);
					vx_float32 dyy = (vx_float32)((*curr_neighbors[0]) + (*curr_neighbors[4]) - 2 * d);
					vx_float32 dxy = (vx_float32)(((*curr_neighbors[1]) - (*curr_neighbors[3]) - (*curr_neighbors[5]) - (*curr_neighbors[7])) / 4.0);

					vx_float32 tr = dxx + dyy;
					vx_float32 det = dxx * dyy - dxy * dxy;

					//is edge?
					if (det > 0 && tr * tr / det < (10 + 1.0)*(10 + 1.0) / 10)
					{

						r = y; c = x;
						r = r*(1 << o);
						c = c*(1 << o);
						foundKey.x = (vx_uint32)c;
						foundKey.y = (vx_uint32)r;

						vx_status stt = vxAddArrayItems(arr, 1, &foundKey, sizeof(vx_coordinates2d_t));
						/*
						switch (stt)
						{
						case VX_SUCCESS:
							fprintf(fff, "o[%d %d]\n", c, r);
							break;
						case VX_ERROR_INVALID_REFERENCE:
							fprintf(fff, "x_IR[%d %d]\n", c, r);
							break; 
						case VX_FAILURE:
							fprintf(fff, "f[%d %d]\n", c, r);
							break;
						case VX_ERROR_INVALID_PARAMETERS:
							fprintf(fff, "x_IP[%d %d]\n", c, r);
							break;
						}
						*/

						keyptCnt++;
					}

					//fprintf(fff, "%c", (*ptr2));

					if (keyptCnt >= ((int)MAXIMUM_KEYPOINTS)) break;
				}

				if (keyptCnt >= ((int)MAXIMUM_KEYPOINTS)) break;
			}

		}

		//fprintf(fff, ">");

		//commit current layer
		vxUnmapImagePatch(curr, curr_img_id);
		//commit previous layer
		vxUnmapImagePatch(prev, prev_img_id);
		//commit next layer
		vxUnmapImagePatch(next, next_img_id);
		//commit magnitude image
		vxUnmapImagePatch(mag, mag_img_id);

		//vxAccessScalarValue(octave, &o);
		//fprintf(fff, "received %d as octave\n", o);

		//fclose(fff);
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


		return VX_SUCCESS;
	}

	return VX_ERROR_INVALID_PARAMETERS;
}

static vx_status VX_CALLBACK vxFindSiftKeypointInputValidator(vx_node node, vx_uint32 index)
{
	vx_status status = VX_ERROR_INVALID_PARAMETERS;
	if (index == 0 || index == 1 || index == 2 || index == 3)
	{
		vx_image input = 0;
		vx_parameter param = vxGetParameterByIndex(node, index);

		vxQueryParameter(param, VX_PARAMETER_REF, &input, sizeof(input));
		if (input)
		{
			vx_df_image format = 0;
			vxQueryImage(input, VX_IMAGE_FORMAT, &format, sizeof(format));
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
	if (index == 4 || index == 5)
	{

		vx_parameter param = vxGetParameterByIndex(node, index);
		vx_scalar scalar = 0;
		status = vxQueryParameter(param, VX_PARAMETER_REF, &scalar, sizeof(scalar));


		vx_enum type = 0;
		vxQueryScalar(scalar, VX_SCALAR_TYPE, &type, sizeof(type));
		if (type == VX_TYPE_INT32)
		{
			vx_int32 o = 0;
			vxCopyScalar(scalar, &o, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
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
	if (index == 6)
	{
		vx_parameter param = vxGetParameterByIndex(node, 5);

		if (param)
		{
			vx_scalar maximum;
			vx_int32 max_keypoint_num;
			vxQueryParameter(param, VX_PARAMETER_REF, &maximum, sizeof(maximum));

			vxCopyScalar(maximum, &max_keypoint_num, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);

			ptr->type = VX_TYPE_ARRAY;
			ptr->dim.array.item_type = VX_TYPE_COORDINATES2D;
			ptr->dim.array.capacity = max_keypoint_num;
			//no defined capacity requirement
			status = VX_SUCCESS;
			vxReleaseScalar(&maximum);

		}
		vxReleaseParameter(&param);
	}

	return status;

}


static vx_param_description_t findsiftkeypoint_kernel_params[] = {
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED },
	{ VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED },
	{ VX_OUTPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_OPTIONAL },
};

vx_kernel_description_t findsiftkeypoint_kernel = {
	VX_KERNEL_FINDSIFTKEYPOINT,
	"org.khronos.openvx.nothing",
	vxFindSiftKeypointKernel,
	findsiftkeypoint_kernel_params, dimof(findsiftkeypoint_kernel_params),
	NULL,
	vxFindSiftKeypointInputValidator,
	vxFindSiftKeypointOutputValidator,
	NULL,
	NULL,
};

