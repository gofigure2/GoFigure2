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
#include "vtkPlanes.h"
#include "vtkPlane.h"

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
  if( m_BoxWidget->HasObserver(vtkCommand::InteractionEvent) )
    {
    std::cout << "HAS Observer" << std::endl;
    m_BoxWidget->RemoveObservers(vtkCommand::InteractionEvent);
    }
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
    // Get Extent of the box through planes
    double bextent[6];
    bextent[0] = VTK_DOUBLE_MAX;
    bextent[1] = VTK_DOUBLE_MIN;
    bextent[2] = VTK_DOUBLE_MAX;
    bextent[3] = VTK_DOUBLE_MIN;
    bextent[4] = VTK_DOUBLE_MAX;
    bextent[5] = VTK_DOUBLE_MIN;

    vtkPlanes* planes = vtkPlanes::New();
    m_BoxWidget->GetPlanes( planes );
    double origin[3];
    for( int i=0; i<6; ++i)
      {
      planes->GetPlane(i)->GetOrigin( origin );

      if( origin[0] < bextent[0] )
        bextent[0] = origin[0];
      if( origin[0] > bextent[1] )
        bextent[1] = origin[0];
      if( origin[1] < bextent[2] )
        bextent[2] = origin[1];
      if( origin[1] > bextent[3] )
        bextent[3] = origin[1];
      if( origin[2] < bextent[4] )
        bextent[4] = origin[2];
      if( origin[2] > bextent[5] )
        bextent[5] = origin[2];
      }

    // Get Actors
    m_ListOfModifiedActors.clear();

    m_vtkViewImage3D->GetProp3DCollection()->InitTraversal();
    vtkProp3D* prop_temp = m_vtkViewImage3D->GetProp3DCollection()
        ->GetNextProp3D();

    while( prop_temp )
      {
      if(    (bextent[0] < prop_temp->GetXRange()[0]) && (bextent[1] > prop_temp->GetXRange()[1])
          && (bextent[2] < prop_temp->GetYRange()[0]) && (bextent[3] > prop_temp->GetYRange()[1])
          && (bextent[4] < prop_temp->GetZRange()[0]) && (bextent[5] > prop_temp->GetZRange()[1]))
        {
        std::list<vtkProp3D*>::iterator listOfPickedActorsIterator;
        bool doSth = true;

        for(listOfPickedActorsIterator = m_ListOfPickedActors.begin();
            listOfPickedActorsIterator != m_ListOfPickedActors.end();
            listOfPickedActorsIterator++)
          {
          // if exists in list
        if(    ((*listOfPickedActorsIterator)->GetXRange()[0] == prop_temp->GetXRange()[0])
            && ((*listOfPickedActorsIterator)->GetXRange()[1] == prop_temp->GetXRange()[1])
            && ((*listOfPickedActorsIterator)->GetYRange()[0] == prop_temp->GetYRange()[0])
            && ((*listOfPickedActorsIterator)->GetYRange()[1] == prop_temp->GetYRange()[1])
            && ((*listOfPickedActorsIterator)->GetZRange()[0] == prop_temp->GetZRange()[0])
            && ((*listOfPickedActorsIterator)->GetZRange()[1] == prop_temp->GetZRange()[1]))
            {
            doSth = false;
            break;
            }
          }

        if(doSth)
          {
          // ADD
          m_ListOfPickedActors.push_back( prop_temp );
          m_ListOfModifiedActors.push_back( prop_temp );
          }

        }
      else
        {
        std::list<vtkProp3D*>::iterator listOfPickedActorsIterator;
        bool doSth = false;

        for(listOfPickedActorsIterator = m_ListOfPickedActors.begin();
            listOfPickedActorsIterator != m_ListOfPickedActors.end();
            listOfPickedActorsIterator++)
          {
          // if exists in list
          if(    ((*listOfPickedActorsIterator)->GetXRange()[0] == prop_temp->GetXRange()[0])
              && ((*listOfPickedActorsIterator)->GetXRange()[1] == prop_temp->GetXRange()[1])
              && ((*listOfPickedActorsIterator)->GetYRange()[0] == prop_temp->GetYRange()[0])
              && ((*listOfPickedActorsIterator)->GetYRange()[1] == prop_temp->GetYRange()[1])
              && ((*listOfPickedActorsIterator)->GetZRange()[0] == prop_temp->GetZRange()[0])
              && ((*listOfPickedActorsIterator)->GetZRange()[1] == prop_temp->GetZRange()[1]))
              {
              doSth = true;
              break;
              }
            }
        if( doSth )
          {
          m_ListOfPickedActors.erase( listOfPickedActorsIterator );
          m_ListOfModifiedActors.push_back( prop_temp );
          }
        }
        prop_temp = m_vtkViewImage3D->GetProp3DCollection()->GetNextProp3D();
      }
    planes->Delete();

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
    // Generates memory leaks
    if( !m_BoxWidget->HasObserver(vtkCommand::InteractionEvent))
      {
      m_BoxWidget->AddObserver(vtkCommand::InteractionEvent,this);
      }
    m_BoxWidget->On();
    m_InitializedBoxWidget = true;
    }

  m_BoxWidget->SetEnabled( iValue );
}
