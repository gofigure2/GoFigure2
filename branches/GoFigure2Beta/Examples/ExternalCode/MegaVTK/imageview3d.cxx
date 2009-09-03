#include "vtkMetaImageReader.h"
#include "vtkImageActor.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkViewImage2D.h"
#include "vtkViewImage3D.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cout <<"Usage: imageview3d(.exe)" <<std::endl;
    std::cout <<"1- filename (.mha or .mhd)" <<std::endl;
    std::cout <<"2- Test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }

  vtkMetaImageReader* reader = vtkMetaImageReader::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  vtkImageData* image = reader->GetOutput();

  vtkViewImage2D* view = vtkViewImage2D::New();
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  view->SetupInteractor(iren);
  view->SetInput (image);
  view->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_AXIAL);
  view->Render();

  vtkViewImage2D* view2 = vtkViewImage2D::New();
  vtkRenderWindowInteractor* iren2 = vtkRenderWindowInteractor::New();
  view2->SetupInteractor(iren2);
  view2->SetInput (image);
  view2->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_CORONAL);
  view2->Render();

  vtkViewImage2D* view3 = vtkViewImage2D::New();
  vtkRenderWindowInteractor* iren3 = vtkRenderWindowInteractor::New();
  view3->SetupInteractor(iren3);
  view3->SetInput (image);
  view3->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL);
  view3->Render();

  vtkViewImage3D* view3d = vtkViewImage3D::New();
  vtkRenderWindowInteractor* iren3d = vtkRenderWindowInteractor::New();
  view3d->SetupInteractor(iren3d);
  view3d->Add2DPhantom( 0, view->GetImageActor() );
  view3d->Add2DPhantom( 1, view2->GetImageActor() );
  view3d->Add2DPhantom( 2, view3->GetImageActor() );
  view3d->SetTriPlanarRenderingOn();
  view3d->SetInput( image );

  view3d->Render();

  if( atoi( argv[1] ) == 0 )
    {
    iren3d->Start();
    }

  iren3->Delete();
  view3->Delete();
  iren2->Delete();
  view2->Delete();
  iren->Delete();
  view->Delete();

  iren3d->Delete();
  view3d->Delete();

  reader->Delete();

  return EXIT_SUCCESS;
}
