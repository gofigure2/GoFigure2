#include "vtkNormalEstimationFilter.h"

#include "vtkFloatArray.h"
#include "vtkIdList.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkPointLocator.h"
#include "vtkPoints.h"
#include "vtkDoubleArray.h"

vtkStandardNewMacro(vtkNormalEstimationFilter);

vtkNormalEstimationFilter::vtkNormalEstimationFilter()
  {
  this->NeighborhoodSize = 20;
  }

// some simple routines for vector math
void vtkCopyBToA(double* a, double* b)
{
  for (int i = 0; i < 3; i++)
    {
    a[i] = b[i];
    }
}
void vtkSubtractBFromA(double* a, double* b)
{
  for (int i = 0; i < 3; i++)
    {
    a[i] -= b[i];
    }
}
void vtkAddBToA(double* a, double* b)
{
  for (int i = 0; i < 3; i++)
    {
    a[i] += b[i];
    }
}
void vtkMultiplyBy(double* a, double f)
{
  for (int i = 0; i < 3; i++)
    {
    a[i] *= f;
    }
}
void vtkDivideBy(double* a, double f)
{
  for (int i = 0; i < 3; i++)
    {
    a[i] /= f;
    }
}

// Routines for matrix creation
void vtkSRFreeMatrix(double **m, long nrl, long nrh, long ncl, long nch);
double **vtkSRMatrix(long nrl, long nrh, long ncl, long nch);
void vtkSRFreeVector(double *v, long nl, long nh);
double *vtkSRVector(long nl, long nh);

// set a matrix to zero
void vtkSRMakeZero(double **m, long nrl, long nrh, long ncl, long nch)
{
  int i, j;
  for (i = nrl; i <= nrh; i++)
    {
    for (j = ncl; j <= nch; j++)
      {
      m[i][j] = 0.0;
      }
    }
}

// add v*Transpose(v) to m, where v is 3x1 and m is 3x3
void vtkSRAddOuterProduct(double **m, double *v);

// scalar multiply a matrix
void vtkSRMultiply(double **m, double f, long nrl, long nrh, long ncl, long nch)
{
  int i, j;
  for (i = nrl; i <= nrh; i++)
    {
    for (j = ncl; j <= nch; j++)
      {
      m[i][j] *= f;
      }
    }
}

//----------------------------------------------------------------------------
int vtkNormalEstimationFilter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

int vtkNormalEstimationFilter::RequestInformation(
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector ** vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // would be nice to compute the whole extent but we need more info to
  // compute it.
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 0, 1, 0, 1, 0, 1);

  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_FLOAT, 1);
  return 1;
}

//-----------------------------------------------------------------------------
struct SurfacePoint
  {
  double loc[3];
  double o[3], n[3]; // plane centre and normal
  vtkIdList *neighbors; // id's of points within LocalRadius of this point
  double *costs; // should have same length as neighbors, cost for corresponding points
  char isVisited;

  // simple constructor to initialise the members
  SurfacePoint() : neighbors(vtkIdList::New()), isVisited(0) {}
  ~SurfacePoint() { delete[] costs; neighbors->Delete(); }
  };

void vtkNormalEstimationFilter::BuildLocalConnectivity(vtkDataSet* input)
{
  vtkPointLocator *locator = vtkPointLocator::New();
  locator->SetDataSet(input);
  vtkIdList *locals = vtkIdList::New();

  const vtkIdType COUNT = input->GetNumberOfPoints();

  // if a pair is close, add each one as a neighbor of the other
  for (vtkIdType i = 0; i < COUNT; i++)
    {
    SurfacePoint *p = &m_SurfacePoints[i];
    vtkCopyBToA(p->loc, input->GetPoint(i));
    locator->FindClosestNPoints(this->NeighborhoodSize, p->loc, locals);
    int iNeighbor;
    for (vtkIdType j = 0; j < locals->GetNumberOfIds(); j++)
      {
      iNeighbor = locals->GetId(j);
      if (iNeighbor != i)
        {
        p->neighbors->InsertNextId(iNeighbor);
        m_SurfacePoints[iNeighbor].neighbors->InsertNextId(i);
        }
      }
    }
  locator->Delete();
  locals->Delete();
}

