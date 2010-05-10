#ifndef __itkBinaryMaskImageToGoFigureMeshAttributes_h
#define __itkBinaryMaskImageToGoFigureMeshAttributes_h

namespace itk
{
  template< class TInput, class TMask >
  class BinaryMaskImageToGoFigureMeshAttributes : public LightObject
  {
  public:
    typedef BinaryMaskImageToGoFigureMeshAttributes              Self;
    typedef LightObject                 Superclass;
    typedef SmartPointer< Self >        Pointer;
    typedef SmartPointer< const Self >  ConstPointer;

    typedef TInput ImageType;
    typedef typename ImageType::Pointer     ImagePointer;
    typedef typename ImageType::SizeType    ImageSizeType;
    typedef typename ImageType::SpacingType ImageSpacingType;

    typedef TMask MaskImageType;
    typedef typename MaskImageType::Pointer MaskImagePointer;

    itkStaticConstMacro( ImageDimension, unsigned int,
                         ImageType::ImageDimension );

    /** Method for creation through object factory */
    itkNewMacro( Self );

    /** Run-time type information */
    itkTypeMacro( BinaryMaskImageToGoFigureMeshAttributes, LightObject );

    /** Display */
    void PrintSelf( std::ostream& os, Indent indent ) const;

    typedef bool                                          LabelType;

    typedef ShapeLabelObject< LabelType, ImageDimension > ShapeLabelObjectType;
    typedef typename ShapeLabelObjectType::Pointer        ShapeLabelObjectPointer;

    typedef StatisticsLabelObject< LabelType, ImageDimension >
                                                          StatLabelObjectType;
    typedef typename StatLabelObjectType::Pointer         StatLabelObjectPointer;

    typedef LabelMap< ShapeLabelObjectType >              ShapeLabelMapType;
    typedef LabelMap< StatLabelObjectType >               StatLabelMapType;

    typedef LabelImageToShapeLabelMapFilter< SegmentImageType, ShapeLabelMapType >
                                                          ShapeConverterType;
    typedef ShapeConverterType::Pointer                   ShapeConverterPointer;

    typedef LabelImageToStatisticsLabelMapFilter< BinaryMaskImageType,
              ImageType, StatLabelMapType >               StatConverterType;
    typedef typename StatConverterType::Pointer           StatConverterPointer;

    itkStaticConstMacro( ImageDimension, unsigned int, TImage::ImageDimension );

    void SetImage( ImageType* iInput );
    void SetMaskImage( MaskImageType* iMask );

    void Update();
  
  protected:
    ImagePointer m_InputImage;
    BinaryMaskImagePointer m_MaskImage;

    unsigned int m_Size;
    double m_PhysicalSize;
    double m_Mean;
    double m_Sum;

    void GenerateData();

  private:
    BinaryMaskImageToGoFigureMeshAttributes( const Self& );
    void operator = ( const Self& );
  };
}
#include "itkBinaryMaskImageToGoFigureMeshAttributes.txx"
#endif
