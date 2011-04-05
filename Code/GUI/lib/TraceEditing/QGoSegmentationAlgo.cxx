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
   * \note Nicolas-to be tested....
   */
  return extractVOI->GetOutput();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

// Convert vtkImageData to itkData
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
