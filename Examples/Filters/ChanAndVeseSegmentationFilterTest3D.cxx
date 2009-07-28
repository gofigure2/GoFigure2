#include <iostream>

#include "itkImageFileReader.h"
#include "itkChanAndVeseSegmentationFilter.h"
#include "vtkImageData.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"

int main( int argc, char** argv )
{
  if( argc < 7 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputFeatureImage seedX seedY seedZ";
    std::cerr << " radius test" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef itk::Image< unsigned char, Dimension > FeatureImageType;

  typedef itk::ChanAndVeseSegmentationFilter< FeatureImageType >
    SegmentationFilterType;

  typedef itk::ImageFileReader< FeatureImageType > ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  FeatureImageType::PointType pt;

  for( unsigned int dim = 0; dim < Dimension; dim++ )
  {
    pt[dim] = atof( argv[dim+2] );
  }

  double cellRadius = atof( argv[Dimension+2] );

  SegmentationFilterType::Pointer filter = SegmentationFilterType::New();
  filter->SetFeatureImage( reader->GetOutput() );
  filter->SetRadius( cellRadius );
  filter->SetCenter( pt );
  filter->Update();

  vtkImageData* image = filter->GetOutput();

  // create iso-contours
  vtkMarchingCubes *contours = vtkMarchingCubes::New();
  contours->SetInput( image );
  contours->GenerateValues ( 1, 0, 0 );

  vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
  writer->SetInput( contours->GetOutput() );
  writer->Write();
  writer->Delete();

  // map to graphics library
  vtkPolyDataMapper *map = vtkPolyDataMapper::New();
  map->SetInput( contours->GetOutput() );
  map->SetScalarRange ( -10, 10 );

  // actor coordinates geometry, properties, transformation
  vtkActor *contActor = vtkActor::New();
  contActor->SetMapper( map );
  contActor->GetProperty()->SetLineWidth ( 1.5 );
  contActor->GetProperty()->SetColor ( 0,0,1 ); // sphere color blue
  contActor->GetProperty()->SetOpacity ( 1.0 );

  vtkRenderer *ren = vtkRenderer::New();
  ren->AddActor ( contActor );
  ren->SetBackground ( 1., 1., 1. );

  vtkRenderWindow *renWin1 = vtkRenderWindow::New();
  renWin1->AddRenderer ( ren );

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow ( renWin1 );

  bool test = atoi( argv[Dimension+3] );
  renWin1->Render();

  if( !test )
    {
    iren->Start();
    }

  iren->Delete();
  renWin1->Delete();
  ren->Delete();
  contActor->Delete();
  map->Delete();
  contours->Delete();

  return EXIT_SUCCESS;
}
