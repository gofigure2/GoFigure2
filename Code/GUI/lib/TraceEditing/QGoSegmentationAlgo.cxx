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
#include <vtkExtractVOI.h>

// convert vtk to itk
#include "vtkImageExport.h"
#include "vtkitkAdaptor.h"

// convert itk to vtk
#include "itkImageToVTKImageFilter.h"

// reconstruct polydata
#include "vtkMarchingSquares.h"
#include "vtkStripper.h"
#include "vtkCellArray.h"
#include "vtkPolylineDecimation.h"
#include "vtkContourFilter.h"
#include "vtkFeatureEdges.h"
#include "vtkFillHolesFilter.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkWindowedSincPolyDataFilter.h"


// test code
#include <assert.h>


QGoSegmentationAlgo::QGoSegmentationAlgo(QWidget *iParent)
  :m_AlgoWidget(NULL)
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
  // make sure there are bounds....
  assert( iBounds[0] );
  assert( iBounds[1] );
  assert( iBounds[2] );
  assert( iBounds[3] );
  assert( iBounds[4] );
  assert( iBounds[5] );

  vtkSmartPointer<vtkExtractVOI> extractVOI =
      vtkSmartPointer<vtkExtractVOI>::New();
  extractVOI->SetInput( iImage );
  extractVOI->SetVOI( iBounds[0] ,iBounds[1],iBounds[2],iBounds[3], iBounds[4], iBounds[5]);
  extractVOI->Update();

  /*
   * \note Nicolas-to be tested....might nedd deepcopy
   */
  return extractVOI->GetOutput();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class PixelType, unsigned int VImageDimension >
typename itk::Image< PixelType, VImageDimension >::Pointer
QGoSegmentationAlgo::
ConvertVTK2ITK(vtkImageData *iInput)
{
  // make sure there is an input
  assert ( iInput );

  //Export VTK image to ITK
  vtkSmartPointer<vtkImageExport> exporter =
      vtkSmartPointer<vtkImageExport>::New();
  exporter->SetInput(iInput);
  exporter->Update();

  // ImageType
  typedef itk::Image< PixelType, VImageDimension > ImageType;
  // Import VTK Image to ITK
  typedef itk::VTKImageImport< ImageType >  ImageImportType;
  typedef typename ImageImportType::Pointer ImageImportPointer;
  ImageImportPointer importer = ImageImportType::New();

  ConnectPipelines< vtkImageExport, ImageImportPointer >(
    exporter,
    importer);

  typename ImageType::Pointer itkImage = importer->GetOutput();
  itkImage->DisconnectPipeline();

  return itkImage;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class PixelType, unsigned int VImageDimension >
vtkImageData *
QGoSegmentationAlgo::
ConvertITK2VTK(typename itk::Image< PixelType, VImageDimension >::Pointer iInput)
{
  typedef itk::Image< PixelType, VImageDimension >        InternalImageType;
  typedef itk::ImageToVTKImageFilter< InternalImageType > ConverterType;
  typedef typename ConverterType::Pointer                 ConverterPointer;

  ConverterPointer converter = ConverterType::New();
  converter->SetInput(iInput);

  try
    {
    converter->Update();
    }
  catch (itk::ExceptionObject & err)
    {
    std::cerr << "converter Exception:" << err << std::endl;
    }

  /*
   * \note Nicolas-to be tested.... might need deepcopy?
   */
  return converter->GetOutput();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkPolyData*>
QGoSegmentationAlgo::
ReconstructPolyData(std::vector<vtkImageData*> iInputImage,
    const double & iThreshold)
{
  // vector to be returned
  std::vector<vtkPolyData*> listOfPolys;

  //iterator on the images
  std::vector<vtkImageData*>::iterator it = iInputImage.begin();

  while( it != iInputImage.end())
    {
    listOfPolys.push_back( ReconstructPolyData(*it, iThreshold) );
    ++it;
    }

  return listOfPolys;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkPolyData *
QGoSegmentationAlgo::
ReconstructPolyData(vtkImageData *iInputImage, const double & iThreshold)
{
  // make sure there is an image
  assert( iInputImage );

  // test dimension of the input
  int dimension = iInputImage->GetDataDimension();

  switch ( dimension ) {
      case 2 :
        return ReconstructContour( iInputImage, iThreshold);
      case 3 :
        return ReconstructMesh( iInputImage, iThreshold);
      default :
        std::cout << "dimension unknown (Reconstruct polydata)" << std::endl;
      }

  return NULL;
}
//-------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData *
QGoSegmentationAlgo::
ReconstructContour(vtkImageData *iInputImage, const double & iThreshold)
{
  // create iso-contours
  vtkMarchingSquares *contours = vtkMarchingSquares::New();

  contours->SetInput(iInputImage);
  contours->GenerateValues (1, iThreshold, iThreshold);
  contours->Update();

  vtkPolyData *outputToOrganize = vtkPolyData::New();
  outputToOrganize->DeepCopy( contours->GetOutput() );

  vtkPolyData *output = ReorganizeContour(outputToOrganize, true);

  contours->Delete();
  outputToOrganize->Delete();

  return output;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData *
QGoSegmentationAlgo::
ReorganizeContour(vtkPolyData *iInputImage, bool iDecimate)
{
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

  vtkPolyData *testPolyD = vtkPolyData::New();
  testPolyD->SetPoints(points);
  testPolyD->SetLines(lines);

  if ( iDecimate )
    {
    //Decimation (has to be after points reorganization)
    vtkPolylineDecimation *decimator = vtkPolylineDecimation::New();
    decimator->SetInput(testPolyD);
    /// \todo instead os setting it to 0.9, compute it to make 10 to 20 control
    // points
    decimator->SetTargetReduction(0.9);
    decimator->Update();

    vtkPolyData *output = vtkPolyData::New();
    output->DeepCopy( decimator->GetOutput() );

    lines->Delete();
    points->Delete();
    stripper->Delete();
    decimator->Delete();
    testPolyD->Delete();

    return output;
    }
  else
    {
    lines->Delete();
    points->Delete();
    stripper->Delete();

    return testPolyD;
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData *
QGoSegmentationAlgo::
ReconstructMesh(vtkImageData *iInputImage, const double & iThreshold)
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

  vtkPolyData *output = vtkPolyData::New();
  output->DeepCopy( connectivityFilter->GetOutput() );

  return output;
}
//--------------------------------------------------------------------------
