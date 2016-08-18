/*
 * Copyright (c) 2016 The Khronos Group Inc.
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

#include <VX/vx.h>
#include <VX/vx_helper.h>

#include <vx_internal.h>
#include <c_model.h>

#include <c_model.h>

#if (C_MAX_NONLINEAR_DIM != VX_INT_MAX_NONLINEAR_DIM)
#if defined(_WIN32)
#pragma error("C Model does not support VX required nonlinear filter size")
#elif defined(__GNUC__)
#error "C Model does not support VX required nonlinear filter size"
#endif
#endif

static vx_status VX_CALLBACK vxNonLinearFilterKernel(vx_node node, const vx_reference *parameters, vx_uint32 num)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    if (num == 4)
    {
        vx_border_t bordermode;
        vx_scalar function = (vx_scalar)parameters[0];
        vx_image src  = (vx_image)parameters[1];
        vx_matrix mask = (vx_matrix)parameters[2];
        vx_image dst = (vx_image)parameters[3];
        status = vxQueryNode(node, VX_NODE_BORDER, &bordermode, sizeof(bordermode));
        if (status == VX_SUCCESS)
        {
            status = vxNonLinearFilter(function, src, mask, dst, &bordermode);
        }
    }
    return status;
}

static vx_status VX_CALLBACK vxNonLinearFilterInputValidator(vx_node node, vx_uint32 index)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    if (index == 0)
    {
        vx_scalar scalar = 0;
        vx_parameter param = vxGetParameterByIndex(node, index);

        vxQueryParameter(param, VX_PARAMETER_REF, &scalar, sizeof(scalar));
        if (scalar)
        {
            vx_enum stype = 0;
            vxQueryScalar(scalar, VX_SCALAR_TYPE, &stype, sizeof(stype));
            if (stype == VX_TYPE_ENUM)
            {
                vx_enum function = 0;
                vxCopyScalar(scalar, &function, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
                if ((function == VX_NONLINEAR_FILTER_MEDIAN) ||
                    (function == VX_NONLINEAR_FILTER_MIN) ||
                    (function == VX_NONLINEAR_FILTER_MAX))
                {
                    status = VX_SUCCESS;
                }
                else
                {
                    status = VX_ERROR_INVALID_VALUE;
                }
            }
            else
            {
                status = VX_ERROR_INVALID_TYPE;
            }
            vxReleaseScalar(&scalar);
        }
        vxReleaseParameter(&param);
    }
    else if (index == 1)
    {
        vx_image input = 0;
        vx_parameter param = vxGetParameterByIndex(node, index);

        vxQueryParameter(param, VX_PARAMETER_REF, &input, sizeof(input));
        if (input)
        {
            vx_df_image format = 0;
            vxQueryImage(input, VX_IMAGE_FORMAT, &format, sizeof(format));
            if (format == VX_DF_IMAGE_U8)
            {
                status = VX_SUCCESS;
            }
            vxReleaseImage(&input);
        }
        vxReleaseParameter(&param);
    }
    else if (index == 2)
    {
        vx_parameter param = vxGetParameterByIndex(node, index);
        if (param)
        {
            vx_matrix matrix;
            vxQueryParameter(param, VX_PARAMETER_REF, &matrix, sizeof(matrix));
            if (matrix)
            {
                vx_enum data_type = 0;
                vx_size cols = 0, rows = 0;
                vxQueryMatrix(matrix, VX_MATRIX_TYPE, &data_type, sizeof(data_type));
                vxQueryMatrix(matrix, VX_MATRIX_COLUMNS, &cols, sizeof(cols));
                vxQueryMatrix(matrix, VX_MATRIX_ROWS, &rows, sizeof(rows));
                if ((rows <= VX_INT_MAX_NONLINEAR_DIM) &&
                    (cols <= VX_INT_MAX_NONLINEAR_DIM) &&
                    (data_type == VX_TYPE_UINT8))
                {
                    status = VX_SUCCESS;
                }
                vxReleaseMatrix(&matrix);
            }
            vxReleaseParameter(&param);
        }
    }
    return status;
}

static vx_status VX_CALLBACK vxNonLinearFilterOutputValidator(vx_node node, vx_uint32 index, vx_meta_format_t *ptr)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    if (index == 3)
    {
        vx_parameter param = vxGetParameterByIndex(node, 1); /* we reference the input image */
        if (param)
        {
            vx_image input = 0;
            vxQueryParameter(param, VX_PARAMETER_REF, &input, sizeof(input));
            if (input)
            {
                vx_uint32 width = 0, height = 0;
                vxQueryImage(input, VX_IMAGE_WIDTH, &width, sizeof(width));
                vxQueryImage(input, VX_IMAGE_HEIGHT, &height, sizeof(height));
                ptr->type = VX_TYPE_IMAGE;
                ptr->dim.image.format = VX_DF_IMAGE_U8;
                ptr->dim.image.width = width;
                ptr->dim.image.height = height;
                status = VX_SUCCESS;
                vxReleaseImage(&input);
            }
            vxReleaseParameter(&param);
        }
    }
    return status;
}

static vx_param_description_t filter_kernel_params[] = {
    { VX_INPUT,  VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED },
    { VX_INPUT,  VX_TYPE_IMAGE,  VX_PARAMETER_STATE_REQUIRED },
    { VX_INPUT,  VX_TYPE_MATRIX, VX_PARAMETER_STATE_REQUIRED },
    { VX_OUTPUT, VX_TYPE_IMAGE,  VX_PARAMETER_STATE_REQUIRED },
};

vx_kernel_description_t nonlinearfilter_kernel = {
    VX_KERNEL_NON_LINEAR_FILTER,
    "org.khronos.openvx.non_linear_filter",
    vxNonLinearFilterKernel,
    filter_kernel_params, dimof(filter_kernel_params),
    NULL,
    vxNonLinearFilterInputValidator,
    vxNonLinearFilterOutputValidator,
    NULL,
    NULL,
};
