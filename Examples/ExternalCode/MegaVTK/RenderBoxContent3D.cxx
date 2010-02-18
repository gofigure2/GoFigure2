#include "vtkLSMReader.h"
#include "vtkCamera.h"
#include "vtkFiniteDifferenceGradientEstimator.h"
#include "vtkPiecewiseFunction.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkStructuredPoints.h"
#include "vtkStructuredPointsReader.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeRayCastIsosurfaceFunction.h"
#include "vtkVolumeRayCastMIPFunction.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeTextureMapper2D.h"
#include "vtkColorTransferFunction.h"
#include "vtkLight.h"
#include <vtkLightKit.h>
#include "vtkOrientedBoxWidget.h"
#include "vtkCommand.h"

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"
#include "vtkDebugLeaks.h"


int main( int argc, char *argv[] )
{
  if ( argc < 2 )
    {
    std::cout << "Usage: ";
    std::cout << "(exe) FileName InputImage";
    std::cout << std::endl;
    return EXIT_FAILURE;
    }

  //read the LSM file - can be multiple channels and time-points:
  vtkLSMReader* reader1 = vtkLSMReader::New();
  reader1->SetFileName( argv[1]);
  reader1->SetUpdateTimePoint( 0 );
  reader1->SetUpdateChannel( 0 );
  reader1->Update();

  //read the LSM file - can be multiple channels and time-points:
  vtkLSMReader* reader2 = vtkLSMReader::New();
  reader2->SetFileName( argv[1]);
  reader2->SetUpdateTimePoint( 0 );
  reader2->SetUpdateChannel( 1 );
  reader2->Update();

  // Get the number of time-points
  int NumberOfTimePoints = reader1->GetNumberOfTimePoints();
  std::cout << NumberOfTimePoints << std::endl;

  // Get the number of channels
  int NumberOfChannels = reader1->GetNumberOfChannels();
  std::cout << NumberOfChannels << std::endl;

  // Create the renderers, render window, and interactor
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  vtkRenderer *ren = vtkRenderer::New();
  renWin->AddRenderer(ren);

  // Create a transfer function mapping scalar value to opacity
  vtkPiecewiseFunction *oTFunMembrane = vtkPiecewiseFunction::New();
  oTFunMembrane->AddPoint( 0, 0.0 );
  oTFunMembrane->AddPoint( 128, 0.6 );

  // Create a transfer function mapping scalar value to opacity
  vtkPiecewiseFunction *oTFunNuclei = vtkPiecewiseFunction::New();
  oTFunNuclei->AddPoint( 0, 0.0 );
  oTFunNuclei->AddPoint( 128, 0.5);

  // Create a transfer function mapping scalar value to color (color)
  vtkColorTransferFunction *cTFunMembrane = vtkColorTransferFunction::New();
  for( unsigned int i = 0; i < 256; i++ )
  cTFunMembrane->AddRGBPoint(   0, 0.0, 0.0, 0.0 );
  cTFunMembrane->AddRGBPoint(   64, 0.65, 0.0, 0.0 );
  cTFunMembrane->AddRGBPoint(   128, 1.0, 0.0, 0.0 );
  cTFunMembrane->AddRGBPoint(   256, 1.0, 0.0, 0.0 );

  vtkColorTransferFunction *cTFunNuclei = vtkColorTransferFunction::New();
  for( unsigned int i = 0; i < 256; i++ )
  cTFunNuclei->AddRGBPoint(   0, 0.0, 0.0, 0.0 );
  cTFunNuclei->AddRGBPoint(   64, 0.0, 0.50, 0.0 );
  cTFunNuclei->AddRGBPoint(   128, 0.0, 0.7, 0.0 );
  cTFunNuclei->AddRGBPoint(   256, 0.0, 1.0, 0.0 );

  // Create a set of properties with varying options
  vtkVolumeProperty *prop1 = vtkVolumeProperty::New();
  prop1->SetShade( 0 );
  prop1->SetAmbient( 1.0 );
  prop1->SetDiffuse( 1.0 );
  prop1->SetSpecular( 1.0 );
  prop1->SetSpecularPower( 10.0 );
  prop1->SetScalarOpacity( oTFunMembrane );
  prop1->SetColor( cTFunMembrane );
  prop1->SetInterpolationTypeToLinear();

  // Create a set of properties with varying options
  vtkVolumeProperty *prop2 = vtkVolumeProperty::New();
  prop2->SetShade( 1 );
  prop2->SetAmbient( 1.0 );
  prop2->SetDiffuse( 1.0 );
  prop2->SetSpecular( 1.0 );
  prop2->SetSpecularPower( 30.0 );
  prop2->SetScalarOpacity(oTFunNuclei);
  prop2->SetColor( cTFunNuclei );
  prop2->SetInterpolationTypeToLinear();

  // Create compositing ray functions
  vtkVolumeRayCastCompositeFunction *compositeFunction =
    vtkVolumeRayCastCompositeFunction::New();
  compositeFunction->SetCompositeMethodToInterpolateFirst();

  vtkVolume *volume1 = vtkVolume::New();
  ren->AddViewProp( volume1 );

  vtkVolumeRayCastMapper *raycastMapper1 = vtkVolumeRayCastMapper::New();
  raycastMapper1 = vtkVolumeRayCastMapper::New();
  raycastMapper1->SetInputConnection( reader2->GetOutputPort() );

  vtkVolume *volume2 = vtkVolume::New();
  ren->AddViewProp( volume2 );

  vtkVolumeRayCastMapper *raycastMapper2 = vtkVolumeRayCastMapper::New();
  raycastMapper2 = vtkVolumeRayCastMapper::New();
  raycastMapper2->SetInputConnection( reader1->GetOutputPort() );

  volume1->SetMapper( raycastMapper1 );
  volume1->SetProperty( prop1 );
  raycastMapper1->SetVolumeRayCastFunction( compositeFunction );

  volume2->SetMapper( raycastMapper2 );
  volume2->SetProperty( prop2 );
  raycastMapper2->SetVolumeRayCastFunction( compositeFunction );

  vtkLight *light=vtkLight::New();
  light->SetPosition( 0, 0, 0 );
  light->SetFocalPoint( 1.0, 0.0, 0.0 );
  light->SetColor( 1.0, 1.0, 1.0 );
  light->SetPositional( 0 );
  ren->AddLight(light);

  // lighting the sphere
  vtkLight *l2=vtkLight::New();
  l2->SetPosition( 0, 0, 0 );
  l2->SetFocalPoint( 100, 100, 25 );
  l2->SetColor( 1.0, 1.0, 1.0 );
  l2->SetPositional( 1 );
  ren->AddLight( l2 );

  vtkLightKit *lkit = vtkLightKit::New();
  lkit->SetFillLightWarmth( 1 );
  lkit->SetKeyLightWarmth( 1 );
  lkit->SetHeadLightWarmth( 1 );
  lkit->SetBackLightWarmth( 1 );
  lkit->SetKeyLightElevation( 45 );
  lkit->SetKeyLightAzimuth( -20 );
  lkit->SetFillLightElevation( 90 );
  lkit->SetFillLightAzimuth( 0 );
  lkit->SetKeyLightIntensity( 1 );
  lkit->SetKeyToFillRatio( 1.0 );
  lkit->SetKeyToHeadRatio( 1.0 );
  lkit->AddLightsToRenderer( ren );

  vtkOrientedBoxWidget *box1 = vtkOrientedBoxWidget::New();
  box1->SetInteractor( iren );
  box1->RotationEnabledOff();
  box1->SetPlaceFactor (1.0);
  box1->SetKeyPressActivationValue ('b');
  box1->Off();
  box1->InsideOutOff();
  box1->SetInput( reader1->GetOutput() );
  box1->PlaceWidget();

  ren->SetBackground(1,1,1);
  ren->LightFollowCameraOn();

  renWin->SetSize( 500, 500 );
  ren->ResetCamera();
//   ren->GetActiveCamera()->Zoom( 1.5 );
  renWin->Render();

  iren->Start();

  // Clean up
  box1->Delete();
  light->Delete();
  l2->Delete();
  prop1->Delete();
  prop2->Delete();
  oTFunMembrane->Delete();
  oTFunNuclei->Delete();
  cTFunMembrane->Delete();
  cTFunNuclei->Delete();
  compositeFunction->Delete();
  volume1->Delete();
  volume2->Delete();
  raycastMapper1->Delete();
  raycastMapper2->Delete();
  ren->Delete();
  iren->Delete();
  renWin->Delete();
  reader1->Delete();
  reader2->Delete();

  return EXIT_SUCCESS;
}


