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

#include "vtkViewImage3D.h"

#include "vtkImageAppendComponents.h"
#include "vtkImageExtractComponents.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkInteractorStyleImage.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkScalarBarActor.h"
#include "vtkOrientationAnnotation.h"
#include "vtkCornerAnnotation.h"
#include "vtkTextProperty.h"
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkProp3DCollection.h"
#include "vtkDataSetCollection.h"
#include "vtkPoints.h"
#include "vtkIdList.h"
#include "vtkOutlineSource.h"
#include "vtkMatrixToLinearTransform.h"
#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"
#include "vtkIntArray.h"
#include "vtkImageAccumulate.h"


#include <vtkClipPolyData.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkRendererCollection.h>
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkVolumeMapper.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
#include <vtkVolumeTextureMapper2D.h>
#include <vtkProperty.h>
#include <vtkVolume.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageActor.h>
#include <vtkAxes.h>
#include <vtkMatrix4x4.h>
#include <vtkTubeFilter.h>
#include <vtkLookupTable.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkPropAssembly.h>
#include <vtkAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkCaptionActor2D.h>
#include <vtkPointData.h>
#include <vtkImageBlend.h>
#include <vtkImageReslice.h>
#include "vtkRenderWindow.h"
#include "vtkScalarsToColors.h"
#include "vtkColorTransferFunction.h"
#include <vtkCamera.h>
#include <vtkImageShiftScale.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkGeometryFilter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkCellData.h>

#include <vector>
#include <string>

vtkCxxRevisionMacro(vtkViewImage3D, "$Revision$");
vtkStandardNewMacro(vtkViewImage3D);


//----------------------------------------------------------------------------
class VTK_RENDERINGADDON2_EXPORT ImageActorCallback : public vtkCommand
{
public:

  static ImageActorCallback *New() { return new ImageActorCallback; }

  void Execute(vtkObject *caller,
               unsigned long event,
               void *vtkNotUsed(callData))
  {
    if (!this->Actor)
      return;
    vtkImageActor* imagecaller = vtkImageActor::SafeDownCast (caller);

    if( !imagecaller )
      return;
    if( !imagecaller->GetInput() )
      return;

    if( event == vtkCommand::ModifiedEvent )
    {
      this->Actor->SetInput(imagecaller->GetInput());
      this->Actor->SetInterpolate(imagecaller->GetInterpolate());
      this->Actor->SetOpacity(imagecaller->GetOpacity());
      this->Actor->SetDisplayExtent (imagecaller->GetDisplayExtent());
    }
  }

  vtkImageActor* Actor;
protected:
  ImageActorCallback() : Actor( 0 ) {}
  ~ImageActorCallback() {}

};



//----------------------------------------------------------------------------
vtkViewImage3D::vtkViewImage3D()
{
  this->VolumeProperty  = vtkVolumeProperty::New();
  this->VolumeActor     = vtkVolume::New();
  this->VolumeRayCastMapper = vtkVolumeRayCastMapper::New();
  this->VolumeRayCastMIPFunction = vtkVolumeRayCastMIPFunction::New();
  this->VolumeRayCastCompositeFunction =
    vtkVolumeRayCastCompositeFunction::New();
  this->VolumeRayCastIsosurfaceFunction =
    vtkVolumeRayCastIsosurfaceFunction::New();
  this->OpacityFunction = vtkPiecewiseFunction::New();
  this->BoxWidget = vtkOrientedBoxWidget::New();
  this->Callback = vtkImage3DCroppingBoxCallback::New();
  this->Blender = vtkImageBlend::New();
  //this->PlaneWidget = vtkPlaneWidget::New();
  this->VolumeMapper3D = vtkVolumeTextureMapper3D::New();

  this->Phantom.push_back( vtkImageActor::New() );
  this->Phantom.push_back( vtkImageActor::New() );
  this->Phantom.push_back( vtkImageActor::New() );

  this->PhantomCallback.push_back( ImageActorCallback::New() );
  this->PhantomCallback.push_back( ImageActorCallback::New() );
  this->PhantomCallback.push_back( ImageActorCallback::New() );

  this->BoundsActor.push_back( vtkActor::New() );
  this->BoundsActor.push_back( vtkActor::New() );
  this->BoundsActor.push_back( vtkActor::New() );

//   this->SetupVolumeRendering();
  this->SetupWidgets();
}

