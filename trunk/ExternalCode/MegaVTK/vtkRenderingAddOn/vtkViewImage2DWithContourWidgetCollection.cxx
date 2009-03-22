#include "vtkViewImage2DWithContourWidgetCollection.h"

#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyleImage2D.h>
#include <vtkProperty.h>
#include <vtkMath.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>

#include "vtkViewImage2DCommand.h"

vtkCxxRevisionMacro(vtkViewImage2DWithContourWidgetCollection, "$Revision: 1 $");
vtkStandardNewMacro(vtkViewImage2DWithContourWidgetCollection);

vtkViewImage2DWithContourWidgetCollection::
vtkViewImage2DWithContourWidgetCollection( )
{
  this->Command = vtkViewImage2DWithContourWidgetCollectionCommand::New();
  this->Command->SetCollection( this );
}

vtkViewImage2DWithContourWidgetCollection::
~vtkViewImage2DWithContourWidgetCollection()
{
  this->Command->Delete();
}

void
vtkViewImage2DWithContourWidgetCollection::
AddItem( vtkViewImage2DWithContourWidget* a )
{
  this->Superclass::AddItem (a);
//   a->GetInteractorStyle()->RemoveAllObservers();
  a->GetInteractorStyle()->RemoveObservers(
    vtkCommand::ResetWindowLevelEvent );
//   a->GetInteractorStyle()->RemoveObservers( vtkCommand::WindowLevelEvent );
  a->GetInteractorStyle()->RemoveObservers(
    vtkViewImage2DCommand::SliceMoveEvent );
//   a->GetInteractorStyle()->RemoveObservers(
//     vtkViewImage2DCommand::ZoomEvent );
  a->GetInteractorStyle()->RemoveObservers(
    vtkViewImage2DCommand::ResetViewerEvent );
  a->GetInteractorStyle()->RemoveObservers( vtkViewImage2DCommand::
    RequestedPositionEvent );

  a->GetInteractorStyle()->AddObserver( vtkCommand::ResetWindowLevelEvent,
    this->Command );
  a->GetInteractorStyle()->AddObserver( vtkCommand::WindowLevelEvent,
    this->Command );
  a->GetInteractorStyle()->AddObserver(
    vtkViewImage2DCommand::SliceMoveEvent,
    this->Command );
  a->GetInteractorStyle()->AddObserver(
    vtkViewImage2DCommand::ZoomEvent,
    this->Command );
  a->GetInteractorStyle()->AddObserver(
    vtkViewImage2DCommand::PanEvent,
    this->Command );
  a->GetInteractorStyle()->AddObserver(
    vtkViewImage2DCommand::RequestedPositionEvent,
    this->Command );
  a->GetInteractorStyle()->AddObserver(
    vtkViewImage2DCommand::ResetViewerEvent,
    this->Command );
}


void
vtkViewImage2DWithContourWidgetCollection::
ReplaceItem( int i, vtkViewImage2DWithContourWidget* a )
{
  this->Superclass::ReplaceItem( i, a );
}

void
vtkViewImage2DWithContourWidgetCollection::
RemoveItem( int i )
{
  this->Superclass::RemoveItem( i );
}

void
vtkViewImage2DWithContourWidgetCollection::
RemoveItem( vtkViewImage2DWithContourWidget* a )
{
  this->Superclass::RemoveItem( a );
}

void
vtkViewImage2DWithContourWidgetCollection::
RemoveAllItems()
{
  this->Superclass::RemoveAllItems();
}

void
vtkViewImage2DWithContourWidgetCollection::
Initialize()
{
  vtkProperty* plane_property = vtkProperty::New();
  plane_property->SetRepresentationToWireframe();

  for (int i=0; i<this->GetNumberOfItems(); i++)
  {
    for (int j=0; j<this->GetNumberOfItems(); j++)
    {
      this->GetItem( j )->AddDataSet(
        static_cast<vtkDataSet*>( this->GetItem( i )->GetSlicePlane() ),
        plane_property, ( i != j ) );
    }
  }
  plane_property->Delete();
}


void vtkViewImage2DWithContourWidgetCollection::
SyncSetBackground( double* rgb )
{
  this->InitTraversal();
  vtkViewImage2DWithContourWidget* item = this->GetNextItem();
  while(item)
  {
    item->SetBackground( rgb );
    item = this->GetNextItem();
  }
  if (this->ExtraRenderWindow)
  {
    vtkRenderer* ren =
      this->ExtraRenderWindow->GetRenderers()->GetFirstRenderer();
    ren->SetBackground( rgb );
  }
}

