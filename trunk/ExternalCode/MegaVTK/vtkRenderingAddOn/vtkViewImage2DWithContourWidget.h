#ifndef __vtkViewImage2DWithContourWidget_h
#define __vtkViewImage2DWithContourWidget_h

#include "vtkViewImage2D.h"
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkContourWidget.h>
#include <vtkLookupTable.h>
#include <vtkCornerAnnotation.h>

class vtkViewImage2DWithContourWidget : public vtkViewImage2D
{
public:
  static vtkViewImage2DWithContourWidget* New();
  vtkTypeRevisionMacro( vtkViewImage2DWithContourWidget, vtkViewImage2D );

  virtual void SetupInteractor( vtkRenderWindowInteractor* );

  void SetContourWidgetInteractionOn( )
  {
    this->ContourWidgetEnabled = true;
    this->ContourWidget->On();
    this->CornerAnnotation->SetText( 1, "Tracer ON");
  }
  void SetContourWidgetInteractionOff( )
  {
    this->ContourWidgetEnabled = false;
    this->ContourWidget->Off();
    this->CornerAnnotation->SetText( 1, "Tracer OFF");
  }
  void SetContourWidgetInteraction( const bool& iState )
  {
    if( iState )
      this->SetContourWidgetInteractionOn();
    else
      this->SetContourWidgetInteractionOff();
  }

  vtkSetMacro( CurrentLabel, unsigned int );
  vtkGetMacro( CurrentLabel, unsigned int );

  vtkGetMacro( ContourWidgetEnabled, bool );
  vtkGetObjectMacro( ContourWidget, vtkContourWidget );
  vtkGetObjectMacro( ContourRepresentation,
    vtkOrientedGlyphContourRepresentation );
  virtual void SetInput( vtkImageData* image );

  void ValidateTracing();

protected:
  vtkViewImage2DWithContourWidget();
  ~vtkViewImage2DWithContourWidget();

  unsigned int CurrentLabel;
  bool ContourWidgetEnabled;
  vtkOrientedGlyphContourRepresentation* ContourRepresentation;
  vtkContourWidget* ContourWidget;
  vtkLookupTable* LUT;
  vtkImageData* Tracing;

private:
  vtkViewImage2DWithContourWidget( const vtkViewImage2DWithContourWidget& );
  void operator = ( const vtkViewImage2DWithContourWidget& );
};

#endif
