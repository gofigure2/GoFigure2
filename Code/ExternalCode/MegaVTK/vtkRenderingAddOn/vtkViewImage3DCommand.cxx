#include "vtkViewImage3DCommand.h"

#include "vtkSmartPointer.h"

#include "vtkInteractorStyleImage3D.h"
#include "vtkProp.h"
#include "vtkViewImage3D.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkImageData.h"

#include "vtkOrientedBoxWidget.h"

#include "vtkProp3DCollection.h"
#include "vtkPolyData.h"

#include <algorithm>

//----------------------------------------------------------------------------
vtkViewImage3DCommand::
vtkViewImage3DCommand()
{
  m_BoxWidget = vtkOrientedBoxWidget::New();

  m_InitializedBoxWidget = false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkViewImage3DCommand::
~vtkViewImage3DCommand()
{
  // not working, generates leaks
  // but boxwidget has to be reiplemented somewhere else
  m_BoxWidget->RemoveAllObservers();
  m_BoxWidget->Delete();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkViewImage3DCommand*
vtkViewImage3DCommand::New()
{
  return new vtkViewImage3DCommand;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void
vtkViewImage3DCommand::
Execute(vtkObject *caller, unsigned long event, void *callData)
{
  vtkInteractorStyleImage3D *isi =
    vtkInteractorStyleImage3D::SafeDownCast(caller);

  (void) isi;
  (void) callData;

  vtkImageData* data = m_vtkViewImage3D->GetInput();
  int extent[6];
  //double spacing[3];
  data->GetExtent( extent );
  //data->GetSpacing( spacing );

  if ( event == vtkViewImage3DCommand::MeshPickingEvent )
    {
    vtkInteractorStyleImage3D* test = static_cast<vtkInteractorStyleImage3D*>(caller);
    m_ListOfModifiedActors.clear();
    m_ListOfModifiedActors.push_back( (vtkProp3D*)test->GetCurrentProp() );
    test->InvokeEventTest();
    return;
    }
  if ( event == vtkCommand::InteractionEvent )
    {
    vtkPolyData* pd = vtkPolyData::New();
    m_BoxWidget->GetPolyData( pd );

    double pt[3];
    pd->GetPoint( 0, pt );
  
    double bextent[6];
    bextent[0] = pt[0];
    bextent[1] = pt[0];
    bextent[2] = pt[1];
    bextent[3] = pt[1];
    bextent[4] = pt[2];
    bextent[5] = pt[2];
    
    for( int i = 1; i < 8; i+=2 )
      {
      pd->GetPoint( i, pt );

      if( pt[0] < bextent[0] )
        bextent[0] = pt[0];
      if( pt[0] > bextent[1] )
        bextent[1] = pt[0];
      if( pt[1] < bextent[2] )
        bextent[2] = pt[1];
      if( pt[1] > bextent[3] )
        bextent[3] = pt[1];
      if( pt[2] < bextent[4] )
        bextent[4] = pt[2];
      if( pt[2] > bextent[5] )
        bextent[5] = pt[2];
      }
    pd->Delete();

    // Get Actors
    m_ListOfModifiedActors.clear();

    vtkProp3DCollection* prop_collection = m_vtkViewImage3D->GetProp3DCollection();

    
    prop_collection->InitTraversal();
    vtkProp3D* prop_temp = prop_collection->GetNextProp3D();
        
    double bounds[6];

    while( prop_temp )
      {
      bool inside = true;

      // Don't do anything if an actor is invisible
      /*if(!prop_temp->GetVisibility())
        {
        std::cout << "Visibility: " << prop_temp->GetVisibility();
        return;
        }*/

      prop_temp->GetBounds( bounds );

      for( int i = 0; ( i < 3 ) && inside; ++i )
        {
        inside = ( bextent[2*i] < bounds[2*i] ) && ( bounds[2*i+1] < bextent[2*i+1] );
        }

      std::list<vtkProp3D*>::iterator it = std::find( m_ListOfPickedActors.begin(),
                                                      m_ListOfPickedActors.end(),
                                                      prop_temp );

      if( inside )
        {
        if( it == m_ListOfPickedActors.end() )
          {
          // ADD
          m_ListOfPickedActors.push_back( prop_temp );
          m_ListOfModifiedActors.push_back( prop_temp );
          }
        }
      else
        {
        if( it != m_ListOfPickedActors.end() )
          {
          // ADD
          m_ListOfPickedActors.erase( it );
          m_ListOfModifiedActors.push_back( prop_temp );
          }
        }
      prop_temp = prop_collection->GetNextProp3D();
      }

    // Tell that list is ready
    vtkInteractorStyleImage3D* test = static_cast<vtkInteractorStyleImage3D*>(caller);
    test->InvokeEventTest2();
    return;
    }
}

//----------------------------------------------------------------------------
void
vtkViewImage3DCommand::
SetVtkImageView3D( vtkViewImage3D* iViewImage3D )
{
  if( iViewImage3D )
    {
    m_vtkViewImage3D = iViewImage3D;
    }
}
//----------------------------------------------------------------------------
vtkOrientedBoxWidget*
vtkViewImage3DCommand::
GetBoxWidget()
{
  return m_BoxWidget;
}
//----------------------------------------------------------------------------
std::list< vtkProp3D* >
vtkViewImage3DCommand::
GetListOfModifiedActors()
{
  return m_ListOfModifiedActors;
}
//----------------------------------------------------------------------------
void
vtkViewImage3DCommand::
Enable3DBoxWidget( bool iValue )
{
  vtkImageData* data2 = m_vtkViewImage3D->GetInput();
  int extent2[6];
  double spacing2[3];
  data2->GetExtent( extent2 );
  data2->GetSpacing( spacing2 );

  if( iValue && !m_InitializedBoxWidget )
    {
    m_BoxWidget->SetInteractor( m_vtkViewImage3D->GetInteractor() );
    m_BoxWidget->SetPlaceFactor( 0.5 );
    m_BoxWidget->PlaceWidget( extent2[0], extent2[1]*spacing2[0],
        extent2[2], extent2[3]*spacing2[1],
        extent2[4], extent2[5]*spacing2[2]);
    m_BoxWidget->RotationEnabledOff();
    m_BoxWidget->SetKeyPressActivationValue ('b');
    m_BoxWidget->On();
    m_InitializedBoxWidget = true;
    }

  // Generates memory leaks
  if( iValue)
    {
    m_BoxWidget->AddObserver(vtkCommand::InteractionEvent,this);
    }
  else
    {
    m_BoxWidget->RemoveObservers(vtkCommand::InteractionEvent);
    }

  m_BoxWidget->SetEnabled( iValue );
}
