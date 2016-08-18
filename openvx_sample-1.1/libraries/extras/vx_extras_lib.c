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
 * \brief The Khronos Extras Extension Node and Immediate Interfaces
 * \author Erik Rainey <erik.rainey@gmail.com>
 */

#include <VX/vx.h>
#include <VX/vx_lib_extras.h>
#include <VX/vx_helper.h>

//*****************************************************************************
// PUBLIC INTERFACE
//*****************************************************************************

vx_node vxNonMaxSuppressionNode(vx_graph graph, vx_image mag, vx_image phase, vx_image edge)
{
    vx_reference params[] = {
        (vx_reference)mag,
        (vx_reference)phase,
        (vx_reference)edge,
    };
    return vxCreateNodeByStructure(graph,
                                   VX_KERNEL_EXTRAS_NONMAXSUPPRESSION,
                                   params, dimof(params));
}

vx_status vxuNonMaxSuppression(vx_context context, vx_image mag, vx_image phase, vx_image edge)
{
    vx_status status = VX_SUCCESS;
    vx_graph graph = vxCreateGraph(context);
    if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
    {
        vx_node node = vxNonMaxSuppressionNode(graph, mag, phase, edge);
        if (node)
        {
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                status = vxProcessGraph(graph);
            }
            vxReleaseNode(&node);
        }
        vxClearLog((vx_reference)graph);
        vxReleaseGraph(&graph);
    }
    return status;
}

vx_node vxLaplacian3x3Node(vx_graph graph, vx_image input, vx_image output)
{
    vx_reference params[] = {
        (vx_reference)input,
        (vx_reference)output,
    };
    return vxCreateNodeByStructure(graph,
                                   VX_KERNEL_EXTRAS_LAPLACIAN_3x3,
                                   params,
                                   dimof(params));
}

vx_status vxuLaplacian3x3(vx_context context, vx_image input, vx_image output)
{
    vx_status status = VX_FAILURE;
    vx_graph graph = vxCreateGraph(context);
    if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
    {
        vx_node node = vxLaplacian3x3Node(graph, input, output);
        if (node)
        {
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                status = vxProcessGraph(graph);
            }
            vxReleaseNode(&node);
        }
        vxClearLog((vx_reference)graph);
        vxReleaseGraph(&graph);
    }
    return status;
}

vx_node vxScharr3x3Node(vx_graph graph, vx_image input, vx_image output1, vx_image output2)
{
    vx_reference params[] = {
        (vx_reference)input,
        (vx_reference)output1,
        (vx_reference)output2
    };
    return vxCreateNodeByStructure(graph,
                                   VX_KERNEL_EXTRAS_SCHARR_3x3,
                                   params,
                                   dimof(params));
}

vx_status vxuScharr3x3(vx_context context, vx_image input, vx_image output1,vx_image output2)
{
    vx_status status = VX_FAILURE;
    vx_graph graph = vxCreateGraph(context);
    if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
    {
        vx_node node = vxScharr3x3Node(graph, input, output1,output2);
        if (node)
        {
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                status = vxProcessGraph(graph);
            }
            vxReleaseNode(&node);
        }
        vxClearLog((vx_reference)graph);
        vxReleaseGraph(&graph);
    }
    return status;
}

vx_node vxSobelMxNNode(vx_graph graph, vx_image input, vx_scalar ws, vx_image gx, vx_image gy)
{
    vx_reference params[] = {
        (vx_reference)input,
        (vx_reference)ws,
        (vx_reference)gx,
        (vx_reference)gy,
    };
    vx_node node = vxCreateNodeByStructure(graph,
                                   VX_KERNEL_EXTRAS_SOBEL_MxN,
                                   params,
                                   dimof(params));
    return node;
}

vx_status vxuSobelMxN(vx_context context, vx_image input, vx_scalar win, vx_image gx, vx_image gy)
{
    vx_status status = VX_FAILURE;
    vx_graph graph = vxCreateGraph(context);
    if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
    {
        vx_node node = vxSobelMxNNode(graph, input, win, gx, gy);
        if (node)
        {
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                status = vxProcessGraph(graph);
            }
            vxReleaseNode(&node);
        }
        vxClearLog((vx_reference)graph);
        vxReleaseGraph(&graph);
    }
    return status;
}

