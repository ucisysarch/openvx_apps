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
 * \brief The internal implementation of the debug kernels.
 * \author Erik Rainey <erik.rainey@gmail.com>
 * \defgroup group_debug_ext Debugging Extension
 */

#include <VX/vx.h>
#include <VX/vx_lib_debug.h>
#include <VX/vx_helper.h>
#include "vx_debug_module.h"

static
vx_kernel_description_t* kernels[] =
{
    &fwrite_image_kernel,
    &fread_image_kernel,
    &fwrite_array_kernel,
    &fread_array_kernel,
    &check_image_kernel,
    &check_array_kernel,
    &copy_image_kernel,
    &copy_array_kernel,
    &fill_image_kernel,
    &compare_images_kernel,
};

static vx_uint32 num_kernels = dimof(kernels);

//**********************************************************************
//  PUBLIC FUNCTION
//**********************************************************************

/*! \brief The entry point into this module to add the base kernels to OpenVX.
 * \param context The handle to the implementation context.
 * \return vx_status Returns errors if some or all kernels were not added
 * correctly.
 * \ingroup group_implementation
 */
/*VX_API_ENTRY*/ vx_status VX_API_CALL vxPublishKernels(vx_context context)
{
    vx_uint32 p = 0;
    vx_uint32 k = 0;
    vx_status status = VX_FAILURE;

    for (k = 0; k < num_kernels; k++)
    {
        vx_kernel kernel = vxAddUserKernel(context,
                kernels[k]->name,
                kernels[k]->enumeration,
                kernels[k]->function,
                kernels[k]->numParams,
                kernels[k]->validate,
                kernels[k]->initialize,
                kernels[k]->deinitialize);

        if (kernel)
        {
            status = VX_SUCCESS; // temporary
            for (p = 0; p < kernels[k]->numParams; p++)
            {
                status = vxAddParameterToKernel(kernel, p,
                                                kernels[k]->parameters[p].direction,
                                                kernels[k]->parameters[p].data_type,
                                                kernels[k]->parameters[p].state);
                if (status != VX_SUCCESS)
                {
                    vxAddLogEntry((vx_reference)context, status, "Failed to add parameter %d to kernel %s! (%d)\n", p, kernels[k]->name, status);
                    break;
                }
            }
            if (status == VX_SUCCESS)
            {
                status = vxFinalizeKernel(kernel);
                if (status != VX_SUCCESS)
                {
                    vxAddLogEntry((vx_reference)context, status, "Failed to finalize kernel[%u]=%s\n",k, kernels[k]->name);
                }
            }
            else
            {
                status = vxRemoveKernel(kernel);
                if (status != VX_SUCCESS)
                {
                    vxAddLogEntry((vx_reference)context, status, "Failed to remove kernel[%u]=%s\n",k, kernels[k]->name);
                }
            }
            /*! \todo should i continue with errors or fail and unwind? */
        }
        else
        {
            vxAddLogEntry((vx_reference)context, status, "Failed to add kernel %s\n", kernels[k]->name);
        }
    }

    return status;
}

/*! \brief The destructor to remove a user loaded module from OpenVX.
 * \param [in] context The handle to the implementation context.
 * \return A \ref vx_status_e enumeration. Returns errors if some or all kernels were not added
 * correctly.
 * \note This follows the function pointer definition of a \ref vx_unpublish_kernels_f
 * and uses the predefined name for the entry point, "vxUnpublishKernels".
 * \ingroup group_example_kernel
 */
/*VX_API_ENTRY*/ vx_status VX_API_CALL vxUnpublishKernels(vx_context context)
{
    vx_status status = VX_FAILURE;

    vx_uint32 k = 0;
    for (k = 0; k < num_kernels; k++)
    {
        vx_kernel kernel = vxGetKernelByName(context, kernels[k]->name);
        vx_kernel kernelcpy = kernel;

        if (kernel)
        {
            status = vxReleaseKernel(&kernelcpy);
            if (status != VX_SUCCESS)
            {
                vxAddLogEntry((vx_reference)context, status, "Failed to release kernel[%u]=%s\n",k, kernels[k]->name);
            }
            else
            {
                kernelcpy = kernel;
                status = vxRemoveKernel(kernelcpy);
                if (status != VX_SUCCESS)
                {
                    vxAddLogEntry((vx_reference)context, status, "Failed to remove kernel[%u]=%s\n",k, kernels[k]->name);
                }
            }
        }
        else
        {
            vxAddLogEntry((vx_reference)context, status, "Failed to get added kernel %s\n", kernels[k]->name);
        }
    }

    return status;
}