//----------------------------------------------------------------------------
vtkViewImage3D::~vtkViewImage3D()
{
  // delete all vtk objetcts:
  this->VolumeMapper3D->Delete();
  this->VolumeProperty->Delete();
  this->VolumeActor->Delete();
  this->OpacityFunction->Delete();
  this->BoxWidget->Delete();
  this->Callback->Delete();
  this->Cube->Delete();
  this->Marker->Delete();
  this->Blender->Delete();
  //this->PlaneWidget->Delete();
  this->VolumeRayCastMapper->Delete();
  this->VolumeRayCastMIPFunction->Delete();
  this->VolumeRayCastCompositeFunction->Delete();
  this->VolumeRayCastIsosurfaceFunction->Delete();
  this->Phantom[0]->Delete();
  this->Phantom[1]->Delete();
  this->Phantom[2]->Delete();
  this->PhantomCallback[0]->Delete();
  this->PhantomCallback[1]->Delete();
  this->PhantomCallback[2]->Delete();
  this->BoundsActor[0]->Delete();
  this->BoundsActor[1]->Delete();
  this->BoundsActor[2]->Delete();
}


//----------------------------------------------------------------------------
void vtkViewImage3D::SetupVolumeRendering()
{
  this->Blender->SetBlendModeToNormal();
  this->Blender->SetOpacity (0, 0.25);
  this->Blender->SetOpacity (1, 0.75);
  vtkVolumeTextureMapper3D* texturemapper =
    vtkVolumeTextureMapper3D::SafeDownCast ( this->VolumeActor->GetMapper() );
  if (texturemapper)
  {
    texturemapper->SetSampleDistance(0.5);
    //texturemapper->SetPreferredMethodToNVidia();
  }

  this->VolumeMapper3D->CroppingOn();
  this->VolumeMapper3D->SetCroppingRegionFlagsToSubVolume();
  this->VolumeMapper3D->SetCroppingRegionFlags (0x7ffdfff);
  this->VolumeRayCastMapper->SetSampleDistance(2.0);
  this->VolumeRayCastMapper->CroppingOn();
  this->VolumeRayCastMapper->SetCroppingRegionFlags (0x7ffdfff);
  vtkFiniteDifferenceGradientEstimator *gradest =
    vtkFiniteDifferenceGradientEstimator::New();
  this->VolumeRayCastMapper->SetGradientEstimator(gradest);
  gradest->Delete();
  this->VolumeRayCastMapper->SetVolumeRayCastFunction (
    this->VolumeRayCastCompositeFunction );
  this->OpacityFunction->AddPoint (0, 0.0);
  this->OpacityFunction->AddPoint (255, 1.0);
  vtkColorTransferFunction* colorfunction = vtkColorTransferFunction::New();
  colorfunction->AddRGBPoint (0, 0.0, 0.0, 0.0);
  colorfunction->AddRGBPoint (255, 1.0, 1.0, 1.0);
  this->VolumeProperty->IndependentComponentsOff();
  this->VolumeProperty->SetColor (colorfunction);
  colorfunction->Delete();
  this->VolumeProperty->SetScalarOpacity(this->OpacityFunction);
  this->VolumeProperty->SetInterpolationTypeToLinear();
  this->VolumeProperty->ShadeOff();
  this->VolumeProperty->SetDiffuse (0.9);
  this->VolumeProperty->SetAmbient (0.2);
  this->VolumeProperty->SetSpecular (0.3);
  this->VolumeProperty->SetSpecularPower (15.0);
  // set up the vtk pipeline: volume rendering
  this->VolumeActor->SetProperty ( this->VolumeProperty );
  this->VolumeActor->SetMapper ( this->VolumeMapper3D );
  this->VolumeActor->PickableOff();
  this->VolumeActor->DragableOff();
  this->VolumeActor->SetVisibility (0);

  // set up the boxwidget/ callback
  this->Callback->SetVolumeMapper ( this->VolumeMapper3D );

  this->Renderer->AddViewProp (this->VolumeActor);

}


