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
#include "QGoMeshSplitDanielssonDistanceAlgo.h"

#include "itkvtkMeshSplitterDanielssonDistanceImageFilter.h"
#include "GoImageProcessor.h"

#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

// temp
#include "vtkPolyDataWriter.h"
#include "itkImageFileWriter.h"

QGoMeshSplitDanielssonDistanceAlgo::QGoMeshSplitDanielssonDistanceAlgo(std::vector< vtkPoints* >* iSeeds, QWidget* iParent)
    :QGoSplitDanielssonDistanceAlgo(iSeeds, iParent)
{

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoMeshSplitDanielssonDistanceAlgo::~QGoMeshSplitDanielssonDistanceAlgo()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkPolyData*> QGoMeshSplitDanielssonDistanceAlgo::ApplyAlgo(
    GoImageProcessor* iImages,
    std::string iChannel,
    vtkPolyData* iPolyData,
    bool iIsInvertedOn)
{
  std::vector<vtkPolyData*> oVector;

  if( (*this->m_Seeds)[0]->GetNumberOfPoints() >= 2)
    {
    size_t nb_ch = iImages->getNumberOfChannels();

    const unsigned int Dimension = 3;
    typedef unsigned char PixelType;

    typedef itk::Image< PixelType, Dimension > ImageType;

    typedef itk::vtkMeshSplitterDanielssonDistanceImageFilter< ImageType >
        SplitterType;
    SplitterType::Pointer filter = SplitterType::New();
    filter->SetNumberOfImages( 1 );

    // work on smaller region
    typedef itk::Image< PixelType, Dimension >  ImageType;
    std::vector< double > bounds(6);
    double* boundsPointer = iPolyData->GetBounds();
    for(int i = 0; i<Dimension; ++i)
      {
      bounds[i*2] = static_cast<int>(boundsPointer[i*2] -10);
      bounds[i*2+1] = static_cast<int>(boundsPointer[i*2+1] +10);
      std::cout <<"bounds: " << bounds[i*2] << std::endl;
      std::cout <<"bounds: " << bounds[i*2+1] << std::endl;
      }

    // origins have to match!
  vtkSmartPointer<vtkTransform> translation =
    vtkSmartPointer<vtkTransform>::New();
  translation->Translate(- bounds[0], - bounds[2], - bounds[4]);

  std::cout <<"tanslation: " <<  - bounds[0] << std::endl;
  std::cout <<"tanslation: " <<  - bounds[2] << std::endl;
  std::cout <<"tanslation: " <<  - bounds[4] << std::endl;

  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetInput(iPolyData);
  transformFilter->SetTransform(translation);
  transformFilter->Update();

 // transformFilter->GetOutput()->Print(cout);

  filter->SetMesh( iPolyData );

    ImageType::PointType origin;
    for( size_t i = 0; i < 1; i++ )
    {
      // then let's extract the Region of Interest
      ImageType::Pointer ITK_ROI_Image =
          this->ITKExtractROI< PixelType, Dimension >( bounds,
                                                       iImages->getImageITK<PixelType, Dimension>(
                                                                iImages->getChannelName(i)));
/*
      double origin[3] = {0.0, 0.0, 0.0};
      ITK_ROI_Image->SetOrigin(origin);*/
      filter->SetFeatureImage( i,
                               ITK_ROI_Image);

      typedef  itk::ImageFileWriter< ImageType  > WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName("extractedROI.mhd");
        writer->SetInput(ITK_ROI_Image);
        writer->Update();
      }

    typedef SplitterType::PointSetType PointSetType;
    PointSetType::Pointer seeds = PointSetType::New();

    ImageType::PointType itk_pt;
    double vtk_pt[3];

    for( vtkIdType id = 0; id < 2; id++ )
      {
      (*this->m_Seeds)[0]->GetPoint( id, vtk_pt );
      std::cout << "Point:" << std::endl;
      itk_pt[0] = vtk_pt[0];
      itk_pt[1] = vtk_pt[1];
      itk_pt[2] = vtk_pt[2];
      std::cout << itk_pt[0] << "-" << itk_pt[1] << "-" << itk_pt[2] << std::endl;
      seeds->SetPoint( id, itk_pt );
      }

    filter->SetSeeds( seeds );
    filter->Update();
    oVector = filter->GetOutputs();

    oVector[0]->Print(cout);
    oVector[1]->Print(cout);

    std::cout << "done" << std::endl;

    oVector[0]->Print(cout);
    oVector[1]->Print(cout);

    vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
    writer->SetInput(oVector[0]);
    writer->SetFileName("firstMesh.vtk");
    writer->Write();

    writer->SetInput(oVector[1]);
    writer->SetFileName("secondMesh.vtk");
    writer->Write();

    writer->Delete();
  }
  return oVector;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
