#include "vtkViewImage2DWithContourWidget.h"

#include <vtkObjectFactory.h>
#include <vtkImageActorPointPlacer.h>
#include <vtkCornerAnnotation.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>

#include "vtkFillImageWithPolyData.h"

vtkCxxRevisionMacro(vtkViewImage2DWithContourWidget, "$Revision 1$");
vtkStandardNewMacro(vtkViewImage2DWithContourWidget);


vtkViewImage2DWithContourWidget::vtkViewImage2DWithContourWidget()
{
  this->ContourWidgetEnabled = false;

  this->ContourRepresentation = vtkOrientedGlyphContourRepresentation::New();

  this->ContourWidget = vtkContourWidget::New();
  this->ContourWidget->SetPriority( 10.0 );

  this->CornerAnnotation->SetText( 1, "Tracer OFF");

  this->CurrentLabel = 1;

  this->LUT = vtkLookupTable::New();
  this->LUT->SetNumberOfTableValues( 256 );

  this->LUT->SetTableValue (1, 1.0, 0.0, 0.0, 0.5);
  this->LUT->SetTableValue (2, 0.0, 1.0, 0.0, 0.5);
  this->LUT->SetTableValue (3, 0.0, 0.0, 1.0, 0.5);
  this->LUT->SetTableValue (4, 1.0, 1.0, 0.0, 0.5);
  this->LUT->SetTableValue (5, 0.0, 1.0, 1.0, 0.5);
  this->LUT->SetTableValue (6, 1.0, 0.0, 1.0, 0.5);

  double d_id;
  for( int i = 7; i < 256; i++ )
  {
    d_id = static_cast< double >( i );
    if( i % 3 == 0 )
    {
    this->LUT->SetTableValue( i,
      ( 200. * ( 84.0 - d_id ) / 85. + 50. ) / 255.,
      ( 200. * d_id / 85. + 50. ) / 255.,
      0.,
      0.5);
    }
    else
    {
      if( i % 3 == 1 )
      {
        this->LUT->SetTableValue( i,
          0,
          ( ( 169. - d_id ) / 85. * 200. + 50. ) / 255.,
          ( ( d_id - 85. ) / 85. * 200. + 50. ) / 255.,
          0.5 );
      }
      else
      {
        this->LUT->SetTableValue( i,
          ( ( d_id -170. ) / 85.0 * 200.0 + 50. ) / 255.0,
          0.0,
          ( ( 255.0 - d_id ) / 85.0 * 200.0 + 50. ) /255.0,
          0.5);
      }
    }
  }
}

vtkViewImage2DWithContourWidget::~vtkViewImage2DWithContourWidget()
{
  this->ContourRepresentation->Delete();
  this->ContourWidget->Delete();
  this->LUT->Delete();
}

void vtkViewImage2DWithContourWidget::SetupInteractor(
  vtkRenderWindowInteractor* arg )
{
  vtkViewImage2D::SetupInteractor( arg );
  if( arg )
  {
    this->ContourWidget->SetInteractor( arg );
  }
}

void vtkViewImage2DWithContourWidget::SetInput( vtkImageData* image )
{
  vtkViewImage2D::SetInput( image );
  if( image )
  {
    vtkImageActorPointPlacer* point_placer = vtkImageActorPointPlacer::New();
    point_placer->SetImageActor( this->GetImageActor() );

    this->ContourRepresentation->SetPointPlacer( point_placer );
    this->ContourWidget->SetRepresentation(this->ContourRepresentation);

    point_placer->Delete();
    this->SetContourWidgetInteractionOff();
  }
}

void vtkViewImage2DWithContourWidget::ValidateTracing()
{
  vtkImageData* input = this->GetInput();

  if( !input )
  {
    return;
  }

  vtkPolyData* contour =
    this->ContourRepresentation->GetContourRepresentationAsPolyData( );

  if( !contour || contour->GetNumberOfPoints()<3 )
  {
    return;
  }

  double val = this->CurrentLabel;

  vtkFillImageWithPolyData* filler = vtkFillImageWithPolyData::New();
  filler->SetInput ( input );
// VIEW_ORIENTATION_SAGITTAL = 0,
// VIEW_ORIENTATION_CORONAL = 1,
// VIEW_ORIENTATION_AXIAL = 2
  filler->SetExtractionDirection( this->ViewOrientation );
  filler->SetPolyData( contour );
  filler->SetInsidePixelValue (val);
  filler->Update();

  this->Tracing = this->GetMaskImage();

  // must check if the dimensions of the image match the dimensions of
  // the tracing
  if( this->Tracing )
  {
    int* dims1 = this->Tracing->GetDimensions();
    int* dims2 = input->GetDimensions();
    if( (dims1[0]!=dims2[0]) || (dims1[1]!=dims2[1]) || (dims1[2]!=dims2[2]) )
    {
      // I set to 0 the previous tracing
      this->Tracing = 0;
    }
  }


  // the output of the filler is merged with the previous tracing, if any
  if( this->Tracing )
  {
    // do the merging myself
    unsigned long numPoints = this->Tracing->GetNumberOfPoints();
    unsigned char* buff1 = (unsigned char*)( this->Tracing->GetScalarPointer() );
    unsigned char* buff2 = (unsigned char*)(
      filler->GetOutput()->GetScalarPointer() );
    unsigned char u_char_0 = static_cast< unsigned char >( 0 );
    unsigned char u_char_255 = static_cast< unsigned char >( 255 );

    for( unsigned long i=0; i<numPoints; ++i, ++buff1, ++buff2 )
    {
      if ( *buff2 == u_char_255 ) // erase label in fact
        *buff1 = u_char_0;
      else if( val > u_char_0 )
        *buff1 = *buff2;
    }

    this->Tracing->Modified();
  }
  else
  {
    this->Tracing = vtkImageData::New();
    this->Tracing->DeepCopy ( filler->GetOutput() );
    this->Tracing->SetOrigin( input->GetOrigin() );

    this->SetMaskImage( this->Tracing, this->LUT, true );
//     this->SyncSetMaskImage (this->Tracing, this->LUT);
    this->Tracing->Delete();
    this->ContourWidget->Initialize( 0 );
  }

  filler->Delete();
}
