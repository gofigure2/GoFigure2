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
#include "QGoFilterSemiAutoBase.h"

#include "QGoGUILibConfigure.h"

#include "vtkSmartPointer.h"
#include "vtkImageData.h"

//#include "QGoSeedBaseWidget.h"

//Extract one slice
#include "vtkMatrix4x4.h"
#include "vtkImageReslice.h"

// construct contour
#include "vtkContourFilter.h"
#include "vtkMarchingSquares.h"
#include "vtkCellArray.h"
#include "vtkPolylineDecimation.h"
#include "vtkStripper.h"
#include "vtkFeatureEdges.h"

// construct mesh
#include "vtkMarchingCubes.h"
// keep the largest region
#include "vtkPolyDataConnectivityFilter.h"
// fill the holes!
#include "vtkFillHolesFilter.h"
// and smooth it...!
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkPolyDataWriter.h"

// to cut
#include "vtkPlane.h"
#include "vtkCutter.h"

#include "vtkImageExport.h"

//--------------------------------------------------------------------------
QGoFilterSemiAutoBase::QGoFilterSemiAutoBase(QObject *iParent) :
  QObject(iParent),
  m_Widget(NULL),
  m_Radius(3.),
  m_Number(0),
  m_Channel(0),
  m_Points(NULL),
  m_OriginalImageMC(NULL),
  m_Sampling(3)
{
  m_Output = vtkImageData::New();
  m_vtk2itkImage = vtkImageExport::New();

  m_Center[0] = 0;
  m_Center[1] = 0;
  m_Center[2] = 0;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoFilterSemiAutoBase::
~QGoFilterSemiAutoBase()
{
  m_Output->Delete();
  m_vtk2itkImage->Delete();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::setName(QString iName)
{
  m_Name = iName;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoFilterSemiAutoBase::getName()
{
  return m_Name;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::setWidget(QWidget *iWidget)
{
  //m_Widget = iWidget;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QWidget *
QGoFilterSemiAutoBase::getWidget()
{
  return m_Widget;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Original data without ROI
vtkSmartPointer< vtkImageData >
QGoFilterSemiAutoBase::getInput()
{
  return ( *m_OriginalImageMC )[m_Channel];
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Original data without ROI
void
QGoFilterSemiAutoBase::setOutput(vtkImageData *iOutputImage)
{
  m_Output->DeepCopy(iOutputImage);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkImageData *
QGoFilterSemiAutoBase::getOutput()
{
  return m_Output;
}

//--------------------------------------------------------------------------
// Center of the ROI to apply the segmentation algorithm
//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::setCenter(double *iCenter)
{
  m_Center[0] = iCenter[0];
  m_Center[1] = iCenter[1];
  m_Center[2] = iCenter[2];
}

//--------------------------------------------------------------------------
// Center of the ROI to apply the segmentation algorithm
//--------------------------------------------------------------------------
double *
QGoFilterSemiAutoBase::getCenter()
{
  return m_Center;
}

//--------------------------------------------------------------------------
// Radius to define ROI
//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::setRadius(double iRadius)
{
  m_Radius = iRadius;
}

//--------------------------------------------------------------------------
// Radius to define ROI
//--------------------------------------------------------------------------
double
QGoFilterSemiAutoBase::getRadius()
{
  return m_Radius;
}

//--------------------------------------------------------------------------
// Radius to define ROI
//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::setSampling(int iSampling)
{
  m_Sampling = iSampling;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int
QGoFilterSemiAutoBase::getSampling()
{
  return m_Sampling;
}

//--------------------------------------------------------------------------
// Radius to define ROI
//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::setChannel(int iChannel)
{
  m_Channel = iChannel;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int
QGoFilterSemiAutoBase::getChannel()
{
  return m_Channel;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPoints *
QGoFilterSemiAutoBase::getPoints()
{
  return m_Points;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::setPoints(vtkPoints *iPoints)
{
  m_Points = iPoints;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::setOriginalImageMC(std::vector< vtkSmartPointer< vtkImageData > > *iOriginalImage)
{
  m_OriginalImageMC = iOriginalImage;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::UpdateVisibility(int iCurrentFilter)
{
  /*QWidget *w = m_Widget->parentWidget()->parentWidget();

  if ( m_Number == iCurrentFilter )
    {
    m_Widget->show();
    QObject::connect( w, SIGNAL( Apply() ),
                      this, SLOT( Apply() ) );
    }
  else
    {
    m_Widget->hide();
    QObject::disconnect( w, SIGNAL( Apply() ),
                         this, SLOT( Apply() ) );
    }*/
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::UpdateAdvancedMode(bool checked)
{
  //QWidget *          w = m_Widget->parentWidget()->parentWidget();
  //QGoSeedBaseWidget *baseWidget = dynamic_cast< QGoSeedBaseWidget * >( w );

  //if ( checked && ( m_Number != baseWidget->GetCurrentFilter() ) )
  //  {
  //  m_Widget->hide();
  //  }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::ConnectSignals(int iFilterNumber)
{
  /*m_Number = iFilterNumber;

  QWidget *w = m_Widget->parentWidget()->parentWidget();

  // Buttons connections
  QObject::connect( w, SIGNAL( Apply() ),
                    this, SLOT( Apply() ) );
  QObject::connect( w, SIGNAL( Filter(int) ),
                    this, SLOT( UpdateVisibility(int) ) );
  QObject::connect( w, SIGNAL( Radius(double) ),
                    this, SLOT( setRadius(double) ) );
  QObject::connect( w, SIGNAL( Channel(int) ),
                    this, SLOT( setChannel(int) ) );
  QObject::connect( w, SIGNAL( Sampling(int) ),
                    this, SLOT( setSampling(int) ) );
  QObject::connect( w, SIGNAL( Clicked(bool) ),
                    this, SLOT( UpdateAdvancedMode(bool) ) );

  // End of segmentation signals
  QObject::connect( this, SIGNAL( MeshCreated(vtkPolyData *, int) ),
                    w, SIGNAL( MeshCreated(vtkPolyData *, int) ) );
  QObject::connect( this, SIGNAL( ContourCreated(vtkPolyData *) ),
                    w, SIGNAL( ContourCreated(vtkPolyData *) ) );
  QObject::connect( this, SIGNAL( ImageProcessed() ),
                    w, SIGNAL( ImageProcessed() ) );

  QObject::connect( this, SIGNAL( CreateCorrespondingMesh(int) ),
                    w, SIGNAL( CreateCorrespondingMesh(int) ) );
  QObject::connect( this, SIGNAL( AddContourForMeshToContours(vtkPolyData *) ),
                    w, SIGNAL( AddContourForMeshToContours(vtkPolyData *) ) );

  QObject::connect( this, SIGNAL( UpdateSeeds() ),
                    w, SIGNAL( UpdateSeeds() ) );
  QObject::connect( this, SIGNAL( SegmentationFinished() ),
                    w, SIGNAL( SegmentationFinished() ) );*/
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkImageData *
QGoFilterSemiAutoBase::extractOneSlice(vtkImageData *iOriginalImage, double *iOrigin, int iDirection)
{
  static double elements[3][16] = { {
                                      1,  0,  0,  0,
                                      0,  1,  0,  0,
                                      0,  0,  1,  0,
                                      0,  0,  0,  1
                                      },
                                      {
                                      1,  0,  0,  0,
                                      0,  0,  1,  0,
                                      0, -1,  0,  0,
                                      0,  0,  0,  1
                                      },
                                      {
                                      0,  0, -1,  0,
                                      1,  0,  0,  0,
                                      0, -1,  0,  0,
                                      0,  0,  0,  1
                                      } };

  // Set the slice orientation
  vtkMatrix4x4 *resliceAxes = vtkMatrix4x4::New();

  resliceAxes->DeepCopy(elements[iDirection]);
  // Set the point through which to slice
  resliceAxes->SetElement(0, 3, iOrigin[0]);
  resliceAxes->SetElement(1, 3, iOrigin[1]);
  resliceAxes->SetElement(2, 3, iOrigin[2]);

  vtkImageReslice *reslicer = vtkImageReslice::New();
  reslicer->SetOutputDimensionality(2);
  reslicer->SetInformationInput(iOriginalImage);
  reslicer->SetInterpolationModeToLinear();
  reslicer->SetInput(iOriginalImage);
  reslicer->SetResliceAxes(resliceAxes);
  reslicer->Update();

  vtkImageData *output = vtkImageData::New();
  output->DeepCopy( reslicer->GetOutput() );

  reslicer->Delete();
  resliceAxes->Delete();

  return output;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData *
QGoFilterSemiAutoBase::ReconstructContour(vtkImageData *iInputImage, const double & iThreshold)
{
  // create iso-contours
  vtkMarchingSquares *contours = vtkMarchingSquares::New();

  contours->SetInput(iInputImage);
  contours->GenerateValues (1, iThreshold, iThreshold);
  contours->Update();

  vtkPolyData *outputToOrganize = vtkPolyData::New();
  outputToOrganize->DeepCopy( contours->GetOutput() );

  vtkPolyData *output = ReorganizeContour(outputToOrganize);

  contours->Delete();
  outputToOrganize->Delete();

  return output;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData *
QGoFilterSemiAutoBase::ReorganizeContour(vtkPolyData *iInputImage, bool iDecimate)
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

    double ratio = 1. - 20. / static_cast< double >( npts );

    decimator->SetTargetReduction(ratio);
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
QGoFilterSemiAutoBase::ReconstructMesh(vtkImageData *iInputImage, const double & iThreshold)
{
  // create iso-contours
  // Problem[Kishore]: Creates holes in some meshes although image has no hole
//   vtkMarchingCubes *contours = vtkMarchingCubes::New();
//
//   contours->SetInput(iInputImage);
//   contours->GenerateValues (1, iThreshold, iThreshold);
//   contours->SetComputeGradients(0);
//   contours->SetComputeNormals(1);
//   contours->SetComputeScalars(0);
//   contours->SetNumberOfContours(1);
//   contours->Update();

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