//----------------------------------------------------------------------------
void vtkViewImage3D::SetupWidgets()
{

  // Create an annotated cube actor (directions)
  this->Cube = vtkAnnotatedCubeActor::New();
  this->Cube->SetXPlusFaceText ("L");
  this->Cube->SetXMinusFaceText ("R");
  this->Cube->SetYPlusFaceText ("P");
  this->Cube->SetYMinusFaceText ("A");
  this->Cube->SetZPlusFaceText ("D");
  this->Cube->SetZMinusFaceText ("V");
  this->Cube->SetZFaceTextRotation (90);
  this->Cube->SetFaceTextScale (0.65);
  this->Cube->GetCubeProperty()->SetColor (0.5, 1, 1);
  this->Cube->GetTextEdgesProperty()->SetLineWidth (1);
  this->Cube->GetTextEdgesProperty()->SetDiffuse (0);
  this->Cube->GetTextEdgesProperty()->SetAmbient (1);
  this->Cube->GetTextEdgesProperty()->SetColor (0.18, 0.28, 0.23);

#if VTK_MAJOR_VERSION==5 && VTK_MINOR_VERSION>=1
  this->Cube->SetTextEdgesVisibility (1);
  this->Cube->SetCubeVisibility(1);
  this->Cube->SetFaceTextVisibility(1);
#else
  this->Cube->TextEdgesOn ();
  this->Cube->FaceTextOn();
  this->Cube->CubeOn();
#endif

  this->Cube->GetXPlusFaceProperty()->SetColor (1, 0, 0);
  this->Cube->GetXPlusFaceProperty()->SetInterpolationToFlat();
  this->Cube->GetXMinusFaceProperty()->SetColor (1, 0, 0);
  this->Cube->GetXMinusFaceProperty()->SetInterpolationToFlat();
  this->Cube->GetYPlusFaceProperty()->SetColor (0, 1, 0);
  this->Cube->GetYPlusFaceProperty()->SetInterpolationToFlat();
  this->Cube->GetYMinusFaceProperty()->SetColor (0, 1, 0);
  this->Cube->GetYMinusFaceProperty()->SetInterpolationToFlat();
  this->Cube->GetZPlusFaceProperty()->SetColor (0, 0, 1);
  this->Cube->GetZPlusFaceProperty()->SetInterpolationToFlat();
  this->Cube->GetZMinusFaceProperty()->SetColor (0, 0, 1);
  this->Cube->GetZMinusFaceProperty()->SetInterpolationToFlat();

  this->Marker = vtkOrientationMarkerWidget::New();
  this->Marker->SetOutlineColor (0.93, 0.57, 0.13);
  this->Marker->SetOrientationMarker (this->Cube);
  this->Marker->SetViewport (0.0, 0.05, 0.15, 0.15);

  this->BoxWidget->RotationEnabledOff();
  this->BoxWidget->SetPlaceFactor (0.5);
  this->BoxWidget->SetKeyPressActivationValue ('b');
  this->BoxWidget->AddObserver (vtkCommand::InteractionEvent, this->Callback);

  //this->PlaneWidget->SetKeyPressActivationValue ('p');
  //this->PlaneWidget->NormalToZAxisOn();

}

//----------------------------------------------------------------------------
void vtkViewImage3D::Render()
{

  if (this->FirstRender)
  {
    // Initialize the size if not set yet

    vtkImageData *input = this->GetInput();
    if (this->RenderWindow->GetSize()[0] == 0 && input)
    {
      if (this->Renderer)
      {
        this->Renderer->ResetCamera();
      }
      this->FirstRender = 0;
    }
  }
  if (this->GetInput())
  {
    this->RenderWindow->Render();
  }
}

void vtkViewImage3D::SetVolumeRenderingOff()
{
  this->VolumeActor->SetVisibility (false);
  this->BoxWidget->Off();
}

