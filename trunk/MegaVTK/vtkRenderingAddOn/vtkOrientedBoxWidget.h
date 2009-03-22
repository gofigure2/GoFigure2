#ifndef _vtkOrientedBoxWidget_h_
#define _vtkOrientedBoxWidget_h_

#include "vtkBoxWidget.h"

class vtkMatrix4x4;

/**
   \author Nicolas Toussaint
   \class vtkOrientedBoxWidget
   \brief orthogonal hexahedron 3D widget with pre-defined orientation

   Description

   This 3D widget defines a region of interest. By default it behaves exactly as
   the vtkBoxWidget class does. But if the user set the Orientation matrix
   with SetOrientationMatrix() then all the actors of the widgets (handles,
   hexahedron, etc) will be oriented according to the argument.

   \see vtk3DWidget vtkBoxWidget
 */
class vtkOrientedBoxWidget : public vtkBoxWidget
{

 public:

  static vtkOrientedBoxWidget* New();
  vtkTypeRevisionMacro(vtkOrientedBoxWidget, vtkBoxWidget);

  vtkGetObjectMacro (OrientationMatrix, vtkMatrix4x4);
  virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);

 protected:

  vtkOrientedBoxWidget();
  ~vtkOrientedBoxWidget();

  virtual void OnMouseMove();

  vtkMatrix4x4* OrientationMatrix;
  vtkMatrix4x4* InvertedOrientationMatrix;


};

#endif /* _vtkOrientedBoxWidget_h_ */
