#include "QGoManualSegmentationSettingsDialog.h"

#include <QColorDialog>

#include "vtkOrientedGlyphContourRepresentation.h"
#include "vtkContourWidget.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkProperty.h"

#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkMath.h"

QGoManualSegmentationSettingsDialog::
QGoManualSegmentationSettingsDialog( QWidget* iParent,
  const double& iWidth,
  const QColor& iLineColor,
  const QColor& iNodeColor,
  const QColor& iActivatedNodeColor ) : QDialog( iParent ),
    m_LineWidth( iWidth ),
    m_LineColor( iLineColor ),
    m_NodeColor( iNodeColor ),
    m_ActivatedNodeColor( iActivatedNodeColor )
{
  this->setupUi( this );

  m_Renderer = vtkRenderer::New();

  vtkRenderWindow* renwin = this->qvtkWidget->GetRenderWindow();
  renwin->AddRenderer( m_Renderer );

  m_ContourRepresentation = vtkOrientedGlyphContourRepresentation::New();
  m_ContourRepresentation->GetLinesProperty()->SetLineWidth( m_LineWidth );
  m_ContourRepresentation->GetLinesProperty()->SetColor( m_LineColor.redF(),
    m_LineColor.greenF(), m_LineColor.blueF() );
  m_ContourRepresentation->GetProperty()->SetColor( m_NodeColor.redF(),
    m_NodeColor.greenF(), m_NodeColor.blueF() );
  m_ContourRepresentation->GetActiveProperty()->SetColor( m_ActivatedNodeColor.redF(),
    m_ActivatedNodeColor.greenF(), m_ActivatedNodeColor.blueF() );

  m_ContourWidget = vtkContourWidget::New();
  m_ContourWidget->SetInteractor( this->qvtkWidget->GetInteractor() );
  m_ContourWidget->SetRepresentation( m_ContourRepresentation );
  m_ContourWidget->On();

  m_InitPD = vtkPolyData::New();
  vtkPoints    *points      = vtkPoints::New();
  vtkCellArray *lines       = vtkCellArray::New();
  vtkIdType    *lineIndices = new vtkIdType[7];
  for( int i = 0; i< 6; i++ )
    {
    const double angle = 2.0*vtkMath::Pi()*i/6.0;
    points->InsertPoint(static_cast<vtkIdType>(i), 0.1*cos(angle),
      0.1*sin(angle), 0.0 );
    lineIndices[i] = static_cast<vtkIdType>(i);
    }

  lineIndices[6] = 0;
  lines->InsertNextCell(7,lineIndices);
  delete [] lineIndices;
  m_InitPD->SetPoints(points);
  m_InitPD->SetLines(lines);
  points->Delete();
  lines->Delete();

  m_ContourWidget->Initialize( m_InitPD );
  m_ContourWidget->Render();
  m_Renderer->ResetCamera();
  renwin->Render();

  QObject::connect( this->LineWidthSpinBox, SIGNAL( valueChanged( double) ),
    this, SLOT( SetLineWidth( double ) ) );

  QObject::connect( this->LineColorBtn, SIGNAL( pressed() ),
    this, SLOT(SelectLineColor() ) );

  QObject::connect( this->NodeColorBtn, SIGNAL( pressed() ),
    this, SLOT(SelectNodeColor() ) );
  QObject::connect( this->ActivatedNodeColorBtn, SIGNAL( pressed() ),
    this, SLOT(SelectActivatedNodeColor() ) );
}

QGoManualSegmentationSettingsDialog::
~QGoManualSegmentationSettingsDialog()
{
  m_InitPD->Delete();
  m_ContourWidget->Delete();
  m_ContourRepresentation->Delete();
  m_Renderer->Delete();

  delete this->qvtkWidget;
}

double
QGoManualSegmentationSettingsDialog::GetLineWidth( ) const
{
  return m_LineWidth;
}

QColor QGoManualSegmentationSettingsDialog::GetLineColor( ) const
{
  return m_LineColor;
}

QColor QGoManualSegmentationSettingsDialog::GetNodeColor( ) const
{
  return m_NodeColor;
}

QColor QGoManualSegmentationSettingsDialog::GetActivatedNodeColor( ) const
{
  return m_ActivatedNodeColor;
}

/// \todo check the implementation of this method. strange that iValue is not used at all
void QGoManualSegmentationSettingsDialog::SetLineWidth( const double& iValue )
{
  m_LineWidth = this->LineWidthSpinBox->value();
  m_ContourRepresentation->GetLinesProperty()->SetLineWidth( m_LineWidth );
  m_ContourWidget->Render();
  m_Renderer->Render();
}

void QGoManualSegmentationSettingsDialog::SelectLineColor( )
{
  m_LineColor = QColorDialog::getColor( m_LineColor, this,
    tr( "Select Line Color" ) );
  m_ContourRepresentation->GetLinesProperty()->SetColor( m_LineColor.redF(),
    m_LineColor.greenF(), m_LineColor.blueF() );
  m_ContourWidget->Render();
  m_Renderer->Render();
}

void QGoManualSegmentationSettingsDialog::SelectNodeColor( )
{
  m_NodeColor = QColorDialog::getColor( m_NodeColor, this,
    tr( "Select Node Color" ) );
  m_ContourRepresentation->GetProperty()->SetColor( m_NodeColor.redF(),
    m_NodeColor.greenF(), m_NodeColor.blueF() );
  m_ContourWidget->Render();
  m_Renderer->Render();
}

void QGoManualSegmentationSettingsDialog::SelectActivatedNodeColor( )
{
  m_ActivatedNodeColor = QColorDialog::getColor( m_ActivatedNodeColor, this,
    tr( "Select Activated Node Color" ) );
  m_ContourRepresentation->GetActiveProperty()->SetColor( m_ActivatedNodeColor.redF(),
    m_ActivatedNodeColor.greenF(), m_ActivatedNodeColor.blueF() );
  m_ContourWidget->Render();
  m_Renderer->Render();
}
