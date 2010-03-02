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

#include "QGoManualSegmentationDockWidget.h"

#include <QColorDialog>
#include <cstdlib>

QGoManualSegmentationDockWidget::
QGoManualSegmentationDockWidget( QWidget* iParent ) :
  QDockWidget( iParent )
{
  m_SettingsDialog = new QGoManualSegmentationSettingsDialog( this );

  this->setupUi( this );

  QObject::connect( this->SettingsBtn, SIGNAL( pressed() ),
    m_SettingsDialog, SLOT( exec() ) );

  QObject::connect( m_SettingsDialog, SIGNAL( accepted() ),
    this, SLOT( GenerateContourRepresentationProperties( ) ) );

  QObject::connect( m_SettingsDialog, SIGNAL( accepted() ),
    this, SLOT( SetContourColor() ) );

  QObject::connect( this->ReinitializeBtn, SIGNAL( pressed() ),
    this, SIGNAL( ReinitializePressed() ) );

  QObject::connect( this->ValidateBtn, SIGNAL( pressed() ),
    this, SIGNAL( ValidatePressed() ) );
}

QGoManualSegmentationDockWidget::
~QGoManualSegmentationDockWidget()
{
}

void
QGoManualSegmentationDockWidget::
ActivateManualSegmentation( bool iActivate )
{
  this->ValidateBtn->setEnabled( iActivate );
  this->ReinitializeBtn->setEnabled( iActivate );
  this->SettingsBtn->setEnabled( iActivate );
}

void QGoManualSegmentationDockWidget::
SetContourColor( )
{
  QColor color = m_SettingsDialog->GetLineColor();
//   GenerateOrderVector( color );
}

// void QGoManualSegmentationDockWidget::
// GenerateOrderVector( const QColor& iSelectedColor )
// {
//   int h, s, v;
//   iSelectedColor.getHsv( &h, &s, &v );
//
//   unsigned int closest_id = static_cast< unsigned int >(
//     floor( static_cast< double >( h ) / 60. + 0.5 ) );
//
//   unsigned int k = 0;
//   for( unsigned int i = 0; i < 6; i++ )
//     {
//     if( i != closest_id )
//       {
//       m_OrderVector[k] = i;
//       k++;
//       }
//     }
// }

// void
// QGoManualSegmentationDockWidget::
// GenerateRandomColorForGivenId( int iId )
// {
//   unsigned int k = static_cast< unsigned int >( iId ) % 5;
//   int t = 60 * static_cast< int >( m_OrderVector[k] );
//
//   m_ValidatedColor.setHsv( rand() % 60 + t, rand() % 256, rand() % 128 + 127 );
// }


QColor
QGoManualSegmentationDockWidget::GetLinesColor() const
{
  return m_LinesColor;
}

double
QGoManualSegmentationDockWidget::GetLinesWidth() const
{
  return m_LinesWidth;
}

QColor
QGoManualSegmentationDockWidget::GetNodesColor() const
{
  return m_NodesColor;
}

QColor
QGoManualSegmentationDockWidget::GetActiveNodesColor() const
{
  return m_ActiveNodesColor;
}

void
QGoManualSegmentationDockWidget::
GenerateContourRepresentationProperties()
{
  m_LinesWidth = m_SettingsDialog->GetLineWidth();
  m_LinesColor = m_SettingsDialog->GetLineColor();
  m_NodesColor = m_SettingsDialog->GetNodeColor();
  m_ActiveNodesColor = m_SettingsDialog->GetActivatedNodeColor();

  emit ContourRepresentationPropertiesChanged();
}
