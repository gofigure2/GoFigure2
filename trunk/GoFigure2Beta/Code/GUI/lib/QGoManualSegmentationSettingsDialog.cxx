/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

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

  m_Renderer = vtkSmartPointer< vtkRenderer >::New();

  vtkRenderWindow* renwin = this->qvtkWidget->GetRenderWindow();
  renwin->AddRenderer( m_Renderer );

  m_ContourRepresentation = vtkSmartPointer< vtkOrientedGlyphContourRepresentation >::New();
  m_ContourRepresentation->GetLinesProperty()->SetLineWidth( m_LineWidth );
  m_ContourRepresentation->GetLinesProperty()->SetColor( m_LineColor.redF(),
    m_LineColor.greenF(), m_LineColor.blueF() );
  m_ContourRepresentation->GetProperty()->SetColor( m_NodeColor.redF(),
    m_NodeColor.greenF(), m_NodeColor.blueF() );
  m_ContourRepresentation->GetActiveProperty()->SetColor( m_ActivatedNodeColor.redF(),
    m_ActivatedNodeColor.greenF(), m_ActivatedNodeColor.blueF() );

  m_ContourWidget = vtkSmartPointer< vtkContourWidget >::New();
  m_ContourWidget->SetInteractor( this->qvtkWidget->GetInteractor() );
  m_ContourWidget->SetRepresentation( m_ContourRepresentation );
  m_ContourWidget->On();

  m_InitPD = vtkSmartPointer< vtkPolyData >::New();
  vtkSmartPointer< vtkPoints >   points = vtkSmartPointer< vtkPoints >::New();
  vtkSmartPointer< vtkCellArray > lines = vtkSmartPointer< vtkCellArray >::New();

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

  m_ContourWidget->Initialize( m_InitPD );
  m_ContourWidget->Render();
  m_Renderer->ResetCamera();
  renwin->Render();

  QObject::connect( this->LineWidthSpinBox, SIGNAL( valueChanged( double ) ),
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

void QGoManualSegmentationSettingsDialog::SetLineWidth( const double& iValue )
{
  if( m_LineWidth != iValue )
    {
    m_LineWidth = iValue;
    m_ContourRepresentation->GetLinesProperty()->SetLineWidth( m_LineWidth );
    m_ContourWidget->Render();
    m_Renderer->Render();
    }
}

void QGoManualSegmentationSettingsDialog::SelectLineColor( )
{
  m_LineColor = QColorDialog::getColor( m_LineColor, this,
    tr( "Select Line Color" ) );

  if( m_LineColor.isValid() )
    {
    m_ContourRepresentation->GetLinesProperty()->SetColor( m_LineColor.redF(),
      m_LineColor.greenF(), m_LineColor.blueF() );
    m_ContourWidget->Render();
    m_Renderer->Render();
    }
}

void QGoManualSegmentationSettingsDialog::SelectNodeColor( )
{
  m_NodeColor = QColorDialog::getColor( m_NodeColor, this,
    tr( "Select Node Color" ) );

  if( m_NodeColor.isValid() )
    {
    m_ContourRepresentation->GetProperty()->SetColor( m_NodeColor.redF(),
      m_NodeColor.greenF(), m_NodeColor.blueF() );
    m_ContourWidget->Render();
    m_Renderer->Render();
    }
}

void QGoManualSegmentationSettingsDialog::SelectActivatedNodeColor( )
{
  m_ActivatedNodeColor = QColorDialog::getColor( m_ActivatedNodeColor, this,
    tr( "Select Activated Node Color" ) );

  if( m_ActivatedNodeColor.isValid() )
    {
    m_ContourRepresentation->GetActiveProperty()->SetColor(
      m_ActivatedNodeColor.redF(), m_ActivatedNodeColor.greenF(),
      m_ActivatedNodeColor.blueF() );
    m_ContourWidget->Render();
    m_Renderer->Render();
    }
}
