#include "vtkViewImage.h"

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
#include "vtkInteractorStyleImage.h"

#include <vector>
#include <string>

vtkCxxRevisionMacro(vtkViewImage, "$Revision: 2 $");
vtkStandardNewMacro(vtkViewImage);

//----------------------------------------------------------------------------
vtkViewImage::vtkViewImage()
{
  this->OrientationMatrix = vtkMatrix4x4::New();
  this->OrientationAnnotation = vtkOrientationAnnotation::New();
  this->CornerAnnotation = vtkCornerAnnotation::New();
  this->TextProperty = vtkTextProperty::New();
  this->LookupTable = vtkLookupTable::New();
  this->ScalarBarActor = vtkScalarBarActor::New();
  this->Prop3DCollection = vtkProp3DCollection::New();
  this->DataSetCollection = vtkDataSetCollection::New();
  this->OrientationTransform = vtkMatrixToLinearTransform::New();

  this->MaskImage = 0;

  this->OrientationMatrix->Identity();
  this->CornerAnnotation->SetNonlinearFontScaleFactor (0.22);
  this->CornerAnnotation->SetTextProperty ( this->TextProperty );
  this->OrientationAnnotation->SetNonlinearFontScaleFactor (0.25);
  this->OrientationAnnotation->SetTextProperty ( this->TextProperty );
  this->ScalarBarActor->GetLabelTextProperty()->BoldOff();
  this->ScalarBarActor->GetLabelTextProperty()->ItalicOff();
  this->ScalarBarActor->SetNumberOfLabels (3);
  this->ScalarBarActor->SetWidth (0.1);
  this->ScalarBarActor->SetHeight (0.5);
  this->ScalarBarActor->SetPosition (0.9,0.3);
  this->LookupTable->SetTableRange (0, 1);
  this->LookupTable->SetSaturationRange (0, 0);
  this->LookupTable->SetHueRange (0, 0);
  this->LookupTable->SetValueRange (0, 1);
  this->LookupTable->Build();

  this->ShowAnnotations = true;
  this->ShowScalarBar = true;

  this->OrientationTransform->SetInput (this->OrientationMatrix);

  // LINE COMMENTED FOR COLOR RENDERING
  //this->WindowLevel->SetLookupTable (this->LookupTable);
  this->ScalarBarActor->SetLookupTable (this->LookupTable);

  this->Renderer->AddViewProp ( this->CornerAnnotation );
  this->Renderer->AddViewProp ( this->OrientationAnnotation );

  // LINE COMMENTED FOR COLOR RENDERING
  //this->Renderer->AddViewProp ( this->ScalarBarActor );
}

//----------------------------------------------------------------------------
vtkViewImage::~vtkViewImage()
{
  this->OrientationMatrix->Delete();
  this->OrientationAnnotation->Delete();
  this->CornerAnnotation->Delete();
  this->LookupTable->Delete();
  this->ScalarBarActor->Delete();
  this->Prop3DCollection->Delete();
  this->DataSetCollection->Delete();
  this->OrientationTransform->Delete();
  this->TextProperty->Delete();
}


//----------------------------------------------------------------------------
void vtkViewImage::SetOrientationMatrix (vtkMatrix4x4* matrix)
{
  vtkSetObjectMacro2Body (OrientationMatrix, vtkMatrix4x4, matrix);
  this->ImageActor->SetUserMatrix (this->OrientationMatrix);
  this->OrientationTransform->SetInput (this->OrientationMatrix);

  this->UpdateOrientation();
}

//----------------------------------------------------------------------------
void vtkViewImage::SetLookupTable (vtkLookupTable* lookuptable)
{
  vtkSetObjectMacro2Body (LookupTable, vtkLookupTable, lookuptable);
  this->WindowLevel->SetLookupTable (this->LookupTable);
  this->ScalarBarActor->SetLookupTable (this->LookupTable);
}

//----------------------------------------------------------------------------
void vtkViewImage::SetTextProperty (vtkTextProperty* textproperty)
{
  vtkSetObjectMacro2Body (TextProperty, vtkTextProperty, textproperty);
  this->CornerAnnotation->SetTextProperty (this->TextProperty);
}

//----------------------------------------------------------------------------
void vtkViewImage::SetColorWindow(double s)
{
  if (s<0)
    s = 1.0;

  this->Superclass::SetColorWindow(s);
  double v_min = this->GetColorLevel() - 0.5*s;
  double v_max = this->GetColorLevel() + 0.5*s;
  this->GetLookupTable()->SetRange (v_min, v_max);

}

//----------------------------------------------------------------------------
void vtkViewImage::SetColorLevel(double s)
{
  this->Superclass::SetColorLevel(s);
  double v_min = s - 0.5*this->GetColorWindow();
  double v_max = s + 0.5*this->GetColorWindow();
  this->GetLookupTable()->SetRange (v_min, v_max);

}


