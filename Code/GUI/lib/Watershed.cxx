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
#include "Watershed.h"

#include "QGoGUILibConfigure.h"

// for apply method
#include "vtkImageExport.h"
#include "vtkImageData.h"

//Connection VTK/ITK
#include "vtkImageExport.h"
#include "itkVTKImageImport.h"
#include "itkVTKImageToImageFilter.h"
#include "vtkitkAdaptor.h"

// ITK filter
#include "itkWatershedBasedCellSegmentation.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageToVTKImageFilter.h"

//--------------------------------------------------------------------------
Watershed::Watershed()
{
  /*m_Radius = 3.0;
  m_Iterations = 15;
  m_Curvature = 5;
  m_Center[0] = 0;
  m_Center[1] = 0;
  m_Dimension = 3;*/
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
Watershed::
~Watershed()
{
  //m_Input->Delete();
}

//--------------------------------------------------------------------------
/*
//--------------------------------------------------------------------------
void
ChanAndVesLevelSet::
setCenter(double* iCenter)
{
  m_Center[0] = iCenter[0];
  m_Center[1] = iCenter[1];
  m_Center[2] = iCenter[2];
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
ChanAndVesLevelSet::
setRadius(double iRadius)
{
  m_Radius = iRadius;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
ChanAndVesLevelSet::
setIterations(int iIterations)
{
  m_Iterations = iIterations;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
ChanAndVesLevelSet::
setCurvature(int iCurvature)
{
  m_Curvature = iCurvature;
}
//--------------------------------------------------------------------------
*/
/*
//--------------------------------------------------------------------------
void
ChanAndVesLevelSet::
setDimension(int iDimension)
{
  m_Dimension = iDimension;
}
//--------------------------------------------------------------------------
*/

//--------------------------------------------------------------------------
vtkImageData *
Watershed::Apply()
{
  //itkWatershedBasedCellSegmentation
/*
  std::cout<<"PARAMETERS:" <<std::endl;
  std::cout << "Radius: " << m_Radius<<std::endl;
  std::cout << "Iterations: " << m_Iterations<<std::endl;
  std::cout << "Curvature: " << m_Curvature<<std::endl;
  std::cout << "Seed position: " << m_Center[0]
                        << " - " << m_Center[1]
                        << " - " << m_Center[2] << std::endl;
*/

  //Export VTK image to ITK
  vtkImageExport *vtk2itkImage = vtkImageExport::New();

  vtk2itkImage->SetInput( getInput() );
  vtk2itkImage->Update();

  // Define the dimension of the images
  const unsigned int Dimension = 3;
  typedef itk::Image< unsigned char, Dimension > FeatureImageType;
  typedef FeatureImageType::Pointer              FeatureImagePointer;
  typedef itk::Image< double, Dimension >        InputImageType;
  typedef InputImageType::IndexType              InputImageIndexType;
  typedef InputImageType::Pointer                InputImagePointer;
  typedef itk::Image< int, Dimension >           SegmentImageType;
  typedef SegmentImageType::Pointer              SegmentImagePointer;

  typedef itk::ImageFileReader< FeatureImageType > FeatureReaderType;
  typedef itk::ImageFileWriter< SegmentImageType > SegmentWriterType;
  typedef itk::WatershedBasedCellSegmentation< FeatureImageType, InputImageType, SegmentImageType >
  SegmentationFilterType;

  // ImageType
  typedef itk::Image< unsigned char, Dimension > ImageType;
  // Import VTK Image to ITK
  typedef itk::VTKImageImport< ImageType > ImageImportType;
  typedef ImageImportType::Pointer         ImageImportPointer;
  ImageImportPointer movingImporter = ImageImportType::New();

  ConnectPipelines< vtkImageExport, ImageImportPointer >(
    vtk2itkImage,
    movingImporter);

  std::cout << "===================IMAGE==========================" << std::endl;
  std::cout << "==================================================" << std::endl;

  //movingImporter->Update();
  movingImporter->GetOutput()->Print(cout);

  SegmentationFilterType::Pointer filter = SegmentationFilterType::New();
  filter->SetInput( movingImporter->GetOutput() );
  //filter->Set
  filter->Update();
  // segmentation parameters

  std::cout << "segmentation finished" << std::endl;

  typedef itk::ImageToVTKImageFilter< SegmentImageType > ConverterType;

  ConverterType::Pointer converter = ConverterType::New();
  converter->SetInput( filter->GetOutput() );
  converter->Update();

  std::cout << "conversion finished" << std::endl;

  vtkImageData *output = vtkImageData::New();
  output->DeepCopy( converter->GetOutput() );

  std::cout << "copy finished" << std::endl;

  return output;
}

//--------------------------------------------------------------------------
