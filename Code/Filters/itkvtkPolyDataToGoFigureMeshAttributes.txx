#ifndef __itkvtkPolyDataToGoFigureMeshAttributes_txx
#define __itkvtkPolyDataToGoFigureMeshAttributes_txx

#include "itkvtkPolyDataToGoFigureMeshAttributes.h"

#include "vtkTriangle.h"

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
double vtkPolyDataToGoFigureMeshAttributes< TImage >::GetArea()
{
  return m_Area;
}

template< class TImage >
void
vtkPolyDataToGoFigureMeshAttributes< TImage >::
GenerateData()
{
  m_Binarizer->SetInput( m_Image );
  m_Binarizer->SetPolyData( m_Mesh );
  try
  {
  m_Binarizer->Update();
  }
  catch( itk::ExceptionObject & e )
  {
    std::cerr << "Exception: " << e << std::endl;
  }

/* 
 typename WriterType::Pointer writer = WriterType::New();
 writer->SetInput( m_Binarizer->GetOutput() );
 writer->SetFileName( "output.mha" );
 writer->Update();
*/

  m_AttributeCalculator->SetImage( m_Image );
  m_AttributeCalculator->SetMaskImage( m_Binarizer->GetOutput() );
  m_AttributeCalculator->Update();

  m_Size = m_AttributeCalculator->GetSize();
  m_PhysicalSize = m_AttributeCalculator->GetPhysicalSize();
  m_Mean = m_AttributeCalculator->GetMeanIntensity();
  m_Sum = m_AttributeCalculator->GetSumIntensity();

  ComputeArea();
}

template< class TImage >
void
vtkPolyDataToGoFigureMeshAttributes< TImage >::
ComputeArea()
{
  vtkIdType NbOfCells = m_Mesh->GetNumberOfCells();
  m_Area = 0.;

  for( vtkIdType i = 0; i < NbOfCells; ++i )
    {
    vtkTriangle* t = dynamic_cast< vtkTriangle* >( m_Mesh->GetCell( i ) );
    m_Area += t->ComputeArea();
    }
}

}

#endif
