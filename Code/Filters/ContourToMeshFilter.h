#ifndef __ContourToMeshFilter_h
#define __ContourToMeshFilter_h

#include "itkLightObject.h"
#include "itkObjectFactory.h"

#include "vtkAppendPolyData.h"
#include "vtkMath.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoissonReconstruction.h"
#include "vtkNormalEstimationFilter.h"
#include "vtkPolyDataWriter.h"

#include <iostream>

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

        // compute center of mass
        ContainerConstIterator it = iContainer.begin();

        while( it != iContainer.end() )
          {
          if( (*it) )
            {
            append->AddInput( *it );
            }
          ++it;
          }


        vtkSmartPointer< vtkNormalEstimationFilter > normal_filter = 
          vtkSmartPointer< vtkNormalEstimationFilter >::New();
        normal_filter->SetInput( append->GetOutput() );
        normal_filter->Update();

        // run the Poisson Reconstruction
        vtkSmartPointer< vtkPoissonReconstruction > poissonFilter =
          vtkSmartPointer< vtkPoissonReconstruction >::New();
        poissonFilter->SetInput(  normal_filter->GetOutput() );
        poissonFilter->SetDepth( 7 );
        poissonFilter->SetConfidence( 1. );
        poissonFilter->Update();

        if( !m_Output )
          {
          m_Output = vtkPolyData::New();
          }
        m_Output->ShallowCopy( poissonFilter->GetOutput() );
	
	double bounds[6];
        m_Output->GetBounds( bounds );

        vtkSmartPointer< vtkPolyDataWriter > writer = vtkSmartPointer< vtkPolyDataWriter >::New();
        writer->SetInput( m_Output );
        writer->SetFileName( "mesh.vtk" );
        writer->Write();
        }
      }

    vtkPolyData* GetOutput()
      {
      return m_Output;
      }
    
  protected:
    ContourToMeshFilter() : m_Output( NULL )
    {}
    ~ContourToMeshFilter() {}

    vtkPolyData* m_Output;
};
}
#endif
