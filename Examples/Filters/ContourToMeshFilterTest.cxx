#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <vector>

#include "ContourToMeshFilter.h"

void CreateCircle( const double& iZ,
                   const double& iRadius, 
                   const int& iResolution, 
                   vtkPolyData* ioContour )
{
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
 
  points->SetNumberOfPoints( iResolution );
  cells->Allocate( 1, iResolution );
  cells->InsertNextCell( iResolution );
 
  for( int i = 0 ; i < iResolution; ++i )
    {
    double theta = vtkMath::RadiansFromDegrees(360.*i/double(iResolution));
    double x = iRadius*cos(theta);
    double y = iRadius*sin(theta);
    points->SetPoint( i, x, y, iZ );
    cells->InsertCellPoint( i );
    }
 
  ioContour->Initialize();
  ioContour->SetPolys( cells );
  ioContour->SetPoints( points );
}

int main( int argc, char* argv[] )
{
  int resolution = 10;
  double z = 0.;
  double lastz = 0.;
  double sphereRadius = 1.;
  double zmin = -0.9*sphereRadius;
  double zmax = -zmin;

  std::vector< vtkSmartPointer< vtkPolyData > > contours( 20 );

  for( int i = 0; i < 20; i++ )
    {
    lastz = z;
    double u = i / 20;
    z = ( 1. - u ) * zmin + u * zmax;
    double radius = sqrt( sphereRadius * sphereRadius - z*z );
    contours[i] = vtkSmartPointer< vtkPolyData >::New();
    CreateCircle( z, radius, resolution, contours[i] );
    }

  vtkImageData* image = vtkImageData::New();
  image->SetDimensions( 100, 100, 100 );
  image->SetNumberOfScalarComponents(1);
  image->SetScalarTypeToUnsignedChar();


  typedef itk::ContourToMeshFilter< std::vector< vtkSmartPointer< vtkPolyData > > > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetImage( image );
  filter->ProcessContours( contours );

  vtkSmartPointer< vtkPolyDataMapper > mapper =
    vtkSmartPointer< vtkPolyDataMapper >::New();
  mapper->SetInput( filter->GetOutput() );

  vtkSmartPointer< vtkActor > actor =
    vtkSmartPointer< vtkActor >::New();
  actor->SetMapper( mapper );

  vtkSmartPointer< vtkRenderer > renderer =
    vtkSmartPointer< vtkRenderer >::New();
  renderer->AddActor( actor );

  vtkSmartPointer< vtkRenderWindow > renwin =
    vtkSmartPointer< vtkRenderWindow >::New();
  renwin->AddRenderer( renderer );

  vtkSmartPointer< vtkRenderWindowInteractor > iren =
    vtkSmartPointer< vtkRenderWindowInteractor >::New();
  iren->SetRenderWindow( renwin );

  renwin->Render();
  iren->Start();
  
  return EXIT_SUCCESS;
}
