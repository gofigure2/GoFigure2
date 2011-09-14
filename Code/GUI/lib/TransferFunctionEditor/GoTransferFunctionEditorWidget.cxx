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

#include "GoTransferFunctionEditorWidget.h"

// Gofigure
#include "GoTransferFunctionWidget.h"
#include "QGoLUTDialog.h"

//qt
#include "hoverpoints.h"
#include <QSpacerItem>

//vtk
#include "vtkLookupTable.h"
#include "vtkImageAccumulate.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPiecewiseFunction.h"

// temp
#include <iostream>

//-------------------------------------------------------------------------

GoTransferFunctionEditorWidget::GoTransferFunctionEditorWidget(QWidget *parent,
                                                               QString iChannel,
                                                               const std::vector<double>& iColor)
    : QWidget(parent )
{
  // update color
  m_Color.setRedF(iColor[0]/255);
  m_Color.setGreenF(iColor[1]/255);
  m_Color.setBlueF(iColor[2]/255);
  m_Color.setAlphaF(iColor[3]/255);

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setSpacing(1);
  vbox->setMargin(1);

  m_red_shade = new GoTransferFunctionWidget(
        m_Color,
        this);

  m_LUT = NULL;

  m_Channel = iChannel;

  QPushButton *presetLUTPushButton = new QPushButton("Preset LUT", this);
  presetLUTPushButton->setEnabled(false);

  QHBoxLayout *lutLayout = new QHBoxLayout;
  QPushButton *saveLUTPushButton = new QPushButton("Save LUT", this);
  QPushButton *loadLUTPushButton = new QPushButton("Load LUT", this);
  lutLayout->addWidget(loadLUTPushButton);
  lutLayout->addWidget(saveLUTPushButton);

  QPushButton *okPushButton = new QPushButton("OK", this);
  QPushButton *resetLUTPushButton = new QPushButton("Reset", this);

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(okPushButton);
  layout->addWidget(resetLUTPushButton);

  QLabel* channelName = new QLabel("Channel:");
  QLabel* channelNameText = new QLabel(iChannel);
  QHBoxLayout *nameLayout = new QHBoxLayout;
  nameLayout->addWidget(channelName);
  nameLayout->addWidget(channelNameText);

  QLabel* gammaName = new QLabel("Gamma:");
  m_GammaSlider = new QSlider(this);
  m_GammaSlider->setOrientation(Qt::Horizontal);
  m_GammaSlider->setMaximum(5000);
  m_GammaSlider->setMinimum(50);
  m_GammaSlider->setValue(500);
  connect(m_GammaSlider, SIGNAL(valueChanged(int)), this, SLOT(gammaValueChanged(int)));

  QHBoxLayout *gammaLayout = new QHBoxLayout;
  gammaLayout->addWidget(gammaName);
  gammaLayout->addWidget(m_GammaSlider);

  QLabel* minName = new QLabel("Min:");
  m_MinSlider = new QSlider(this);
  m_MinSlider->setOrientation(Qt::Horizontal);
  m_MinSlider->setMaximum(255);
  m_MinSlider->setValue(20);
  m_MinSlider->setStyleSheet("QSlider::add-page:horizontal {background: blue;} QSlider::sub-page:vertical {background: white;}");
  connect(m_MinSlider, SIGNAL(valueChanged(int)), this, SLOT(minValueChanged(int)));

  QLabel* maxName = new QLabel("Max:");
  m_MaxSlider = new QSlider(this);
  m_MaxSlider->setOrientation(Qt::Horizontal);
  m_MaxSlider->setMaximum(255);
  m_MaxSlider->setValue(230);
  m_MaxSlider->setStyleSheet("QSlider::add-page:horizontal {background: white;} QSlider::sub-page:vertical {background: blue;}");
  connect(m_MaxSlider, SIGNAL(valueChanged(int)), this, SLOT(maxValueChanged(int)));

  QHBoxLayout *posLayout = new QHBoxLayout;
  posLayout->addWidget(minName);
  posLayout->addWidget(m_MinSlider);
  posLayout->addWidget(maxName);
  posLayout->addWidget(m_MaxSlider);

  QCheckBox* tfCB = new QCheckBox("Show TF");
  tfCB->setChecked(true);
  connect(tfCB, SIGNAL(clicked(bool)), m_red_shade, SIGNAL(enableGammaPoints(bool)));

  QCheckBox* tfoCB = new QCheckBox("Show TF Opacity");
  tfoCB->setChecked(true);
  connect(tfoCB, SIGNAL(clicked(bool)), m_red_shade, SIGNAL(enableHoverPoints(bool)));

  QCheckBox* histoCB = new QCheckBox("Show Log Histogram");
  histoCB->setChecked(true);
  histoCB->setEnabled(false);

  vbox->addLayout(nameLayout);

  QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);
  vbox->addItem(spacer);

  vbox->addWidget(m_MaxSlider);
  vbox->addWidget(m_red_shade);
  vbox->addWidget(m_MinSlider);

  QSpacerItem* spacer2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);
  vbox->addItem(spacer2);



  vbox->addLayout(gammaLayout);
  vbox->addLayout(posLayout);
  vbox->addWidget(tfCB);
  vbox->addWidget(tfoCB);
  vbox->addWidget(histoCB);
  vbox->addLayout(lutLayout);
  vbox->addWidget(presetLUTPushButton);
  vbox->addLayout(layout);

  connect(m_red_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
  connect(m_red_shade, SIGNAL(colorsChanged()), this, SLOT(updateOpacityTF()));

  connect(okPushButton, SIGNAL(released()), this, SLOT(close()));
  /*connect(okPushButton, SIGNAL(released()), this, SLOT(savePoints()));

  connect(resetLUTPushButton, SIGNAL(pressed()), this, SLOT(resetLUT()));
  connect(saveLUTPushButton, SIGNAL(pressed()), this, SLOT(saveLUT()));
  connect(loadLUTPushButton, SIGNAL(pressed()), this, SLOT(readLUT()));*/
  // useless...?
  connect(presetLUTPushButton, SIGNAL(pressed()), this, SLOT(presetLUT()));


  // enable event filter
  this->installEventFilter(this);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
inline static bool x_less_than(const QPointF &p1, const QPointF &p2)
{
  return p1.x() < p2.x();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoTransferFunctionEditorWidget::pointsUpdated()
{
  if(m_LUT)
    {
    // update the LUT
    m_red_shade->UpdateLookupTable(m_LUT,
                                   (qreal)m_GammaSlider->value(),
                                   (qreal)m_MinSlider->value(),
                                   (qreal)m_MaxSlider->value());
    // send signal to update the visualization
    emit updateVisualization();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
static void set_shade_points(const QPolygonF &points, GoTransferFunctionWidget *shade)
{
  shade->hoverPoints()->setPoints(points);
  shade->hoverPoints()->setPointLock(0, HoverPoints::LockToLeft);
  shade->hoverPoints()->setPointLock(points.size() - 1, HoverPoints::LockToRight);
  shade->update();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddPoints( const std::vector< std::map< unsigned int, unsigned int> >& iRGBA)
{
  //red
  QPolygonF redPoints;
  // add gamma points
  computePointsFromMap(iRGBA[0], redPoints);
  m_red_shade->AddGammaPoints(redPoints);

  QPolygonF alphaPoints;
  // add alpha points
  computePointsFromMap(iRGBA[3], alphaPoints);
  m_red_shade->AddPoints(alphaPoints);

  // update histogram and alpha gradient
  pointsUpdated();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
computePointsFromMap(const std::map< unsigned int, unsigned int>& iMap, QPolygonF& iPoints)
{
  // all shades have same width and height
  qreal width = m_red_shade->width();
  qreal height = m_red_shade->height();

  std::map< unsigned int, unsigned int>::const_iterator it0;
  std::map< unsigned int, unsigned int>::const_iterator it255;
  it0 = iMap.begin();
  it255 = iMap.end();

  // check x and y
  while(it0!=it255)
    {
    iPoints << QPointF((qreal)(it0->first)*width/255,
                         height*(1-(qreal)(it0->second)/255));
    ++it0;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddLookupTable(vtkLookupTable* iLUT)
{
  m_LUT = iLUT;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddOpacityTransferFunction(vtkPiecewiseFunction* iOpacity)
{
  m_OpacityTF = iOpacity;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddHistogram(vtkImageAccumulate* iHistogram)
{
  int x_range = iHistogram->GetOutput()->GetNumberOfPoints();
  vtkDataArray* scalars = iHistogram->GetOutput()->GetPointData()->GetScalars();
  double* range = iHistogram->GetOutput()->GetScalarRange();

  QVector<qreal> histo;

  // do we want the background values in the histogram...?
  for(int i=0; i<x_range; ++i)
    {
    double value;
    value = scalars->GetTuple1(i);
    histo.push_back(log(value)/log(range[1]));
    }

  m_red_shade->SetHistogram(histo);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddColor(const std::vector<double>& iColor)
{
  m_Color.setRedF(iColor[0]/255);
  m_Color.setGreenF(iColor[1]/255);
  m_Color.setBlueF(iColor[2]/255);
  m_Color.setAlphaF(iColor[3]/255);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddName(QString iChannel)
{
  m_Channel = iChannel;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
presetLUT()
{
  vtkLookupTable *lut = QGoLUTDialog::GetLookupTable( this,
                                                      tr("Choose one look-up table") );
  if ( lut )
    {
      if(m_LUT)
        {
          m_LUT->DeepCopy(lut);
          //m_red_shade->setEnabled(false);
          m_red_shade->setDisabled(true);
          //m_green_shade->setEnabled(false);
          //m_blue_shade->setEnabled(false);
          // update alpha color..

        //m_->UpdateLookupTable(m_LUT);
        // send signal to update the visualization
        emit updateVisualization();
        }
    // free memory since it is not freed in the QGoLUTDialog
    lut->Delete();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
resetLUT()
{
  m_red_shade->Reset();

  // update transfer function
  pointsUpdated();

  //update opacity TF
  updateOpacityTF();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
savePoints()
{
  std::vector< std::map< unsigned int, unsigned int> > pointsVector;
  pointsVector.resize(4);

  // RED ------------------------------
  QPolygonF redPoints = m_red_shade->points();
  computeMapFromPoints(pointsVector[0], redPoints);

  emit updatePoints(m_Channel, pointsVector);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
computeMapFromPoints(std::map< unsigned int, unsigned int>& iMap, const QPolygonF& iPoints)
{
  // all shades have same width and height
  qreal width = m_red_shade->width();
  qreal height = m_red_shade->height();
  int numberOfPoints = iPoints.size();

  if(numberOfPoints>255)
  {
    qDebug() << "Too many points: " << numberOfPoints << " points";
    return;
  }

  for(int i=0; i<numberOfPoints; ++i)
    {
    unsigned int x = (iPoints.at(i).x())*255/width;
    unsigned int y = (1-(iPoints.at(i).y())/height)*255;
    iMap[x]  = y;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
GoTransferFunctionEditorWidget::
eventFilter(QObject *object, QEvent *event)
{
      switch (event->type()) {

      case QEvent::Resize:
        {
        return true;
        }
      default:
        break;
      }
      return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
saveLUT()
{
  /*QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"));
  //qDebug() << "filename: " << fileName;
  QFile file(fileName);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
      //if (!)
         // return 1;

  QTextStream out(&file);

  // format text file - xml style
  // RED ------------------------------
  out <<"<RED> ";
  WriteLUTComponent(m_red_shade, out);
  out <<"</RED>\n";
  // GREEN ------------------------------
  out <<"<GREEN> ";
  WriteLUTComponent(m_green_shade, out);
  out <<"</GREEN>\n";
  // BLUE ------------------------------
  out <<"<BLUE> ";
  WriteLUTComponent(m_blue_shade, out);
  out <<"</BLUE>\n";
  // ALPHA ------------------------------
  out <<"<ALPHA> ";
  WriteLUTComponent(m_alpha_shade, out);
  out <<"</O>\n";

  file.close();*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
WriteLUTComponent(GoTransferFunctionWidget* iTFWidget, QTextStream& iStream)
{
  qreal width = iTFWidget->width();
  qreal height = iTFWidget->height();

  int numberOfPoints = iTFWidget->points().size();

  for(int i=0; i<numberOfPoints; ++i)
    {
    unsigned int x = (iTFWidget->points().at(i).x())*255/width;
    unsigned int y = (1-(iTFWidget->points().at(i).y())/height)*255;
    iStream << x  << " " << y << " ";
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
readLUT()
{
  /*QString fileName = QFileDialog::getOpenFileName(this, tr("Save File"));
  //qDebug() << "filename: " << fileName;
  QFile file(fileName);
  file.open(QIODevice::ReadOnly | QIODevice::Text);

  QTextStream in(&file);
  QString firstBalise;
  in >> firstBalise;

  ReadLUTComponent(m_red_shade, in, "</RED>");
  ReadLUTComponent(m_green_shade, in, "</GREEN>");
  ReadLUTComponent(m_blue_shade, in, "</BLUE>");
  ReadLUTComponent(m_alpha_shade, in, "</ALPHA>");

  file.close();

  // update histogram and alpha gradient
  pointsUpdated();*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
ReadLUTComponent(GoTransferFunctionWidget* iTFWidget, QTextStream& iStream, const QString& iBalise)
{
  /*qreal width = m_red_shade->width();
  qreal height = m_red_shade->height();

  QString x,y;

  QPolygonF points;
  while (1)
    {
    iStream >> x;
    iStream >> y;
    if(x!=iBalise)
      {
      points << QPointF((x.toDouble())*width/255,
                             height*(1-(y.toDouble()/255)));
      }
    else
      {
      iTFWidget->AddPoints(points);
      break;
      }

    }*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
updateOpacityTF()
{
  // all shades have same width and height
  qreal width = m_red_shade->width();
  qreal height = m_red_shade->height();
  int numberOfPoints = m_red_shade->points().size();

  m_OpacityTF->Initialize();

  for(int i=0; i<numberOfPoints; ++i)
    {
    // x 0 to 255
    double x = (m_red_shade->points().at(i).x())*255/width;
    // y 0 to 1
    double y = (1-(m_red_shade->points().at(i).y())/height);
    m_OpacityTF->AddPoint(x, y);
    }
  m_OpacityTF->Modified();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
gammaValueChanged(int iValue)
{
  qDebug() << "gamma value changed: " << iValue;

  // update transfer function
  pointsUpdated();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
minValueChanged(int iValue)
{
  qDebug() << "min value changed: " << iValue;

  // update transfer function
  pointsUpdated();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
maxValueChanged(int iValue)
{
  qDebug() << "max value changed: " << iValue;

  // update transfer function
  pointsUpdated();
}
//-------------------------------------------------------------------------
