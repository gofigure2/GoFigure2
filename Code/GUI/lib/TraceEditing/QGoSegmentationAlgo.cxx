/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "QGoSegmentationAlgo.h"

// Extract ROI
#include "vtkExtractVOI.h"

// extract polydata 2d
#include "vtkMarchingSquares.h"
//reorder contour
#include "vtkStripper.h"
#include "vtkCellArray.h"
// extract polydata 3d
#include "vtkContourFilter.h"
#include "vtkFeatureEdges.h"
#include "vtkFillHolesFilter.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkWindowedSincPolyDataFilter.h"

#include "vtkMath.h"

// Decimation 2d
#include "vtkPolylineDecimation.h"
// Decimation 3d
#include "vtkDecimatePro.h"


// test code
#include <assert.h>


QGoSegmentationAlgo::QGoSegmentationAlgo(QWidget *iParent)
  : QObject( iParent ), m_Decimate(true),
    m_NumberOfPoints(100), m_AlgoWidget(NULL)
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoSegmentationAlgo::~QGoSegmentationAlgo()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoAlgorithmWidget* QGoSegmentationAlgo::GetAlgoWidget()
{
  return this->m_AlgoWidget;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkImageData*>
QGoSegmentationAlgo::VTKExtractROI(
  const std::vector<double>& iBounds,
  const std::vector< vtkSmartPointer< vtkImageData > > & iImages)
{
  assert( iBounds.size() == 6 );

  // vector to be returned
  std::vector<vtkImageData*> listOfImages;

  //iterator on the images
  std::vector< vtkSmartPointer< vtkImageData > >::const_iterator
      it = iImages.begin();

  while( it != iImages.end())
    {
    listOfImages.push_back( VTKExtractROI(iBounds, *it) );
    ++it;
    }

  return listOfImages;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkImageData*
QGoSegmentationAlgo::
VTKExtractROI(const std::vector<double>& iBounds,
            const vtkSmartPointer< vtkImageData > & iImage)
{
  // make sure there
  assert( iBounds.size() == 6 );

  double org[3];
  iImage->GetOrigin( org );

  double spacing[3];
  iImage->GetSpacing( spacing );

  std::vector< int > bounds_idx(6);

  int k = 0;
  for( int i = 0; i < 3; i++ )
    {
    bounds_idx[k] = vtkMath::Round( ( iBounds[k] - org[i] ) / spacing[i] );
    ++k;
    bounds_idx[k] = vtkMath::Round( ( iBounds[k] - org[i] ) / spacing[i] );
    ++k;
    }

  vtkSmartPointer<vtkExtractVOI> extractVOI =
      vtkSmartPointer<vtkExtractVOI>::New();
  extractVOI->SetInput( iImage );
  extractVOI->SetVOI( bounds_idx[0], bounds_idx[1],
                      bounds_idx[2], bounds_idx[3],
                      bounds_idx[4], bounds_idx[5]);
  extractVOI->Update();

  vtkImageData* temp_image = vtkImageData::New();
  temp_image->DeepCopy( extractVOI->GetOutput() );

  return temp_image;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkPolyData*>
QGoSegmentationAlgo::
ExtractPolyData(
  std::vector<vtkImageData*>& iInputImage,
  const double & iThreshold)
{
  // vector to be returned
  std::vector<vtkPolyData*> listOfPolys;

  //iterator on the images
  std::vector<vtkImageData*>::iterator it = iInputImage.begin();

  while( it != iInputImage.end())
    {
    vtkPolyData* extractedPolyData = ExtractPolyData(*it, iThreshold);
    if( extractedPolyData )
      {
      listOfPolys.push_back( extractedPolyData );
      }
    ++it;
    }

  return listOfPolys;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSegmentationAlgo::
ExtractPolyData(vtkImageData *iInputImage, const double & iThreshold)
{
  // make sure there is an image
  assert( iInputImage );

  // test dimension of the input
  int extent[6];
  iInputImage->GetExtent( extent );

  int dimension = 3;

  for( int i = 0; i < 3; i++ )
    {
    if( extent[2*i] == extent[2*i+1] )
      {
      --dimension;
      }
    }

  std::cout << "dimension here: " << dimension << std::endl;
  switch ( dimension )
    {
      case 2 :
        return ExtractContour( iInputImage, iThreshold);
      case 3 :
        return ExtractMesh( iInputImage, iThreshold);
      default :
        std::cout << "dimension unknown (Reconstruct polydata)" << std::endl;
      }

  return NULL;
}
//-------------------------------------------------------------------------

/*
 * \todo Nicolas-do a better reconstruction..
 */
//--------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSegmentationAlgo::
ExtractContour( vtkSmartPointer<vtkImageData> iInputImage, const double & iThreshold)
{
  // create iso-contours
  vtkSmartPointer<vtkMarchingSquares> contours =
      vtkSmartPointer<vtkMarchingSquares>::New();

  contours->SetInput(iInputImage);
  contours->GenerateValues (1, iThreshold, iThreshold);
  contours->Update();

  return ReorganizeContour( contours->GetOutput() );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSegmentationAlgo::
ReorganizeContour(vtkSmartPointer<vtkPolyData> iInputImage)
{
  /*
   * \note Nicolas-to be enhanced
   */
  // Create reorganize contours
  vtkStripper *stripper = vtkStripper::New();

  stripper->SetInput(iInputImage);
  //Is it useful?? Which number is the best suited?
  stripper->SetMaximumLength(999);
  stripper->Update();

  // Reorder points
  stripper->GetOutput()->GetLines()->InitTraversal();

  // npts = nb of points in the line
  // *pts = pointer to each point

  vtkIdType *pts = NULL;
  vtkIdType  npts = 0;
  stripper->GetOutput()->GetLines()->GetNextCell(npts, pts);
  vtkPoints *points = vtkPoints::New();

  vtkCellArray *lines       = vtkCellArray::New();
  vtkIdType *   lineIndices = new vtkIdType[static_cast< int >( npts + 1 )];

  for ( int k = 0; k < static_cast< int >( npts ); k++ )
    {
    points->InsertPoint( k, stripper->GetOutput()->GetPoints()->GetPoint(pts[k]) );
    lineIndices[k] = k;
    }

  lineIndices[static_cast< int >( npts )] = 0;
  lines->InsertNextCell(npts + 1, lineIndices);
  delete[] lineIndices;

  vtkSmartPointer<vtkPolyData> output = vtkSmartPointer<vtkPolyData>::New();
  output->SetPoints(points);
  output->SetLines(lines);

  lines->Delete();
  points->Delete();
  stripper->Delete();

  return output;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSegmentationAlgo::
ExtractMesh(vtkSmartPointer<vtkImageData> iInputImage, const double & iThreshold)
{
  vtkSmartPointer< vtkContourFilter > contours = vtkSmartPointer< vtkContourFilter >::New();
  contours->SetInput(iInputImage);
  contours->SetComputeGradients(0);
  contours->SetComputeNormals(0);
  contours->SetComputeScalars(0);
  contours->SetNumberOfContours(1);
  contours->SetValue(0, iThreshold);
  contours->Update();

  std::cout << "Number Of Points: " << iInputImage->GetNumberOfPoints()
            << std::endl;

  vtkSmartPointer< vtkFeatureEdges > feature =
      vtkSmartPointer< vtkFeatureEdges >::New();
  feature->SetInputConnection( contours->GetOutputPort() );
  feature->BoundaryEdgesOn();
  feature->FeatureEdgesOff();
  feature->NonManifoldEdgesOn();
  feature->ManifoldEdgesOff();
  feature->Update();

  vtkSmartPointer< vtkFillHolesFilter > fillFilter =
      vtkSmartPointer< vtkFillHolesFilter >::New();

  vtkSmartPointer< vtkPolyDataConnectivityFilter > connectivityFilter =
      vtkSmartPointer< vtkPolyDataConnectivityFilter >::New();
  connectivityFilter->SetExtractionModeToLargestRegion();


  if ( feature->GetOutput()->GetNumberOfCells() > 0 )
    {
    // fill holes if any!
    fillFilter->SetInputConnection( contours->GetOutputPort() );
    fillFilter->Update();

    connectivityFilter->SetInputConnection( fillFilter->GetOutputPort() );
    }
  else
    {
    connectivityFilter->SetInputConnection( contours->GetOutputPort() );
    }

  // keep the largest region
  connectivityFilter->Update();
  /*
   * \note Nicolas-Smoother not connected
   */
/*
  unsigned int smoothingIterations = 15;
  double passBand = 0.001;
  double featureAngle = 120.0;

  // smoothing
  vtkSmartPointer< vtkWindowedSincPolyDataFilter > smoother =
    vtkSmartPointer< vtkWindowedSincPolyDataFilter >::New();
  smoother->SetInputConnection( connectivityFilter->GetOutputPort() );
  smoother->SetNumberOfIterations( smoothingIterations );
  smoother->BoundarySmoothingOff();
  smoother->FeatureEdgeSmoothingOff();
  smoother->SetFeatureAngle(featureAngle);
  smoother->SetPassBand(passBand);
  smoother->NonManifoldSmoothingOn();
  smoother->NormalizeCoordinatesOn();
  smoother->Update();
*/
  vtkPolyData* output = vtkPolyData::New();
  output->DeepCopy( connectivityFilter->GetOutput() );

  return output;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSegmentationAlgo::
DecimatePolyData(
  vtkSmartPointer<vtkPolyData>& iPolyData,
  const unsigned int& iNumberOfPoints)
{
  // make sure there is a polydata
  assert( iPolyData );

  /*
   * \todo Nicolas-might be better solution to test dimension of a PolyData
   */
  // test dimension of the input
  double* bounds = iPolyData->GetBounds();
  int dimension = 3;

  for( int i = 0; i < 3; i++ )
    {
    if( bounds[2*i] == bounds[2*i+1] )
      {
      --dimension;
      }
    }

  switch ( dimension )
    {
    case 2 :
      return DecimateContour( iPolyData, iNumberOfPoints);
    case 3 :
      return DecimateMesh( iPolyData, iNumberOfPoints);
    default :
      itkGenericExceptionMacro( << "dimension unknown (Reconstruct polydata)" );
    }

  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSegmentationAlgo::
DecimateContour( vtkSmartPointer<vtkPolyData> iPolyData, unsigned int iNumberOfPoints)
{
  // define target reduction
  unsigned int numberOfPoints = iPolyData->GetNumberOfPoints();

  double target = 1 - (double)iNumberOfPoints/(double)numberOfPoints;

  vtkSmartPointer<vtkPolylineDecimation> decimator =
      vtkSmartPointer<vtkPolylineDecimation>::New();
  decimator->SetInput(iPolyData);
  decimator->SetTargetReduction(target);
  decimator->Update();

  return decimator->GetOutput();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSegmentationAlgo::
DecimateMesh( vtkSmartPointer<vtkPolyData> iPolyData, unsigned int iNumberOfPoints)
{
  // define target reduction
  unsigned int numberOfPoints = iPolyData->GetNumberOfPoints();

  double target = 1 - (double)iNumberOfPoints/(double)numberOfPoints;

  vtkSmartPointer<vtkDecimatePro> decimator =
      vtkSmartPointer<vtkDecimatePro>::New();
  decimator->SetInput(iPolyData);
  decimator->SetTargetReduction(target);
  decimator->Update();

  /*
   * \todo Nicolas-fill holes -smooth..?
   */

  return decimator->GetOutput();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoSegmentationAlgo::
SetDecimate(bool& iDecimate)
{
  m_Decimate = iDecimate;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool
QGoSegmentationAlgo::
GetDecimate()
{
  return m_Decimate;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoSegmentationAlgo::
SetNumberOfPoints( const unsigned int& iNumberOfPoints)
{
  m_NumberOfPoints = iNumberOfPoints;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
unsigned int
QGoSegmentationAlgo::
GetNumberOfPoints() const
{
  return m_NumberOfPoints;
}
//--------------------------------------------------------------------------
