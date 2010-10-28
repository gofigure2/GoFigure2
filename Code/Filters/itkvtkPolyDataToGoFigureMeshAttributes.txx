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
#ifndef __itkvtkPolyDataToGoFigureMeshAttributes_txx
#define __itkvtkPolyDataToGoFigureMeshAttributes_txx

#include "itkvtkPolyDataToGoFigureMeshAttributes.h"

#include "vtkTriangle.h"

namespace itk
{
template< class TImage >
vtkPolyDataToGoFigureMeshAttributes< TImage >::vtkPolyDataToGoFigureMeshAttributes()
{
  m_IntensityComputation = true;
  m_Binarizer = PolyDataToBinaryMaskImageFilterType::New();
  m_AttributeCalculator = BinaryMaskImageToGoFigureMeshAttributesType::New();
}

template< class TImage >
vtkPolyDataToGoFigureMeshAttributes< TImage >::
~vtkPolyDataToGoFigureMeshAttributes()
{}

template< class TImage >
void
vtkPolyDataToGoFigureMeshAttributes< TImage >::
SetIntensityBasedComputation( const bool& iComputation )
{
  this->m_IntensityComputation = iComputation;
}

template< class TImage >
void
vtkPolyDataToGoFigureMeshAttributes< TImage >::SetPolyData(vtkPolyData *iMesh)
{
  m_Mesh = iMesh;
}

template< class TImage >
void
vtkPolyDataToGoFigureMeshAttributes< TImage >::SetImage(ImageType *iImage)
{
  m_Image = iImage;
}

template< class TImage >
void
vtkPolyDataToGoFigureMeshAttributes< TImage >::Update()
{
  GenerateData();
}

template< class TImage >
unsigned int
vtkPolyDataToGoFigureMeshAttributes< TImage >::GetSize()
{
  return m_Size;
}

template< class TImage >
double
vtkPolyDataToGoFigureMeshAttributes< TImage >::GetPhysicalSize()
{
  return m_PhysicalSize;
}

template< class TImage >
double vtkPolyDataToGoFigureMeshAttributes< TImage >::GetMeanIntensity()
{
  return m_Mean;
}

template< class TImage >
double vtkPolyDataToGoFigureMeshAttributes< TImage >::GetSumIntensity()
{
  return m_Sum;
}

template< class TImage >
double vtkPolyDataToGoFigureMeshAttributes< TImage >::GetArea()
{
  return m_Area;
}

template< class TImage >
void
vtkPolyDataToGoFigureMeshAttributes< TImage >::GenerateData()
{
  m_Binarizer->SetInput(m_Image);
  m_Binarizer->SetPolyData(m_Mesh);
  try
    {
    m_Binarizer->Update();
    }
  catch(itk::ExceptionObject & e)
    {
    std::cerr << "Exception: " << e << std::endl;
    }

/*
 typename WriterType::Pointer writer = WriterType::New();
 writer->SetInput( m_Binarizer->GetOutput() );
 writer->SetFileName( "output.mha" );
 writer->Update();
*/

  m_AttributeCalculator->SetImage(m_Image);
  m_AttributeCalculator->SetMaskImage( m_Binarizer->GetOutput() );
  m_AttributeCalculator->SetIntensityBasedComputation( this->m_IntensityComputation );
  m_AttributeCalculator->Update();

  m_Size = m_AttributeCalculator->GetSize();
  m_PhysicalSize = m_AttributeCalculator->GetPhysicalSize();
  m_Mean = m_AttributeCalculator->GetMeanIntensity();
  m_Sum = m_AttributeCalculator->GetSumIntensity();

  ComputeArea();
}

template< class TImage >
void
vtkPolyDataToGoFigureMeshAttributes< TImage >::ComputeArea()
{
  vtkIdType NbOfCells = m_Mesh->GetNumberOfCells();

  m_Area = 0.;

  for ( vtkIdType i = 0; i < NbOfCells; ++i )
    {
    vtkTriangle *t = dynamic_cast< vtkTriangle * >( m_Mesh->GetCell(i) );
    m_Area += t->ComputeArea();
    }
}
}

#endif
