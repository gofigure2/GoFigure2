#ifndef __itkvtkPolyDataToGoFigureMeshAttributes_txx
#define __itkvtkPolyDataToGoFigureMeshAttributes_txx

#include "itkvtkPolyDataToGoFigureMeshAttributes.h"

namespace itk
{
template< class TImage >
vtkPolyDataToGoFigureMeshAttributes< TImage >::
vtkPolyDataToGoFigureMeshAttributes()
{
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
SetPolyData( vtkPolyData* iMesh )
{
  m_Mesh = iMesh;
}

template< class TImage >
void
vtkPolyDataToGoFigureMeshAttributes< TImage >::
SetImage( ImageType* iImage )
{
  m_Image = iImage;
}

template< class TImage >
void
vtkPolyDataToGoFigureMeshAttributes< TImage >::
Update()
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
void
vtkPolyDataToGoFigureMeshAttributes< TImage >::
GenerateData()
{
  m_Binarizer->SetInput( m_Image );
  m_Binarizer->SetPolyData( m_Mesh );
  m_Binarizer->Update();

  m_AttributeCalculator->SetImage( m_Image );
  m_AttributeCalculator->SetMaskImage( m_Binarizer->GetOutput() );
  m_AttributeCalculator->Update();

  m_Size = m_AttributeCalculator->GetSize();
  m_PhysicalSize = m_AttributeCalculator->GetPhysicalSize();
  m_Mean = m_AttributeCalculator->GetMeanIntensity();
  m_Sum = m_AttributeCalculator->GetSumIntensity();
}
}

#endif
