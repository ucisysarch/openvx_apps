/* 
 * Copyright (c) 2012-2016 The Khronos Group Inc.
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
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
 *    https://www.khronos.org/registry/
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
 * \brief The Filter Kernel (Extras)
 * \author Erik Rainey <erik.rainey@gmail.com>
 */

#include <VX/vx.h>
#include <VX/vx_lib_extras.h>
#include <VX/vx_helper.h>
#include <extras_k.h>


static
vx_param_description_t laplacian3x3_kernel_params[] =
{
    { VX_INPUT,  VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
    { VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
};

static
vx_status VX_CALLBACK ownLaplacian3x3Kernel(vx_node node, const vx_reference parameters[], vx_uint32 num)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;

    if (num == dimof(laplacian3x3_kernel_params))
    {
        vx_border_t bordermode;
        vx_image src = (vx_image)parameters[0];
        vx_image dst = (vx_image)parameters[1];

        status = vxQueryNode(node, VX_NODE_BORDER, &bordermode, sizeof(bordermode));
        if (status == VX_SUCCESS)
        {
            status = ownLaplacian3x3(src, dst, &bordermode);
        }
    }

    return status;
} /* ownLaplacian3x3Kernel() */

static
vx_status VX_CALLBACK set_laplacian3x3_valid_rectangle(
    vx_node node,
    vx_uint32 index,
    const vx_rectangle_t* const input_valid[], vx_rectangle_t* const output_valid[])
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;

    if (NULL != node && index < dimof(laplacian3x3_kernel_params) && NULL != input_valid && NULL != output_valid)
    {
        vx_border_t borders = { VX_BORDER_UNDEFINED, { { 0 } } };

        if (VX_SUCCESS == vxQueryNode(node, VX_NODE_BORDER, &borders, sizeof(borders)))
        {
            if (VX_BORDER_UNDEFINED == borders.mode)
            {
                vx_uint32 border_size = 1;

                output_valid[0]->start_x = input_valid[0]->start_x + border_size;
                output_valid[0]->start_y = input_valid[0]->start_y + border_size;
                output_valid[0]->end_x   = input_valid[0]->end_x   - border_size;
                output_valid[0]->end_y   = input_valid[0]->end_y   - border_size;
                status = VX_SUCCESS;
            }
            else
                status = VX_ERROR_NOT_IMPLEMENTED;
        }
    } // if ptrs non NULL

    return status;
} /* set_laplacian3x3_valid_rectangle() */

static
vx_status VX_CALLBACK own_laplacian3x3_validator(
    vx_node node,
    const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;

    if (NULL != node &&
        num == dimof(laplacian3x3_kernel_params) &&
        NULL != metas)
    {
        vx_parameter param = 0;
        vx_image     src   = 0;

        param = vxGetParameterByIndex(node, 0);

        if (VX_SUCCESS != vxGetStatus((vx_reference)param))
        {
            return VX_ERROR_INVALID_PARAMETERS;
        }

        status = vxQueryParameter(param, VX_PARAMETER_REF, &src, sizeof(src));

        if (VX_SUCCESS == status)
        {
            vx_uint32   src_width  = 0;
            vx_uint32   src_height = 0;
            vx_df_image src_format = 0;

            /* validate input image */
            if (VX_SUCCESS == status &&
                VX_SUCCESS == vxGetStatus((vx_reference)src))
            {
                status |= vxQueryImage(src, VX_IMAGE_WIDTH,  &src_width,  sizeof(src_width));
                status |= vxQueryImage(src, VX_IMAGE_HEIGHT, &src_height, sizeof(src_height));
                status |= vxQueryImage(src, VX_IMAGE_FORMAT, &src_format, sizeof(src_format));

                /* expected VX_DF_IMAGE_U8 format and size not less than filter kernel size */
                if (VX_SUCCESS == status &&
                    src_width >= 3 && src_height >= 3 && src_format == VX_DF_IMAGE_U8)
                {
                    status = VX_SUCCESS;
                }
                else
                    status = VX_ERROR_INVALID_PARAMETERS;
            }
            else
                status = VX_ERROR_INVALID_PARAMETERS;

            /* validate output images */
            if (VX_SUCCESS == status)
            {
                vx_border_t borders = { VX_BORDER_UNDEFINED, { { 0 } } };

                status |= vxSetMetaFormatAttribute(metas[1], VX_IMAGE_WIDTH,  &src_width,  sizeof(src_width));
                status |= vxSetMetaFormatAttribute(metas[1], VX_IMAGE_HEIGHT, &src_height, sizeof(src_height));
                status |= vxSetMetaFormatAttribute(metas[1], VX_IMAGE_FORMAT, &src_format, sizeof(src_format));

                status |= vxQueryNode(node, VX_NODE_BORDER, &borders, sizeof(borders));

                if (VX_SUCCESS == status)
                {
                    if (VX_BORDER_UNDEFINED == borders.mode)
                    {
                        vx_kernel_image_valid_rectangle_f callback = &set_laplacian3x3_valid_rectangle;

                        status = vxSetMetaFormatAttribute(metas[1], VX_VALID_RECT_CALLBACK, &callback, sizeof(callback));
                    }
                    else
                        status = VX_ERROR_NOT_IMPLEMENTED;
                }
            }
        }
        else
            status = VX_ERROR_INVALID_PARAMETERS;

        if (NULL != src)
            vxReleaseImage(&src);

        if (NULL != param)
            vxReleaseParameter(&param);
    } /* if input ptrs != NULL */

    return status;
} /* own_laplacian3x3_validator() */

vx_kernel_description_t laplacian3x3_kernel =
{
    VX_KERNEL_EXTRAS_LAPLACIAN_3x3,
    "org.khronos.extras.laplacian3x3",
    ownLaplacian3x3Kernel,
    laplacian3x3_kernel_params, dimof(laplacian3x3_kernel_params),
    own_laplacian3x3_validator,
    NULL,
    NULL,
    NULL,
    NULL,
};
