#ifndef __itkBinaryMaskImageToGoFigureMeshAttributes_txx
#define __itkBinaryMaskImageToGoFigureMeshAttributes_txx

namespace itk
{

template< class TInput, class TMask >
BinaryMaskImageToGoFigureMeshAttributes< TInput, TMask >::
BinaryMaskImageToGoFigureMeshAttributes()
{
}

template< class TInput, class TMask >
BinaryMaskImageToGoFigureMeshAttributes< TInput, TMask >::
~BinaryMaskImageToGoFigureMeshAttributes()
{
}

template< class TInput, class TMask >
void
BinaryMaskImageToGoFigureMeshAttributes< TInput, TMask >::
SetImage( ImageType* iInput )
{
  m_InputImage = iInput;
}

template< class TInput, class TMask >
void
BinaryMaskImageToGoFigureMeshAttributes< TInput, TMask >::
SetMaskImage( MaskImageType* iMask )
{
  m_MaskImage = iMask;
}

template< class TInput, class TMask >
void
BinaryMaskImageToGoFigureMeshAttributes< TInput, TMask >::
GenerateData()
{
  // shape stuff
  ShapeConverterPointer shapeConverter = ShapeConverterType::New();
  shapeConverter->SetInput( m_MaskImage );
  shapeConverter->SetBackgroundValue ( 0 );
  shapeConverter->Update();

  ShapeLabelMapPointer shapeLabelMap = shapeConverter->GetOutput();
  const ShapeLabelObjectType *shapeObject = shapeLabelMap->GetLabelObject();

  // stat stuff
  StatConverterPointer statConverter = StatConverterType::New();
  statConverter->SetInput( m_MaskImage );
  statConverter->SetFeatureImage( m_InputImage );
  statConverter->SetBackgroundValue( 0 );
  statConverter->Update();

  StatLabelMapPointer statLabelMap = statConverter->GetOutput();
  const StatLabelObjectType *statObject = statLabelMap->GetLabelObject();

  // Number of voxels;
  m_Size = shapeObject->Size();

  // Volume or area in um^3
  m_PhysicalSize = shapeObject->GetPhysicalSize();

  m_Mean = statObject->GetMean();
  m_Sum = statObject->GetSum();
}

}

#endif