//----------------------------------------------------------------------------
double vtkViewImage::GetValueAtPosition(double worldcoordinates[3],
  int component )
{
  if (!this->GetInput())
    return 0.0;

  int* indices = this->GetImageCoordinatesFromWorldCoordinates (worldcoordinates);
  int* extent = this->GetInput()->GetWholeExtent();
  if ( (indices[0] < extent[0]) || (indices[0] > extent[1]) ||
       (indices[1] < extent[2]) || (indices[1] > extent[3]) ||
       (indices[2] < extent[4]) || (indices[2] > extent[5]) )
    return 0;
  else
    return this->GetInput()->GetScalarComponentAsDouble (indices[0], indices[1],
      indices[2], component);
}


//----------------------------------------------------------------------------
bool vtkViewImage::RemoveDataSet (vtkDataSet* dataset)
{
  unsigned int index = this->DataSetCollection->IsItemPresent (dataset);
  if (!index)
    return false;
  this->Renderer->RemoveViewProp (
    vtkProp::SafeDownCast (this->Prop3DCollection->GetItemAsObject (index)));
  this->DataSetCollection->RemoveItem (index);
  this->Prop3DCollection->RemoveItem (index);

  return true;
}

//----------------------------------------------------------------------------
double* vtkViewImage::GetWorldCoordinatesFromImageCoordinates(int indices[3])
{
  if (!this->GetInput())
  {
    double* nullpos = new double[3];
    nullpos[0] = 0; nullpos[1] = 0; nullpos[2] = 0;
    return nullpos;
  }

  // Get information
  double* spacing = this->GetInput()->GetSpacing();
  double* origin = this->GetInput()->GetOrigin();

  double unorientedposition[4];
  for (unsigned int i=0; i<3; i++)
    unorientedposition[i] = origin[i] + spacing[i]*indices[i];
  unorientedposition[3] = 1;

  // apply orientation matrix
  double* position = new double[4];
  this->GetOrientationMatrix()->MultiplyPoint (unorientedposition, position);
  return position;
}

//----------------------------------------------------------------------------
int* vtkViewImage::GetImageCoordinatesFromWorldCoordinates(double position[3])
{
  if (!this->GetInput())
  {
    int* nullpos = new int[3];
    nullpos[0] = 0; nullpos[1] = 0; nullpos[2] = 0;
    return nullpos;
  }

  // Get information
  double unorientedposition[4] = {position[0], position[1], position[2], 1};
  double spacing[4] = {this->GetInput()->GetSpacing()[0],
    this->GetInput()->GetSpacing()[1],
    this->GetInput()->GetSpacing()[2],
    0};
  double origin[4] = {this->GetInput()->GetOrigin()[0],
    this->GetInput()->GetOrigin()[1],
    this->GetInput ()->GetOrigin()[2],
    1};

  // apply inverted orientation matrix to the world-coordinate position
  vtkMatrix4x4* inverse = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert (this->GetOrientationMatrix(), inverse);
  inverse->MultiplyPoint (unorientedposition, unorientedposition);

  int* indices = new int[3];
  for (unsigned int i=0; i<3;i++)
  {
    if (fabs (spacing[i]) > 1e-5)
      indices[i] = vtkMath::Round((unorientedposition[i]-origin[i])/spacing[i]);
    else
      indices[i] = 0;
  }
  inverse->Delete();

  return indices;
}

//----------------------------------------------------------------------------
void vtkViewImage::SetBackground(double rgb[3])
{
  if (this->Renderer)
    this->Renderer->SetBackground(rgb);
}
//----------------------------------------------------------------------------
void vtkViewImage::SetBackground(double r, double g, double b)
{
  if (this->Renderer)
    this->Renderer->SetBackground(r,g,b);
}

//----------------------------------------------------------------------------
double* vtkViewImage::GetBackground()
{
  if (this->Renderer)
    return this->Renderer->GetBackground();
  return NULL;
}

//----------------------------------------------------------------------------
void vtkViewImage::ResetCamera (void)
{
  if (this->Renderer)
    this->Renderer->ResetCamera();
}

//----------------------------------------------------------------------------
void vtkViewImage::Reset (void)
{
  this->ResetCamera();
}

//----------------------------------------------------------------------------
void vtkViewImage::Enable (void)
{
  this->Interactor->Enable();
}
//----------------------------------------------------------------------------
void vtkViewImage::Disable (void)
{
  this->Interactor->Disable();
}
//----------------------------------------------------------------------------
int vtkViewImage::GetEnabled (void)
{
  return this->Interactor->GetEnabled();
}

//----------------------------------------------------------------------------
void vtkViewImage::SetShowAnnotations (int val)
{
  this->ShowAnnotations = val;
  this->CornerAnnotation->SetVisibility (val);
  this->OrientationAnnotation->SetVisibility (val);
}
//----------------------------------------------------------------------------
void vtkViewImage::SetShowScalarBar (int val)
{
  this->ShowScalarBar = val;
  this->ScalarBarActor->SetVisibility (val);
}

vtkRenderWindowInteractor* vtkViewImage::GetRenderWindowInteractor()
{
  return this->GetRenderWindow()->GetInteractor();
} 
