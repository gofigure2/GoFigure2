#include "itkvtkPolyDataToBinaryMaskImageFilter.h"

#include "vtkPointData.h"
#include "vtkitkAdaptor.h"

namespace itk
{
template< class TInput, class TOutput >
vtkPolyDataToBinaryMaskImageFilter< TInput, TOutput >::
vtkPolyDataToBinaryMaskImageFilter()
{
  m_WhiteImage = vtkSmartPointer<vtkImageData>::New();
  m_Pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
  m_ImageStencil = vtkSmartPointer<vtkImageStencil>::New();
  m_VTKExporter = vtkSmartPointer<vtkImageExport>::New();
  m_ITKImporter = ImageImportType::New();
}

template< class TInput, class TOutput >
vtkPolyDataToBinaryMaskImageFilter< TInput, TOutput >::
~vtkPolyDataToBinaryMaskImageFilter()
{}

template< class TInput, class TOutput >
void
vtkPolyDataToBinaryMaskImageFilter< TInput, TOutput >::
SetPolyData( vtkPolyData* iMesh )
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
  m_Mesh->GetBounds( bounds );

  // origin is the lowest bound!
  double origin[3];
  origin[0] = bounds[0];
  origin[1] = bounds[2];
  origin[2] = bounds[4];

  // spacing is got from the input image
  InputImageSpacingType itk_spacing = input->GetSpacing();
  double vtk_spacing[3] = {0., 0., 0.};
  int vtk_size[3] = {0, 0, 0 };

  for( unsigned int dim = 0; dim < ImageDimension; dim++ )
    {
    vtk_spacing[dim] = static_cast< double >( itk_spacing[dim] );
    vtk_size[dim] = static_cast< int >( vcl_ceil( bounds[2*dim+1] - bounds[2*dim] ) / vtk_spacing[dim] );
    }

  m_WhiteImage->SetSpacing( vtk_spacing );
  m_WhiteImage->SetDimensions( vtk_size );
  m_WhiteImage->SetExtent(0, vtk_size[0], 0, vtk_size[1], 0, vtk_size[2] );
  m_WhiteImage->SetOrigin( origin );
  m_WhiteImage->SetScalarTypeToUnsignedChar();
  m_WhiteImage->AllocateScalars();

  // fill the image with foreground voxels:
  unsigned char inval = 255;
  unsigned char outval = 0;
  vtkIdType count = m_WhiteImage->GetNumberOfPoints();
  for (vtkIdType i = 0; i < count; ++i)
    {
    m_WhiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
    }

  m_Pol2stenc->SetInput( m_Mesh );
  m_Pol2stenc->SetOutputOrigin(origin);
  m_Pol2stenc->SetOutputSpacing(vtk_spacing);
  m_Pol2stenc->SetOutputWholeExtent(m_WhiteImage->GetExtent());
  m_Pol2stenc->Update();

  // cut the corresponding white image and set the background:
  m_ImageStencil->SetInput( m_WhiteImage );
  m_ImageStencil->SetStencil( m_Pol2stenc->GetOutput() );
  m_ImageStencil->ReverseStencilOff();
  m_ImageStencil->SetBackgroundValue( outval );
  m_ImageStencil->Update();

  //Export VTK image to ITK
  m_VTKExporter->SetInput( m_ImageStencil->GetOutput() );

//   typedef itk::VTKImageImport<ImageType> ImageImportType;
//   ImageImportType::Pointer m_ITKImporter = ImageImportType::New();

  ConnectPipelines< vtkImageExport, ImageImportPointer >( m_VTKExporter, m_ITKImporter );

  this->GraftOutput( m_ITKImporter->GetOutput() );
}

}