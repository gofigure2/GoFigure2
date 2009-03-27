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

#ifndef _vtkViewColorImage2D_h_
#define _vtkViewColorImage2D_h_

#include "vtkViewColorImage.h"

class vtkPlane;
class vtkViewColorImage2DCommand;
class vtkTransform;
class vtkImageBlendWithMask;

/**

*/


class VTK_RENDERINGADDON2_EXPORT vtkViewColorImage2D : public vtkViewColorImage
{

 public:

  static vtkViewColorImage2D* New();
  vtkTypeRevisionMacro(vtkViewColorImage2D, vtkViewColorImage);
  /**
     Description:
     The orientation of the view is a abstract representation of the object
     we are looking at. It results from the acquisition plane. Setting the View
     Orientation by calling SetViewOrientation() will imply the view to set its
     inner "slice" orientation. (slice orientation == 2 means plane of
     acquisition.)

     IMPORTANT NOTE:

     The view orientations defined here are orthogonal to the normal basis
     in the scanner. A very interesting improvement would be to define "oblique"
     view orientations for cardiac imaging, something like:

     VIEW_ORIENTATION_SHORT_AXIS, VIEW_ORIENTATION_LONG_AXIS, and
     VIEW_ORIENTATION_FOUR_CHAMBER could define the different views that are
     usually used in cardiac imaging.

     From this user-input information, the idea would be to evaluate which slice
     orientation does correspond to the requested view. This can be done by
     evaluating the dot product between the axis of acquisition and a
     pre-defined oblique axis that "should" correspond to the requested
     orientation...
  **/
  //BTX
  enum
  {
    VIEW_ORIENTATION_SAGITTAL = 0,
    VIEW_ORIENTATION_CORONAL = 1,
    VIEW_ORIENTATION_AXIAL = 2
  };
  //ETX
  /**
     Description:
  **/
  //BTX
  enum
  {
    VIEW_CONVENTION_RADIOLOGICAL = 0,
    VIEW_CONVENTION_NEUROLOGICAL = 1
  };
  //ETX

  /**
     The SliceImplicitPlane instance (GetImplicitSlicePlane()) is the
     implicit function that cuts every dataset that is added with AddDataSet().
  */
  vtkGetObjectMacro (SliceImplicitPlane, vtkPlane);
  /**
     The SlicePlane instance (GetSlicePlane()) is the polygonal
     square corresponding to the slice plane,
     it is updated each time the slice changes,
     and is color-coded according to conventions
  */
  vtkGetObjectMacro (SlicePlane, vtkPolyData);
  /**
     The world is not always what we think it is ...

     Use this method to move the viewer slice such that the position
     (in world coordinates) given by the arguments is contained by
     the slice plane. If the given position is outside the bounds
     of the image, then the slice will be as close as possible.
  */
  virtual void SetWorldCoordinates (double pos[3]);

  /**
     Set/Get the current slice to display (depending on the orientation
     this can be in X, Y or Z).
  */
  virtual void SetSlice(int s);

  /**
     Instead of setting the slice orientation to an axis (YZ - XZ - XY),
     you can force the view to be axial (foot-head), coronal (front-back),
     or sagittal (left-right). It will just use the OrientationMatrix
     (GetOrientationMatrix()) to check which slice orientation to pick.
  */
  vtkGetMacro (ViewOrientation, int);
  virtual void SetViewOrientation (int orientation);
  virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);

  /**
     The ViewConvention instance explains where to place the camera around
     the patient. Default behaviour is Radiological convention, meaning
     we respectively look at the patient from his feet, his face and his left
     ear.

     For Neurological convention, we respectively look from the top of his head,
     the the back of his head, and his left ear.
  */
  ///\todo Why not adding cardiologic conventions where we look at the patient in
  /// oblique angles ?
  vtkGetMacro (ViewConvention, int);
  virtual void SetViewConvention (int convention);
  /**
     Convert an indices coordinate point (image coordinates) into a world
     coordinate point
  */
  virtual double* GetWorldCoordinatesForSlice (int slice);
  /**
     Convert a world coordinate point into an image indices coordinate point
  */
  virtual int GetSliceForWorldCoordinates(double pos[3]);

  virtual void ResetCamera(void);
  /**
     Reset the 3D position to center
  */
  virtual void ResetPosition(void);
  /**
     Reset position - zoom - to default
  */
  virtual void Reset(void);
  /**
   */
  virtual double* GetWorldCoordinatesFromDisplayPosition( int xy[2] );
  virtual double* GetWorldCoordinatesFromDisplayPosition( const int& x, const int& y )
  {
    int xy[2] = {x,y};
    return this->GetWorldCoordinatesFromDisplayPosition( xy );
  }

  vtkSetMacro( Zoom, double );
  vtkGetMacro( Zoom, double );

  vtkSetVector3Macro( CameraMotionVector, double );
  vtkGetVector3Macro( CameraMotionVector, double );

  void SetCameraFocalAndPosition( double focal[3], double pos[3] );
  void GetCameraFocalAndPosition( double focal[3], double pos[3] );

  vtkGetObjectMacro (Command, vtkViewColorImage2DCommand);

  virtual void SetInterpolate( const int& val );
  virtual int GetInterpolate ();
  vtkBooleanMacro (Interpolate, int);
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
    const bool& intersection = true );

  virtual void AddDataSet( vtkPolyData* polydata,
    vtkProperty* property = NULL,
    const bool& intersection = true );

 protected:

  vtkViewColorImage2D();
  ~vtkViewColorImage2D();

  // Description:
  virtual void UpdateOrientation();
  virtual int SetCameraToConvention (void);
  virtual void SetAnnotationToConvention(void);
  virtual void InitializeSlicePlane (void);
  virtual void UpdateSlicePlane (void);
  virtual void SetSlicePlaneToConvention (unsigned int axis);
  virtual void InstallPipeline();

  vtkMatrix4x4* ConventionMatrix;
  vtkPlane* SliceImplicitPlane;
  vtkTransform* AdjustmentTransform;
  vtkPolyData* SlicePlane;

  vtkViewColorImage2DCommand* Command;

  int ViewOrientation;
  int ViewConvention;

  double Zoom;
  double CameraMotionVector[3];
};

#endif /* _vtkViewColorImage2D_h_ */
