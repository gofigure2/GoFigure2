#ifndef __ContourToMeshFilter_h
#define __ContourToMeshFilter_h

#include "itkLightObject.h"

template< typename TContainer >
class ContourToMeshFilter : public LightObject
{
  public:
    void SetContours( const TContainer& iContainer )
    {
      vtkSmartPointer< vtkAppendPolyData > append =
        vtkSmartPointer< vtkAppendPolyData >::New();

      ContainerIterator it = iContainer.begin();

      while( it != iContainer.end() )
        {
        append->AddInput( *it );
        ++it;
        }

      m_Input = append->GetOutput();
    }

    void Update()
    {
      vtkSmartPointer< vtkLinearExtrusionFilter > extrude =
        vtkSmartPointer< vtkLinearExtrusionFilter >::New();
      extrude->SetInput( m_Input );
      extrude->SetVector( m_Spacing );
      extrude->CappingOn();
      extrude->Update();

      vtkSmartPointer< vtkPolyDataToImageStencil > stencil_converter =
        vtkSmartPointer< vtkPolyDataToImageStencil >::New();
      vtkSmartPointer< vtkImageStencil > stencil =
        vtkSmartPointer< vtkImageStencil >::New();
      stencil_converter->SetInput( extrude->GetOutput() );
      stencil_converter->SetInformationInput( m_Image );
      stencil_converter->Update();

      stencil->SetStencil( stencil_converter->GetOutput() );
      stencil->SetInput( m_Image );

      vtkSmartPointer< vtkMarchingCubes > mc =
        vtkSmartPointer< vtkMarchingCubes >::New();
      mc->SetInput( stencil->GetOutput() );
      mc->marching->SetValue( 0., 0.5 );
      mc->Update();

      m_Output = mc->GetOutput();
    }

    vtkPolyData* GetOutput()
    {
      return m_Output;
    }
  protected:
    ContourToMeshFilter();
    ~ContourToMeshFilter();

    vtkPolyData* m_Input;
    vtkPolyData* m_Output;
};
#endif
