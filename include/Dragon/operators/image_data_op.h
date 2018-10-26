// --------------------------------------------------------
// Dragon
// Copyright(c) 2017 SeetaTech
// Written by Ting Pan
// --------------------------------------------------------

#ifndef DRAGON_OPERATORS_MISC_IMAGE_DATA_OP_H_
#define DRAGON_OPERATORS_MISC_IMAGE_DATA_OP_H_

#include "core/operator.h"

namespace dragon {

template <class Context>
class ImageDataOp final : public Operator<Context> {
 public:
    ImageDataOp(const OperatorDef& op_def, Workspace* ws)
         : Operator<Context>(op_def, ws),
           dtype(OperatorBase::GetSingleArg<string>("dtype", "FLOAT32")),
           mean_values(OperatorBase::GetRepeatedArg<float>("mean_values")),
           std_values(OperatorBase::GetRepeatedArg<float>("std_values")),
           data_format(OperatorBase::GetSingleArg<string>("data_format", "NCHW")) {
        if (mean_values.size() > 0) {
            CHECK_EQ((int)mean_values.size(), 3)
                << "The mean values should be a list with length 3.";
            mean.Reshape(vector<TIndex>(1, 3));
            for (int i = 0; i < 3; i++)
                mean.mutable_data<float, CPUContext>()[i] = mean_values[i];
        }
        if (std_values.size() > 0) {
            CHECK_EQ((int)std_values.size(), 3)
                << "The std values should be a list with length 3.";
            std.Reshape(vector<TIndex>(1, 3));
            for (int i = 0; i < 3; i++)
                std.mutable_data<float, CPUContext>()[i] = std_values[i];
        }
    }

    void RunOnDevice() override;
    template <typename Tx, typename Ty> void RunWithType();

 protected:
    string dtype, data_format;
    vector<float> mean_values, std_values;
    TIndex n, c, h, w;
    Tensor mean, std;
};

}    // namespace dragon

#endif    // DRAGON_OPERATORS_MISC_IMAGE_DATA_OP_H_