vx_node vxHarrisScoreNode(vx_graph	graph,
                          vx_image	gx,
                          vx_image	gy,
                          vx_scalar sensitivity,
                          vx_scalar grad_size,
                          vx_scalar block_size,
                          vx_image	score)
{
    vx_reference params[] = {
        (vx_reference)gx,
        (vx_reference)gy,
        (vx_reference)sensitivity,
        (vx_reference)grad_size,
        (vx_reference)block_size,
        (vx_reference)score,
    };
    vx_node node = vxCreateNodeByStructure(graph,
                                           VX_KERNEL_EXTRAS_HARRIS_SCORE,
                                           params,
                                           dimof(params));
    return node;
}

vx_status vxuHarrisScore(vx_context context,
                         vx_image	gx,
                         vx_image	gy,
                         vx_scalar	sensitivity,
                         vx_scalar	grad_size,
                         vx_scalar	block_size,
                         vx_image	score)
{
    vx_status status = VX_FAILURE;
    vx_graph graph = vxCreateGraph(context);
    if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
    {
        vx_node node = vxHarrisScoreNode(graph, gx, gy, sensitivity, grad_size, block_size, score);
        if (node)
        {
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                status = vxProcessGraph(graph);
            }
            vxReleaseNode(&node);
        }
        vxClearLog((vx_reference)graph);
        vxReleaseGraph(&graph);
    }
    return status;
}

vx_node vxEuclideanNonMaxHarrisNode(vx_graph graph,
                              vx_image input,
                              vx_scalar strength_thresh,
                              vx_scalar min_distance,
                              vx_image output)
{
    vx_reference params[] = {
        (vx_reference)input,
        (vx_reference)strength_thresh,
        (vx_reference)min_distance,
        (vx_reference)output,
    };
    vx_node node = vxCreateNodeByStructure(graph,
                                           VX_KERNEL_EXTRAS_EUCLIDEAN_NONMAXSUPPRESSION_HARRIS,
                                           params,
                                           dimof(params));
    return node;
}

vx_status vxuEuclideanNonMaxHarris(vx_context context, vx_image input,
                             vx_scalar strength_thresh,
                             vx_scalar min_distance,
                             vx_image output)
{
    vx_status status = VX_FAILURE;
    vx_graph graph = vxCreateGraph(context);
    if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
    {
        vx_node node = vxEuclideanNonMaxHarrisNode(graph, input, strength_thresh, min_distance, output);
        if (node)
        {
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                status = vxProcessGraph(graph);
            }
            vxReleaseNode(&node);
        }
        vxClearLog((vx_reference)graph);
        vxReleaseGraph(&graph);
    }
    return status;
}

vx_node vxImageListerNode(vx_graph graph, vx_image input, vx_array arr, vx_scalar num_points)
{
    vx_reference params[] = {
        (vx_reference)input,
        (vx_reference)arr,
        (vx_reference)num_points,
    };
    return vxCreateNodeByStructure(graph,
                                   VX_KERNEL_EXTRAS_IMAGE_LISTER,
                                   params,
                                   dimof(params));
}

vx_status vxuImageLister(vx_context context, vx_image input,
                         vx_array arr, vx_scalar num_points)
{
    vx_status status = VX_FAILURE;
    vx_graph graph = vxCreateGraph(context);
    if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
    {
        vx_node node = vxImageListerNode(graph, input, arr, num_points);
        if (node)
        {
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                status = vxProcessGraph(graph);
            }
            vxReleaseNode(&node);
        }
        vxClearLog((vx_reference)graph);
        vxReleaseGraph(&graph);
    }
    return status;
}

vx_node vxElementwiseNormNode(vx_graph graph,
                              vx_image input_x,
                              vx_image input_y,
                              vx_scalar norm_type,
                              vx_image output)
{
    vx_reference params[] = {
        (vx_reference)input_x,
        (vx_reference)input_y,
        (vx_reference)norm_type,
        (vx_reference)output,
    };
    vx_node node = vxCreateNodeByStructure(graph,
                                           VX_KERNEL_EXTRAS_ELEMENTWISE_NORM,
                                           params,
                                           dimof(params));
    return node;
}

vx_node vxEdgeTraceNode(vx_graph graph,
                        vx_image norm,
                        vx_threshold threshold,
                        vx_image output)
{
    vx_reference params[] = {
        (vx_reference)norm,
        (vx_reference)threshold,
        (vx_reference)output,
    };
    vx_node node = vxCreateNodeByStructure(graph,
                                           VX_KERNEL_EXTRAS_EDGE_TRACE,
                                           params,
                                           dimof(params));
    return node;
}
