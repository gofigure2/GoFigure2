/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

/*=========================================================================
 Modifications were made by the GoFigure Dev. Team.
 while at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
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

#include "GoTransferFunctionWidget.h"

#include "hoverpoints.h"

//vtk
#include "vtkLookupTable.h"

//-------------------------------------------------------------------------

GoTransferFunctionWidget::GoTransferFunctionWidget(QColor iColor,
                                                   QWidget *parent)
    : QWidget(parent), m_color(iColor),
    m_alpha_gradient(QLinearGradient(0, 0, 0, 0))
{

  this->setMinimumWidth(50);
  this->setMinimumHeight(50);

  setAttribute(Qt::WA_NoBackground);

  // gamma TF
  m_gammaPoints = new HoverPoints(this, HoverPoints::RectangleShape);
  m_gammaPoints->setConnectionType(HoverPoints::LineConnection);
  connect(this, SIGNAL(enableGammaPoints(bool)), m_gammaPoints, SLOT(setEnabled(bool)));

  // opacity TF
  m_hoverPoints = new HoverPoints(this, HoverPoints::CircleShape);
  m_hoverPoints->setConnectionType(HoverPoints::LineConnection);
  connect(this, SIGNAL(enableHoverPoints(bool)), m_hoverPoints, SLOT(setEnabled(bool)));

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  connect(m_hoverPoints, SIGNAL(pointsChanged(QPolygonF)), this, SIGNAL(opacityChanged()));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QPolygonF GoTransferFunctionWidget::points() const
{
  return m_hoverPoints->points();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoTransferFunctionWidget::paintEvent(QPaintEvent * event)
{
  // generate shade, if necessary
  if (m_shade.isNull() || m_shade.size() != size())
    {
    generateShade();
    }

  // draw shade
  QPainter p(this);
  p.drawImage(0, 0, m_shade);
  p.setPen(QColor(146, 146, 146));
  p.drawRect(0, 0, width() - 1, height() - 1);

  // draw histogram
  if(m_Histogram.size() > 0)
    {
    QVector<QPointF> listOfPoints;
    qreal x_range = m_Histogram.size();

    for(int i=0; i<x_range; ++i)
      {
      QPointF point(i*(width()-1)/x_range, (height()-1) - m_Histogram[i]*(height()-1));
      QPointF point2(i*(width()-1)/x_range, (height()-1));
      listOfPoints.push_back(point2);
      listOfPoints.push_back(point);
      }
    p.drawLines(listOfPoints);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoTransferFunctionWidget::generateShade()
{
  m_shade = QImage(size(), QImage::Format_RGB32);
  QLinearGradient shade(0, 0, 0, height());
  shade.setColorAt(1, Qt::black);

  shade.setColorAt(0, m_color);

  QPainter p(&m_shade);
  p.fillRect(rect(), shade);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
AddPoints(const QPolygonF& iPoints)
{
  m_hoverPoints->setPoints(iPoints);
  m_hoverPoints->setPointLock(0, HoverPoints::LockToLeft);
  m_hoverPoints->setPointLock(iPoints.size()-1, HoverPoints::LockToRight);
  m_hoverPoints->setSortType(HoverPoints::XSort);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
AddGammaPoints(const QPolygonF& iPoints)
{
  m_gammaPoints->setPoints(iPoints);
  m_gammaPoints->setPointLock(0, HoverPoints::LockToLeft);
  m_gammaPoints->setPointLock(iPoints.size()-1, HoverPoints::LockToRight);
  m_gammaPoints->setSortType(HoverPoints::XSort);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
UpdateLookupTable(vtkLookupTable* iLUT, qreal iGamma, qreal iMin, qreal iMax)
{
  QPolygonF iPoints;
  int numTableValues = iLUT->GetNumberOfTableValues();
  qreal width = this->width();
  qreal height = this->height();

  // before window
  for(int i=0; i<iMin; ++i)
    {
    iPoints << QPointF((qreal)(i)*(width-1)/numTableValues, height-1);

    QColor color(m_shade.pixel(i*(width-1)/numTableValues, height-1));
    iLUT->SetTableValue(i, color.redF(), color.greenF(), color.blueF());
    }

  // points affected with gamma correction, in the window
  for(int i=iMin; i<iMax; ++i)
    {
    qreal input = ((qreal)i - iMin)/((iMax-iMin));
    qreal power = (qreal)(pow(input, iGamma));
    qreal temp_height = (height-1)*(1-power);
    if(temp_height<0)
      {
      qDebug()<< temp_height << " must be >0!";
      }
    else
      {
      iPoints << QPointF((qreal)(i)*(width-1)/numTableValues,temp_height);

      QColor color(m_shade.pixel(i*(width-1)/numTableValues, temp_height));
      iLUT->SetTableValue(i, color.redF(), color.greenF(), color.blueF());
      }
  }

  // after window
  for(int i=iMax; i<255; ++i)
    {
    iPoints << QPointF((qreal)(i)*(width-1)/numTableValues, 0);

    QColor color(m_shade.pixel(i*(width-1)/numTableValues, 0));
    iLUT->SetTableValue(i, color.redF(), color.greenF(), color.blueF());
    }

  // update LUT to modify visualization
  iLUT->Modified();

  // print new curve
  UpdateGamma(iPoints);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
SetHistogram(QVector<qreal> iHistogram)
{
  m_Histogram = iHistogram;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
Reset()
{
// reset alpha
QPolygonF points;
points << QPointF(0, height())
       << QPointF(width(),0);
m_hoverPoints->setPoints(points);
m_hoverPoints->setPointLock(0, HoverPoints::LockToLeft);
m_hoverPoints->setPointLock(1, HoverPoints::LockToRight);
m_hoverPoints->setSortType(HoverPoints::XSort);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
UpdateGamma( QPolygonF& iPoints)
{
m_gammaPoints->setPoints(iPoints);
m_gammaPoints->setSortType(HoverPoints::XSort);

update();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
setColor(QColor iColor)
{
  m_color = iColor;
  // generate new shade
  generateShade();
  update();
}
//-------------------------------------------------------------------------
