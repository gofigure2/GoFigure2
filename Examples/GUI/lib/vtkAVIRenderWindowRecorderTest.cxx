/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

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

//For the visualization
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtksys/SystemTools.hxx"

#include "vtkAVIRenderWindowRecorder.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
  if ( argc != 2 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << "" << std::endl;
    return EXIT_FAILURE;
    }

  int           err = 0;
  int           exists = 0;
  unsigned long length = 0;

  // create sphere geometry
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(1.0);
  sphere->SetThetaResolution(18);
  sphere->SetPhiResolution(18);

  // map to graphics library
  vtkPolyDataMapper *map = vtkPolyDataMapper::New();
  map->SetInput( sphere->GetOutput() );

  // actor creation and properties definition
  vtkActor *aSphere = vtkActor::New();
  aSphere->SetMapper(map);
  aSphere->GetProperty()->SetColor(1, 1, 1);
  aSphere->GetProperty()->SetOpacity(0.5);

  // a renderer and render window
  vtkRenderer *    ren1 = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(ren1);

  // an interactor
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  iren->Initialize();

  // add the actor to the scene
  ren1->AddActor(aSphere);
  ren1->SetBackground(1, 0, 0); // Background color white

  // create the video
  vtkAVIRenderWindowRecorder *testRecorder = vtkAVIRenderWindowRecorder::New();
  testRecorder->SetRenderingWindow(renWin);
  testRecorder->SetFileName(argv[1]);
  testRecorder->SetFrameRate(10);
  testRecorder->SetVideoQuality(1);
  testRecorder->SetSpecificParameters();
  testRecorder->StartCapture();

  for ( int i = 0; i < 30; i++ )
    {
    renWin->Render();
    testRecorder->TakeSnapshot();
    }

  ren1->SetBackground(0, 1, 0);
  for ( int i = 0; i < 30; i++ )
    {
    renWin->Render();
    testRecorder->TakeSnapshot();
    }

  ren1->SetBackground(0, 0, 1);
  for ( int i = 0; i < 30; i++ )
    {
    renWin->Render();
    testRecorder->TakeSnapshot();
    }

  testRecorder->EndCapture();

  std::cout << "End capture" << std::endl;

  // Test
  exists = (int)vtksys::SystemTools::FileExists(argv[1]);
  length = vtksys::SystemTools::FileLength(argv[1]);
  cout << "TestAVIRecorder file exists: " << exists << endl;
  cout << "TestAVIRecorder file length: " << length << endl;
  if ( !exists )
    {
    err = 1;
    cerr << "ERROR: 1 - Test failing because TestAVIRecorder file doesn't exist..." << endl;
    }
  else
    {
    vtksys::SystemTools::RemoveFile(argv[1]);
    }
  if ( 0 == length )
    {
    err = 2;
    cerr << "ERROR: 2 - Test failing because TestAVIRecorder file has zero length..." << endl;
    }

  // Delete everything

  sphere->Delete();
  map->Delete();
  aSphere->Delete();
  ren1->Delete();
  renWin->Delete();
  iren->Delete();

  testRecorder->Delete();

  // err = 0 means that the test succeeded
  return err;
}