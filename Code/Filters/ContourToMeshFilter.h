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

        double center[3];
        center[0] = 0.;
        center[1] = 0.;
        center[2] = 0.;
        vtkIdType counter = 0;

        double p[3];
        p[0] = 0.;
        p[1] = 0.;
        p[2] = 0.;

        double n[3];
        n[0] = 0.;
        n[1] = 0.;
        n[2] = 0.;
        
        while( it != iContainer.end() )
          {
          if( (*it) )
            {
            append->AddInput( *it );
            for( vtkIdType k = 0; k < (*it)->GetNumberOfPoints(); ++k )
              {
              (*it)->GetPoint( k, p );
              center[0] += p[0];
              center[1] += p[1];
              center[2] += p[2];
              ++counter;
              }
            }
          ++it;
          }

        center[0] /= static_cast< double >( counter );
        center[1] /= static_cast< double >( counter );
        center[2] /= static_cast< double >( counter );

        append->Update();
        vtkSmartPointer< vtkPolyData > input = append->GetOutput();
        vtkSmartPointer< vtkFloatArray > normals =
          vtkSmartPointer< vtkFloatArray >::New();
        normals->Allocate( 3 * counter );
        normals->SetNumberOfTuples( counter );
        normals->SetNumberOfComponents( 3 );
        normals->SetName( "Normals" );
        
        for( vtkIdType k = 0; k < input->GetNumberOfPoints(); ++k )
          {
          input->GetPoint( k, p );
          n[0] = p[0] - center[0];
          n[1] = p[1] - center[1];
          n[2] = p[2] - center[2];
          vtkMath::Normalize( n );
          normals->SetTuple( k, n );
          }

        double bounds[6];
        input->GetBounds( bounds );
        
        // compute normal of the contour
        input->GetPointData()->SetNormals( normals );

        // run the Poisson Reconstruction
        vtkSmartPointer< vtkPoissonReconstruction > poissonFilter =
          vtkSmartPointer< vtkPoissonReconstruction >::New();
        poissonFilter->SetInput( input );
        poissonFilter->SetDepth( 7 );
        poissonFilter->SetConfidence( 1. );
        poissonFilter->Update();

        if( !m_Output )
          {
          m_Output = vtkPolyData::New();
          }
        m_Output->ShallowCopy( poissonFilter->GetOutput() );
        m_Output->GetBounds( bounds );

//        vtkSmartPointer< vtkPolyDataWriter > writer = vtkSmartPointer< vtkPolyDataWriter >::New();
//        writer->SetInput( m_Output );
//        writer->SetFileName( "mesh.vtk" );
//        writer->Write();
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
