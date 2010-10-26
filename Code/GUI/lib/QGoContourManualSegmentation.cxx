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
#include "QGoContourManualSegmentation.h"

#include "QGoContourManualSegmentationWidget.h"
#include "QGoManualSegmentationSettingsDialog.h"

//--------------------------------------------------------------------------
QGoContourManualSegmentation::QGoContourManualSegmentation(QWidget *iParent):
  QObject(iParent),
  m_LinesWidth(2.),
  m_LinesColor(Qt::magenta),
  m_NodesColor(Qt::yellow),
  m_ActiveNodesColor(Qt::cyan),
  m_ReeditMode(false)
{
  m_ContourSegmentationWidget = new QGoContourManualSegmentationWidget(iParent);

  QObject::connect(
    m_ContourSegmentationWidget, SIGNAL( UpdateContourRepresentationProperties() ),
    this, SLOT( GenerateContourRepresentationProperties() ) );

  QObject::connect(
    m_ContourSegmentationWidget, SIGNAL( ValidatePressed() ),
    this, SIGNAL( validateContour() ) );

  QObject::connect(
    m_ContourSegmentationWidget, SIGNAL( ReinitializePressed() ),
    this, SIGNAL( reinitializeContour() ) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoContourManualSegmentation::
~QGoContourManualSegmentation()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QWidget *
QGoContourManualSegmentation::getWidget()
{
  return m_ContourSegmentationWidget;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoContourManualSegmentation::GenerateContourRepresentationProperties(bool iModified)
{
  bool haschanged = iModified;

  double temp = m_ContourSegmentationWidget->m_SettingsDialog->GetLineWidth();

  if ( m_LinesWidth != temp )
    {
    m_LinesWidth = temp;
    haschanged = true;
    }

  QColor temp_color = m_ContourSegmentationWidget->m_SettingsDialog->GetLineColor();

  if ( m_LinesColor != temp_color )
    {
    m_LinesColor = temp_color;
    haschanged = true;
    }

  temp_color = m_ContourSegmentationWidget->m_SettingsDialog->GetNodeColor();

  if ( m_NodesColor != temp_color )
    {
    m_NodesColor = temp_color;
    haschanged = true;
    }
  temp_color = m_ContourSegmentationWidget->m_SettingsDialog->GetActivatedNodeColor();

  if ( m_ActiveNodesColor != temp_color )
    {
    m_ActiveNodesColor = temp_color;
    haschanged = true;
    }

  if ( haschanged )
    {
    emit changeContourRepresentationProperty(
      static_cast< float >( m_LinesWidth ),
      m_LinesColor,
      m_NodesColor,
      m_ActiveNodesColor);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoContourManualSegmentation::SetReeditMode(bool iReeditMode)
{
  m_ReeditMode = iReeditMode;
  // show enable disable button here
  // or emit a signal
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool
QGoContourManualSegmentation::GetReeditMode()
{
  return m_ReeditMode;
}

//--------------------------------------------------------------------------
