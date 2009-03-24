/*========================================================================
 Copyright (c) INRIA - ASCLEPIOS Project (http://www-sop.inria.fr/asclepios).
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of INRIA or ASCLEPIOS, nor the names of any contributors
 may be used to endorse or promote products derived from this software 
 without specific prior written permission.
 
 * Modified source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =========================================================================*/

/*=========================================================================
 Modifications were made by the GoFigure Dev. Team.
 while at Megason Lab, Systems biology, Harvard Medical school, 2009 
 
 Copyright (c) 2009, President and Fellows of Harvard College.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 Redistributions of source code must retain the above copyright notice, 
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation 
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College 
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written 
 permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 =========================================================================*/

#ifndef _vtkViewColorImage3D_h_
#define _vtkViewColorImage3D_h_

#include "vtkViewColorImage.h"

#include <vtkImageActor.h>
#include <vtkRenderer.h>
#include <vtkOrientedBoxWidget.h>
#include <vtkPlaneWidget.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkVolumeProperty.h>
#include "vtkImage3DCroppingBoxCallback.h"
#include <vtkObjectFactory.h>

#include <vector>

class vtkVolumeTextureMapper3D;
class vtkVolumeMapper;
class vtkPiecewiseFunction;
class vtkVolume;
class vtkImageActor;
class vtkAxes;
class vtkDataSet3DCroppingPlaneCallback;
class vtkTubeFilter;
class vtkAnnotatedCubeActor;
class vtkOrientationMarkerWidget;
class vtkAxesActor;
class vtkImageBlend;
class vtkViewColorImage2D;


/**

*/

class ImageActorCallback;

class vtkViewColorImage3D : public vtkViewColorImage
{

 public:

  static vtkViewColorImage3D* New();
  vtkTypeRevisionMacro(vtkViewColorImage3D, vtkViewColorImage);

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
  }

  void SetVolumeRayCastFunctionToComposite (void)
  {
  }


  void SetVolumeRayCastFunctionToMIP (void)
  {
  }


  void SetVolumeRayCastFunctionToIsosurface (void)
  {
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

  vtkViewColorImage3D();
  ~vtkViewColorImage3D();

  // Description:
  virtual void InstallPipeline();
  virtual void UpdateOrientation(){}

  virtual void SetupVolumeRendering();
  virtual void SetupWidgets();
  virtual void SetupTextureMapper();

  // texture mapper in 3D
  vtkVolumeMapper*                VolumeMapper3D;
  // volume property
  vtkVolumeProperty*              VolumeProperty;
  // volume actor
  vtkVolume*                      VolumeActor;
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

#endif /* _vtkViewColorImage3D_h_ */
