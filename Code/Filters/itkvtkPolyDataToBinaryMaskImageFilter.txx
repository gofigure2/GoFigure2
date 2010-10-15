/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
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
#ifndef __itkvtkPolyDataToBinaryMaskImageFilter_txx
#define __itkvtkPolyDataToBinaryMaskImageFilter_txx

#include "itkvtkPolyDataToBinaryMaskImageFilter.h"

#include "vtkPointData.h"
#include "vtkitkAdaptor.h"

namespace itk
{
template< class TInput, class TOutput >
vtkPolyDataToBinaryMaskImageFilter< TInput, TOutput >::vtkPolyDataToBinaryMaskImageFilter()
{
  m_WhiteImage = vtkSmartPointer< vtkImageData >::New();
  m_Pol2stenc = vtkSmartPointer< vtkPolyDataToImageStencil >::New();
  m_ImageStencil = vtkSmartPointer< vtkImageStencil >::New();
  m_VTKExporter = vtkSmartPointer< vtkImageExport >::New();
  m_ITKImporter = ImageImportType::New();
}

template< class TInput, class TOutput >
vtkPolyDataToBinaryMaskImageFilter< TInput, TOutput >::
~vtkPolyDataToBinaryMaskImageFilter()
{}

template< class TInput, class TOutput >
void
vtkPolyDataToBinaryMaskImageFilter< TInput, TOutput >::SetPolyData(vtkPolyData *iMesh)
{
  m_Mesh = iMesh;
  this->Modified();
}

template< class TInput, class TOutput >
void
vtkPolyDataToBinaryMaskImageFilter< TInput, TOutput >::GenerateData()
{
  InputImageConstPointer input = this->GetInput();

  double bounds[6];

  m_Mesh->GetBounds(bounds);

  if ( ( m_Mesh->GetNumberOfCells() == 0 )
       && ( m_Mesh->GetNumberOfPoints() == 0 ) )
    {
    itkExceptionMacro(
      "vtkPolyDataToBinaryMaskImageFilter::GenerateData(): m_Mesh has nor cells nor points");
    return;
    }

  // origin is the lowest bound!
  double origin[3];
  origin[0] = bounds[0];
  origin[1] = bounds[2];
  origin[2] = bounds[4];

  // spacing is got from the input image
  InputImageSpacingType itk_spacing = input->GetSpacing();
  double                vtk_spacing[3] = { 0., 0., 0. };
  int                   vtk_size[3] = { 0, 0, 0 };

  for ( unsigned int dim = 0; dim < ImageDimension; dim++ )
    {
    vtk_spacing[dim] = static_cast< double >(itk_spacing[dim]);
    vtk_size[dim] = static_cast< int >(vcl_ceil(bounds[2 * dim + 1] - bounds[2 * dim]) / vtk_spacing[dim]);
    }

  m_WhiteImage->SetSpacing(vtk_spacing);
  m_WhiteImage->SetDimensions(vtk_size);
  m_WhiteImage->SetExtent(0, vtk_size[0], 0, vtk_size[1], 0, vtk_size[2]);
  m_WhiteImage->SetOrigin(origin);
  m_WhiteImage->SetScalarTypeToUnsignedChar();
  m_WhiteImage->AllocateScalars();

  // fill the image with foreground voxels:
  unsigned char inval = 255;
  unsigned char outval = 0;
  vtkIdType     count = m_WhiteImage->GetNumberOfPoints();
  for ( vtkIdType i = 0; i < count; ++i )
    {
    m_WhiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
    }

  m_Pol2stenc->SetInput(m_Mesh);
  m_Pol2stenc->SetOutputOrigin(origin);
  m_Pol2stenc->SetOutputSpacing(vtk_spacing);
  m_Pol2stenc->SetOutputWholeExtent( m_WhiteImage->GetExtent() );
  m_Pol2stenc->Update();

  // cut the corresponding white image and set the background:
  m_ImageStencil->SetInput(m_WhiteImage);
  m_ImageStencil->SetStencil( m_Pol2stenc->GetOutput() );
  m_ImageStencil->ReverseStencilOff();
  m_ImageStencil->SetBackgroundValue(outval);
  m_ImageStencil->Update();

  //Export VTK image to ITK
  m_VTKExporter->SetInput( m_ImageStencil->GetOutput() );

//   typedef itk::VTKImageImport<ImageType> ImageImportType;
//   ImageImportType::Pointer m_ITKImporter = ImageImportType::New();

  ConnectPipelines< vtkImageExport, ImageImportPointer >(m_VTKExporter, m_ITKImporter);

  this->GraftOutput( m_ITKImporter->GetOutput() );
}
}

#endif
