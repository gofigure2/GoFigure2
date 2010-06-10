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
  this->m_BoxWidget = vtkOrientedBoxWidget::New();
  // When BoxWidget is modified
  this->m_BoxWidget->AddObserver(vtkCommand::InteractionEvent,this);
  m_InitializedBoxWidget = false;

}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkViewImage3DCommand::
~vtkViewImage3DCommand()
{
  this->m_BoxWidget->Delete();

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
Execute(vtkObject *caller, unsigned long event, void *vtkNotUsed(callData))
{
  vtkInteractorStyleImage3D *isi =
    vtkInteractorStyleImage3D::SafeDownCast(caller);

  (void) isi;

  vtkImageData* data = this->m_vtkViewImage3D->GetInput();
  int extent[6];
  //double spacing[3];
  data->GetExtent( extent );
  //data->GetSpacing( spacing );

  if ( event == vtkViewImage3DCommand::MeshPickingEvent )
    {
    std::cout << "where first? here" << std::endl;
    vtkInteractorStyleImage3D* test = static_cast<vtkInteractorStyleImage3D*>(caller);
    m_ListOfModifiedActors.clear();
    m_ListOfModifiedActors.push_back( (vtkProp3D*)test->GetCurrentProp() );
    test->InvokeEventTest();
    }
  if ( event == vtkViewImage3DCommand::BoxPickingEvent )
    {
    /*if( this->m_BoxPickingEnabled )
      {
      this->m_BoxWidget->SetEnabled( 0 );
      this->m_BoxPickingEnabled = false;
      }
    else
      {
      this->m_BoxWidget->SetEnabled( 0 );
      this->m_BoxPickingEnabled = false;
      }*/
    /*this->m_BoxWidget->SetInteractor( this->m_vtkViewImage3D->GetInteractor() );
    this->m_BoxWidget->SetPlaceFactor( 0.5 );
    this->m_BoxWidget->PlaceWidget( extent[0], extent[1]*spacing[0],
                                    extent[2], extent[3]*spacing[1],
                                    extent[4], extent[5]*spacing[2]);
    this->m_BoxWidget->RotationEnabledOff();
    this->m_BoxWidget->SetEnabled( 1 );*/
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
    this->m_BoxWidget->GetPlanes( planes );
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

    this->m_vtkViewImage3D->GetProp3DCollection()->InitTraversal();
    vtkProp3D* prop_temp = this->m_vtkViewImage3D->GetProp3DCollection()
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
        prop_temp = this->m_vtkViewImage3D->GetProp3DCollection()->GetNextProp3D();
      }
    planes->Delete();
    }
}

//----------------------------------------------------------------------------
void
vtkViewImage3DCommand::
SetVtkImageView3D( vtkViewImage3D* vtkViewImage3D )
{
  m_vtkViewImage3D = vtkViewImage3D;
}
//----------------------------------------------------------------------------
vtkOrientedBoxWidget*
vtkViewImage3DCommand::
GetBoxWidget()
{
  return this->m_BoxWidget;
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
  vtkImageData* data2 = this->m_vtkViewImage3D->GetInput();
  int extent2[6];
  double spacing2[3];
  data2->GetExtent( extent2 );
  data2->GetSpacing( spacing2 );

  if( iValue && !m_InitializedBoxWidget )
    {
    this->m_BoxWidget->SetInteractor( this->m_vtkViewImage3D->GetInteractor() );
    this->m_BoxWidget->SetPlaceFactor( 0.5 );
    this->m_BoxWidget->PlaceWidget( extent2[0], extent2[1]*spacing2[0],
        extent2[2], extent2[3]*spacing2[1],
        extent2[4], extent2[5]*spacing2[2]);
    this->m_BoxWidget->RotationEnabledOff();
    m_InitializedBoxWidget = true;
  }

  this->m_BoxWidget->SetEnabled( iValue );

  // Update seleced mesh list...?
  // which behavior do we want?
}
