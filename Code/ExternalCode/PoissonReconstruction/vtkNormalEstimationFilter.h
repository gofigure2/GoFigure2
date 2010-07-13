// .NAME vtkNormalEstimationFilter - estimate normals from unorganized points
// .SECTION Description
// vtkNormalEstimationFilter takes a list of points assumed to lie on
// the surface of a solid 3D object. This procedure is based on the PhD
// work of Hugues Hoppe: http://www.research.microsoft.com/~hoppe

#ifndef __vtkNormalEstimationFilter_h
#define __vtkNormalEstimationFilter_h

#include "vtkPolyDataAlgorithm.h"

struct SurfacePoint;

class VTK_IMAGING_EXPORT vtkNormalEstimationFilter : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkNormalEstimationFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct with NeighborhoodSize=20.
  static vtkNormalEstimationFilter* New();

  // Description: 
  // Specify the number of neighbors each point has, used for estimating the
  // local surface orientation.  The default value of 20 should be OK for
  // most applications, higher values can be specified if the spread of
  // points is uneven. Values as low as 10 may yield adequate results for
  // some surfaces. Higher values cause the algorithm to take longer. Higher
  // values will cause errors on sharp boundaries.
  vtkGetMacro(NeighborhoodSize,int);
  vtkSetMacro(NeighborhoodSize,int);

protected:
  vtkNormalEstimationFilter();
  ~vtkNormalEstimationFilter() {};
  
  SurfacePoint* m_SurfacePoints;

  virtual int RequestInformation (vtkInformation *, 
                                  vtkInformationVector **, 
                                  vtkInformationVector *);
  virtual int RequestData (vtkInformation *, 
                           vtkInformationVector **, 
                           vtkInformationVector *);

  int NeighborhoodSize;
  
  virtual int FillInputPortInformation(int, vtkInformation*);
  
  void BuildLocalConnectivity( vtkDataSet* input );
  void EstimatePlanes( vtkDataSet* input );
  void ComputeCostForMST( vtkDataSet* input );
  int ConsistencyPropagation();

private:
  vtkNormalEstimationFilter(const vtkNormalEstimationFilter&);  // Not implemented.
  void operator=(const vtkNormalEstimationFilter&);  // Not implemented.
};

#endif

