#ifndef __ContourToMeshFilter_h
#define __ContourToMeshFilter_h

#include "itkLightObject.h"
#include "itkObjectFactory.h"

#include "vtkAppendPolyData.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkImageStencil.h"
#include "vtkMarchingCubes.h"
#include "vtkImageData.h"

namespace itk
{
template< typename TContainer >
class ContourToMeshFilter : public LightObject
{
  public:
    typedef ContourToMeshFilter Self;
    typedef LightObject Superclass;
    typedef SmartPointer< Self > Pointer;
    typedef SmartPointer< const Self > ConstPointer;

    /** Method for creation through object factory */
    itkNewMacro( Self );

    /** Run-time type information */
    itkTypeMacro( ContourToMeshFilter, LightObject );
    
    typedef TContainer ContainerType;
    typedef typename ContainerType::const_iterator ContainerConstIterator;

    void SetImage( vtkImageData* iImage )
    {
      m_Image = iImage;
    }
    void SetSpacing( double iSpacing[3] )
    {
      m_Spacing[0] = iSpacing[0];
      m_Spacing[1] = iSpacing[1];
      m_Spacing[2] = iSpacing[2];
    }
    
    void ProcessContours( const ContainerType& iContainer )
    {
      if( iContainer.empty() )
        {
        std::cerr << "error, no contours!" << std::endl;
        return;
        }
      else
        {
        vtkSmartPointer< vtkAppendPolyData > append =
          vtkSmartPointer< vtkAppendPolyData >::New();

        ContainerConstIterator it = iContainer.begin();

        while( it != iContainer.end() )
          {
          append->AddInput( *it );
          ++it;
          }
        append->Update();

        vtkSmartPointer< vtkImageData > binary_image =
          vtkSmartPointer< vtkImageData >::New();
        binary_image->SetScalarTypeToUnsignedChar();
        
        vtkSmartPointer< vtkLinearExtrusionFilter > extrude =
          vtkSmartPointer< vtkLinearExtrusionFilter >::New();
        extrude->SetInput( append->GetOutput() );
        extrude->SetVector( m_Spacing );
        extrude->CappingOn();
        extrude->Update();

        vtkSmartPointer< vtkPolyDataToImageStencil > stencil_converter =
          vtkSmartPointer< vtkPolyDataToImageStencil >::New();
        vtkSmartPointer< vtkImageStencil > stencil =
          vtkSmartPointer< vtkImageStencil >::New();
        stencil_converter->SetInput( extrude->GetOutput() );
        stencil_converter->SetTolerance( 0 );
        stencil_converter->SetInformationInput( m_Image );
        stencil_converter->Update();

        stencil->SetStencil( stencil_converter->GetOutput() );
        stencil->SetInput( m_Image );

        vtkSmartPointer< vtkMarchingCubes > mc =
          vtkSmartPointer< vtkMarchingCubes >::New();
        mc->SetInput( stencil->GetOutput() );
        mc->SetValue( 0., 0.5 );
        mc->Update();

        if( !m_Output )
          {
          m_Output = vtkPolyData::New();
          }
        m_Output->ShallowCopy( mc->GetOutput() );
        }
    }

    vtkPolyData* GetOutput()
    {
      return m_Output;
    }
    
  protected:
    ContourToMeshFilter() : m_Image( NULL ), m_Output( NULL )
    {
      m_Spacing[0] = 0.;
      m_Spacing[1] = 0.;
      m_Spacing[2] = 0.;
    }
    ~ContourToMeshFilter() {}

    vtkImageData* m_Image;
    vtkPolyData* m_Output;

    double m_Spacing[3];
};
}
#endif
