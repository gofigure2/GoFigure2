#include <iostream>

#include "itkImageFileReader.h"
#include "itkChanAndVeseSegmentationFilter.h"
#include "vtkImageData.h"
#include "vtkMarchingSquares.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"


int main( int argc, char** argv )
{
const unsigned int Dimension = 2;
  typedef itk::Image< float, Dimension > LevelSetImageType;
  typedef itk::Image< unsigned char, Dimension > FeatureImageType;

  typedef itk::ChanAndVeseSegmentationFilter< LevelSetImageType, FeatureImageType >
    SegmentationFilterType;

  typedef itk::ImageFileReader< FeatureImageType > ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  LevelSetImageType::PointType pt;

  for( unsigned int dim = 0; dim < Dimension; dim++ )
  {
    pt[dim] = atof( argv[dim+1] );
  }

  double cellRadius = atof( argv[Dimension+1] );

  SegmentationFilterType::Pointer filter = SegmentationFilterType::New();
  filter->SetFeatureImage( reader->GetOutput() );
  filter->SetRadius( cellRadius );
  filter->SetCenter( pt );
  filter->Update();

  vtkImageData* image = filter->GetOutput();

  // create iso-contours
  vtkMarchingSquares *contours = vtkMarchingSquares::New();
  contours->SetInput( image );
  contours->GenerateValues ( 1, 0, 0 );

  // map to graphics library
  vtkPolyDataMapper *map = vtkPolyDataMapper::New();
  map->SetInput( contours->GetOutput() );

  // actor coordinates geometry, properties, transformation
  vtkActor *contActor = vtkActor::New();
  contActor->SetMapper( map );

  vtkRenderer *ren = vtkRenderer::New();
  ren->AddActor ( contActor );

  vtkRenderWindow *renWin1 = vtkRenderWindow::New();
  renWin1->AddRenderer ( ren );

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow ( renWin1 );

  renWin1->Render();
  iren->Start();

  iren->Delete();
  renWin1->Delete();
  ren->Delete();
  contActor->Delete();
  map->Delete();
  contours->Delete();
  image->Delete();

  return EXIT_SUCCESS;
}
