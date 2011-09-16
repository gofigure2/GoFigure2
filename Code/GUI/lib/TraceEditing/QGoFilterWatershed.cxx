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
#include "QGoFilterWatershed.h"

#include "QGoGUILibConfigure.h"

// for apply method
#include "vtkImageExport.h"
#include "vtkImageData.h"
#include "vtkMetaImageWriter.h"
// ITK filter
#include "itkImageFileWriter.h"
#include "itkWatershedBasedCellSegmentation.h"
#include "itkImage.h"
#include "itkVTKImageImport.h"

// to cut
#include "vtkPlane.h"
#include "vtkCutter.h"

// to translate
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include "GoImageProcessor.h"

//--------------------------------------------------------------------------
QGoFilterWatershed::QGoFilterWatershed(QObject *iParent, int iDimension) :
  QGoFilterSemiAutoBase(iParent)
{
  m_Image3D = itk::Image< int, 3 >::New();
  m_Image2D = itk::Image< int, 2 >::New();

  m_Dimension = iDimension;
  m_TreshMin  = 10;
  m_TreshMax  = 30;
  m_CorrTresh = 0.50;
  m_Alpha = 1.5;
  m_Beta = 3.0;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoFilterWatershed::
~QGoFilterWatershed()
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterWatershed::Filter2D(double *iCenter, const int & iOrientation)
{
  (void)iCenter;
  (void)iOrientation;
  /*
  const int dimension = 2;

  // useful to translate the polydata afterwards
  setCenter(iCenter);

  vtkImageData* slice = vtkImageData::New();

  // Extract one slice if dimenesion == 2
  vtkImageData* input = extractOneSlice(getInput(), iCenter, iOrientation);
  slice->DeepCopy(input);
  input->Delete();

  // Recompute new center
  double* newOrigin = slice->GetOrigin();
  double center[3];

  switch (iOrientation)
    {
    case 0:
      {
      center[0] = iCenter[0] + newOrigin[0];
      center[1] = iCenter[1] + newOrigin[1];
      center[2] = 0.;
      break;
      }
    case 1:
      {
      center[0] = iCenter[0] + newOrigin[0];
      center[1] = iCenter[2] + newOrigin[1];
      center[2] = 0.;
      break;
      }
    case 2:
      {
      center[0] = iCenter[1] + newOrigin[0];
      center[1] = iCenter[2] + newOrigin[1];
      center[2] = 0.;
      break;
      }
    default:
      {
      break;
      }
    }

  // run filter
  typedef itk::Image<unsigned char, dimension> FeatureImageType;
  typedef itk::Image<float, dimension>         OutputImageType;

  //VTK to ITK
  //---------------------------------------------------------
  FeatureImageType::Pointer
    itkImage = ConvertVTK2ITK<unsigned char, dimension>( slice );

  // Extract ROI
  //---------------------------------------------------------
  FeatureImageType::Pointer
    test2 = ExtractROI<unsigned char, dimension>(itkImage, center, getRadius());

  // Apply filter
  // Apply LevelSet segmentation filter
  //---------------------------------------------------------
  typedef itk::ChanAndVeseSegmentationFilter<FeatureImageType>
    SegmentationFilterType;

  FeatureImageType::PointType pt;

  SegmentationFilterType::Pointer filter = SegmentationFilterType::New();

  filter->SetFeatureImage(test2);
  filter->SetPreprocess(1);

  pt[0] = center[0];
  pt[1] = center[1];
  pt[2] = center[2];
  filter->SetCenter(pt);

  filter->SetRadius(getRadius());
  filter->SetNumberOfIterations(m_Iterations);
  filter->SetCurvatureWeight(m_Curvature);
  filter->Update();

  OutputImageType::Pointer test3 = filter->GetOutput();

  // Convert output
  //---------------------------------------------------------
  vtkImageData* itk2vtk = ConvertITK2VTK<float, dimension>(test3);
  setOutput(itk2vtk);
  itk2vtk->Delete();

  vtkPolyData* reconstructed = ReconstructContour( getOutput(), 0. );

  // Translate to real location (i.e. see_pos[])
  vtkTransform* t = vtkTransform::New();
  t->Translate(getCenter()[0],
               getCenter()[1],
               getCenter()[2]);

  vtkTransformPolyDataFilter* tf = vtkTransformPolyDataFilter::New();
  tf->SetTransform(t);
  tf->SetInput( reconstructed );
  tf->Update();

  vtkPolyData* contour = vtkPolyData::New();
  contour->DeepCopy( tf->GetOutput() );

  tf->Delete();
  t->Delete();
  reconstructed->Delete();

  emit ContourCreated( contour );
  */
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<std::vector<vtkPolyData*> > QGoFilterWatershed::
  ApplyFilterSetOf2D(double iRadius,
    int iThresMin, int iThresMax, double iCorrTresh, double iAlpha,
    double iBeta,  int iSampling,
    std::vector< vtkPoints* >* iPoints,
    GoImageProcessor* iImages, int iChannel)
{
  std::vector<std::vector<vtkPolyData*> > oSetOfContours =
    std::vector<std::vector<vtkPolyData*> >();


  return oSetOfContours;
}
