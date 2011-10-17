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
                                                   double iMax,
                                                   QWidget *parent)
    : QWidget(parent), m_color(iColor)
{

  // set max value
  m_Max = iMax;

  // set minimum size to avoid bad appearance
  this->setMinimumWidth(50);
  this->setMinimumHeight(50);

  setAttribute(Qt::WA_NoBackground);

  // LUT curve
  m_LUTPoints = new HoverPoints(this, HoverPoints::RectangleShape);
  m_LUTPoints->setConnectionType(HoverPoints::LineConnection);
  // connect signal
  connect(this, SIGNAL(enableLUTCurve(bool)), m_LUTPoints, SLOT(setEnabled(bool)));

  // opacity TF
  m_OpacityTFPoints = new HoverPoints(this, HoverPoints::CircleShape);
  m_OpacityTFPoints->setConnectionType(HoverPoints::LineConnection);
  // connect signal
  connect(this, SIGNAL(enableOpacityTF(bool)), m_OpacityTFPoints, SLOT(setEnabled(bool)));
  connect(m_OpacityTFPoints, SIGNAL(pointsChanged(QPolygonF)), this, SIGNAL(opacityChanged()));

  // set size policy
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QPolygonF GoTransferFunctionWidget::points() const
{
  return m_OpacityTFPoints->points();
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
AddPointsToOpacityTF(const QPolygonF& iPoints)
{
  m_OpacityTFPoints->setPoints(iPoints);
  m_OpacityTFPoints->setPointLock(0, HoverPoints::LockToLeft);
  m_OpacityTFPoints->setPointLock(iPoints.size()-1, HoverPoints::LockToRight);
  m_OpacityTFPoints->setSortType(HoverPoints::XSort);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
AddPointsToLUT(const QPolygonF& iPoints)
{
  m_LUTPoints->setPoints(iPoints);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
UpdateLookupTable(vtkLookupTable* iLUT, qreal iGamma, qreal iMin, qreal iMax)
{
  QPolygonF iPoints;
  int numTableValues = m_Max;
  qreal width = this->width();
  qreal height = this->height();
  iLUT->SetNumberOfTableValues(iMax-iMin);
  // update LUT to modify visualization

  // points affected with gamma correction, in the window
  int count = 0;

  // first point
  iPoints << QPointF((qreal)(iMin)*(width-1)/numTableValues, height - 1);
  QColor colorMin(m_shade.pixel(iMin*(width-1)/numTableValues, height - 1));
  iLUT->SetTableValue(count, colorMin.redF(), colorMin.greenF(), colorMin.blueF());
  count++;

  for(int i=iMin+1; i<iMax-1; ++i)
    {
    qreal input = ((qreal)i - iMin)/((iMax-iMin));
    qreal power = (qreal)(pow(input, iGamma));
    qreal temp_height = (height-1)*(1-power);

    iPoints << QPointF((qreal)(i)*(width-1)/numTableValues,temp_height);

    QColor color(m_shade.pixel(i*(width-1)/numTableValues, temp_height));
    iLUT->SetTableValue(count, color.redF(), color.greenF(), color.blueF());
    count++;
    }


  // last point
  iPoints << QPointF((qreal)(iMax)*(width-1)/numTableValues,0);
  QColor colorMax(m_shade.pixel(iMax*(width-1)/numTableValues, 0));
  iLUT->SetTableValue(count, colorMax.redF(), colorMax.greenF(), colorMax.blueF());

  iLUT->SetRange(iMin, iMax);

  iLUT->Modified();

  // print new curve
  m_LUTPoints->setPoints(iPoints);
  update();
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
ResetOpacity()
{
  // reset opacity TF
  QPolygonF points;
  points << QPointF(0, height())
         << QPointF(width(),0);
  m_OpacityTFPoints->setPoints(points);
  m_OpacityTFPoints->setPointLock(0, HoverPoints::LockToLeft);
  m_OpacityTFPoints->setPointLock(1, HoverPoints::LockToRight);
  m_OpacityTFPoints->setSortType(HoverPoints::XSort);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
setColor(QColor iColor)
{
  // modify color
  m_color = iColor;
  // generate new shade
  generateShade();
  // update the shade
  update();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
setMax(double iMax)
{
  m_Max = iMax;
}
//-------------------------------------------------------------------------
