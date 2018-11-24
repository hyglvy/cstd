/*
TODO:
- only load parts of the file, in accordance with a prototxt param "max_mem"
*/

#include <stdint.h>


#include "hdf5.h"
#include "hdf5_hl.h"

#include "caffe/layers/hdf5_data_layer.hpp"

namespace {

template <typename Dtype>
void HDF5DataLayer::Forward_(GPUContext* context, const vector<Blob*>& bottom,
      const vector<Blob*>& top) {
  const int batch_size = this->param_->hdf5_data_param().batch_size();
  for (int i = 0; i < batch_size; ++i, ++current_row_) {
    if (current_row_ == hdf_blobs_[0]->shape(0)) {
      if (num_files_ > 1) {
        current_file_ += 1;
        if (current_file_ == num_files_) {
          current_file_ = 0;
          if (this->param_->hdf5_data_param().shuffle()) {
            std::random_shuffle(file_permutation_.begin(),
                                file_permutation_.end());
          }
          DLOG(INFO) << "Looping around to first file.";
        }
        LoadHDF5FileData(
            hdf_filenames_[file_permutation_[current_file_]].c_str());
      }
      current_row_ = 0;
      if (this->param_->hdf5_data_param().shuffle())
        std::random_shuffle(data_permutation_.begin(), data_permutation_.end());
    }
    for (int j = 0; j < this->param_->top_size(); ++j) {
      int data_dim = top[j]->count() / top[j]->shape(0);
      caffe_copy(data_dim,
          &hdf_blobs_[j]->data()[data_permutation_[current_row_]
            * data_dim], &top[j]->mdata()[i * data_dim]);
    }
  }
}

INSTANTIATE_LAYER_GPU_FUNCS(HDF5DataLayer);

}  // namespace
