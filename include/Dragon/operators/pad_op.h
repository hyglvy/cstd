// --------------------------------------------------------
// Dragon
// Copyright(c) 2017 SeetaTech
// Written by Ting Pan
// --------------------------------------------------------

#ifndef DRAGON_OPERATORS_NDARRAY_PAD_OP_H_
#define DRAGON_OPERATORS_NDARRAY_PAD_OP_H_

#include "core/operator.h"

namespace dragon {

template <class Context>
class PadOp final : public Operator<Context> {
 public:
    PadOp(const OperatorDef& op_def, Workspace* ws)
        : Operator<Context>(op_def, ws),
          pad_l(OperatorBase::GetRepeatedArg<int>("pad_l")),
          pad_r(OperatorBase::GetRepeatedArg<int>("pad_r")),
          mode(OperatorBase::GetSingleArg<string>("mode", "CONSTANT")), 
          value(OperatorBase::GetSingleArg<float>("value", 0.0f)) {
        if (pad_r.size() == 0) pad_r = pad_l;
        else CHECK_EQ(pad_l.size(), pad_r.size())
            << "The pad_l and pad_r should have the same length.";
        for (int i = 0; i < pad_l.size(); i++) {
            int padding_size = pad_l[i] + pad_r[i];
            if (padding_size > 0) 
                process_axes.push_back({ padding_size, i });
        }
        std::sort(process_axes.begin(), process_axes.end());
    }

    void RunOnDevice() override;
    template <typename T> void ConstRunWithType();
    template <typename T> void ReflectRunWithType();
    template <typename T> void EdgeRunWithType();

 protected:
    vector<int> pad_l, pad_r;
    string mode;
    float value;
    vector< pair<int, int> > process_axes;
    TIndex axis, inner_dim, dim;
    Tensor* dest, *source;
};

template <class Context>
class PadGradientOp final : public Operator<Context> {
 public:
    PadGradientOp(const OperatorDef& op_def, Workspace* ws)
        : Operator<Context>(op_def, ws),    
          pad_l(OperatorBase::GetRepeatedArg<int>("pad_l")),
          pad_r(OperatorBase::GetRepeatedArg<int>("pad_r")),
          mode(OperatorBase::GetSingleArg<string>("mode", "CONSTANT")) {
        if (pad_r.size() == 0) pad_r = pad_l;
        else CHECK_EQ(pad_l.size(), pad_r.size())
            << "The pad_l and pad_r should have the same length.";
        for (int i = 0; i < pad_l.size(); i++) {
            int padding_size = pad_l[i] + pad_r[i];
            if (padding_size > 0) 
                process_axes.push_back({ padding_size, i });
        }
        std::sort(process_axes.begin(), process_axes.end());
        std::reverse(process_axes.begin(), process_axes.end());
        DISABLE_SHARE_GRADIENT;
    }

    void RunOnDevice() override;
    template <typename T> void ConstRunWithType();
    template <typename T> void ReflectRunWithType();
    template <typename T> void EdgeRunWithType();

 protected:
    vector<int> pad_l, pad_r;
    string mode;
    vector< pair<int, int> > process_axes;
    TIndex axis, inner_dim, dim;
    Tensor* dest, *source;
};

}    // namespace dragon

#endif    // DRAGON_OPERATORS_NDARRAY_PAD_OP_H_