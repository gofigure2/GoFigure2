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

#include "QGoContourSemiAutoWatershedWidget.h"

//--------------------------------------------------------------------------
QGoFilterWatershed::QGoFilterWatershed(QObject *iParent, int iDimension):
  QGoFilterSemiAutoBase(iParent)
{
  m_Dimension = iDimension;

  QString name = "Watershed ";
  if ( m_Dimension < 2 )
    {
    name.append( QString::number(m_Dimension + 2, 10) );
    name.append("D");
    }
  else
    {
    name = "2D Watershed within 1 mesh";
    }

  setName(name);
  QGoContourSemiAutoWatershedWidget *widget = new QGoContourSemiAutoWatershedWidget;
  setWidget(widget);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoFilterWatershed::
~QGoFilterWatershed()
{}

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

void
QGoFilterWatershed::Filter3D(double *iCenter)
{
  const int dimension = 3;

  // useful to translate the polydata afterwards
  setCenter(iCenter);

  vtkImageData *slice = vtkImageData::New();
  slice->DeepCopy( getInput() );

  // run filter
  const unsigned int Dimension = 3;
  typedef itk::Image< unsigned char, Dimension > FeatureImageType;
  typedef FeatureImageType::Pointer              FeatureImagePointer;
  typedef itk::Image< double, Dimension >        InputImageType;
  typedef InputImageType::IndexType              InputImageIndexType;
  typedef InputImageType::Pointer                InputImagePointer;
  typedef itk::Image< int, Dimension >           SegmentImageType;
  typedef SegmentImageType::Pointer              SegmentImagePointer;

  typedef itk::Image< float, dimension > OutputImageType;

  typedef itk::WatershedBasedCellSegmentation< FeatureImageType, InputImageType, SegmentImageType >
    SegmentationFilterType;

  //VTK to ITK
  //---------------------------------------------------------
  FeatureImageType::Pointer
    itkImage = ConvertVTK2ITK< unsigned char, dimension >(slice);
  slice->Delete();

  // Extract ROI
  //---------------------------------------------------------
  FeatureImageType::Pointer
    test2 = ExtractROI< unsigned char, dimension >( itkImage, iCenter, getRadius() );

  // Apply filter
  // Apply watershed segmentation filter
  //---------------------------------------------------------
  SegmentationFilterType::Pointer filter = SegmentationFilterType::New();
  filter->SetInput(test2);
  filter->Update();
  SegmentImagePointer test3 = filter->GetOutput();
  
  typedef itk::ImageFileWriter< SegmentImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( "output.mha" );
  writer->SetInput( test3 );
  writer->Update();

  // Convert output
  //---------------------------------------------------------
  vtkImageData *itk2vtk = ConvertITK2VTK< int, dimension >(test3);
  setOutput(itk2vtk);
  itk2vtk->Delete();

//   vtkSmartPointer< vtkMetaImageWriter > writer = vtkSmartPointer< vtkMetaImageWriter >::New(); 
//   writer->SetFileDimensionality( 3 ); 
//   writer->SetInput( getOutput() ); 
//   writer->SetFileName("test.mha"); 
//   writer->Write(); 
  
  // 3D when m_Dimension = 1
  /// \todo rename m_Dimension
  if ( m_Dimension == 1 )
    {
    vtkPolyData *output = ReconstructMesh(getOutput(), .5);
    emit         MeshCreated(output);
    }
  else
    {
    // Extract each slice according top the sampling
    vtkPlane *implicitFunction = vtkPlane::New();
    implicitFunction->SetNormal(0, 0, 1);

    vtkCutter *  cutter = vtkCutter::New();
    vtkPolyData *reconstructed = ReconstructMesh(getOutput(), .5);
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
    }
}

//--------------------------------------------------------------------------
vtkPolyData *
QGoFilterWatershed::Apply()
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
    /*int orientation = 0;

    // LOOP  FOR EACH SEED
    for (int i = 0; i < getPoints()->GetNumberOfPoints(); i++)
      {
      getPoints()->GetPoint(i, center2);

      this->Filter2D( center2, orientation );
      }
      */
    }
  else
  //if dimension is 3 - i.e. m_Dimension == 1
    {
    // LOOP  FOR EACH SEED
    for ( int i = 0; i < getPoints()->GetNumberOfPoints(); i++ )
      {
      getPoints()->GetPoint(i, center2);

      this->Filter3D(center2);
      }
    }

  emit SegmentationFinished();

  delete[] center2;

  return NULL;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterWatershed::ConnectSignals(int iFilterNumber)
{
  QGoFilterSemiAutoBase::ConnectSignals(iFilterNumber);
}

//--------------------------------------------------------------------------
