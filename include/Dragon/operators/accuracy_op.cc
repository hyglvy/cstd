#include <algorithm>
#include "operators/misc/accuracy_op.h"
#include "utils/math_functions.h"

namespace dragon {
template <class Context> template <typename T>
void AccuracyOp<Context>::RunWithType() {
    if (OutputSize() > 1) {
        math::Set<T, CPUContext>(num_classes, 0, 
            output(1)->template mutable_data<T, CPUContext>());
    }
    Map<int, int> num_per_class;

    T acc = 0, count = 0;
    auto* Xdata = input(0).template data<T, CPUContext>();
    auto* labels = input(1).template data<T, CPUContext>();
    auto* ignores = ignore_labels.count() > 0 ?
                        ignore_labels.data<int, CPUContext>() : nullptr;
    const TIndex dim = input(0).count() / outer_dim;
    for (int i = 0; i < outer_dim; i++) {
        for (int j = 0; j < inner_dim; j++) {
            const int label = labels[i * inner_dim + j];
            for (int k = 0; k < ignore_labels.count(); k++)
                if (label == ignores[k]) continue;
            if (OutputSize() > 1) num_per_class[label]++;
            vector<pair<T, int> > vec;
            for (int k = 0; k < num_classes; k++)
                vec.push_back(std::make_pair(Xdata[i * dim + k * inner_dim + j], k));
            std::partial_sort(vec.begin(), vec.begin() + top_k, vec.end(), std::greater<pair<T, int> >());
            for (int k = 0; k < top_k; k++) {
                if (vec[k].second == label) {
                    if (OutputSize() > 1)
                        output(1)->template mutable_data<T, CPUContext>()[label]++;
                    acc++;
                    break;
                }
            }
            count++;
        }    //  end inner_dim
    }    // end outer_dim

    output(0)->template mutable_data<T, CPUContext>()[0] = acc / count;
    if (OutputSize() > 1) {
        auto* acc_per_class = output(1)->template mutable_data<T, CPUContext>();
        for (int i = 0; i < num_classes; i++)
            acc_per_class[i] = num_per_class[i] == 0 ? 0 : acc_per_class[i] / acc_per_class[i];
    }
}

template <class Context>
void AccuracyOp<Context>::RunOnDevice() {
    outer_dim = input(0).count(0, axis);
    inner_dim = input(0).count(axis + 1);
    num_classes = input(0).dim(axis);
    CHECK_EQ(outer_dim * inner_dim, input(1).count())
        << "\nGiven (" << outer_dim << "," << inner_dim << ") predictions,"
        << "\nbut provided " << input(1).count() << " labels.";
    output(0)->Reshape(vector<TIndex>(1, 1));
    if (OutputSize() > 1) output(1)->Reshape(vector<TIndex>(1, num_classes)); 

    if (input(0).template IsType<float>()) RunWithType<float>();
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(Accuracy);
#ifdef WITH_CUDA
DEPLOY_CUDA(Accuracy);
#endif
OPERATOR_SCHEMA(Accuracy).NumInputs(2).NumOutputs(1, 2);

NO_GRADIENT(Accuracy);

}    // namespace dragon