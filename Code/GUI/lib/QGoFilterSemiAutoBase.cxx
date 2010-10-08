/*=========================================================================
  Author: $Author: nicolasrannou $  // Author of last commit
  Version: $Rev: 2037 $  // Revision of last commit
  Date: $Date: 2010-08-23 16:33:20 -0400 (Mon, 23 Aug 2010) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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

#include "vtkImageData.h"

#include "QGoSeedBaseWidget.h"

//Extract one slice
#include "vtkMatrix4x4.h"
#include "vtkImageReslice.h"

// construct contour
#include "vtkMarchingSquares.h"
#include "vtkCellArray.h"
#include "vtkPolylineDecimation.h"
#include "vtkStripper.h"

// construct mesh
#include "vtkMarchingCubes.h"
  // and smooth it...!
#include "vtkSmoothPolyDataFilter.h"

// to cut
#include "vtkPlane.h"
#include "vtkCutter.h"

//--------------------------------------------------------------------------
QGoFilterSemiAutoBase::
QGoFilterSemiAutoBase( QObject* iParent ) :
  QObject( iParent ),
  m_Widget( NULL ),
  m_Radius( 3. ),
  m_Number( 0 ),
  m_Channel( 0 ),
  m_Points( NULL ),
  m_OriginalImageMC( NULL ),
  m_Sampling( 3 )
{
  m_Output = vtkImageData::New();

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
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::
setName(QString iName)
{
  m_Name = iName;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoFilterSemiAutoBase::
getName()
{
  return m_Name;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::
setWidget(QWidget* iWidget)
{
  m_Widget = iWidget;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QWidget*
QGoFilterSemiAutoBase::
getWidget()
{
  return m_Widget;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Original data without ROI
vtkImageData*
QGoFilterSemiAutoBase::
getInput()
{
  return (*m_OriginalImageMC)[m_Channel];
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Original data without ROI
void
QGoFilterSemiAutoBase::
setOutput(vtkImageData* iOutputImage)
{
  m_Output->DeepCopy(iOutputImage);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkImageData*
QGoFilterSemiAutoBase::
getOutput()
{
  return m_Output;
}
//--------------------------------------------------------------------------
// Center of the ROI to apply the segmentation algorithm
//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::
setCenter(double* iCenter)
{
  m_Center[0] = iCenter[0];
  m_Center[1] = iCenter[1];
  m_Center[2] = iCenter[2];
}
//--------------------------------------------------------------------------
// Center of the ROI to apply the segmentation algorithm
//--------------------------------------------------------------------------
double*
QGoFilterSemiAutoBase::
getCenter()
{
  return m_Center;
}
//--------------------------------------------------------------------------
// Radius to define ROI
//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::
setRadius(double iRadius)
{
  m_Radius = iRadius;
}
//--------------------------------------------------------------------------
// Radius to define ROI
//--------------------------------------------------------------------------
double
QGoFilterSemiAutoBase::
getRadius()
{
  return m_Radius;
}
//--------------------------------------------------------------------------
// Radius to define ROI
//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::
setSampling(int iSampling)
{
  m_Sampling = iSampling;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int
QGoFilterSemiAutoBase::
getSampling()
{
  return m_Sampling;
}
//--------------------------------------------------------------------------
// Radius to define ROI
//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::
setChannel(int iChannel)
{
  m_Channel = iChannel;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPoints*
QGoFilterSemiAutoBase::
getPoints()
{
  return m_Points;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::
setPoints(vtkPoints* iPoints)
{
  m_Points = iPoints;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::
setOriginalImageMC(std::vector<vtkImageData*>* iOriginalImage)
{
  m_OriginalImageMC = iOriginalImage;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::
UpdateVisibility(int iCurrentFilter)
{
  QWidget* w = m_Widget->parentWidget()->parentWidget();
  if(m_Number == iCurrentFilter )
    {
    m_Widget->show();
    QObject::connect( w , SIGNAL(Apply()),
        this, SLOT(Apply()));
    }
  else
    {
    m_Widget->hide();
    QObject::disconnect( w, SIGNAL(Apply()),
        this, SLOT(Apply()));
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterSemiAutoBase::
ConnectSignals(int iFilterNumber)
{
  m_Number = iFilterNumber;

  QWidget* w = m_Widget->parentWidget()->parentWidget();

  // Buttons connections
  QObject::connect(  w, SIGNAL(Apply()),
      this, SLOT(Apply()));
  QObject::connect(  w, SIGNAL(Filter(int)),
      this, SLOT(UpdateVisibility(int)));
  QObject::connect(  w, SIGNAL(Radius(double)),
      this, SLOT(setRadius(double)));
  QObject::connect(  w, SIGNAL(Channel(int)),
      this, SLOT(setChannel(int)));
  QObject::connect(  m_Widget->parentWidget()->parentWidget(), SIGNAL(Sampling(int)),
      this, SLOT(setSampling(int)));

  // End of segmentation signals
  QObject::connect(  this , SIGNAL(MeshCreated(vtkPolyData*)),
      w, SIGNAL(MeshCreated(vtkPolyData*)));
  QObject::connect(  this , SIGNAL(ContourCreated(vtkPolyData*)),
      w, SIGNAL(ContourCreated(vtkPolyData*)));
  QObject::connect(  this , SIGNAL(ImageProcessed()),
      w, SIGNAL(ImageProcessed()));

  QObject::connect(  this , SIGNAL(CreateEmptyMesh()),
        m_Widget->parentWidget()->parentWidget(), SIGNAL(CreateEmptyMesh()));
  QObject::connect(  this , SIGNAL(AddContourToCurrentMesh(vtkPolyData*)),
        m_Widget->parentWidget()->parentWidget(), SIGNAL(AddContourToCurrentMesh(vtkPolyData*)));


  QObject::connect(this, SIGNAL(UpdateSeeds()),
      w, SIGNAL(UpdateSeeds()));
  QObject::connect(this, SIGNAL(SegmentationFinished()),
      w, SIGNAL(SegmentationFinished()));

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkImageData*
QGoFilterSemiAutoBase::
extractOneSlice(vtkImageData* iOriginalImage, double* iOrigin, int iDirection)
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
                                    }};

  // Set the slice orientation
  vtkMatrix4x4* resliceAxes = vtkMatrix4x4::New();
  resliceAxes->DeepCopy(elements[iDirection]);
  // Set the point through which to slice
  resliceAxes->SetElement(0, 3, iOrigin[0]);
  resliceAxes->SetElement(1, 3, iOrigin[1]);
  resliceAxes->SetElement(2, 3, iOrigin[2]);

  vtkImageReslice* reslicer = vtkImageReslice::New();
  reslicer->SetOutputDimensionality(2);
  reslicer->SetInformationInput(iOriginalImage);
  reslicer->SetInterpolationModeToLinear();
  reslicer->SetInput(iOriginalImage);
  reslicer->SetResliceAxes(resliceAxes);
  reslicer->Update();

  vtkImageData* output = vtkImageData::New();
  output->DeepCopy(reslicer->GetOutput());

  reslicer->Delete();
  resliceAxes->Delete();

  return output;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData*
QGoFilterSemiAutoBase::
ReconstructContour(vtkImageData* iInputImage, const double& iThreshold )
{
  // create iso-contours
  vtkMarchingSquares* contours = vtkMarchingSquares::New();
  contours->SetInput(iInputImage);
  contours->GenerateValues (1, iThreshold, iThreshold);
  contours->Update();

  vtkPolyData* outputToOrganize = vtkPolyData::New();
  outputToOrganize->DeepCopy(contours->GetOutput());

  vtkPolyData* output = ReorganizeContour(outputToOrganize);

  contours->Delete();
  outputToOrganize->Delete();

  return output;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData*
QGoFilterSemiAutoBase::
ReorganizeContour(vtkPolyData* iInputImage, bool iDecimate)
{
  // Create reorganize contours
  vtkStripper* stripper = vtkStripper::New();
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
  vtkPoints* points = vtkPoints::New();

  vtkCellArray *lines       = vtkCellArray::New();
  vtkIdType *   lineIndices = new vtkIdType[static_cast<int>(npts + 1)];

  for (int k = 0; k < static_cast<int>(npts); k++)
    {
    points->InsertPoint(k, stripper->GetOutput()->GetPoints()->GetPoint(pts[k]));
    lineIndices[k] = k;
    }

  lineIndices[static_cast<int>(npts)] = 0;
  lines->InsertNextCell(npts + 1, lineIndices);
  delete[] lineIndices;

  vtkPolyData* testPolyD = vtkPolyData::New();
  testPolyD->SetPoints(points);
  testPolyD->SetLines(lines);

  if(iDecimate)
    {
    //Decimation (has to be after points reorganization)
    vtkPolylineDecimation* decimator = vtkPolylineDecimation::New();
    decimator->SetInput(testPolyD);
    /// \todo instead os setting it to 0.9, compute it to make 10 to 20 control points
    decimator->SetTargetReduction(0.9);
    decimator->Update();

    vtkPolyData* output = vtkPolyData::New();
    output->DeepCopy(decimator->GetOutput());

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
vtkPolyData*
QGoFilterSemiAutoBase::
ReconstructMesh(vtkImageData* iInputImage, const double& iThreshold)
{
  // create iso-contours
  vtkMarchingCubes* contours = vtkMarchingCubes::New();
  contours->SetInput(iInputImage);
  contours->GenerateValues (1, iThreshold, iThreshold);
  contours->SetComputeGradients(0);
  contours->SetComputeNormals(0);
  contours->SetComputeScalars(0);
  contours->SetNumberOfContours(1);

  //Update required here!!
  contours->Update();

  // smooth the output mesh..?
/*
  std::cout<< "time consumming??" << std::endl;

  vtkSmoothPolyDataFilter* smoother =
      vtkSmoothPolyDataFilter::New();
  smoother->SetInput(contours->GetOutput());
  smoother->SetNumberOfIterations(400);

  //Update required here!!
  smoother->Update();

  std::cout<< "hopefully not..." << std::endl;
*/

  vtkPolyData* output = vtkPolyData::New();
  output->DeepCopy(contours->GetOutput());

  contours->Delete();

  return output;
}
//--------------------------------------------------------------------------