void vtkNormalEstimationFilter::EstimatePlanes(vtkDataSet* input)
{
  double * pointi;
  double **covar, *v3d, *eigenvalues, **eigenvectors;
  covar = vtkSRMatrix(0, 2, 0, 2);
  v3d = vtkSRVector(0, 2);
  eigenvalues = vtkSRVector(0, 2);
  eigenvectors = vtkSRMatrix(0, 2, 0, 2);

  const vtkIdType COUNT = input->GetNumberOfPoints();

  vtkIdType i, j;
  int       k;

  for (i = 0; i < COUNT; i++)
    {
    SurfacePoint *p = &m_SurfacePoints[i];

    // first find the centroid of the neighbors
    vtkCopyBToA(p->o, p->loc);
    int       number = 1;
    vtkIdType neighborIndex;
    for (j = 0; j < p->neighbors->GetNumberOfIds(); j++)
      {
      neighborIndex = p->neighbors->GetId(j);
      pointi = input->GetPoint(neighborIndex);
      vtkAddBToA(p->o, pointi);
      number++;
      }
    vtkDivideBy(p->o, number);
    // then compute the covariance matrix
    vtkSRMakeZero(covar, 0, 2, 0, 2);
    for (k = 0; k < 3; k++)
      v3d[k] = p->loc[k] - p->o[k];
    vtkSRAddOuterProduct(covar, v3d);
    for (j = 0; j < p->neighbors->GetNumberOfIds(); j++)
      {
      neighborIndex = p->neighbors->GetId(j);
      pointi = input->GetPoint(neighborIndex);
      for (k = 0; k < 3; k++)
        {
        v3d[k] = pointi[k] - p->o[k];
        }
      vtkSRAddOuterProduct(covar, v3d);
      }
    vtkSRMultiply(covar, 1.0 / number, 0, 2, 0, 2);
    // then extract the third eigenvector
    vtkMath::Jacobi(covar, eigenvalues, eigenvectors);
    // third eigenvector (column 2, ordered by eigenvalue magnitude) is plane normal
    for (k = 0; k < 3; k++)
      {
      p->n[k] = eigenvectors[k][2];
      }
    }
  vtkSRFreeMatrix(covar, 0, 2, 0, 2);
  vtkSRFreeVector(v3d, 0, 2);
  vtkSRFreeVector(eigenvalues, 0, 2);
  vtkSRFreeMatrix(eigenvectors, 0, 2, 0, 2);
}

void vtkNormalEstimationFilter::ComputeCostForMST(vtkDataSet* input)
{
  const vtkIdType COUNT = input->GetNumberOfPoints();

  vtkIdType i, j;
  for (i = 0; i < COUNT; i++)
    {
    SurfacePoint *p = &m_SurfacePoints[i];
    p->costs = new double[p->neighbors->GetNumberOfIds()];

    // compute cost between all its neighbors
    // (bit inefficient to do this for every point, as cost is symmetric)
    for (j = 0; j < p->neighbors->GetNumberOfIds(); j++)
      {
      p->costs[j] = 1.0 -
                    fabs(vtkMath::Dot(p->n, m_SurfacePoints[p->neighbors->GetId(j)].n));
      }
    }
}

