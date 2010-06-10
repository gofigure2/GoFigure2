#include "vtkViewImage3DCommand.h"

#include "vtkInteractorStyleImage3D.h"
#include "vtkProp.h"
#include "vtkViewImage3D.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkImageData.h"

#include "vtkOrientedBoxWidget.h"

//----------------------------------------------------------------------------
vtkViewImage3DCommand::
vtkViewImage3DCommand()
{
  this->m_BoxWidget = vtkOrientedBoxWidget::New();
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
