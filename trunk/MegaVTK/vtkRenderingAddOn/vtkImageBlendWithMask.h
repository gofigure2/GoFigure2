#ifndef __vtkImageBlendWithMask_h
#define __vtkImageBlendWithMask_h

#include "vtkThreadedImageAlgorithm.h"
#include "vtkLookupTable.h"

class vtkImageBlendWithMask :
  public vtkThreadedImageAlgorithm
{
 public:
  static vtkImageBlendWithMask *New();
  vtkTypeRevisionMacro (vtkImageBlendWithMask, vtkThreadedImageAlgorithm);
  void PrintSelf (ostream &os, vtkIndent indent);


  // Set/Get the LUT to map the mask
  vtkSetObjectMacro (LookupTable, vtkLookupTable);
  vtkGetObjectMacro (LookupTable, vtkLookupTable);




  // Description:
  // Set the input to be masked.
  void SetImageInput(vtkImageData *in);

  // Description:
  // Set the mask to be used.
  void SetMaskInput(vtkImageData *in);

  // Description:
  // Set the two inputs to this filter
  virtual void SetInput1(vtkDataObject *in) { this->SetInput(0,in); }
  virtual void SetInput2(vtkDataObject *in) { this->SetInput(1,in); }

 protected:
  vtkImageBlendWithMask();
  ~vtkImageBlendWithMask();

  vtkLookupTable* LookupTable;

  virtual int RequestInformation (vtkInformation *,
                                  vtkInformationVector **,
                                  vtkInformationVector *);


  virtual void ThreadedRequestData(vtkInformation *request,
                                   vtkInformationVector **inputVector,
                                   vtkInformationVector *outputVector,
                                   vtkImageData ***inData,
                                   vtkImageData **outData,
                                   int extent[6], int threadId);


 private:
  vtkImageBlendWithMask (const vtkImageBlendWithMask&);
  void operator=(const vtkImageBlendWithMask&);

};



#endif