int vtkNormalEstimationFilter::ConsistencyPropagation()
{
  int       orientationPropagation = 1;
  vtkIdType i, j;
  if (orientationPropagation)
    { // set to false if you don't want orientation propagation (for testing)
    vtkIdList *nearby = vtkIdList::New(); // list of nearby, unvisited points

    // start with some vertex
    int first = 0; // index of starting vertex
    m_SurfacePoints[first].isVisited = 1;
    // add all the neighbors of the starting vertex into nearby
    for (j = 0; j < m_SurfacePoints[first].neighbors->GetNumberOfIds(); j++)
      {
      nearby->InsertNextId(m_SurfacePoints[first].neighbors->GetId(j));
      }

    double cost, lowestCost;
    int    cheapestNearby = 0, connectedVisited = 0;

    // repeat until nearby is empty:
    while (nearby->GetNumberOfIds() > 0)
      {
      // for each nearby point:
      vtkIdType iNearby, iNeighbor;
      lowestCost = VTK_DOUBLE_MAX;
      for (i = 0; i < nearby->GetNumberOfIds(); i++)
        {
        iNearby = nearby->GetId(i);
        // test cost against all neighbors that are members of visited
        for (j = 0; j < m_SurfacePoints[iNearby].neighbors->GetNumberOfIds(); j++)
          {
          iNeighbor = m_SurfacePoints[iNearby].neighbors->GetId(j);
          if (m_SurfacePoints[iNeighbor].isVisited)
            {
            cost = m_SurfacePoints[iNearby].costs[j];
            // pick lowest cost for this nearby point
            if (cost < lowestCost)
              {
              lowestCost = cost;
              cheapestNearby = iNearby;
              connectedVisited = iNeighbor;
              // optional: can break out if satisfied with parallelness
              if (lowestCost < 0.1)
                {
                i = nearby->GetNumberOfIds();
                break;
                }
              }
            }
          }
        }
      if (connectedVisited == cheapestNearby)
        {
        vtkErrorMacro (<< "Internal error in vtkNormalEstimationFilter");
        nearby->Delete();
        return 0;
        }

      // correct the orientation of the point if necessary
      if (vtkMath::Dot(m_SurfacePoints[cheapestNearby].n,
                       m_SurfacePoints[connectedVisited].n) < 0.0F)
        {
        // flip this normal
        vtkMultiplyBy(m_SurfacePoints[cheapestNearby].n, -1);
        }
      // add this nearby point to visited
      if (m_SurfacePoints[cheapestNearby].isVisited != 0)
        {
        vtkErrorMacro (<< "Internal error in vtkNormalEstimationFilter");
        nearby->Delete();
        return 0;
        }

      m_SurfacePoints[cheapestNearby].isVisited = 1;
      // remove from nearby
      nearby->DeleteId(cheapestNearby);
      // add all new nearby points to nearby
      for (j = 0; j < m_SurfacePoints[cheapestNearby].neighbors->GetNumberOfIds(); j++)
        {
        iNeighbor = m_SurfacePoints[cheapestNearby].neighbors->GetId(j);
        if (m_SurfacePoints[iNeighbor].isVisited == 0)
          {
          nearby->InsertUniqueId(iNeighbor);
          }
        }
      }

    nearby->Delete();
    }
  return 1;
}
//-----------------------------------------------------------------------------
int vtkNormalEstimationFilter::RequestData(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  // get the input
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataSet *    input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));

  // get the output
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkPolyData *   output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  const vtkIdType COUNT = input->GetNumberOfPoints();

  if (COUNT < 1)
    {
    vtkErrorMacro(<< "No points to reconstruct");
    return 1;
    }
  m_SurfacePoints = new SurfacePoint[COUNT];

  vtkDebugMacro(<< "Reconstructing " << COUNT << " points");

  //time_t start_time,t1,t2,t3,t4;
  //time(&start_time);

  // --------------------------------------------------------------------------
  // 1. Build local connectivity graph
  // -------------------------------------------------------------------------
  BuildLocalConnectivity(input);

  //time(&t1);
  // --------------------------------------------------------------------------
  // 2. Estimate a plane at each point using local points
  // --------------------------------------------------------------------------
  EstimatePlanes(input);

  //time(&t2);
  //--------------------------------------------------------------------------
  // 3a. Compute a cost between every pair of neighbors for the MST
  // --------------------------------------------------------------------------
  // cost = 1 - |normal1.normal2|
  // ie. cost is 0 if planes are parallel, 1 if orthogonal (least parallel)
  ComputeCostForMST(input);

  // --------------------------------------------------------------------------
  // 3b. Ensure consistency in plane direction between neighbors
  // --------------------------------------------------------------------------
  // method: guess first one, then walk through tree along most-parallel
  // neighbors MST, flipping the new normal if inconsistent

  // to walk minimal spanning tree, keep record of vertices visited and list
  // of those near to any visited point but not themselves visited. Start
  // with just one vertex as visited.  Pick the vertex in the neighbors list
  // that has the lowest cost connection with a visited vertex. Record this
  // vertex as visited, add any new neighbors to the neighbors list.
  int ok = ConsistencyPropagation();

  vtkPoints* points = vtkPoints::New();
  points->SetNumberOfPoints(input->GetNumberOfPoints());

  vtkFloatArray* normals = vtkFloatArray::New();
  normals->SetNumberOfComponents(3);
  normals->SetNumberOfTuples(input->GetNumberOfPoints());
  normals->SetName("normals");

  for (vtkIdType i = 0; i < input->GetNumberOfPoints(); i++)
    {
    points->SetPoint(i, input->GetPoint(i));
    normals->SetTuple(i, m_SurfacePoints[i].n);
    }

  output->SetPoints(points);
  output->GetPointData()->SetNormals(normals);

  normals->Delete();
