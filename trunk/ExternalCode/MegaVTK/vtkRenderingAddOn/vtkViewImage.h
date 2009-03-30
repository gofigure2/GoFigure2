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

#ifndef _vtkViewImage_h_
#define _vtkViewImage_h_

#include "vtkImageViewer2.h"
#include "MegaVTK2Configure.h"

/**
   This macro can be useful as we frequently set
   an instance this way, meaning unregistering previously set instance,
   registering given instance, and call a modified event.
   However this might be not the best place to define it...
*/
//BTX
#define vtkSetObjectMacro2Body(object, type, _arg)\
  if (this->object == _arg)			\
    return;					\
  if (this->object)				\
  {						\
    this->object->UnRegister(this);		\
  }						\
  this->object = _arg;				\
  						\
  if (this->object != NULL)			\
  {						\
    this->object->Register(this);		\
  }						\
  						\
  this->Modified();				\
//ETX

/**
   This macro can be useful as we frequently set
   an instance this way, meaning unregistering previously set instance,
   registering given instance, and call a modified event.
   However this might be not the best place to define it...
*/
//BTX
#define vtkSetObjectMacro2(object, type)	\
  virtual void Set##object (type* _arg)		\
  {						\
    vtkSetObjectMacro2Body (object, type, _arg);\
  }						\

//ETX


class vtkMatrix4x4;
class vtkScalarBarActor;
class vtkLookupTable;
class vtkTextProperty;
class vtkCornerAnnotation;
class vtkOrientationAnnotation;
class vtkActor;
class vtkDataSet;
class vtkPolyData;
class vtkProperty;
class vtkProp3DCollection;
class vtkDataSetCollection;
class vtkMatrixToLinearTransform;

/**

*/


class VTK_RENDERINGADDON2_EXPORT vtkViewImage : public vtkImageViewer2
{

 public:

  static vtkViewImage* New();
  vtkTypeRevisionMacro(vtkViewImage, vtkImageViewer2);

