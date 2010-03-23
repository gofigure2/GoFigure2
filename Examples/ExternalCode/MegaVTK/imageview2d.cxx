#include "vtkPNGReader.h"
#include "vtkImageData.h"
#include "vtkViewImage2D.h"
#include "vtkRenderWindowInteractor.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cout <<"Usage: imageview2d(.exe)" <<std::endl;
    std::cout <<"1- file.png" <<std::endl;
    std::cout <<"2- Test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }

  vtkPNGReader* reader = vtkPNGReader::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  vtkViewImage2D* view = vtkViewImage2D::New();

  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  view->SetupInteractor(iren);

  view->SetInput( reader->GetOutput() );
  view->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_AXIAL);
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
