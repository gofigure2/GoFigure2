#ifndef _vtkViewImage3D_h_
#define _vtkViewImage3D_h_

#include "vtkViewImage.h"

#include <vtkImageActor.h>
#include <vtkRenderer.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageMapToColors.h>
#include <vtkOrientedBoxWidget.h>
#include <vtkPlaneWidget.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMIPFunction.h>
#include <vtkVolumeRayCastIsosurfaceFunction.h>
#include <vtkVolumeProperty.h>
#include "vtkImage3DCroppingBoxCallback.h"
#include <vtkObjectFactory.h>

#include <vector>

class vtkVolumeTextureMapper3D;
class vtkVolumeMapper;
class vtkVolumeRayCastFunction;
class vtkPiecewiseFunction;
class vtkColorTransferFunction;
class vtkVolume;
class vtkImageActor;
class vtkAxes;
class vtkDataSet3DCroppingPlaneCallback;
class vtkTubeFilter;
class vtkAnnotatedCubeActor;
class vtkOrientationMarkerWidget;
class vtkAxesActor;
class vtkImageBlend;
class vtkViewImage2D;
class vtkScalarsToColors;
class vtkColorTransferFunction;


/**

*/

class ImageActorCallback;

class vtkViewImage3D : public vtkViewImage
{

 public:

  static vtkViewImage3D* New();
  vtkTypeRevisionMacro(vtkViewImage3D, vtkViewImage);

  // Description:
  // Render the resulting image.
  virtual void Render(void);

  /**
    Add a dataset to the view (has to be subclass of vtkPointSet).
    The dataset will be cut through the implicit slice plane
    (GetImplicitSlicePlane()).

    This results in a loss of dimensionality, i.e. tetrahedron will be displayed
    as triangles, triangles as lines, lines as points.
    A vtkProperty of the dataset can be specified.
  */
  virtual void AddDataSet (vtkDataSet* dataset,
    vtkProperty* property = NULL,
    const bool& intersection = true ) {};
  /**
     Set/Get the current slice to display (depending on the orientation
     this can be in X, Y or Z).
  */
  virtual void SetSlice(int s){};
  virtual void SetSliceOrientation(int orientation){};
  // Description:
  // Update the display extent manually so that the proper slice for the
  // given orientation is displayed. It will also try to set a
  // reasonable camera clipping range.
  // This method is called automatically when the Input is changed, but
  // most of the time the input of this class is likely to remain the same,
  // i.e. connected to the output of a filter, or an image reader. When the
  // input of this filter or reader itself is changed, an error message might
  // be displayed since the current display extent is probably outside
  // the new whole extent. Calling this method will ensure that the display
  // extent is reset properly.
  virtual void UpdateDisplayExtent(){};

  virtual void Add2DPhantom(
    const unsigned int& i,
    vtkImageActor* input,
    vtkPolyData* in_bounds = NULL );

  virtual void SetInput (vtkImageData* input);

  virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);

  void SetVolumeRenderingOff()
  {
    this->VolumeActor->SetVisibility (false);
    this->BoxWidget->Off();
  }

  void SetVolumeRenderingOn()
  {
    this->VolumeActor->SetVisibility(true);
  }

  void SetTriPlanarRenderingOn()
  {
    this->VolumeActor->SetVisibility(false);
    for( int i = 0; i < 3; i++ )
    {
      this->Phantom[i]->SetVisibility(true);
      this->BoundsActor[i]->SetVisibility(true);
    }
  }

  void SetTriPlanarRenderingOff()
  {
    this->VolumeActor->SetVisibility(true);
    for( int i = 0; i < 3; i++ )
    {
      this->Phantom[i]->SetVisibility(false);
      this->BoundsActor[i]->SetVisibility(false);
    }
  }

  void SetVolumeMapperToTexture (void)
  {
    this->VolumeActor->SetMapper ( this->VolumeMapper3D );
    this->Callback->SetVolumeMapper ( this->VolumeMapper3D );
  }

  void SetVolumeMapperToRayCast (void)
  {
    this->VolumeActor->SetMapper ( this->VolumeRayCastMapper );
    this->Callback->SetVolumeMapper ( this->VolumeRayCastMapper );
  }

  void SetVolumeRayCastFunctionToComposite (void)
  {
    this->VolumeRayCastMapper->SetVolumeRayCastFunction
      (this->VolumeRayCastCompositeFunction );
  }


  void SetVolumeRayCastFunctionToMIP (void)
  {
    this->VolumeRayCastMapper->SetVolumeRayCastFunction
      ( this->VolumeRayCastMIPFunction );
  }


  void SetVolumeRayCastFunctionToIsosurface (void)
  {
    this->VolumeRayCastMapper->SetVolumeRayCastFunction
      ( this->VolumeRayCastIsosurfaceFunction );
  }



  /** Set the box widget visibility */
  void SetBoxWidgetVisibility (int a)
  {
    if (this->Interactor)
      this->BoxWidget->SetEnabled (a);
  }
  bool GetBoxWidgetVisibility (void)
  {
    return ( this->BoxWidget->GetEnabled() == 1 );
  }
  vtkBooleanMacro (BoxWidgetVisibility, int);

  /** Set the plane widget on */
  void SetPlaneWidgetVisibility (int a)
  {
    if (this->Interactor)
      this->PlaneWidget->SetEnabled (a);
  }
  bool GetPlaneWidgetVisibility (void)
  {
    return ( this->PlaneWidget->GetEnabled() == 1 );
  }
  vtkBooleanMacro (PlaneWidgetVisibility, int);

  /** Set the cube widget on */
  void SetCubeVisibility (int a)
  {
    if (this->Interactor)
      this->Marker->SetEnabled (a);
  }
  bool GetCubeVisibility (void)
  {
    return ( this->Marker->GetEnabled() == 1 );
  }
  vtkBooleanMacro (CubeVisibility, int);

  void SetShade (int a)
  {
    this->VolumeProperty->SetShade (a);
  }
  bool GetShade (void)
  {
    return ( this->VolumeProperty->GetShade() == 1 );
  }
  vtkBooleanMacro (Shade, int);


  /** Get volume actor */
  vtkGetObjectMacro (VolumeActor, vtkVolume);
  vtkGetObjectMacro (PlaneWidget, vtkPlaneWidget);
  vtkGetObjectMacro (VolumeProperty, vtkVolumeProperty);
  vtkGetObjectMacro (BoxWidget, vtkOrientedBoxWidget);



 protected:

  vtkViewImage3D();
  ~vtkViewImage3D();

  // Description:
  virtual void InstallPipeline();
  virtual void UpdateOrientation(){}

  virtual void SetupVolumeRendering();
  virtual void SetupWidgets();
  virtual void SetupTextureMapper();

  // texture mapper in 3D
  vtkVolumeMapper*                VolumeMapper3D;
  // volume ray cast mapper
  vtkVolumeRayCastMapper*         VolumeRayCastMapper;
  // ray cast function
  vtkVolumeRayCastMIPFunction*        VolumeRayCastMIPFunction;
  // ray cast function
  vtkVolumeRayCastCompositeFunction*  VolumeRayCastCompositeFunction;
  // ray cast function
  vtkVolumeRayCastIsosurfaceFunction* VolumeRayCastIsosurfaceFunction;
  // volume property
  vtkVolumeProperty*              VolumeProperty;
  // volume actor
  vtkVolume*                      VolumeActor;
  // opacity transfer function
  vtkPiecewiseFunction*           OpacityFunction;
  // blender
  vtkImageBlend*                  Blender;
  // image 3D cropping box callback
  vtkImage3DCroppingBoxCallback*  Callback;

  std::vector< vtkImageActor* >   Phantom;
  std::vector< ImageActorCallback* > PhantomCallback;
  std::vector< vtkActor* >  BoundsActor;

  // box widget
  vtkOrientedBoxWidget*                   BoxWidget;
  // vtkPlane widget
  vtkPlaneWidget*                 PlaneWidget;
  // annotated cube actor
  vtkAnnotatedCubeActor*          Cube;
  vtkOrientationMarkerWidget*     Marker;

};

#endif /* _vtkViewImage3D_h_ */