  /**
     Get the orientation annotation. This annotation describes the orientation
     of the slice plane, according to the rule
     Right(R)-Left(L) Anterior(A)-Posterior(P) Inferior(I)-Superior(S)
  */
  vtkGetObjectMacro (OrientationAnnotation, vtkOrientationAnnotation);
  /**
     Get the corner annotation.
  */
  ///\todo make the corner annotation such that it follows the slice number, the
  ///image scalar value at cursor, the spacing, etc
  vtkGetObjectMacro (CornerAnnotation, vtkCornerAnnotation);
  /**
     Get the scalar bar actor. This instance follows the color window/level
     of the viewer.
  */
  ///\todo make this scalar bar actually follow the WindowLevel filter. It does
  /// not seems to work yet
  vtkGetObjectMacro (ScalarBarActor, vtkScalarBarActor);
  /**
     The OrientationMatrix instance (GetOrientationMatrix()) is a very important
     added feature of this viewer. It describes the rotation and translation to
     apply to the image bouding box (axis aligned) to the world coordinate
     system.

     Rotation part is usually given by the GetDirection() method on an itk::Image
     for instance. Translation usually correspond to the origin of the image
     given by GetOrigin() on an itk::Image.

     CAUTION: if you provide non-zero origin to the viewer vtkImageData input
     (SetInput()), then don't provide translation to the OrientationMatrix
     instance, otherwise the information is redundant.

     The best behaviour is to force the origin of the vtkImageData input to zero
     and provide this origin information in the OrientationMatrix.

  */
  vtkGetObjectMacro (OrientationMatrix, vtkMatrix4x4);
  virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);
  /**
     The LookupTable instance (GetLookupTable()) can be used to set a
     user-defined color-table to the viewer. Default is a linear black to
     white table.
  */
  vtkGetObjectMacro (LookupTable, vtkLookupTable);
  virtual void SetLookupTable (vtkLookupTable* lookuptable);
  /**
     The TextProperty instance (GetTextProperty()) describes the font and
     other settings of the CornerAnnotation instance (GetCornerAnnotation())
  */
  vtkGetObjectMacro (TextProperty, vtkTextProperty);
  virtual void SetTextProperty (vtkTextProperty* textproperty);
  /**
     The wolld is not always what we think it is ...

     Use this method to move the viewer slice such that the position
     (in world coordinates) given by the arguments is contained by
     the slice plane. If the given position is outside the bounds
     of the image, then the slice will be as close as possible.
  */
  virtual void SetWorldCoordinates (double x, double y, double z)
  {
    double pos[3] = {x,y,z};
    this->SetWorldCoordinates (pos);
  }
  virtual void SetWorldCoordinates (double pos[3]){};

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
  virtual bool RemoveDataSet (vtkDataSet* dataset);

  /**
     Set/Get the current slice to display (depending on the orientation
     this can be in X, Y or Z).
  */
  virtual void SetSlice(int s)
  { this->Superclass::SetSlice (s); };

  /**
     Convert an indices coordinate point (image coordinates) into a world
     coordinate point
  */
  virtual double* GetWorldCoordinatesFromImageCoordinates (int indices[3]);
  /**
     Convert a world coordinate point into an image indices coordinate point
  */
  virtual int* GetImageCoordinatesFromWorldCoordinates (double position[3]);
  /**
     Get the pixel value at a given world coordinate point in space, return
     zero if out of bounds.
  */
  virtual double GetValueAtPosition(double worldcoordinates[3],
    int component=0 );
  /**
     Set the background color. Format is RGB, 0 <= R,G,B <=1
     Example: SetBackground(0.9,0.9,0.9) for grey-white.
  */
  virtual void SetBackground(double rgb[3]);
  virtual void SetBackground(double r, double g, double b);
  virtual double* GetBackground(void);
  /**
     Reset the camera
  */
  virtual void ResetCamera(void);
  /**
     Reset position - zoom - window/level to default
  */
  virtual void Reset(void);
  /**
     Show/Hide the annotations.
  */
  vtkGetMacro (ShowAnnotations, int);
  /**
     Show/Hide the annotations.
  */
  virtual void SetShowAnnotations (int);
  /**
     Show/Hide the annotations.
  */
  vtkBooleanMacro (ShowAnnotations, int);
  /**
     Enable or Disable interaction on the view.
  */
  virtual void Enable (void);
  /**
     Enable or Disable interaction on the view.
  */
  virtual void Disable (void);
  /**
     Enable or Disable interaction on the view.
  */
  virtual int GetEnabled (void);
  /**
     Show/Hide the annotations.
  */
  vtkGetMacro (ShowScalarBar, int);
  /**
     Show/Hide the annotations.
  */
  virtual void SetShowScalarBar (int);
  /**
     Show/Hide the annotations.
  */
  vtkBooleanMacro (ShowScalarBar, int);
  // Description:
  // Set window and level for mapping pixels to colors.
  virtual void SetColorWindow(double s);
  virtual void SetColorLevel(double s);

  /**
     Set a mask image and its corresponding LookupTable. The mask image will
     be overlapped to the current image, and the lookup table is used to assess
     the color of the label: label 0 will have color given by entry 0 of the LUT,
     etc.
     The image has to be of type unsigned char.
  */
  virtual void SetMaskImage( vtkImageData* mask, vtkLookupTable* lut )
  {
    this->MaskImage = mask;
    this->MaskLUT = lut;
  }
  vtkGetObjectMacro( MaskImage, vtkImageData );
  vtkGetObjectMacro( MaskLUT, vtkLookupTable );

  virtual vtkRenderWindowInteractor* GetRenderWindowInteractor();

 protected:

  vtkViewImage();
  ~vtkViewImage();

  // Description:
  vtkMatrix4x4*               OrientationMatrix;
  vtkCornerAnnotation*        CornerAnnotation;
  vtkOrientationAnnotation*   OrientationAnnotation;
  vtkTextProperty*            TextProperty;
  vtkLookupTable*             LookupTable;
  vtkScalarBarActor*          ScalarBarActor;
  vtkProp3DCollection*        Prop3DCollection;
  vtkDataSetCollection*       DataSetCollection;
  vtkMatrixToLinearTransform* OrientationTransform;

  vtkImageData*               MaskImage;
  vtkLookupTable*             MaskLUT;

  int ShowAnnotations;
  int ShowScalarBar;

};

#endif /* _vtkViewImage_h_ */
