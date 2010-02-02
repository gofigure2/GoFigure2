/*
 * vtkFFMPEGRenderWindowRecorderTest.cxx
 *
 *  Created on: Feb 2, 2010
 *      Author: nicolas
 */

//For the visualization
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtksys/SystemTools.hxx"

#include "vtkFFMPEGRenderWindowRecorder.h"

#include <stdio.h>

int main( int argc, char* argv[] )
{
  if( argc != 2 )
    {
    std::cerr <<"Usage: " <<std::endl;
    std::cerr <<"" <<std::endl;
    return EXIT_FAILURE;
    }

  int err = 0;
  int exists = 0;
  unsigned long length = 0;

  // create sphere geometry
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(1.0);
  sphere->SetThetaResolution(18);
  sphere->SetPhiResolution(18);

  // map to graphics library
  vtkPolyDataMapper *map = vtkPolyDataMapper::New();
  map->SetInput(sphere->GetOutput());

  // actor creation and properties definition
  vtkActor *aSphere = vtkActor::New();
  aSphere->SetMapper(map);
  aSphere->GetProperty()->SetColor(1,1,1);
  aSphere->GetProperty()->SetOpacity(0.5);

  // a renderer and render window
  vtkRenderer *ren1 = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(ren1);

  // an interactor
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  iren->Initialize();

  // add the actor to the scene
  ren1->AddActor(aSphere);
  ren1->SetBackground(1,0,0); // Background color white

  // create the video
  vtkFFMPEGRenderWindowRecorder *testRecorder = vtkFFMPEGRenderWindowRecorder::New();
  testRecorder->SetRenderingWindow(renWin);
  testRecorder->SetFileName( argv[1] );
  testRecorder->StartCapture();

  for (int i = 0; i < 30; i ++)
    {
    renWin->Render();
    testRecorder->TakeSnapshot();
    }

  ren1->SetBackground(0,1,0);
  for (int i = 0; i < 30; i ++)
      {
      renWin->Render();
      testRecorder->TakeSnapshot();
      }

  ren1->SetBackground(0,0,1);
  for (int i = 0; i < 30; i ++)
        {
        renWin->Render();
        testRecorder->TakeSnapshot();
        }

  testRecorder->EndCapture();

  // Test
  exists = (int) vtksys::SystemTools::FileExists(argv[1]);
  length = vtksys::SystemTools::FileLength(argv[1]);
  cout << "TestFFMPEGRecorder file exists: " << exists << endl;
  cout << "TestFFMPEGRecorder file length: " << length << endl;
  if (!exists)
    {
    err = 1;
    cerr << "ERROR: 1 - Test failing because TestFFMPEGRecorder file doesn't exist..." << endl;
    }
  else
    {
    vtksys::SystemTools::RemoveFile(argv[1]);
    }
  if (0==length)
    {
    err = 2;
    cerr << "ERROR: 2 - Test failing because TestFFMPEGRecorder file has zero length..." << endl;
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
