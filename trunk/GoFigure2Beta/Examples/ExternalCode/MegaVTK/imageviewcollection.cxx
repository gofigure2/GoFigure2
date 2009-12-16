#include "vtkMetaImageReader.h"
#include "vtkImageActor.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkViewImage2D.h"
#include "vtkViewImage3D.h"
#include "vtkViewImage2DCollection.h"

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

  vtkViewImage2DCollection* pool = vtkViewImage2DCollection::New();

  vtkViewImage3D* view3d = vtkViewImage3D::New();
  vtkRenderWindowInteractor* iren3d = vtkRenderWindowInteractor::New();
  view3d->SetupInteractor(iren3d);

  vtkViewImage2D* view = vtkViewImage2D::New();
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  view->SetupInteractor(iren);
  view->SetInput (image);
  view->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_AXIAL);

  view3d->Add2DPhantom( 0, view->GetImageActor(), view->GetSlicePlane() );
  pool->AddItem( view );

  iren->Delete();
  view->Delete();

  vtkViewImage2D* view2 = vtkViewImage2D::New();
  vtkRenderWindowInteractor* iren2 = vtkRenderWindowInteractor::New();
  view2->SetupInteractor(iren2);
  view2->SetInput (image);
  view2->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_CORONAL);
  view3d->Add2DPhantom( 1, view2->GetImageActor(), view2->GetSlicePlane() );
  pool->AddItem( view2 );

  iren2->Delete();
  view2->Delete();

  vtkViewImage2D* view3 = vtkViewImage2D::New();
  vtkRenderWindowInteractor* iren3 = vtkRenderWindowInteractor::New();
  view3->SetupInteractor(iren3);
  view3->SetInput (image);
  view3->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL);
  view3d->Add2DPhantom( 2, view3->GetImageActor(), view3->GetSlicePlane() );
  pool->AddItem( view3 );

  iren3->Delete();
  view3->Delete();

  view3d->SetTriPlanarRenderingOn();
  view3d->SetInput( image );

  pool->SetExtraRenderWindow( view3d->GetRenderWindow() );
  pool->InitializeAllObservers();
  pool->Initialize();
  pool->SyncSetBackground( pool->GetItem(0)->GetBackground() );
  pool->SyncSetShowAnnotations( true );

  int size[2] = {400, 400};
  pool->SyncSetSize( size );
  pool->SyncPan();
  pool->SyncRender();
  pool->SyncReset();

  if( atoi( argv[2] ) == 1 )
    {
    iren3d->CreateOneShotTimer( 1 );
    }
  else
    {
    iren3d->Start();
    }

  pool->Delete();

  iren3d->Delete();
  view3d->Delete();

  reader->Delete();

  return EXIT_SUCCESS;
}
