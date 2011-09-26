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

//--------------------------------------------------------------------------
QGoFilterWatershed::QGoFilterWatershed(QObject *iParent, int iDimension) :
  QGoFilterSemiAutoBase(iParent)
{
  m_Dimension = iDimension;

  m_TreshMin  = 10;
  m_TreshMax  = 30;
  m_CorrTresh = 0.50;
  m_Alpha = 1.5;
  m_Beta = 3.0;

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
  QGoContourSemiAutoWatershedWidget *m_Widget = new QGoContourSemiAutoWatershedWidget;
  setWidget(m_Widget);
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
  //set up parameters
  filter->SetNucleusThresholdMin(m_TreshMin);
  filter->SetNucleusThresholdMax(m_TreshMax);
  filter->SetCorrelationThreshold1(m_CorrTresh);
  filter->SetAlpha(m_Alpha);
  filter->SetBeta(m_Beta);
  // run the filter
  filter->Update();
  SegmentImagePointer test3 = filter->GetOutput();

/*
  typedef itk::ImageFileWriter< SegmentImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( "output.mha" );
  writer->SetInput( test3 );
  writer->Update();
*/

  // Convert output
  //---------------------------------------------------------
  vtkImageData *itk2vtk = ConvertITK2VTK< int, dimension >(test3);
  setOutput(itk2vtk);
  itk2vtk->Delete();

//   vtkSmartPointer< vtkMetaImageWriter > writer = vtkSmartPointer<
// vtkMetaImageWriter >::New();
//   writer->SetFileDimensionality( 3 );
//   writer->SetInput( getOutput() );
//   writer->SetFileName("test.mha");
//   writer->Write();

  // 3D when m_Dimension = 1
  /// \todo rename m_Dimension
  if ( m_Dimension == 1 )
    {
    vtkPolyData *output = ReconstructMesh(getOutput(), .5);
    emit         MeshCreated(output, this->getChannel() - 1);
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

  // connect specific
  QObject::connect( getWidget(), SIGNAL( TreshMin(int) ),
                    this, SLOT( setTreshMin(int) ) );
  QObject::connect( getWidget(), SIGNAL( TreshMax(int) ),
                    this, SLOT( setTreshMax(int) ) );
  QObject::connect( getWidget(), SIGNAL( CorrTresh(double) ),
                    this, SLOT( setCorrTresh(double) ) );
  QObject::connect( getWidget(), SIGNAL( Alpha(double) ),
                    this, SLOT( setAlpha(double) ) );
  QObject::connect( getWidget(), SIGNAL( Beta(double) ),
                    this, SLOT( setBeta(double) ) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterWatershed::setTreshMin(int iTreshmin)
{
  m_TreshMin = iTreshmin;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterWatershed::setTreshMax(int iTreshmax)
{
  m_TreshMax = iTreshmax;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterWatershed::setCorrTresh(double iCorrTresh)
{
  m_CorrTresh = iCorrTresh;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterWatershed::setAlpha(double iAlpha)
{
  m_Alpha = iAlpha;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterWatershed::setBeta(double iBeta)
{
  m_Beta = iBeta;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterWatershed::
SetMaxThreshold(const int& iThreshold)
{
  QGoContourSemiAutoWatershedWidget* widget =
      dynamic_cast<QGoContourSemiAutoWatershedWidget*>(getWidget());
  widget->SetMaxThreshold(iThreshold);
}
//--------------------------------------------------------------------------