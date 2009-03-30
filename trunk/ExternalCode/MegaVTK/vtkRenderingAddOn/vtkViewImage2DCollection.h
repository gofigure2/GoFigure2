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

#ifndef _vtkViewImage2DCollection_h_
#define _vtkViewImage2DCollection_h_

#include "vtkCollection.h"
#include "vtkCommand.h"

#include "vtkViewImage2D.h"
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


class vtkScalarBarActor;
class vtkLookupTable;
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
class vtkViewImage2DCollectionCommand;

/**

*/

class VTK_RENDERINGADDON2_EXPORT vtkViewImage2DCollection : public vtkCollection
{
 public:

  static vtkViewImage2DCollection* New();
  vtkTypeRevisionMacro(vtkViewImage2DCollection, vtkCollection);

  // Description:
  // Get the next vtkViewImage2D in the list. Return NULL when at the end of the
  // list.
  vtkViewImage2D *GetNextItem() {
    return static_cast<vtkViewImage2D *>(this->GetNextItemAsObject());};
  // Description:
  // Get the next vtkViewImage2D2D in the list. Return NULL when at the end of
  // the list.
  vtkViewImage2D *GetItem(int i) {
    return static_cast<vtkViewImage2D *>(this->GetItemAsObject(i));};
  // Description:
  // Add an object to the list. Does not prevent duplicate entries.
  void AddItem(vtkViewImage2D*);

  // Description:
  // Replace the i'th item in the collection with a
  void ReplaceItem(int i, vtkViewImage2D *);

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
  void RemoveItem(vtkViewImage2D *);

  // Description:
  // Remove all objects from the list.
  void RemoveAllItems();

  // Description:
  // Initialize the viewers togethers.
  void Initialize(void);


  vtkGetObjectMacro (Command, vtkViewImage2DCollectionCommand);
  vtkGetObjectMacro (ExtraRenderWindow, vtkRenderWindow);
  void SetExtraRenderWindow (vtkRenderWindow* win)
  { this->ExtraRenderWindow = win; }

  vtkSyncSetMacro (Slice, int, vtkViewImage2D);
  vtkSyncSetMacro (SliceOrientation, int, vtkViewImage2D);
  vtkSyncSetMacro (ShowAnnotations, int, vtkViewImage2D);
  vtkSyncSetMacro (ShowScalarBar, int, vtkViewImage2D);

  vtkSyncSetMacro (ColorWindow, double, vtkViewImage2D);
  vtkSyncSetMacro (ColorLevel, double, vtkViewImage2D);

  vtkSyncSetObjectMacro (OrientationMatrix, vtkMatrix4x4, vtkViewImage2D);
  vtkSyncSetObjectMacro (LookupTable, vtkLookupTable, vtkViewImage2D);
  vtkSyncSetObjectMacro (TextProperty, vtkTextProperty, vtkViewImage2D);
  vtkSyncSetObjectMacro (Input, vtkImageData, vtkViewImage2D);
  vtkSyncSetObjectMacro (InputConnection, vtkAlgorithmOutput, vtkViewImage2D);

  vtkSyncSetObjectMacro (Size, int, vtkViewImage2D);
  vtkSyncSetObjectMacro (Position, int, vtkViewImage2D);
  vtkSyncSetObjectMacro (WorldCoordinates, double, vtkViewImage2D);
  vtkSyncSetObjectMacro (Background, double, vtkViewImage2D);

  void SyncRender (void);
  void SyncResetWindowLevel (void);
  void SyncReset (void);

 protected:

  vtkViewImage2DCollection();
  ~vtkViewImage2DCollection();

  vtkViewImage2DCollectionCommand* Command;
  vtkRenderWindow* ExtraRenderWindow;
};




class vtkViewImage2DCollectionCommand :
  public vtkCommand
{

 public:

  static vtkViewImage2DCollectionCommand *New()
    {return new vtkViewImage2DCollectionCommand;};

  vtkViewImage2DCollection* GetCollection (void)
  { return this->Collection;};
  void SetCollection (vtkViewImage2DCollection* p);

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute(vtkObject *caller,unsigned long event,
    void *vtkNotUsed(callData));

 protected:

  vtkViewImage2DCollectionCommand();
  ~vtkViewImage2DCollectionCommand(){};

 private:
  vtkViewImage2DCollection* Collection;

  double InitialWindow;
  double InitialLevel;

};




#endif /* _vtkViewImage2DCollection_h_ */
