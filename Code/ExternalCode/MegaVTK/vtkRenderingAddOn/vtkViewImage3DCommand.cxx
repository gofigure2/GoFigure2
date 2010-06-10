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
  double spacing[3];
  data->GetExtent( extent );
  data->GetSpacing( spacing );

  if ( event == vtkViewImage3DCommand::MeshPickingEvent )
    {
    vtkInteractorStyleImage3D* test = static_cast<vtkInteractorStyleImage3D*>(caller);
    m_PickedActor = test->GetCurrentProp();
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
    this->m_BoxWidget->SetInteractor( this->m_vtkViewImage3D->GetInteractor() );
    this->m_BoxWidget->SetPlaceFactor( 0.5 );
    this->m_BoxWidget->PlaceWidget( extent[0], extent[1]*spacing[0],
                                    extent[2], extent[3]*spacing[1],
                                    extent[4], extent[5]*spacing[2]);
    this->m_BoxWidget->RotationEnabledOff();
    this->m_BoxWidget->SetEnabled( 1 );
    }
  if ( event == vtkCommand::InteractionEvent )
    {
    // Get Extent of the box through planes
    double extent[6];
    extent[0] = VTK_DOUBLE_MAX;
    extent[1] = VTK_DOUBLE_MIN;
    extent[2] = VTK_DOUBLE_MAX;
    extent[3] = VTK_DOUBLE_MIN;
    extent[4] = VTK_DOUBLE_MAX;
    extent[5] = VTK_DOUBLE_MIN;

    vtkPlanes* planes = vtkPlanes::New();
    this->m_BoxWidget->GetPlanes( planes );
    double origin[3];
    for( int i=0; i<6; ++i)
      {
      planes->GetPlane(i)->GetOrigin( origin );

      if( origin[0] < extent[0] )
        extent[0] = origin[0];
      if( origin[0] > extent[1] )
        extent[1] = origin[0];
      if( origin[1] < extent[2] )
        extent[2] = origin[1];
      if( origin[1] > extent[3] )
        extent[3] = origin[1];
      if( origin[2] < extent[4] )
        extent[4] = origin[2];
      if( origin[2] > extent[5] )
        extent[5] = origin[2];
      }
/*
    std::cout << "Extent: " << extent[0] << std::endl;
    std::cout << "Extent: " << extent[1] << std::endl;
    std::cout << "Extent: " << extent[2] << std::endl;
    std::cout << "Extent: " << extent[3] << std::endl;
    std::cout << "Extent: " << extent[4] << std::endl;
    std::cout << "Extent: " << extent[5] << std::endl;
*/
    // Get Actors
    m_ListOfPickedActors.clear();

    this->m_vtkViewImage3D->GetProp3DCollection()->InitTraversal();
    vtkProp3D* prop_temp = this->m_vtkViewImage3D->GetProp3DCollection()
        ->GetNextProp3D(); // image
    prop_temp = this->m_vtkViewImage3D->GetProp3DCollection()->GetNextProp3D(); // 1st plane
    prop_temp = this->m_vtkViewImage3D->GetProp3DCollection()->GetNextProp3D(); // 2nd plane
    prop_temp = this->m_vtkViewImage3D->GetProp3DCollection()->GetNextProp3D(); // 3rd plane

    while( prop_temp )
      {
      if(    (extent[0] < prop_temp->GetCenter()[0]) && (extent[1] > prop_temp->GetCenter()[0])
          && (extent[2] < prop_temp->GetCenter()[1]) && (extent[3] > prop_temp->GetCenter()[1])
          && (extent[4] < prop_temp->GetCenter()[2]) && (extent[5] > prop_temp->GetCenter()[2]))
        {
        m_ListOfPickedActors.push_back( prop_temp );
        std::cout << "+1" << std::endl;
        }
      prop_temp = this->m_vtkViewImage3D->GetProp3DCollection()->GetNextProp3D();
      }
    // If actor in box, emit signal
    //this->InvokeEvent(vtkViewImage3DCommand::);
    // Update a list of picked actors
    planes->Delete();
    }

}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkProp*
vtkViewImage3DCommand::
GetPickedActor()
{
  return m_vtkViewImage3D->GetInteractorStyle3D()->GetCurrentProp();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void
vtkViewImage3DCommand::
SetVtkImageView3D( vtkViewImage3D* vtkViewImage3D )
{
  m_vtkViewImage3D = vtkViewImage3D;
}
//----------------------------------------------------------------------------

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
GetListOfPickedActors()
{
  return m_ListOfPickedActors;
}
//----------------------------------------------------------------------------
