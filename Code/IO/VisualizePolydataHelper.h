/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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
#ifndef __VisualizePolydataHelper_h
#define __VisualizePolydataHelper_h

#include "vtkSmartPointer.h"

#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkImageViewer2.h"

#include "vtkExtractVOI.h"

void ShowPolyData(vtkPolyData *iPolyData)
{
  vtkSmartPointer< vtkPolyDataMapper > mapper =
    vtkSmartPointer< vtkPolyDataMapper >::New();
  mapper->SetInput(iPolyData);

  vtkSmartPointer< vtkActor > actor =
    vtkSmartPointer< vtkActor >::New();
  actor->SetMapper(mapper);

  vtkSmartPointer< vtkRenderer > renderer =
    vtkSmartPointer< vtkRenderer >::New();
  renderer->AddActor(actor);

  vtkSmartPointer< vtkRenderWindow > renderWindow =
    vtkSmartPointer< vtkRenderWindow >::New();
  renderWindow->AddRenderer(renderer);

  vtkSmartPointer< vtkRenderWindowInteractor > renderWindowInteractor =
    vtkSmartPointer< vtkRenderWindowInteractor >::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindowInteractor->Initialize();
  renderWindow->Render();
  renderWindowInteractor->Start();
}

void ShowImage(vtkImageData *iData)
{
  // Create the rendering window
  vtkSmartPointer< vtkRenderer >               ren1 = vtkSmartPointer< vtkRenderer >::New();
  vtkSmartPointer< vtkRenderWindow >           renWin = vtkSmartPointer< vtkRenderWindow >::New();
  vtkSmartPointer< vtkRenderWindowInteractor > iren = vtkSmartPointer< vtkRenderWindowInteractor >::New();

  // Create the rendering window
  renWin->AddRenderer(ren1);
  iren->SetRenderWindow(renWin);

  vtkSmartPointer< vtkImageViewer2 > viewer = vtkSmartPointer< vtkImageViewer2 >::New();

  /*vtkSmartPointer<vtkExtractVOI> voi = vtkSmartPointer<vtkExtractVOI>::New();
  voi->SetInput(iData);
  voi->SetVOI(25, 75, 25, 75, 25, 75);
  voi->Update();*/


  viewer->SetInput(iData);
  viewer->SetRenderWindow(renWin);
  viewer->SetRenderer(ren1);

  iren->Initialize();
  viewer->Render();
  iren->Start();
}

#endif