//   delete[] m_SurfacePoints;

  return ok;
}

void vtkNormalEstimationFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Neighborhood Size:" << this->NeighborhoodSize << "\n";
}

void vtkSRAddOuterProduct(double **m, double *v)
{
  int i, j;
  for (i = 0; i < 3; i++)
    {
    for (j = 0; j < 3; j++)
      {
      m[i][j] += v[i] * v[j];
      }
    }
}

#define VTK_NR_END 1
#define VTK_FREE_ARG char*

// allocate a float vector with subscript range v[nl..nh]
double *vtkSRVector(long nl, long nh)
{
  double *v;

  v = new double[nh - nl + 1 + VTK_NR_END];
  if (!v)
    {
    vtkGenericWarningMacro(<< "allocation failure in vector()");
    return NULL;
    }

  return (v - nl + VTK_NR_END);
}

// allocate a float matrix with subscript range m[nrl..nrh][ncl..nch]
double **vtkSRMatrix(long nrl, long nrh, long ncl, long nch)
{
  long     i, nrow = nrh - nrl + 1, ncol = nch - ncl + 1;
  double **m;

  // allocate pointers to rows
  m = new double *[nrow + VTK_NR_END];
  if (!m)
    {
    vtkGenericWarningMacro(<< "allocation failure 1 in Matrix()");
    return NULL;
    }

  m += VTK_NR_END;
  m -= nrl;

  // allocate rows and set pointers to them
  m[nrl] = new double[nrow * ncol + VTK_NR_END];
  if (!m[nrl])
    {
    vtkGenericWarningMacro("allocation failure 2 in Matrix()");
    return NULL;
    }

  m[nrl] += VTK_NR_END;
  m[nrl] -= ncl;
  for (i = nrl + 1; i <= nrh; i++)
    {
    m[i] = m[i - 1] + ncol;
    }

  // return pointer to array of pointers to rows
  return m;
}

// free a double vector allocated with SRVector()
void vtkSRFreeVector(double *v, long nl, long vtkNotUsed(nh))
{
  delete[] (v + nl - VTK_NR_END);
}

// free a double matrix allocated by Matrix()
void vtkSRFreeMatrix(double **m, long nrl, long vtkNotUsed(nrh),
                     long ncl, long vtkNotUsed(nch))

{
  delete[] (m[nrl] + ncl - VTK_NR_END);
  delete[] (m + nrl - VTK_NR_END);
}

#undef VTK_NR_END
#undef VTK_FREE_ARG
