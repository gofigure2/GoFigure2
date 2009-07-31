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

#ifndef __vtkViewImage2DWithContourWidgetCollection_h
#define __vtkViewImage2DWithContourWidgetCollection_h

#include <vtkCollection.h>
#include <vtkCommand.h>

#include "vtkViewImage2DCollection.h"
#include "vtkViewImage2DWithContourWidget.h"
#include <vtkTextProperty.h>
#include "MegaVTK2Configure.h"

class vtkViewImage2DWithContourWidgetCollectionCommand;

class VTK_RENDERINGADDON2_EXPORT vtkViewImage2DWithContourWidgetCollection :
public vtkCollection
{
public:
  static vtkViewImage2DWithContourWidgetCollection* New();
  vtkTypeRevisionMacro( vtkViewImage2DWithContourWidgetCollection,
    vtkCollection );

  // Description:
  // Get the next vtkViewImage2D in the list. Return NULL when at the end of the
  // list.
  vtkViewImage2DWithContourWidget *GetNextItem() {
    return static_cast<vtkViewImage2DWithContourWidget*>(
      this->GetNextItemAsObject() );};
  // Description:
  // Get the next vtkViewImage2D2D in the list. Return NULL when at the end of
  // the list.
  vtkViewImage2DWithContourWidget *GetItem(int i) {
    return static_cast<vtkViewImage2DWithContourWidget*>(
      this->GetItemAsObject(i) );};
  // Description:
  // Add an object to the list. Does not prevent duplicate entries.
  void AddItem(vtkViewImage2DWithContourWidget*);

  // Description:
  // Replace the i'th item in the collection with a
  void ReplaceItem(int i, vtkViewImage2DWithContourWidget *);

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
  void RemoveItem(vtkViewImage2DWithContourWidget *);

  // Description:
  // Remove all objects from the list.
  void RemoveAllItems();

  // Description:
  // Initialize the viewers togethers.
  void Initialize(void);

  void InitializeAllObservers(void);

  vtkGetObjectMacro (ExtraRenderWindow, vtkRenderWindow);
  void SetExtraRenderWindow (vtkRenderWindow* win)
  { this->ExtraRenderWindow = win; }

  vtkSyncSetMacro (Slice, int, vtkViewImage2DWithContourWidget );
  vtkSyncSetMacro (SliceOrientation, int, vtkViewImage2DWithContourWidget);
  vtkSyncSetMacro (ShowAnnotations, bool, vtkViewImage2DWithContourWidget);
  vtkSyncSetMacro (ShowScalarBar, bool, vtkViewImage2DWithContourWidget);

  vtkSyncSetMacro (ColorWindow, double, vtkViewImage2DWithContourWidget);
  vtkSyncSetMacro (ColorLevel, double, vtkViewImage2DWithContourWidget);


  vtkSyncSetObjectMacro (OrientationMatrix, vtkMatrix4x4,
    vtkViewImage2DWithContourWidget);
  vtkSyncSetObjectMacro (LookupTable, vtkLookupTable,
    vtkViewImage2DWithContourWidget);
//   vtkSyncSetObjectMacro (TextProperty, vtkTextProperty,
//     vtkViewImage2DWithContourWidget);
  vtkSyncSetObjectMacro (Input, vtkImageData, vtkViewImage2DWithContourWidget);
  vtkSyncSetObjectMacro (InputConnection, vtkAlgorithmOutput,
    vtkViewImage2DWithContourWidget);

  vtkSyncSetObjectMacro (Size, int, vtkViewImage2DWithContourWidget);
  vtkSyncSetObjectMacro (Position, int, vtkViewImage2DWithContourWidget);
  vtkSyncSetObjectMacro (WorldCoordinates, double,
    vtkViewImage2DWithContourWidget);

  void SyncSetTextProperty( vtkTextProperty* p )
  {
    this->InitTraversal();
    vtkViewImage2DWithContourWidget* item = this->GetNextItem();

    while( item )
    {
      item->GetTextProperty( )->ShallowCopy( p );
      item = this->GetNextItem();
    }
  }

  void SyncSetBackground( double* rgb );
  void SyncRender (void);
  void SyncResetWindowLevel (void);
  void SyncReset (void);
  void SyncPan (void);
  void SyncSetZoomAndParallelScale( double Zoom, double ParallelScale );

  template< typename TContourContainer,
            typename TPropertyContainer >
  void SyncAddContours( TContourContainer& iContours,
    TPropertyContainer& iProperty,
    const bool& iIntersection = true )
  {
    this->InitTraversal();
    vtkViewImage2DWithContourWidget* item = this->GetNextItem();

    while( item )
    {
      item->AddContours( iContours, iProperty, iIntersection );
      item->Render();
      item = this->GetNextItem();
    }
  }

  template< typename TPolyDataContainer >
  void SyncRemoveContours( TPolyDataContainer& iContours )
  {
    this->InitTraversal();
    vtkViewImage2DWithContourWidget* item = this->GetNextItem();

    while( item )
    {
      item->RemoveContours( iContours );
      item->Render();
      item = this->GetNextItem();
    }
  }

protected:
  vtkViewImage2DWithContourWidgetCollection();
  ~vtkViewImage2DWithContourWidgetCollection();

  vtkViewImage2DWithContourWidgetCollectionCommand* Command;
  vtkRenderWindow* ExtraRenderWindow;

private:
  vtkViewImage2DWithContourWidgetCollection(
    const vtkViewImage2DWithContourWidgetCollection& );
  void operator = ( const vtkViewImage2DWithContourWidgetCollection& );
};


class vtkViewImage2DWithContourWidgetCollectionCommand
: public vtkCommand
{

 public:

  static vtkViewImage2DWithContourWidgetCollectionCommand *New()
    {return new vtkViewImage2DWithContourWidgetCollectionCommand;};

  vtkViewImage2DWithContourWidgetCollection* GetCollection (void)
  { return this->Collection;};
  void SetCollection( vtkViewImage2DWithContourWidgetCollection* p );

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute(vtkObject *caller,unsigned long event,
    void *vtkNotUsed(callData));

 protected:

  vtkViewImage2DWithContourWidgetCollectionCommand();
  ~vtkViewImage2DWithContourWidgetCollectionCommand(){};

 private:
  vtkViewImage2DWithContourWidgetCollection* Collection;

  double InitialWindow;
  double InitialLevel;

};

#endif
