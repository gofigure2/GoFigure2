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
#include "vtkPNGReader.h"
#include "vtkImageData.h"
#include "vtkViewImage2D.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage2D.h"

int main(int argc, char **argv)
{
  if ( argc != 3 )
    {
    std::cout << "Usage: imageview2d(.exe)" << std::endl;
    std::cout << "1- file.png" << std::endl;
    std::cout << "2- Test (boolean)" << std::endl;
    return EXIT_FAILURE;
    }

  vtkPNGReader *reader = vtkPNGReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkViewImage2D *view = vtkViewImage2D::New();

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  view->SetupInteractor(iren);

  vtkInteractorStyleImage2D *interactorStyle = vtkInteractorStyleImage2D::New();
  iren->SetInteractorStyle(interactorStyle);
  // Studpid tests with interactor style
  interactorStyle->OnMouseMove();
  interactorStyle->OnLeftButtonDown();
  interactorStyle->OnLeftButtonUp();
  interactorStyle->OnMiddleButtonDown();
  interactorStyle->OnMiddleButtonUp();
  interactorStyle->OnRightButtonDown();
  interactorStyle->OnRightButtonUp();
  interactorStyle->OnMouseWheelForward();
  interactorStyle->OnMouseWheelBackward();
  //interactorStyle->OnChar();
  //interactorStyle->OnKeyUp();
  interactorStyle->SliceMove();
  interactorStyle->OnKeyPress();
  interactorStyle->OnKeyRelease();
  interactorStyle->StartSliceMove();
  interactorStyle->EndSliceMove();
  interactorStyle->SliceMove();
  interactorStyle->GetCurrentProp();
  // Generates error on Mac
  //interactorStyle->HighlightCurrentActor();

  view->SetInput( reader->GetOutput() );
  view->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_AXIAL);
  view->Render();

  if ( atoi(argv[2]) == 1 )
    {
    iren->CreateOneShotTimer(1);
    }
  else
    {
    iren->Start();
    }

  interactorStyle->Delete();
  view->Delete();
  iren->Delete();
  reader->Delete();

  return EXIT_SUCCESS;
}