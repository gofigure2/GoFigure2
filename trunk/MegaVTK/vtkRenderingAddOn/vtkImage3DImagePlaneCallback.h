#ifndef _vtk_Image3DImagePlaneCallback_h_
#define _vtk_Image3DImagePlaneCallback_h_

#include <vtkCommand.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>


class vtkViewImage2D;


class vtkImage3DImagePlaneCallback: public vtkCommand
{

 public:

  static vtkImage3DImagePlaneCallback* New()
  { return new vtkImage3DImagePlaneCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  /*
  void SetViewImage2D (vtkViewImage2D* view)
  {
    this->ViewImage2D = view;
    this->FirstRender = true;
    }*/

  vtkImageData* GetOutput (void) const
  {
    return this->Reslice->GetOutput();
  }


  virtual void Reset (void)
  {
    this->Reslice->SetInput (NULL);
    //this->FirstRender = true;
  }

  vtkImageReslice* GetReslice()
  {
    return this->Reslice;
  }
  vtkMatrix4x4* GetMatrix()
  {
    return this->ResliceAxes;
  }




 protected:
 vtkImage3DImagePlaneCallback()
 {
   /*
   this->ViewImage2D = 0;
   this->FirstRender = true;
   */
   this->Reslice     = vtkImageReslice::New();
   this->ResliceAxes = vtkMatrix4x4::New();
 }
 ~vtkImage3DImagePlaneCallback()
 {
   this->Reslice->Delete();
   this->ResliceAxes->Delete();
 }

 private:
 /*
 vtkViewImage2D*   ViewImage2D;
 bool              FirstRender;*/

 vtkImageReslice*  Reslice;
 vtkMatrix4x4*     ResliceAxes;

};


#endif
