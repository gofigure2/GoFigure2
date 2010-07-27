#include "vtkViewImage3DCommand.h"

#include "vtkViewImage3D.h"
#include "vtkPolyData.h"

#include "vtkOrientedBoxWidget.h"

//----------------------------------------------------------------------------
vtkViewImage3DCommand::
vtkViewImage3DCommand()
{
}

//----------------------------------------------------------------------------
vtkViewImage3DCommand::
~vtkViewImage3DCommand()
{
}

//----------------------------------------------------------------------------
vtkViewImage3DCommand*
vtkViewImage3DCommand::New()
{
  return new vtkViewImage3DCommand;
}

//----------------------------------------------------------------------------
void
vtkViewImage3DCommand::
Execute(vtkObject *caller, unsigned long event, void *callData)
{

  if (event == vtkViewImage3DCommand::MeshPickingEvent)
    {
    m_vtkViewImage3D->UpdateCurrentActor();
    return;
    }
  if (event == vtkCommand::InteractionEvent)
    {
    vtkOrientedBoxWidget* test = static_cast<vtkOrientedBoxWidget*>(caller);
    vtkPolyData* pd = vtkPolyData::New();
    test->GetPolyData(pd);

    double pt[3];
    pd->GetPoint(0, pt);

    double bextent[6];
    bextent[0] = pt[0];
    bextent[1] = pt[0];
    bextent[2] = pt[1];
    bextent[3] = pt[1];
    bextent[4] = pt[2];
    bextent[5] = pt[2];

    // Get Extent of the box
    for (int i = 1; i < 8; i += 2)
      {
      pd->GetPoint(i, pt);

      if (pt[0] < bextent[0]) bextent[0] = pt[0];
      if (pt[0] > bextent[1]) bextent[1] = pt[0];
      if (pt[1] < bextent[2]) bextent[2] = pt[1];
      if (pt[1] > bextent[3]) bextent[3] = pt[1];
      if (pt[2] < bextent[4]) bextent[4] = pt[2];
      if (pt[2] > bextent[5]) bextent[5] = pt[2];
      }
    pd->Delete();

    //Get Actors which are in the box (picked/unpicked)
    m_vtkViewImage3D->UpdateActorsStatus(bextent);
    return;
    }
}

//----------------------------------------------------------------------------
void
vtkViewImage3DCommand::
SetVtkImageView3D(vtkViewImage3D* iViewImage3D)
{
  if (iViewImage3D)
    {
    m_vtkViewImage3D = iViewImage3D;
    }
}
