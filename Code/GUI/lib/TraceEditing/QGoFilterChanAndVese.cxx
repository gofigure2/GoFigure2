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
#include "QGoFilterChanAndVese.h"

#include "QGoGUILibConfigure.h"

// for apply method
#include "vtkImageExport.h"
#include "vtkImageData.h"

// ITK filter
#include "itkChanAndVeseSegmentationFilter.h"
#include "itkImage.h"
#include "itkVTKImageImport.h"

// to cut
#include "vtkPlane.h"
#include "vtkCutter.h"

// to translate
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include "QGoContourSemiAutoLevelsetWidget.h"

//--------------------------------------------------------------------------
QGoFilterChanAndVese::QGoFilterChanAndVese(QObject *iParent, int iDimension) :
  QGoFilterSemiAutoBase(iParent)
{
  m_Dimension = iDimension;
  m_Iterations = 15;
  m_Curvature = 5;

  QString name = "Levelset ";
  if ( m_Dimension < 2 )
    {
    name.append( QString::number(m_Dimension + 2, 10) );
    name.append("D");
    }
  else
    {
    name = "2D Levelset in 1 mesh";
    }

  setName(name);
  QGoContourSemiAutoLevelsetWidget *widget = new QGoContourSemiAutoLevelsetWidget;
  setWidget(widget);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoFilterChanAndVese::
~QGoFilterChanAndVese()
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterChanAndVese::setIterations(int iIterations)
{
  m_Iterations = iIterations;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterChanAndVese::setCurvature(int iCurvature)
{
  m_Curvature = iCurvature;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterChanAndVese::Filter2D(double *iCenter, const int & iOrientation)
{
  const int dimension = 2;

  // useful to translate the polydata afterwards
  setCenter(iCenter);

  vtkImageData *slice = vtkImageData::New();

  // Extract one slice if dimenesion == 2
  vtkImageData *input = extractOneSlice(getInput(), iCenter, iOrientation);
  slice->DeepCopy(input);
  input->Delete();

  // Recompute new center
  double *newOrigin = slice->GetOrigin();
  double  center[3];

  switch ( iOrientation )
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
  typedef itk::Image< unsigned char, dimension > FeatureImageType;
  typedef itk::Image< float, dimension >         OutputImageType;

  //VTK to ITK
  //---------------------------------------------------------
  FeatureImageType::Pointer
    itkImage = ConvertVTK2ITK< unsigned char, dimension >(slice);
  slice->Delete();

  // Extract ROI
  //---------------------------------------------------------
  FeatureImageType::Pointer
    test2 = ExtractROI< unsigned char, dimension >( itkImage, center, getRadius() );

  // Apply filter
  // Apply LevelSet segmentation filter
  //---------------------------------------------------------
  typedef itk::ChanAndVeseSegmentationFilter< FeatureImageType >
  SegmentationFilterType;

  FeatureImageType::PointType pt;

  SegmentationFilterType::Pointer filter = SegmentationFilterType::New();

  filter->SetFeatureImage(test2);
  filter->SetPreprocess(1);

  pt[0] = center[0];
  pt[1] = center[1];
  pt[2] = center[2];
  filter->SetCenter(pt);

  filter->SetRadius( getRadius() );
  filter->SetNumberOfIterations(m_Iterations);
  filter->SetCurvatureWeight(m_Curvature);
  filter->Update();

  OutputImageType::Pointer test3 = filter->GetOutput();

  // Convert output
  //---------------------------------------------------------
  vtkImageData *itk2vtk = ConvertITK2VTK< float, dimension >(test3);
  setOutput(itk2vtk);
  itk2vtk->Delete();

  vtkPolyData *reconstructed = ReconstructContour(getOutput(), 0.);

  // Translate to real location (i.e. see_pos[])
  vtkTransform *t = vtkTransform::New();
  t->Translate(getCenter()[0],
               getCenter()[1],
               getCenter()[2]);

  vtkTransformPolyDataFilter *tf = vtkTransformPolyDataFilter::New();
  tf->SetTransform(t);
  tf->SetInput(reconstructed);
  tf->Update();

  vtkPolyData *contour = vtkPolyData::New();
  contour->DeepCopy( tf->GetOutput() );

  tf->Delete();
  t->Delete();
  reconstructed->Delete();

  emit ContourCreated(contour);
}

//--------------------------------------------------------------------------

vtkPolyData *
QGoFilterChanAndVese::Filter3D(double *iCenter, int iCurvature, int iIterations,
  double iRadius, std::vector< vtkSmartPointer< vtkImageData > >* iImages,
  int iChannel)
{
  const int dimension = 3;

  // useful to translate the polydata afterwards
  setCenter(iCenter);

  vtkImageData *slice = vtkImageData::New();
  //slice->DeepCopy( getInput() );
  slice->DeepCopy( ( *iImages )[iChannel] );

  // run filter
  typedef itk::Image< unsigned char, dimension > FeatureImageType;
  typedef itk::Image< float, dimension >         OutputImageType;

  //VTK to ITK
  //---------------------------------------------------------
  FeatureImageType::Pointer
    itkImage = ConvertVTK2ITK< unsigned char, dimension >(slice);
  slice->Delete();

  // Extract ROI
  //---------------------------------------------------------
  FeatureImageType::Pointer
    test2 = ExtractROI< unsigned char, dimension >( itkImage, iCenter, iRadius );

  // Apply filter
  // Apply LevelSet segmentation filter
  //---------------------------------------------------------
  typedef itk::ChanAndVeseSegmentationFilter< FeatureImageType >
  SegmentationFilterType;

  FeatureImageType::PointType pt;

  SegmentationFilterType::Pointer filter = SegmentationFilterType::New();

  filter->SetFeatureImage(test2);
  filter->SetPreprocess(1);

  pt[0] = iCenter[0];
  pt[1] = iCenter[1];
  pt[2] = iCenter[2];
  filter->SetCenter(pt);

  filter->SetRadius(iRadius );
  filter->SetNumberOfIterations(iIterations);
  filter->SetCurvatureWeight(iCurvature);
  filter->Update();

  OutputImageType::Pointer test3 = filter->GetOutput();

  // Convert output
  //---------------------------------------------------------
  vtkImageData *itk2vtk = ConvertITK2VTK< float, dimension >(test3);
  setOutput(itk2vtk);
  itk2vtk->Delete();

  //if ( m_Dimension == 1 )
 //   {
    vtkPolyData *output = ReconstructMesh(getOutput(), 0.);
    //emit         MeshCreated(output, this->getChannel() - 1);
    return output;
 //   }
 /* else
    {
    // Extract each slice according top the sampling
    vtkPlane *implicitFunction = vtkPlane::New();
    implicitFunction->SetNormal(0, 0, 1);

    vtkCutter *  cutter = vtkCutter::New();
    vtkPolyData *reconstructed = ReconstructMesh(getOutput(), 0.);
    cutter->SetInput(reconstructed);
    cutter->SetCutFunction(implicitFunction);
    reconstructed->Delete();

    for ( int j = 0; j < getSampling(); ++j )
      {
      implicitFunction
      ->SetOrigin( ( iCenter[0] - getRadius() + ( j + 1 ) * 2 * getRadius() / ( getSampling() + 1 ) ),
                   ( iCenter[1] - getRadius() + ( j + 1 ) * 2 * getRadius() / ( getSampling() + 1 ) ),
                   ( iCenter[2] - getRadius() + ( j + 1 ) * 2 * getRadius() / ( getSampling() + 1 ) ) );
      cutter->Update();
      //true: we decimate the contour
      vtkPolyData *output = ReorganizeContour(cutter->GetOutput(), true);

      emit AddContourForMeshToContours(output);
      }
    emit CreateCorrespondingMesh( getSampling() );
    implicitFunction->Delete();
    cutter->Delete();
    }*/
}

//--------------------------------------------------------------------------
vtkPolyData *
QGoFilterChanAndVese::Apply()
{
  // Radius has to be > 0
  if ( getRadius() <= 0 )
    {
    std::cerr << "Radius should be > 0 " << std::endl;
    return NULL;
    }
  // update the pointed value of the seeds
  emit UpdateSeeds();

  double *center2 = new double[3];

  if ( m_Dimension == 0 )
    {
    int orientation = 0;

    // LOOP  FOR EACH SEED
    for ( int i = 0; i < getPoints()->GetNumberOfPoints(); i++ )
      {
      getPoints()->GetPoint(i, center2);

      this->Filter2D(center2, orientation);
      }
    }
  else
  //if dimension is 3 - i.e. m_Dimension == 1
    {
    // LOOP  FOR EACH SEED
    for ( int i = 0; i < getPoints()->GetNumberOfPoints(); i++ )
      {
      getPoints()->GetPoint(i, center2);

     // this->Filter3D(center2);
      }
    }

  emit SegmentationFinished();

  delete[] center2;

  return NULL;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterChanAndVese::ConnectSignals(int iFilterNumber)
{
  QGoFilterSemiAutoBase::ConnectSignals(iFilterNumber);

  // connect specific
  QObject::connect( getWidget(), SIGNAL( Curvature(int) ),
                    this, SLOT( setCurvature(int) ) );
  QObject::connect( getWidget(), SIGNAL( Iterations(int) ),
                    this, SLOT( setIterations(int) ) );
}

//--------------------------------------------------------------------------
std::vector<vtkPolyData*> QGoFilterChanAndVese::ApplyFilterLevelSet3D(
  double iRadius, vtkPoints* iPoints, int iIterations, int iCurvature,
  std::vector< vtkSmartPointer< vtkImageData > >* iImages, int iChannel)

{
  std::vector<vtkPolyData*> oMeshes = std::vector<vtkPolyData*>();
   if ( iRadius <= 0 )
    {
    std::cerr << "Radius should be > 0 " << std::endl;
    return oMeshes;
    }
   double *center2 = new double[3];

// LOOP  FOR EACH SEED
   for ( int i = 0; i < iPoints->GetNumberOfPoints(); i++ )
    {
    iPoints->GetPoint(i, center2);

    oMeshes.push_back(
      this->Filter3D(center2, iCurvature, iIterations, iRadius, iImages,
      iChannel) );
    }
   delete[] center2;
   return oMeshes;
}