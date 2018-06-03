
#ifndef vtkGaussianRandomSequence_h
#define vtkGaussianRandomSequence_h

#include "vtkRandomSequence.h"

class VTKCOMMONCORE_EXPORT vtkGaussianRandomSequence : public vtkRandomSequence
{
public:
  vtkTypeMacro(vtkGaussianRandomSequence,vtkRandomSequence);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /**
   * Convenient method to return a value given the mean and standard deviation
   * of the Gaussian distribution from the the Gaussian distribution of mean=0
   * and standard deviation=1.0. There is an initial implementation that can
   * be overridden by a subclass.
   */
  virtual double GetScaledValue(double mean,
                                double standardDeviation);

  vtkGaussianRandomSequence();
  ~vtkGaussianRandomSequence() VTK_OVERRIDE;
private:
  vtkGaussianRandomSequence(const vtkGaussianRandomSequence&) VTK_DELETE_FUNCTION;
  void operator=(const vtkGaussianRandomSequence&) VTK_DELETE_FUNCTION;
};

#endif // #ifndef vtkGaussianRandomSequence_h