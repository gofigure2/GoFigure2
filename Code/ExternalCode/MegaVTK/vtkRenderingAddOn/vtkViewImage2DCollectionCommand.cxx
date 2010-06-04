#include "vtkViewImage2DCollectionCommand.h"
#include "vtkInteractorStyleImage2D.h"
#include "vtkViewImage2DCommand.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCellPicker.h"
#include "vtkProp3DCollection.h"
#include "vtkViewImage2DCollection.h"

//----------------------------------------------------------------------------
vtkViewImage2DCollectionCommand::
vtkViewImage2DCollectionCommand()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkViewImage2DCollectionCommand::
~vtkViewImage2DCollectionCommand()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void
vtkViewImage2DCollectionCommand::
SetCollection(vtkViewImage2DCollection* p)
{
  this->Collection = p;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkViewImage2DCollectionCommand*
vtkViewImage2DCollectionCommand::New()
{
  return new vtkViewImage2DCollectionCommand;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkViewImage2DCollection*
vtkViewImage2DCollectionCommand::
GetCollection()
{
  return this->Collection;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
std::list< vtkProp3D* >
vtkViewImage2DCollectionCommand::
GetListOfPickedActors()
{
  return ListOfPickedActors;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
std::list< vtkProp3D* >
vtkViewImage2DCollectionCommand::
GetListOfUnPickedActors()
{
  return ListOfUnPickedActors;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vtkViewImage2DCollectionCommand::Execute(vtkObject *caller,
  unsigned long event,
  void *vtkNotUsed(callData))
{
  if (!this->Collection)
    {
    return;
    }

  vtkInteractorStyleImage2D *isi =
    vtkInteractorStyleImage2D::SafeDownCast(caller);
  this->GetCollection()->InitTraversal();
  vtkViewImage2D* v = this->GetCollection()->GetNextItem();
  vtkViewImage2D* viewer = NULL;
  while(v)
    {
    if (isi == v->GetInteractorStyle())
      {
      viewer = v;
      break;
      }
    v = this->GetCollection()->GetNextItem();
    }

  if (!isi || !viewer || !viewer->GetInput())
    {
    return;
    }

  // Reset
  if (event == vtkCommand::ResetWindowLevelEvent)
    {
    this->Collection->SyncResetWindowLevel();
    this->Collection->SyncRender();
    return;
    }
  // Reset
  if (event == vtkViewImage2DCommand::ResetViewerEvent)
    {
    this->Collection->SyncReset();
    this->Collection->SyncRender();
    return;
    }

  // Adjust the window level here
  if (event == vtkCommand::WindowLevelEvent)
    {
    this->Collection->SyncSetColorWindow(viewer->GetColorWindow());
    this->Collection->SyncSetColorLevel(viewer->GetColorLevel());
    this->Collection->SyncRender();
    }

  // Move
  if (event == vtkViewImage2DCommand::SliceMoveEvent)
    {
    // do not synchronize this, but render all
    this->Collection->SyncRender();
    }

  if( event == vtkViewImage2DCommand::ZoomEvent )
    {
    double z = viewer->GetZoom();
    double parallel_scale =
        viewer->GetRenderer()->GetActiveCamera()->GetParallelScale();

    this->Collection->SyncSetZoomAndParallelScale( z, parallel_scale );
    }

  if( event == vtkViewImage2DCommand::PanEvent )
    {
    double motion[3];
    viewer->GetCameraMotionVector( motion );

    this->GetCollection()->InitTraversal();
    v = this->GetCollection()->GetNextItem();
    double focal[3], pos[3], n[3];
    double dot = 0., u;

    while(v)
      {
      if( v != viewer )
        {
        v->GetCameraFocalAndPosition( focal, pos );
        v->GetRenderer()->GetActiveCamera()->GetViewPlaneNormal( n );
        dot = vtkMath::Dot( n, motion );

        for( int dim = 0; dim < 3; dim++ )
          {
          u = motion[dim] - dot * n[dim];
          focal[dim] += u;
          pos[dim] += u;
          }
        v->SetCameraFocalAndPosition( focal, pos );

        if( v->GetInteractorStyle()->GetInteractor()->GetLightFollowCamera() )
          {
          v->GetRenderer()->UpdateLightsGeometryToFollowCamera();
          }
        }
      v->Render();
      v = this->GetCollection()->GetNextItem();
      }
    }

  // Position requested
  if (event == vtkViewImage2DCommand::RequestedPositionEvent)
    {
    double* position = viewer->GetWorldCoordinatesFromDisplayPosition (
      isi->GetRequestedPosition ());
    this->Collection->SyncSetWorldCoordinates(position);
    this->Collection->SyncRender();
    }

  if( event == vtkViewImage2DCommand::ContourPickingEvent )
    {
     vtkCellPicker* picker = viewer->GetContourPicker();
     vtkRenderWindowInteractor *rwi =
       viewer->GetRenderWindow()->GetInteractor();

     picker->Pick( rwi->GetEventPosition()[0], rwi->GetEventPosition()[1], 0,
       viewer->GetRenderer() );

     vtkProp3D* prop = picker->GetProp3D();
     if( prop )
       {
       ListOfPickedActors.clear();
       ListOfUnPickedActors.clear();

 //       vtkProp* prop = path->GetFirstNode()->GetViewProp();
       viewer->GetProp3DCollection()->InitTraversal();
       vtkProp3D* prop_temp = viewer->GetProp3DCollection()->GetNextProp3D(); // image
       prop_temp = viewer->GetProp3DCollection()->GetNextProp3D(); // 1st plane
       prop_temp = viewer->GetProp3DCollection()->GetNextProp3D(); // 2nd plane
       prop_temp = viewer->GetProp3DCollection()->GetNextProp3D(); // 3rd plane

       while( prop_temp )
         {
         if( prop_temp == prop )
           {
           // to be highlighted
           ListOfPickedActors.push_back( prop_temp );
           prop->DragableOn();
           // viewer->ChangeActorProperty( prop_temp, );
           // viewer->HighlightContour( prop_temp, true );
           }
         else
           {
           ListOfUnPickedActors.push_back( prop_temp );
 //           viewer->HighlightContour( prop_temp, false );
           }
         prop_temp = viewer->GetProp3DCollection()->GetNextProp3D();
         }
       }
    }

  if( event == vtkViewImage2DCommand::MeshPickingEvent )
    {
    std::cout<< "Mesh pick collection" << std::endl;
    }

  if( event == vtkViewImage2DCommand::SeedEvent )
    {
    std::cout << "in collection"  << std::endl;
    /*this->GetCollection()->InitTraversal();
    v = this->GetCollection()->GetNextItem();

        while(v)
        {
          if( v != viewer )
          {
          std::cout<<" != viewer"<<std::endl;
          }
          std::cout<<" out "<<std::endl;
          v->Render();
          v = this->GetCollection()->GetNextItem();
        }*/
    }
}
