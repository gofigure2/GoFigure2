/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 1392 $  // Revision of last commit
  Date: $Date: 2010-05-03 14:13:52 -0400 (Mon, 03 May 2010) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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
#include "vtkMetaImageReader.h"
#include "vtkImageActor.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkViewImage2D.h"
#include "vtkViewImage3D.h"
#include "vtkViewImage2DCollection.h"

#include "vtkConstrainedPointHandleRepresentation.h"
#include "vtkSeedWidget.h"
#include "vtkSeedRepresentation.h"
#include "vtkProperty.h"

int main(int argc, char **argv)
{
  if ( argc != 3 )
    {
    std::cout << "Usage: imageview3d(.exe)" << std::endl;
    std::cout << "1- filename (.mha or .mhd)" << std::endl;
    std::cout << "2- Test (boolean)" << std::endl;
    return EXIT_FAILURE;
    }

  vtkMetaImageReader *reader = vtkMetaImageReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkImageData *image = reader->GetOutput();

  vtkViewImage2DCollection *pool = vtkViewImage2DCollection::New();

  vtkViewImage3D *           view3d = vtkViewImage3D::New();
  vtkRenderWindowInteractor *iren3d = vtkRenderWindowInteractor::New();
  view3d->SetupInteractor(iren3d);

  vtkViewImage2D *           view = vtkViewImage2D::New();
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  view->SetupInteractor(iren);
  view->SetInput (image);
  view->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_AXIAL);

  view3d->Add2DPhantom( 0, view->GetImageActor(), view->GetSlicePlane() );
  pool->AddItem(view);

  vtkViewImage2D *           view2 = vtkViewImage2D::New();
  vtkRenderWindowInteractor *iren2 = vtkRenderWindowInteractor::New();
  view2->SetupInteractor(iren2);
  view2->SetInput (image);
  view2->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_CORONAL);
  view3d->Add2DPhantom( 1, view2->GetImageActor(), view2->GetSlicePlane() );
  pool->AddItem(view2);

  vtkViewImage2D *           view3 = vtkViewImage2D::New();
  vtkRenderWindowInteractor *iren3 = vtkRenderWindowInteractor::New();
  view3->SetupInteractor(iren3);
  view3->SetInput (image);
  view3->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL);
  view3d->Add2DPhantom( 2, view3->GetImageActor(), view3->GetSlicePlane() );
  pool->AddItem(view3);

  view3d->SetTriPlanarRenderingOn();
  view3d->SetInput(image);

  pool->SetExtraRenderWindow( view3d->GetRenderWindow() );
  pool->InitializeAllObservers();
  pool->Initialize();
  pool->SyncSetBackground( pool->GetItem(0)->GetBackground() );
  pool->SyncSetShowAnnotations(true);

  int size[2] = { 400, 400 };
  pool->SyncSetSize(size);
  pool->SyncPan();
  pool->SyncRender();
  pool->SyncReset();

  std::vector< vtkSeedWidget * >                           SeedWidget;
  std::vector< vtkConstrainedPointHandleRepresentation * > Handle;
  std::vector< vtkSeedRepresentation * >                   SeedRep;

  // Enable seed interaction
  Handle.resize( pool->GetNumberOfItems() );
  SeedRep.resize( pool->GetNumberOfItems() );
  SeedWidget.resize( pool->GetNumberOfItems() );

  for ( int i = 0; i < pool->GetNumberOfItems(); i++ )
    {
    Handle[i] = vtkConstrainedPointHandleRepresentation::New();
    Handle[i]->GetProperty()->SetColor(1, 0, 0);

    SeedRep[i] = vtkSeedRepresentation::New();
    SeedRep[i]->SetHandleRepresentation(Handle[i]);

    SeedWidget[i] = vtkSeedWidget::New();
    SeedWidget[i]->SetPriority(10.0);
    SeedWidget[i]->SetRepresentation(SeedRep[i]);
    }

  Handle[0]->SetProjectionNormal(vtkViewImage2D::VIEW_ORIENTATION_AXIAL);
  Handle[1]->SetProjectionNormal(vtkViewImage2D::VIEW_ORIENTATION_CORONAL);
  Handle[2]->SetProjectionNormal(vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL);

  SeedWidget[0]->SetInteractor(iren);
  SeedWidget[1]->SetInteractor(iren2);
  SeedWidget[2]->SetInteractor(iren3);

  for ( int i = 0; i < pool->GetNumberOfItems(); i++ )
    {
    SeedWidget[i]->On();
    }

  if ( atoi(argv[2]) == 1 )
    {
    iren3d->CreateOneShotTimer(1);
    }
  else
    {
    iren3d->Start();
    }

  for ( int i = 0; i < pool->GetNumberOfItems(); i++ )
    {
    Handle[i]->Delete();
    SeedRep[i]->Delete();
    SeedWidget[i]->Delete();
    }

  iren->Delete();
  view->Delete();

  iren2->Delete();
  view2->Delete();

  iren3->Delete();
  view3->Delete();

  pool->Delete();

  iren3d->Delete();
  view3d->Delete();

  reader->Delete();

  return EXIT_SUCCESS;
}
