#ifndef _vtk_Image3DCroppingBoxCallback_h_
#define _vtk_Image3DCroppingBoxCallback_h_

#include <vtkCommand.h>
#include <vtkVolumeMapper.h>
#include <vtkSetGet.h>
#include <vtkObjectFactory.h>

class vtkImage3DCroppingBoxCallback: 
public vtkCommand
{

 public:
  static vtkImage3DCroppingBoxCallback* New()
  { return new vtkImage3DCroppingBoxCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  void SetVolumeMapper (vtkVolumeMapper* mapper)
  {
    this->VolumeMapper = mapper;
  }
  vtkVolumeMapper* GetVolumeMapper (void) const
  {
    return this->VolumeMapper;
  }


 protected:
  vtkImage3DCroppingBoxCallback() : VolumeMapper( 0 ) {}
  ~vtkImage3DCroppingBoxCallback(){}

 private:

  vtkVolumeMapper* VolumeMapper;

};


#endif
