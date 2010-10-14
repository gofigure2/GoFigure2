/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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

#ifndef __QGoFilterSemiAutoBase_txx
#define __QGoFilterSemiAutoBase_txx

// VTK to ITK
#include "vtkitkAdaptor.h"
// ITK to VTK
#include "itkImageToVTKImageFilter.h"
// ROI extraction
#include "itkRegionOfInterestImageFilter.h"

#include "vtkImageData.h"

//-------------------------------------------------------------------------
// Convert vtkImageData to itkData
template<class PixelType, unsigned int VImageDimension>
typename itk::Image<PixelType, VImageDimension> ::Pointer
QGoFilterSemiAutoBase::
ConvertVTK2ITK(vtkImageData* iInput)
{
  if(!iInput)
    {
    std::cerr << "no input to be converted to itk" << std::endl;
    }

  //Export VTK image to ITK
  m_vtk2itkImage->SetInput(iInput);
  m_vtk2itkImage->Update();

  // ImageType
  typedef itk::Image<PixelType, VImageDimension> ImageType;
  // Import VTK Image to ITK
  typedef itk::VTKImageImport<ImageType>    ImageImportType;
  typedef typename ImageImportType::Pointer ImageImportPointer;
  ImageImportPointer movingImporter = ImageImportType::New();

  ConnectPipelines<vtkImageExport, ImageImportPointer>(
    m_vtk2itkImage,
    movingImporter);

  typename ImageType::Pointer itkImage = movingImporter->GetOutput();
  itkImage->DisconnectPipeline();

  return itkImage;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template<class PixelType, unsigned int VImageDimension>
vtkImageData*
QGoFilterSemiAutoBase::
ConvertITK2VTK(typename itk::Image<PixelType, VImageDimension>::Pointer iInput)
{
  typedef itk::Image<PixelType, VImageDimension> InternalImageType;
  typedef itk::ImageToVTKImageFilter<InternalImageType> ConverterType;
  typedef typename ConverterType::Pointer          ConverterPointer;

  ConverterPointer converter = ConverterType::New();
  converter->SetInput(iInput);

  try
    {
    converter->Update();
    }
  catch (itk::ExceptionObject& err)
    {
    std::cerr << "converter Exception:" << err << std::endl;
    }

  vtkImageData* output = vtkImageData::New();
  output->DeepCopy(converter->GetOutput());

  return output;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template<class PixelType, unsigned int VImageDimension>
typename itk::Image<PixelType, VImageDimension >::Pointer
QGoFilterSemiAutoBase::
ExtractROI(typename itk::Image<PixelType, VImageDimension >::Pointer iInput,
           double* iCenter, double iRadius)
{
  typedef itk::Image<PixelType, VImageDimension>                    InternalImageType;
  typedef typename InternalImageType::PointType      InternalPointType;
  typedef typename InternalImageType::IndexType      InternalIndexType;
  typedef typename InternalImageType::SizeType       InternalSizeType;
  typedef typename InternalSizeType::SizeValueType   InternalSizeValueType;
  typedef typename InternalImageType::RegionType     InternalRegionType;
  typedef typename InternalImageType::SpacingType    InternalSpacingType;

  if (iInput.IsNull())
    {
    std::cerr << "m_FeatureImage is Null" << std::endl;
    }

  InternalSpacingType spacing = iInput->GetSpacing();

  InternalIndexType start;
  InternalPointType origin;
  InternalSizeType     size;
  size.Fill(0);

  for (unsigned int j = 0; j < VImageDimension; j++)
    {
    size[j] =
      1 + 4. * static_cast<InternalSizeValueType>(iRadius / spacing[j]);
    origin[j] = iCenter[j] - 2 * iRadius;
    }

  iInput->TransformPhysicalPointToIndex(origin, start);

  InternalRegionType region;
  region.SetSize(size);
  region.SetIndex(start);

  typedef itk::RegionOfInterestImageFilter<
    InternalImageType, InternalImageType>              ROIFilterType;
  typedef typename ROIFilterType::Pointer ROIFilterPointer;

  ROIFilterPointer roi = ROIFilterType::New();
  roi->SetInput(iInput);
  roi->SetRegionOfInterest(region);
  try
    {
    roi->Update();
    }
  catch (itk::ExceptionObject& err)
    {
    std::cerr << "roi Exception:" << err << std::endl;
    }

  return roi->GetOutput();
}
//-------------------------------------------------------------------------

#endif // QGoFilterSemiAutoBase
