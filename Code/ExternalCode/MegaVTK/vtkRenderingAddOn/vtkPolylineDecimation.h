#ifndef __vtkPolylineDecimation_h
#define __vtkPolylineDecimation_h

#include <map>
#include <vtkPriorityQueue.h>
#include <vtkPolyDataAlgorithm.h>

#include "MegaVTK2Configure.h"

class VTK_RENDERINGADDON2_EXPORT vtkPolylineDecimation : public vtkPolyDataAlgorithm
  {
public:
  // Description:
  // Standard methods for type information and printing.
  vtkTypeRevisionMacro(vtkPolylineDecimation, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Instantiate this object with a target reduction of 0.90.
  static vtkPolylineDecimation *New();

  // Description:
  // Specify the desired reduction in the total number of polygons (e.g., if
  // TargetReduction is set to 0.9, this filter will try to reduce the data set
  // to 10% of its original size).
  vtkSetClampMacro(TargetReduction, double, 0.0, 1.0);
  vtkGetMacro(TargetReduction, double);

protected:
  vtkPolylineDecimation();
  ~vtkPolylineDecimation();

  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *);

  double ComputeError(vtkPolyData* input, int prev, int id, int next);
  void UpdateError(vtkPolyData* input, const int& iId);

  int GetPrev(const int& iId);
  int GetNext(const int& iId);

  bool                  Closed;
  double                TargetReduction;
  std::map<int, double> VertexErrorMap;
  vtkPriorityQueue*     PriorityQueue;

private:
  vtkPolylineDecimation(const vtkPolylineDecimation&);      // Not implemented.
  void operator =(const vtkPolylineDecimation&);     // Not implemented.
  };
#endif
