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
#include "vtkImageReslice.h"

// reconstruct polydata
#include "vtkMarchingSquares.h"
#include "vtkStripper.h"
#include "vtkCellArray.h"
#include "vtkContourFilter.h"
#include "vtkFeatureEdges.h"
#include "vtkFillHolesFilter.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkWindowedSincPolyDataFilter.h"

// Decimation
#include "vtkDecimatePro.h"
#include "vtkPolylineDecimation.h"


// test code
#include <assert.h>


QGoSegmentationAlgo::QGoSegmentationAlgo(QWidget *iParent)
  : m_Decimate(true), m_NumberOfPoints(100), m_AlgoWidget(NULL)
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
QGoSegmentationAlgo::
ExtractROI(double* iBounds, std::vector<vtkImageData*> iImages)
{
  // vector to be returned
  std::vector<vtkImageData*> listOfImages;

  //iterator on the images
  std::vector<vtkImageData*>::iterator it = iImages.begin();

  while( it != iImages.end())
    {
    listOfImages.push_back( ExtractROI(iBounds, *it) );
    ++it;
    }

  return listOfImages;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkImageData*
QGoSegmentationAlgo::
ExtractROI(double* iBounds, vtkImageData* iImage)
{
  // make sure there
  assert( iBounds );

  vtkSmartPointer<vtkExtractVOI> extractVOI =
      vtkSmartPointer<vtkExtractVOI>::New();
  extractVOI->SetInput( iImage );
  extractVOI->SetVOI( iBounds[0] ,iBounds[1],
                      iBounds[2] ,iBounds[3],
                      iBounds[4], iBounds[5]);
  extractVOI->Update();

  vtkImageData* output = vtkImageData::New();
  output->DeepCopy( extractVOI->GetOutput() );

  return output;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkPolyData*>
QGoSegmentationAlgo::
ExtractPolyData(std::vector<vtkImageData*> iInputImage,
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
  int dimension = iInputImage->GetDataDimension();

  switch ( dimension ) {
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

  vtkSmartPointer<vtkPolyData> output = vtkSmartPointer<vtkPolyData>::New();
  output->ShallowCopy( ReorganizeContour( contours->GetOutput() ) );

  return output;
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

  vtkSmartPointer<vtkPolyData> output = vtkSmartPointer<vtkPolyData>::New();
  output->ShallowCopy( connectivityFilter->GetOutput() );

  return output;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData*
QGoSegmentationAlgo::
DecimatePolyData( vtkPolyData* iPolyData, unsigned int iNumberOfPoints)
{
  // make sure there is a polydata
  assert( iPolyData );

  /*
   * \todo Nicolas-might be better solution to test dimension of a PolyData
   */
  // test dimension of the input
  double* bounds = iPolyData->GetBounds();
  int dimension = 3;
  if(bounds[0] == bounds[1] || bounds[2] == bounds[3] || bounds[4] == bounds[5])
    {
    --dimension;
    }

  switch ( dimension ) {
      case 2 :
        return DecimateContour( iPolyData, iNumberOfPoints);
      case 3 :
        return DecimateMesh( iPolyData, iNumberOfPoints);
      default :
        std::cout << "dimension unknown (Reconstruct polydata)" << std::endl;
      }

  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData*
QGoSegmentationAlgo::
DecimateContour( vtkPolyData* iPolyData, unsigned int iNumberOfPoints)
{
  // define target reduction
  unsigned int numberOfPoints = iPolyData->GetNumberOfPoints();

  double target = 1 - (double)iNumberOfPoints/(double)numberOfPoints;

  vtkPolylineDecimation *decimator = vtkPolylineDecimation::New();
  decimator->SetInput(iPolyData);
  decimator->SetTargetReduction(target);
  decimator->Update();

  vtkPolyData *output = vtkPolyData::New();
  output->DeepCopy( decimator->GetOutput() );

  decimator->Delete();

  return output;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData*
QGoSegmentationAlgo::
DecimateMesh( vtkPolyData* iPolyData, unsigned int iNumberOfPoints)
{
  // define target reduction
  unsigned int numberOfPoints = iPolyData->GetNumberOfPoints();

  double target = 1 - (double)iNumberOfPoints/(double)numberOfPoints;

  vtkDecimatePro *decimator = vtkDecimatePro::New();
  decimator->SetInput(iPolyData);
  decimator->SetTargetReduction(target);
  decimator->Update();

  /*
   * \todo Nicolas-fill holes -smooth..?
   */

  vtkPolyData *output = vtkPolyData::New();
  output->DeepCopy( decimator->GetOutput() );

  decimator->Delete();

  return output;
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
SetNumberOfPoints( unsigned int& iNumberOfPoints)
{
  m_NumberOfPoints = iNumberOfPoints;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
unsigned int
QGoSegmentationAlgo::
GetNumberOfPoints()
{
  return m_NumberOfPoints;
}
//--------------------------------------------------------------------------
