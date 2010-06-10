#include "vtkMetaImageReader.h"
#include "vtkImageData.h"
#include "vtkViewImage3D.h"
#include "vtkRenderWindowInteractor.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cout <<"Usage: imageview2d(.exe)" <<std::endl;
    std::cout <<"1- file.mha" <<std::endl;
    std::cout <<"2- Test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }

  vtkMetaImageReader* reader = vtkMetaImageReader::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  vtkViewImage3D* view = vtkViewImage3D::New();

  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  view->SetupInteractor(iren);

  view->SetTriPlanarRenderingOn();
  view->SetInput( reader->GetOutput() );
  view->Render();

  if( atoi( argv[2] ) == 1 )
    {
    iren->CreateOneShotTimer( 1 );
    }
  else
    {
    iren->Start();
    }

  view->Delete();
  iren->Delete();
  reader->Delete();

  return EXIT_SUCCESS;
}
