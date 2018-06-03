

#ifndef LAYER_CONVOLUTIONDEPTHWISE_H
#define LAYER_CONVOLUTIONDEPTHWISE_H

#include "layer.h"
#include "convolution.h"

namespace ncnn
{

struct ConvolutionDepthWise : public Convolution
{
public:
  ConvolutionDepthWise();
  virtual ~ConvolutionDepthWise();

  virtual int load_param(const ParamDict& pd);

  virtual int forward(const img_t* bottom_blob, img_t* top_blob) const;

public:
  int group;
};

} // namespace ncnn

#endif // LAYER_CONVOLUTIONDEPTHWISE_H