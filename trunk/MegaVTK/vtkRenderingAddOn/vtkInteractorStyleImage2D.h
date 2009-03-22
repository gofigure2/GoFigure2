#ifndef _vtk_InteractorStyleImage2D_h_
#define _vtk_InteractorStyleImage2D_h_

#include <vtkInteractorStyleImage.h>

#define VTKIS_SLICE_MOVE  5051

class vtkInteractorStyleImage2D :
  public vtkInteractorStyleImage
{
 public:
  static vtkInteractorStyleImage2D *New();
  vtkTypeRevisionMacro (vtkInteractorStyleImage2D, vtkInteractorStyleImage);

  virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnMouseWheelForward();
  virtual void OnMouseWheelBackward();
  virtual void OnChar();
  virtual void OnKeyUp();
  virtual void OnKeyPress();
  virtual void OnKeyRelease();

  virtual void StartSliceMove();
  virtual void SliceMove();
  virtual void EndSliceMove();


  vtkGetMacro( SliceStep, int );
  int* GetRequestedPosition(void)
  { return this->RequestedPosition; }

 protected:
  vtkInteractorStyleImage2D();
  ~vtkInteractorStyleImage2D();

  int SliceStep;
  int* RequestedPosition;

 private:

  vtkInteractorStyleImage2D(const vtkInteractorStyleImage2D&);  // Not implemented.
  void operator=(const vtkInteractorStyleImage2D&);  // Not implemented.

};

#endif
