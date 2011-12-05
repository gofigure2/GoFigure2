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
#ifndef __itkvtkPolyDataToBinaryMaskImageFilter_h
#define __itkvtkPolyDataToBinaryMaskImageFilter_h

#include "GoFiltersConfigure.h"

#include "itkImageToImageFilter.h"

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkImageStencil.h"
#include "vtkImageExport.h"

#include "itkVTKImageImport.h"
#include "itkVTKImageToImageFilter.h"

namespace itk
{
template< class TInput, class TOutput >
class GOFILTERS_EXPORT vtkPolyDataToBinaryMaskImageFilter:
  public ImageToImageFilter< TInput, TOutput >
{
public:
  typedef vtkPolyDataToBinaryMaskImageFilter    Self;
  typedef ImageToImageFilter< TInput, TOutput > Superclass;
  typedef SmartPointer< Self >                  Pointer;
  typedef SmartPointer< const Self >            ConstPointer;

  /** Method for creation through object factory */
  itkNewMacro(Self);

  /** Run-time type information */
  itkTypeMacro(vtkPolyDataToBinaryMaskImageFilter, ImageToImageFilter);

  typedef TInput                                InputImageType;
  typedef typename InputImageType::Pointer      InputImagePointer;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;
  typedef typename InputImageType::SizeType     InputImageSizeType;
  typedef typename InputImageType::SpacingType  InputImageSpacingType;

  itkStaticConstMacro(ImageDimension, unsigned int,
                      InputImageType::ImageDimension);

  typedef TOutput                               BinaryMaskImageType;
  typedef typename BinaryMaskImageType::Pointer BinaryMaskImagePointer;

  typedef VTKImageImport< BinaryMaskImageType > ImageImportType;
  typedef typename ImageImportType::Pointer     ImageImportPointer;

  virtual void SetPolyData(vtkPolyData *iMesh);

protected:
  vtkPolyDataToBinaryMaskImageFilter();
  ~vtkPolyDataToBinaryMaskImageFilter();

  vtkPolyData *m_Mesh;
  vtkSmartPointer< vtkImageData >              m_WhiteImage;
  vtkSmartPointer< vtkPolyDataToImageStencil > m_Pol2stenc;
  vtkSmartPointer< vtkImageStencil >           m_ImageStencil;
  vtkSmartPointer< vtkImageExport >            m_VTKExporter;
  ImageImportPointer m_ITKImporter;
  virtual void GenerateData();

private:
  vtkPolyDataToBinaryMaskImageFilter(const Self &);
  void operator=(const Self &);
};
}

#include "itkvtkPolyDataToBinaryMaskImageFilter.txx"
#endif
