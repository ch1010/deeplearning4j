/*******************************************************************************
 * Copyright (c) 2015-2018 Skymind, Inc.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License, Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/

//
// Created by raver119 on 01.11.2017.
// Modified by GS <sgazeos@gmail.com> 4/5/2018

#include <op_boilerplate.h>
#if NOT_EXCLUDED(OP_argmax)

#include <ops/declarable/helpers/axis.h>
#include <ops/declarable/CustomOperations.h>
#include <helpers/ConstantTadHelper.h>

namespace nd4j {
    namespace ops {
        DECLARE_TYPES(argmax) {
            getOpDescriptor()
                    ->setAllowedInputTypes(nd4j::DataType::ANY)
                    ->setAllowedOutputTypes({ALL_INTS});
        }

        CUSTOM_OP_IMPL(argmax, 1, 1, false, 0, -2) {
            auto input = INPUT_VARIABLE(0);
            auto output = OUTPUT_VARIABLE(0);

            auto axis = *block.getIArguments();

            // axis might be dynamic (i.e. tf mode)
            if (block.width() > 1 && axis.size() == 0) {
                auto axisVector = INPUT_VARIABLE(1);
                helpers::adjustAxis(input->rankOf(), axisVector, axis);

                input->applyIndexReduce(indexreduce::IndexMax, output, axis);
            } else {
                helpers::adjustAxis(input->rankOf(), axis);

                input->applyIndexReduce(indexreduce::IndexMax, output, axis);
            }

            STORE_RESULT(output);

            return Status::OK();
        }

        DECLARE_SHAPE_FN(argmax) {
            std::vector<int> dims;

            if (block.width() == 1) {
                dims = *block.getIArguments();
            } else {
                auto y = INPUT_VARIABLE(1);
                dims = y->template asVectorT<int>();
            }

            // we're resolving negative axis here
            helpers::adjustAxis(shape::rank(inputShape->at(0)), dims);

            if (dims.size() > 1)
                std::sort(dims.begin(), dims.end());


            for (auto d:dims) {
                REQUIRE_TRUE(inputShape->at(0)[d+1] != 0, 0, "ArgMax: you can't reduce along axis with 0 in shape");
            }

            // special case - output is scalar
            if (dims.size() == 0 || (dims.size() == 1 && dims.at(0) == MAX_INT)) {
                return SHAPELIST(ConstantShapeHelper::getInstance()->scalarShapeInfo(nd4j::DataType::INT64));
            }

            return SHAPELIST(ShapeUtils::evalReduceShapeInfo('c', dims, inputShape->at(0), DataType::INT64, false, false, block.getWorkspace()));
        }
    }
}

#endif
