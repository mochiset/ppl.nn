// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "ppl/nn/engines/cuda/optimizer/opt_kernel_creator_manager.h"
using namespace std;
using namespace ppl::common;

#include "ppl/nn/engines/cuda/optimizer/ops/ppl/bridge_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/ppl/channel_shuffle_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/ppl/shape_operation_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/ppl/reduce_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/conv_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/add_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/argmax_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/average_pool_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/clip_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/concat_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/div_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/equal_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/exp_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/expand_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/flatten_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/gather_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/gather_nd_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/gemm_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/global_average_pool_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/global_max_pool_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/greater_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/identity_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/leaky_relu_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/max_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/min_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/max_pool_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/max_unpool_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/matmul_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/mul_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/non_max_suppression_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/non_zero_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/not_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/pad_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/relu_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/reshape_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/scatter_nd_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/sigmoid_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/softmax_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/split_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/sqrt_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/squeeze_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/sub_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/transpose_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/unsqueeze_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/cast_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/pow_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/slice_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/batch_normalization_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/where_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/range_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/topk_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/tanh_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/resize_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/roialign_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/shape_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/constant_of_shape_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/log_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/depth_to_space_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/tile_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/less_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/floor_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/convtranspose_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/scatter_elements_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/ceil_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/and_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/if_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/loop_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/sequence_at_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/split_to_sequence_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/onnx/lstm_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/mmcv/mmcv_non_max_suppression_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/mmcv/mmcv_roialign_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/mmcv/mmcv_gridsample_op.h"
#include "ppl/nn/engines/cuda/optimizer/ops/mmcv/mmcv_modulated_deform_conv2d_op.h"