void vtkViewImage2DWithContourWidgetCollection::
SyncRender(void)
{
  this->InitTraversal();
  vtkViewImage2DWithContourWidget* item = this->GetNextItem();
  while(item)
  {
    item->Render ();
    item = this->GetNextItem();
  }
  if (this->ExtraRenderWindow)
    this->ExtraRenderWindow->Render();
}


void vtkViewImage2DWithContourWidgetCollection::
SyncReset(void)
{
  this->InitTraversal();
  vtkViewImage2DWithContourWidget* item = this->GetNextItem();
  while(item)
  {
    item->Reset ();
    item = this->GetNextItem();
  }
}

void vtkViewImage2DWithContourWidgetCollection::
SyncResetWindowLevel(void)
{
  this->InitTraversal();
  vtkViewImage2DWithContourWidget* item = this->GetNextItem();
  while(item)
  {
    item->ResetWindowLevel ();
    item = this->GetNextItem();
  }
}

void vtkViewImage2DWithContourWidgetCollection::
SyncMaskImage( void )
{
//   vtkImageData* mask = this->GetItem(0)->GetMaskImage();
//   vtkLookupTable* lut = this->GetItem(0)->GetMaskLUT();

  for (int i=0; i<this->GetNumberOfItems(); i++)
  {
    vtkImageData* mask = this->GetItem(i)->GetMaskImage();
    vtkLookupTable* lut = this->GetItem(i)->GetMaskLUT();

    std::cout <<i <<" "<<mask <<" " <<lut <<std::endl;

    for (int j=0; j<this->GetNumberOfItems(); j++)
    {
      this->GetItem(j)->SetMaskImage( mask, lut, ( i == j ) );
    }
  }
}

void vtkViewImage2DWithContourWidgetCollection::
SyncMaskImage( vtkImageData* mask, vtkLookupTable* lut )
{
  this->InitTraversal();
  vtkViewImage2DWithContourWidget* item = this->GetNextItem();
  while(item)
  {
    item->SetMaskImage( mask, lut, true ); //item->ResetWindowLevel ();
    item = this->GetNextItem();
  }
}
//----------------------------------------------------------------------------
void vtkViewImage2DWithContourWidgetCollection::
SyncPan()
{
  this->InitTraversal();
  vtkViewImage2DWithContourWidget* item = this->GetNextItem();
  while(item)
  {
//     item->SetMaskImage( mask, lut, true ); //item->ResetWindowLevel ();
    item = this->GetNextItem();
  }
}

void vtkViewImage2DWithContourWidgetCollection::
SyncSetZoomAndParallelScale( double Zoom, double ParallelScale )
{
  this->InitTraversal();
  vtkViewImage2DWithContourWidget* item = this->GetNextItem();
  double t = ParallelScale / Zoom;
  while(item)
  {
    item->SetZoom( Zoom );
    item->GetRenderer()->GetActiveCamera()->SetParallelScale( t );

    if( item->GetInteractorStyle()->GetInteractor()->GetLightFollowCamera() )
    {
      item->GetRenderer()->UpdateLightsGeometryToFollowCamera();
    }
    item->Render();
    item = this->GetNextItem();
  }
}

// void vtkViewImage2DWithContourWidgetCollection::
// SyncSetCameraFocalAndZoomRatio( double focal[3], double d )
// {
//   this->InitTraversal();
//   vtkViewImage2DWithContourWidget* item = this->GetNextItem();
//   while(item)
//   {
//     item->SetCameraFocalAndZoomRatio( focal, d );
//     item = this->GetNextItem();
//   }
// }


//----------------------------------------------------------------------------
vtkViewImage2DWithContourWidgetCollectionCommand::
vtkViewImage2DWithContourWidgetCollectionCommand ()
{
}

//----------------------------------------------------------------------------
void
vtkViewImage2DWithContourWidgetCollectionCommand::SetCollection(
vtkViewImage2DWithContourWidgetCollection* p)
{
  this->Collection = p;
}

//----------------------------------------------------------------------------
void vtkViewImage2DWithContourWidgetCollectionCommand::Execute(vtkObject *caller,
  unsigned long event,
  void *vtkNotUsed(callData))
{
  if (!this->Collection)
    return;

  vtkInteractorStyleImage2D *isi =
    vtkInteractorStyleImage2D::SafeDownCast(caller);
  this->GetCollection()->InitTraversal();
  vtkViewImage2DWithContourWidget* v = this->GetCollection()->GetNextItem();
  vtkViewImage2DWithContourWidget* viewer = NULL;
  while(v)
  {
    if (isi == v->GetInteractorStyle())
      viewer = v;
    v = this->GetCollection()->GetNextItem();
  }

  if (!isi || !viewer || !viewer->GetInput())
    return;

  if( !viewer->GetContourWidgetEnabled() )
  {
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
  }
}



