#include "vtkMetaImageReader.h"
#include "vtkImageActor.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkSmartPointer.h"

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

  vtkSmartPointer< vtkMetaImageReader > reader =
    vtkSmartPointer< vtkMetaImageReader >::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  vtkImageData* image = reader->GetOutput();

  vtkSmartPointer< vtkViewImage2DCollection > pool =
    vtkSmartPointer< vtkViewImage2DCollection >::New();

  vtkSmartPointer< vtkViewImage3D > view3d = vtkSmartPointer< vtkViewImage3D >::New();
  vtkSmartPointer< vtkRenderWindowInteractor > iren3d =
    vtkSmartPointer< vtkRenderWindowInteractor >::New();
  view3d->SetupInteractor(iren3d);

  vtkSmartPointer< vtkViewImage2D > view = vtkSmartPointer< vtkViewImage2D >::New();
  vtkSmartPointer< vtkRenderWindowInteractor > iren =
    vtkSmartPointer< vtkRenderWindowInteractor >::New();
  view->SetupInteractor(iren);
  view->SetInput (image);
  view->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_AXIAL);

  view3d->Add2DPhantom( 0, view->GetImageActor(), view->GetSlicePlane() );
  pool->AddItem( view );

  vtkSmartPointer< vtkViewImage2D > view2 = vtkSmartPointer< vtkViewImage2D >::New();
  vtkSmartPointer< vtkRenderWindowInteractor > iren2 = vtkSmartPointer< vtkRenderWindowInteractor >::New();
  view2->SetupInteractor(iren2);
  view2->SetInput (image);
  view2->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_CORONAL);
  view3d->Add2DPhantom( 1, view2->GetImageActor(), view2->GetSlicePlane() );
  pool->AddItem( view2 );

  vtkSmartPointer< vtkViewImage2D > view3 = vtkSmartPointer< vtkViewImage2D >::New();
  vtkSmartPointer< vtkRenderWindowInteractor > iren3 = vtkSmartPointer< vtkRenderWindowInteractor >::New();
  view3->SetupInteractor(iren3);
  view3->SetInput (image);
  view3->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL);
  view3d->Add2DPhantom( 2, view3->GetImageActor(), view3->GetSlicePlane() );
  pool->AddItem( view3 );

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

  return EXIT_SUCCESS;
}
