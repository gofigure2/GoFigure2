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

#ifndef _vtkViewColorImage2DCollection_h_
#define _vtkViewColorImage2DCollection_h_

#include "vtkCollection.h"
#include "vtkCommand.h"

#include "vtkViewColorImage2D.h"
#include "MegaVTK2Configure.h"

/**
   This macro can be useful as we frequently set
   an instance this way, meaning unregistering previously set instance,
   registering given instance, and call a modified event.
   However this might be not the best place to define it...
*/
//BTX
#define vtkSyncSetMacro(name, type, base )			\
  virtual void SyncSet##name (type _arg)		\
  {							\
    this->InitTraversal();				\
    base* item = this->GetNextItem();	\
    while(item)						\
    {							\
      item->Set##name (_arg);				\
      item = this->GetNextItem();			\
    }							\
  }
///ETX

///BTX
#define vtkSyncSetObjectMacro(name, type, base)		\
  virtual void SyncSet##name (type* _arg)		\
  {							\
    this->InitTraversal();				\
    base* item = this->GetNextItem();	\
    while(item)						\
    {							\
      item->Set##name (_arg);				\
      item = this->GetNextItem();			\
    }							\
  }

///ETX


class vtkTextProperty;
class vtkCornerAnnotation;
class vtkOrientationAnnotation;
class vtkPlane;
class vtkActor;
class vtkDataSet;
class vtkPolyData;
class vtkProperty;
class vtkProp3DCollection;
class vtkDataSetCollection;
class vtkMatrixToLinearTransform;
class vtkViewColorImage2DCollectionCommand;

/**

*/
// NOTE ALEX: could do a template here to manage color and greyscale with i
// the same class.
class VTK_RENDERINGADDON2_EXPORT vtkViewColorImage2DCollection :
  public vtkCollection
{
 public:

  static vtkViewColorImage2DCollection* New();
  vtkTypeRevisionMacro(vtkViewColorImage2DCollection, vtkCollection);

  // Description:
  // Get the next vtkViewColorImage2D in the list.
  // Return NULL when at the end of the list.
  vtkViewColorImage2D *GetNextItem() {
    return static_cast<vtkViewColorImage2D *>(this->GetNextItemAsObject());};

  // Description:
  // Get the next vtkViewColorImage2D in the list.Return NULL when at the end of
  // the list.
  vtkViewColorImage2D *GetItem(int i) {
    return static_cast<vtkViewColorImage2D *>(this->GetItemAsObject(i));};

  // Description:
  // Add an object to the list. Does not prevent duplicate entries.
  void AddItem(vtkViewColorImage2D*);

  // Description:
  // Replace the i'th item in the collection with a
  void ReplaceItem(int i, vtkViewColorImage2D *);

  // Description:
  // Remove the i'th item in the list.
  // Be careful if using this function during traversal of the list using
  // GetNextItemAsObject (or GetNextItem in derived class).  The list WILL
  // be shortened if a valid index is given!  If this->Current is equal to the
  // element being removed, have it point to then next element in the list.
  void RemoveItem(int i);

  // Description:
  // Remove an object from the list. Removes the first object found, not
  // all occurrences. If no object found, list is unaffected.  See warning
  // in description of RemoveItem(int).
  void RemoveItem(vtkViewColorImage2D *);

  // Description:
  // Remove all objects from the list.
  void RemoveAllItems();

  // Description:
  // Initialize the viewers togethers.
  void Initialize(void);


  vtkGetObjectMacro (Command, vtkViewColorImage2DCollectionCommand);
  vtkGetObjectMacro (ExtraRenderWindow, vtkRenderWindow);
  void SetExtraRenderWindow (vtkRenderWindow* win)
  { this->ExtraRenderWindow = win; }

  vtkSyncSetMacro (Slice, int, vtkViewColorImage2D);
  vtkSyncSetMacro (SliceOrientation, int, vtkViewColorImage2D);
  vtkSyncSetMacro (ShowAnnotations, int, vtkViewColorImage2D);

  vtkSyncSetObjectMacro (OrientationMatrix, vtkMatrix4x4, vtkViewColorImage2D);
  vtkSyncSetObjectMacro (TextProperty, vtkTextProperty, vtkViewColorImage2D);
  vtkSyncSetObjectMacro (Input, vtkImageData, vtkViewColorImage2D);
  vtkSyncSetObjectMacro (InputConnection, vtkAlgorithmOutput,
    vtkViewColorImage2D);

  vtkSyncSetObjectMacro (Size, int, vtkViewColorImage2D);
  vtkSyncSetObjectMacro (Position, int, vtkViewColorImage2D);
  vtkSyncSetObjectMacro (WorldCoordinates, double, vtkViewColorImage2D);
  vtkSyncSetObjectMacro (Background, double, vtkViewColorImage2D);

  void SyncRender (void);
  void SyncReset (void);

 protected:

  vtkViewColorImage2DCollection();
  ~vtkViewColorImage2DCollection();

  vtkViewColorImage2DCollectionCommand* Command;
  vtkRenderWindow* ExtraRenderWindow;
};




class VTK_RENDERINGADDON2_EXPORT vtkViewColorImage2DCollectionCommand :
  public vtkCommand
{

 public:

  static vtkViewColorImage2DCollectionCommand *New()
    {return new vtkViewColorImage2DCollectionCommand;};

  vtkViewColorImage2DCollection* GetCollection (void)
  { return this->Collection;};
  void SetCollection (vtkViewColorImage2DCollection* p);

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute(vtkObject *caller,unsigned long event,
    void *vtkNotUsed(callData));

 protected:

  vtkViewColorImage2DCollectionCommand();
  ~vtkViewColorImage2DCollectionCommand(){};

 private:
  vtkViewColorImage2DCollection* Collection;
};

#endif /* _vtkViewColorImage2DCollection_h_ */
