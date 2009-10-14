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
QGoManualSegmentationSettingsDialog( QWidget* parent,
  const double& iWidth,
  const QColor& iLineColor,
  const QColor& iNodeColor,
  const QColor& iActivatedNodeColor ) : QDialog( parent ),
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
  m_ContourWidget = vtkContourWidget::New();
  m_ContourWidget->SetRepresentation( m_ContourRepresentation );
  m_ContourWidget->On();

  vtkPolyData* init = vtkPolyData::New();
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
  init->SetPoints(points);
  init->SetLines(lines);
  points->Delete();
  lines->Delete();

  m_ContourWidget->Render();
  m_Renderer->ResetCamera();
  renwin->Render();
}

QGoManualSegmentationSettingsDialog::
~QGoManualSegmentationSettingsDialog()
{
  m_ContourWidget->Delete();
  m_ContourRepresentation->Delete();
  m_Renderer->Delete();
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
  m_ContourRepresentation->GetProperty()->SetColor( m_ActivatedNodeColor.redF(),
    m_ActivatedNodeColor.greenF(), m_ActivatedNodeColor.blueF() );
  m_ContourWidget->Render();
  m_Renderer->Render();
}