void vtkViewImage3D::SetVolumeRenderingOn()
{
  if( 1 ) //  !this->IsColor )
  {
    vtkImageData* image = this->GetInput();

    int* size = image->GetDimensions();

    if ( (size[0] < 2) ||
        (size[1] < 2) ||
        (size[2] < 2) )
    {
      vtkWarningMacro ( <<"Cannot do volume rendering for a single slice, skipping"<<endl);
      return;
    }

    this->SetupVolumeRendering();
    
    int NbOfComp = image->GetNumberOfScalarComponents();
    if( NbOfComp > 1 && NbOfComp != 4 && NbOfComp < 5 )
      {
      vtkImageExtractComponents* extComp = vtkImageExtractComponents::New();
      extComp->SetInput( image );
      extComp->SetComponents( 0 );
      extComp->Update();

      vtkImageAppendComponents*  addComp = vtkImageAppendComponents::New();
      addComp->AddInput( image );
      addComp->AddInput( extComp->GetOutput() );
      addComp->Update();
     
      if( addComp->GetOutput()->GetNumberOfScalarComponents() == 4 )
        {
        this->VolumeMapper3D->SetInput( addComp->GetOutput() );
        }
      else
        {
        vtkImageData* temp = vtkImageData::New();
        temp->ShallowCopy( addComp->GetOutput() );
        addComp->SetInput( 0, temp );
        addComp->Update();
        this->VolumeMapper3D->SetInput( addComp->GetOutput() );
        }        
      }
    else
      {
      if( NbOfComp == 1 || NbOfComp == 4 ) 
        {
        this->VolumeMapper3D->SetInput( image );
        }
      }
    if( !this->IsColor ) this->VolumeRayCastMapper->SetInput( image );

    this->SetupTextureMapper();

    this->BoxWidget->SetInput( image );
    this->BoxWidget->PlaceWidget();

    //this->PlaneWidget->SetInput (this->GetInput());
    //this->PlaneWidget->PlaceWidget();

    // line to be removed: the box has to be called externally
    // this->BoxWidget->On();

    this->VolumeActor->SetVisibility ( true );
  }
}

void vtkViewImage3D::SetTriPlanarRenderingOn()
{
  this->VolumeActor->SetVisibility(false);
  for( int i = 0; i < 3; i++ )
  {
    this->Phantom[i]->SetVisibility(true);
    this->BoundsActor[i]->SetVisibility(true);
  }
}

void vtkViewImage3D::SetTriPlanarRenderingOff()
{
  this->VolumeActor->SetVisibility(true);
  for( int i = 0; i < 3; i++ )
  {
    this->Phantom[i]->SetVisibility(false);
    this->BoundsActor[i]->SetVisibility(false);
  }
}

void vtkViewImage3D::Add2DPhantom(const unsigned int& i,
  vtkImageActor* input,
  vtkPolyData* in_bounds )
{
  if( i >= 3 )
    return;

  vtkRenderer* ren = this->GetRenderer();
  if( ren )
  {
    this->Phantom[i]->SetInput (input->GetInput());
    this->Phantom[i]->SetDisplayExtent (input->GetDisplayExtent());
    this->Phantom[i]->SetUserMatrix (input->GetUserMatrix());
    this->PhantomCallback[i]->Actor = this->Phantom[i];
    input->AddObserver (vtkCommand::ModifiedEvent, this->PhantomCallback[i]);
    ren->AddActor (this->Phantom[i]);

    /**
       IMPORTANT NOTE

       Adding a 2D actor in the 3D scene should be as simple as the next line
       instead of the code above...

       Unfortunately it does not seem to work properly. But this is something
       we should investigate in because it would be much simpler
    */
    //     this->GetRenderer()->AddActor (input);

    if( in_bounds )
    {
      vtkPolyDataMapper* bounds_mapper = vtkPolyDataMapper::New();
      bounds_mapper->SetInput( in_bounds );

      this->BoundsActor[i]->SetMapper( bounds_mapper );
      this->BoundsActor[i]->GetProperty()->SetRepresentationToWireframe();
      this->BoundsActor[i]->GetProperty()->SetLineWidth( 2. );

      ren->AddActor( this->BoundsActor[i] );
      bounds_mapper->Delete();
    }

  }

}

