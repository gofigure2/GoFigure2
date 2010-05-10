#ifndef __vtkPolyDataToBinaryMaskImageFilter_h
#define __vtkPolyDataToBinaryMaskImageFilter_h

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
class vtkPolyDataToBinaryMaskImageFilter :
  public ImageToImageFilter< TInput, TOutput >
{
  public:
    typedef vtkPolyDataToBinaryMaskImageFilter    Self;
    typedef ImageToImageFilter< TInput, TOutput > Superclass;
    typedef SmartPointer< Self >                  Pointer;
    typedef SmartPointer< const Self >            ConstPointer;

    /** Method for creation through object factory */
    itkNewMacro( Self );

    /** Run-time type information */
    itkTypeMacro( vtkPolyDataToBinaryMaskImageFilter, ImageToImageFilter );

    typedef TInput                                  InputImageType;
    typedef typename InputImageType::Pointer        InputImagePointer;
    typedef typename InputImageType::ConstPointer   InputImageConstPointer;
    typedef typename InputImageType::SizeType       InputImageSizeType;
    typedef typename InputImageType::SpacingType    InputImageSpacingType;

    itkStaticConstMacro( ImageDimension, unsigned int,
                         InputImageType::ImageDimension );

    typedef TOutput                                 BinaryMaskImageType;
    typedef typename BinaryMaskImageType::Pointer   BinaryMaskImagePointer;

    typedef VTKImageImport< BinaryMaskImageType >   ImageImportType;
    typedef typename ImageImportType::Pointer       ImageImportPointer;

    virtual void SetPolyData( vtkPolyData* iMesh );

  protected:
    vtkPolyDataToBinaryMaskImageFilter();
    ~vtkPolyDataToBinaryMaskImageFilter();

    vtkPolyData*                                m_Mesh;
    vtkSmartPointer<vtkImageData>               m_WhiteImage;
    vtkSmartPointer<vtkPolyDataToImageStencil>  m_Pol2stenc;
    vtkSmartPointer<vtkImageStencil>            m_ImageStencil;
    vtkSmartPointer<vtkImageExport>             m_VTKExporter;
    ImageImportPointer                          m_ITKImporter;
    virtual void GenerateData();

  private:
    vtkPolyDataToBinaryMaskImageFilter( const Self& );
    void operator = ( const Self& );
};

}

#include "itkvtkPolyDataToBinaryMaskImageFilter.txx"
#endif