namespace ppl { namespace nn { namespace cuda {

template <typename T>
static CudaOptKernel* GenericCreateOptKernel(const ir::Node* node) {
    return new T(node);
}

template <typename T>
static void RegisterOptKernelCreator(const string& domain, const string& type, uint64_t first_version,
                                     uint64_t last_version) {
    if (last_version < first_version) {
        LOG(ERROR) << "register op[" << domain << ":" << type << "] failed: last_version[" << last_version
                   << "] < first_version[" << first_version << "]";
        exit(-1);
    }
    OptKernelCreatorManager::GetInstance()->Register(domain, type, utils::VersionRange(first_version, last_version),
                                                     GenericCreateOptKernel<T>);
}

// NOTE: sorted in alphabet order
void RegisterBuiltinOpImpls() {
    // onnx op's default domain is ""
    // A
    RegisterOptKernelCreator<AddOp>("", "Add", 7, 12);
    RegisterOptKernelCreator<AndOp>("", "And", 7, 16);
    RegisterOptKernelCreator<ArgmaxOp>("", "ArgMax", 11, 11);
    RegisterOptKernelCreator<AveragePoolOp>("", "AveragePool", 11, 16);
    // B
    RegisterOptKernelCreator<BatchNormalizationOp>("", "BatchNormalization", 9, 13);
    // C
    RegisterOptKernelCreator<CastOp>("", "Cast", 9, 12);
    RegisterOptKernelCreator<CeilOp>("", "Ceil", 6, 12);
    RegisterOptKernelCreator<ClipOp>("", "Clip", 11, 11);
    RegisterOptKernelCreator<ConcatOp>("", "Concat", 11, 12);
    RegisterOptKernelCreator<ConstantOfShapeOp>("", "ConstantOfShape", 9, 16);
    RegisterOptKernelCreator<ConvOp>("", "Conv", 11, 16);
    RegisterOptKernelCreator<ConvTransposeOp>("", "ConvTranspose", 11, 16);
    // D
    RegisterOptKernelCreator<DepthToSpaceOp>("", "DepthToSpace", 11, 12);
    RegisterOptKernelCreator<DivOp>("", "Div", 7, 12);
    // E
    RegisterOptKernelCreator<EqualOp>("", "Equal", 11, 12);
    RegisterOptKernelCreator<ExpOp>("", "Exp", 6, 12);
    RegisterOptKernelCreator<ExpandOp>("", "Expand", 8, 12);
    // F
    RegisterOptKernelCreator<FlattenOp>("", "Flatten", 11, 12);
    RegisterOptKernelCreator<FloorOp>("", "Floor", 6, 12);
    // G
    RegisterOptKernelCreator<GatherOp>("", "Gather", 11, 12);
    RegisterOptKernelCreator<GatherNDOp>("", "GatherND", 11, 11);
    RegisterOptKernelCreator<GemmOp>("", "Gemm", 11, 12);
    RegisterOptKernelCreator<GlobalAveragePoolOp>("", "GlobalAveragePool", 1, 16);
    RegisterOptKernelCreator<GlobalMaxPoolOp>("", "GlobalMaxPool", 11, 13);
    RegisterOptKernelCreator<GreaterOp>("", "Greater", 9, 12);
    // I
    RegisterOptKernelCreator<IdentityOp>("", "Identity", 1, 12);
    RegisterOptKernelCreator<IfOp>("", "If", 11, 12);
    // L
    RegisterOptKernelCreator<LeakyReluOp>("", "LeakyRelu", 6, 16);
    RegisterOptKernelCreator<LessOp>("", "Less", 9, 12);
    RegisterOptKernelCreator<LogOp>("", "Log", 6, 12);
    RegisterOptKernelCreator<LoopOp>("", "Loop", 11, 12);
    RegisterOptKernelCreator<LstmOp>("", "LSTM", 7, 13);
    // M
    RegisterOptKernelCreator<MatMulOp>("", "MatMul", 9, 12);
    RegisterOptKernelCreator<MaxOp>("", "Max", 8, 11);
    RegisterOptKernelCreator<MaxPoolOp>("", "MaxPool", 11, 13);
    RegisterOptKernelCreator<MaxUnPoolOp>("", "MaxUnpool", 11, 16);
    RegisterOptKernelCreator<MinOp>("", "Min", 8, 11);
    RegisterOptKernelCreator<MulOp>("", "Mul", 7, 12);
    // N
    RegisterOptKernelCreator<NonMaxSupressionOp>("", "NonMaxSuppression", 11, 16);
    RegisterOptKernelCreator<NonZeroOp>("", "NonZero", 9, 12);
    RegisterOptKernelCreator<NotOp>("", "Not", 1, 16);
    // P
    RegisterOptKernelCreator<PadOp>("", "Pad", 11, 12);
    RegisterOptKernelCreator<PowOp>("", "Pow", 7, 11);
    // R
    RegisterOptKernelCreator<RangeOp>("", "Range", 11, 16);
    RegisterOptKernelCreator<ReduceOp>("", "ReduceMax", 11, 11);
    RegisterOptKernelCreator<ReduceOp>("", "ReduceMean", 11, 12);
    RegisterOptKernelCreator<ReduceOp>("", "ReduceMin", 11, 11);
    RegisterOptKernelCreator<ReduceOp>("", "ReduceProd", 11, 12);
    RegisterOptKernelCreator<ReduceOp>("", "ReduceSum", 11, 12);
    RegisterOptKernelCreator<ReluOp>("", "Relu", 6, 12);
    RegisterOptKernelCreator<ReshapeOp>("", "Reshape", 5, 12);
    RegisterOptKernelCreator<ResizeOp>("", "Resize", 11, 12);
    RegisterOptKernelCreator<ROIAlignOp>("", "RoiAlign", 10, 15);
    // S
    RegisterOptKernelCreator<ScatterElementsOp>("", "ScatterElements", 11, 12);
    RegisterOptKernelCreator<ScatterNDOp>("", "ScatterND", 11, 12);
    RegisterOptKernelCreator<SequenceAtOp>("", "SequenceAt", 11, 16);
    RegisterOptKernelCreator<ShapeOp>("", "Shape", 1, 12);
    RegisterOptKernelCreator<SigmoidOp>("", "Sigmoid", 6, 12);
    RegisterOptKernelCreator<SliceOp>("", "Slice", 11, 12);
    RegisterOptKernelCreator<SoftmaxOp>("", "Softmax", 11, 12);
    RegisterOptKernelCreator<SplitOp>("", "Split", 11, 12);
    RegisterOptKernelCreator<SplitToSequenceOp>("", "SplitToSequence", 11, 16);
    RegisterOptKernelCreator<SqrtOp>("", "Sqrt", 6, 12);
    RegisterOptKernelCreator<SqueezeOp>("", "Squeeze", 11, 12);
    RegisterOptKernelCreator<SubOp>("", "Sub", 7, 12);
    // T
    RegisterOptKernelCreator<TanhOp>("", "Tanh", 6, 12);
    RegisterOptKernelCreator<TileOp>("", "Tile", 6, 12);
    RegisterOptKernelCreator<TopKOp>("", "TopK", 11, 16);
    RegisterOptKernelCreator<TransposeOp>("", "Transpose", 1, 12);
    // U
    RegisterOptKernelCreator<UnsqueezeOp>("", "Unsqueeze", 11, 12);
    // W
    RegisterOptKernelCreator<WhereOp>("", "Where", 9, 15);

    // mmcv op domain is "mmcv"
    RegisterOptKernelCreator<MMCVGridSampleOp>("mmcv", "grid_sampler", 1, 1);
    RegisterOptKernelCreator<MMCVROIAlignOp>("mmcv", "MMCVRoiAlign", 1, 1);
    RegisterOptKernelCreator<MMCVModulatedDeformConv2dOp>("mmcv", "MMCVModulatedDeformConv2d", 1, 1);
    RegisterOptKernelCreator<MMCVNonMaxSupressionOp>("mmcv", "NonMaxSuppression", 1, 1);

    // ppl customize op domain is "ppl"
    RegisterOptKernelCreator<BridgeOp>("ppl", "Bridge", 1, 1);
    RegisterOptKernelCreator<ChannelShuffleOp>("ppl", "ChannelShuffle", 1, 1);
    RegisterOptKernelCreator<PPLShapeOperationOp>("ppl", "Shape", 1, 1);
}

}}} // namespace ppl::nn::cuda