//----------------------------------------------------------------------------
void vtkViewImage3D::InstallPipeline()
{
  if (this->RenderWindow && this->Renderer)
  {
    this->RenderWindow->AddRenderer(this->Renderer);
  }

  if (this->Interactor)
  {

    // init the interactor style:
    vtkInteractorStyleSwitch* interactorStyle = vtkInteractorStyleSwitch::New();
    interactorStyle->SetCurrentStyleToTrackballCamera();
    this->Interactor->SetInteractorStyle (interactorStyle);
    this->BoxWidget->SetInteractor ( this->Interactor );
    //this->PlaneWidget->SetInteractor ( this->Interactor );
    this->Marker->SetInteractor ( this->Interactor );
    interactorStyle->Delete();
    this->Interactor->SetRenderWindow(this->RenderWindow);

    this->Marker->On();
    this->Marker->InteractiveOff ();

  }

}

void vtkViewImage3D::AddDataSet( vtkDataSet* dataset,
  vtkProperty* property,
  const bool& intersection )
{
  vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
  if( !cam )
    return;

  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  vtkActor* actor = vtkActor::New();

  vtkClipPolyData* cutter = vtkClipPolyData::New();

  if( intersection )
  {
    //TODO to be implemented
//     cutter->SetInputConnection( 0, dataset->GetProducerPort());
//     cutter->SetClipFunction( this->SliceImplicitPlane );
//     cutter->InsideOutOn();
//     mapper->SetInputConnection( 0, cutter->GetOutputPort());
  }
  else
  {
    mapper->SetInput( vtkPolyData::SafeDownCast( dataset ) );
  }

  actor->SetMapper( mapper );
  if( property )
    actor->SetProperty( property );

//   actor->SetUserTransform( this->AdjustmentTransform );

  this->Renderer->AddViewProp( actor );
  this->DataSetCollection->AddItem( dataset );
  this->Prop3DCollection->AddItem( actor );

  cutter->Delete();
  mapper->Delete();
  actor->Delete();
}

//----------------------------------------------------------------------------
void vtkViewImage3D::SetOrientationMatrix (vtkMatrix4x4* matrix)
{
  this->Superclass::SetOrientationMatrix (matrix);

  this->VolumeActor->SetUserMatrix (matrix);
  this->BoxWidget->SetOrientationMatrix (matrix);
//   this->PlaneWidget->SetTransform (transform);
}



//----------------------------------------------------------------------------
void vtkViewImage3D::SetupTextureMapper()
{

  if (!this->GetInput())
    return;

  vtkVolumeTextureMapper3D* mapper3D =
    vtkVolumeTextureMapper3D::SafeDownCast( this->VolumeActor->GetMapper() );

  if( mapper3D && !this->GetRenderWindow()->GetNeverRendered() )
  {
    if( !mapper3D->IsRenderSupported ( this->VolumeProperty ) )
    {
      //try the ATI fragment program implementation
      // mapper3D->SetPreferredMethodToFragmentProgram();
      if ( !mapper3D->IsRenderSupported ( this->VolumeProperty ) )
      {
        vtkWarningMacro (
        <<"Warning: 3D Texture volume rendering is not supported by your"
        <<" hardware, I switch to 2D Texture rendering."<<endl);

        vtkVolumeTextureMapper2D* newMapper = vtkVolumeTextureMapper2D::New();
              newMapper->CroppingOn();
              newMapper->SetCroppingRegionFlags (0x7ffdfff);


        double* range = this->GetInput()->GetScalarRange();
        double shift = 0 - range[0];
        double scale = 65535.0/(range[1] - range[0]);

        vtkImageShiftScale* scaler = vtkImageShiftScale::New();
        scaler->SetInput (this->GetInput());
        scaler->SetShift (shift);
        scaler->SetScale (scale);
        scaler->SetOutputScalarTypeToUnsignedShort();
        scaler->Update();
        newMapper->SetInput ( scaler->GetOutput() );
        scaler->Delete();
        this->Callback->SetVolumeMapper (newMapper);

        mapper3D->Delete();
        this->VolumeMapper3D = newMapper;
        this->VolumeActor->SetMapper ( this->VolumeMapper3D );
      }
    }
  }
}
