#ifndef __itkvtkPolyDataToGoFigureMeshAttributes_h
#define __itkvtkPolyDataToGoFigureMeshAttributes_h

#include "itkLightObject.h"
#include "itkvtkPolyDataToBinaryMaskImageFilter.h"
#include "itkBinaryMaskImageToGoFigureMeshAttributes.h"

namespace itk
{
template< class TImage >
class vtkPolyDataToGoFigureMeshAttributes : public LightObject
{
  public:
    typedef vtkPolyDataToGoFigureMeshAttributes Self;
    typedef LightObject                 Superclass;
    typedef SmartPointer< Self >        Pointer;
    typedef SmartPointer< const Self >  ConstPointer;

    typedef TImage ImageType;
    typedef typename ImageType::Pointer ImagePointer;

    /** Method for creation through object factory */
    itkNewMacro( Self );

    /** Run-time type information */
    itkTypeMacro( vtkPolyDataToGoFigureMeshAttributes, LightObject );

    typedef vtkPolyDataToBinaryMaskImageFilter< ImageType, ImageType >
      PolyDataToBinaryMaskImageFilterType;
    typedef typename PolyDataToBinaryMaskImageFilterType::Pointer
      PolyDataToBinaryMaskImageFilterPointer;

    typedef BinaryMaskImageToGoFigureMeshAttributes< ImageType, ImageType >
      BinaryMaskImageToGoFigureMeshAttributesType;
    typedef typename BinaryMaskImageToGoFigureMeshAttributesType::Pointer
      BinaryMaskImageToGoFigureMeshAttributesPointer;

    virtual void SetImage( ImageType* iImage );
    virtual void SetPolyData( vtkPolyData* iMesh );
    virtual void Update();

    unsigned int GetSize();
    double GetPhysicalSize();
    double GetMeanIntensity();
    double GetSumIntensity();

  protected:
    vtkPolyDataToGoFigureMeshAttributes();
    ~vtkPolyDataToGoFigureMeshAttributes();

    vtkPolyData* m_Mesh;
    ImagePointer m_Image;

    PolyDataToBinaryMaskImageFilterPointer m_Binarizer;
    BinaryMaskImageToGoFigureMeshAttributesPointer m_AttributeCalculator;

    unsigned int m_Size;
    double m_PhysicalSize;
    double m_Mean;
    double m_Sum;

    virtual void GenerateData();

  private:
    vtkPolyDataToGoFigureMeshAttributes( const Self& );
    void operator = ( const Self& );
};
}

#include "itkvtkPolyDataToGoFigureMeshAttributes.txx"
#endif
