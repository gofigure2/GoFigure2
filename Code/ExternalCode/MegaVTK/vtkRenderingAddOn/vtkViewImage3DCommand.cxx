#include "vtkViewImage3DCommand.h"

#include "vtkInteractorStyleImage3D.h"
#include "vtkProp.h"
#include "vtkViewImage3D.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

//----------------------------------------------------------------------------
vtkViewImage3DCommand::
vtkViewImage3DCommand()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkViewImage3DCommand::
~vtkViewImage3DCommand()
{
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
  // Caller = interactor
  // utility of this code..?
  if (event)
    {
    vtkInteractorStyleImage3D* test = static_cast<vtkInteractorStyleImage3D*>(caller);
    m_PickedActor = test->GetCurrentProp();
    }
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkProp*
vtkViewImage3DCommand::
GetPickedActor()
{
  return m_vtkViewImage3D->GetInteractorStyle3D()->GetCurrentProp();
}//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void
vtkViewImage3DCommand::
SetVtkImageView3D( vtkViewImage3D* vtkViewImage3D )
{
  m_vtkViewImage3D = vtkViewImage3D;
}
