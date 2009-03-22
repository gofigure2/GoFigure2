/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkLandmarkManagerCallback.h 700 2008-02-22 16:19:25Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-02-22 11:19:25 -0500 (Fri, 22 Feb 2008) $
Version:   $Revision: 700 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_LandmarkManagerCallback_h_
#define _vtk_LandmarkManagerCallback_h_

#include <vtkCommand.h>

class vtkViewImage;
class vtkPolyData;
class vtkLandmarkManager;
class vtkCollection;
class vtkLandmark;
class vtkLandmarkManager;

class vtkLandmarkManagerCallback : public vtkCommand
{    
 public:
  
  static  vtkLandmarkManagerCallback* New() 
    { return new vtkLandmarkManagerCallback; }

  void Execute(vtkObject *   caller, 
               unsigned long event, 
               void *        callData);


  //BTX
  enum EventIds
  {
    LandmarkHasMovedEvent = (vtkCommand::UserEvent+100),
    LandmarkAddedEvent,
    LandmarkPickedEvent,
    LandmarkRemovingEvent,
    LandmarkRemovedEvent
  };
  //ETX
  
  
  void SetViewList (vtkCollection* collection)
  {
    this->ViewList = collection;
  }
  vtkCollection* GetViewList (void)
  {
    return this->ViewList;
  }
  vtkLandmarkManager* GetLandmarkManager (void)
  {
    return this->LandmarkManager;
  }
  void SetLandmarkManager (vtkLandmarkManager* manager)
  {
    this->LandmarkManager = manager;
  }

  vtkLandmark* GetSelectedLandmark (void) const
  { return this->SelectedLandmark; }
  
  
 protected:
  vtkLandmarkManagerCallback()
  {
    this->ViewList = NULL;
    this->SelectedLandmark = NULL;
    this->LandmarkManager = NULL;
    
  };
  ~vtkLandmarkManagerCallback(){};

  
  
 private:

  vtkCollection* ViewList;
  vtkLandmark* SelectedLandmark;
  vtkLandmarkManager* LandmarkManager;
  

};



#endif
