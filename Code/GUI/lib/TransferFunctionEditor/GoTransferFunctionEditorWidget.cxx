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
#include "hoverpoints.h"

//qt
#include <QSpacerItem>
#include <QColorDialog>

//vtk
#include "vtkLookupTable.h"
#include "vtkImageAccumulate.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPiecewiseFunction.h"

// std
#include <iostream>

//-------------------------------------------------------------------------

GoTransferFunctionEditorWidget::
GoTransferFunctionEditorWidget(QString iChannel,
                               const std::vector<double>& iColor,
                               std::vector<int> iLUTParameters,
                               double iMax,
                               QWidget *parent ) : QWidget(parent )
{
  // set current color of the channel
  m_Color.setRedF(iColor[0]/255);
  m_Color.setGreenF(iColor[1]/255);
  m_Color.setBlueF(iColor[2]/255);
  m_Color.setAlphaF(iColor[3]/255);

  // save original color of the channel if the user want to reset it
  m_Color_original = m_Color;

  // set the maximum value in the image
  m_Max = iMax;
  // init LUT to NULL
  m_LUT = NULL;
  // set channel name
  m_Channel = iChannel;

  // global layout of the widget
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setSpacing(1);
  vbox->setMargin(1);

  // first part of the widget: the color
  /*
  ---------------------------
  |          _____________  |
  |  Color: |_P_U_S_H_____| |
  |                         |
  ---------------------------
  */
  // items
  //label
  QLabel *color = new QLabel("Color: ");
  // push button
  m_ColorPushButton = new QPushButton(this);
  m_ColorPushButton->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  QString style = "background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 black, stop: 1 rgb(%1, %2, %3)); border-radius: 4px;";
  m_ColorPushButton->setStyleSheet(
        style.arg(m_Color.red()).arg(m_Color .green()).arg(m_Color.blue()));
  m_ColorPushButton->setToolTip("Click to modify the color of this channel");
  // add items to layout
  QHBoxLayout *colorLayout = new QHBoxLayout;
  colorLayout->addWidget(color);
  colorLayout->addWidget(m_ColorPushButton);
  // connect signals
  connect(m_ColorPushButton, SIGNAL(clicked()), this, SLOT(ChangeColor()));

  // add layout to the widget
  vbox->addLayout(colorLayout);


  // second part of the widget: the TH Widget
  /*
  -----------------------------
  |  |------------------|--|  |
  |   _____________________   |
  |  |                     |  |
  |  |                     |  |
  |  |                     |  |
  |  |                     |  |
  |  |_____________________|  |
  |                           |
  |  |---|-----------------|  |
  -----------------------------
  */
  // items
  // Left spacer
  QSpacerItem* spacer_red1 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Minimum);
  // right spacer
  QSpacerItem* spacer_red2 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Minimum);
  // TF widget
  m_TFWidget = new GoTransferFunctionWidget(
        m_Color,
        m_Max,
        this);
  //min slider
  m_MinSlider = new QSlider(this);
  m_MinSlider->setObjectName("min");
  m_MinSlider->setOrientation(Qt::Horizontal);
  m_MinSlider->setMaximum(m_Max);
  m_MinSlider->setValue(iLUTParameters[1]);
  m_MinSlider->setStyleSheet("QSlider::groove:horizontal {border: 1px solid #bbb;background: rgba(0, 0, 0, 0);height: 4px;position: absolute; right: 10px;left: 10px; }QSlider::handle:horizontal {image: url(/home/nr52/gitroot/gofigure/Resources/widget/arrow_up.png);width: 20px;height: 6px;margin-top: -2px;margin-bottom: -2px; right: -10px; left:-10px; border: 1px solid black; background: rgba(255, 255, 255, 200); border-radius: 4px;}QSlider::sub-page:horizontal {background: #909090;border: 1px solid black;}QSlider::add-page:horizontal {background: rgba(0, 0, 0, 0);border: 1px solid black;}");
  // max slider
  m_MaxSlider = new QSlider(this);
  m_MaxSlider->setObjectName("max");
  m_MaxSlider->setOrientation(Qt::Horizontal);
  m_MaxSlider->setMaximum(m_Max);
  m_MaxSlider->setValue(m_Max);
  m_MaxSlider->setStyleSheet("QSlider::groove:horizontal {border: 1px solid #bbb;background: rgba(0, 0, 0, 0);height: 4px;position: absolute; right: 10px;left: 10px; }QSlider::handle:horizontal {image: url(/home/nr52/gitroot/gofigure/Resources/widget/arrow_down.png);width: 20px;height: 6px;margin-top: -2px;margin-bottom: -2px; right: -10px; left:-10px; border: 1px solid black; background: rgba(255, 255, 255, 200); border-radius: 4px;}QSlider::sub-page:horizontal {background: rgba(0, 0, 0, 0);border: 1px solid black;}QSlider::add-page:horizontal {background: #909090;border: 1px solid black;}");
  // more spacer
  QSpacerItem* spacerShade1 =
      new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Minimum);
  // more spacer
  QSpacerItem* spacerShade2 =
      new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Minimum);
  // connect signals
  connect(m_MinSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSliders(int)));
  connect(m_MaxSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSliders(int)));
  // add items to layout 1
  QHBoxLayout *m_TFWidget_layout = new QHBoxLayout;
  m_TFWidget_layout->addSpacerItem(spacer_red1);
  m_TFWidget_layout->addWidget(m_TFWidget);
  m_TFWidget_layout->addSpacerItem(spacer_red2);
  // add items to layout 2
  QVBoxLayout* shadeVerticalLayout = new QVBoxLayout;
  shadeVerticalLayout->addWidget(m_MaxSlider);
  shadeVerticalLayout->addLayout(m_TFWidget_layout);
  shadeVerticalLayout->addWidget(m_MinSlider);
  // add items to layout 3
  QHBoxLayout* shadeHorizontalLayout=  new QHBoxLayout;
  shadeHorizontalLayout->addItem(spacerShade1);
  shadeHorizontalLayout->addLayout(shadeVerticalLayout);
  shadeHorizontalLayout->addItem(spacerShade2);
  // connect signals
  connect(m_TFWidget, SIGNAL(opacityChanged()), this, SLOT(updateOpacityTF()));

  // add layout to the widget
  vbox->addLayout(shadeHorizontalLayout);


  // third part:  the gamma slider
  /*
  ------------------------------
  | Gamma: |---|------------|  |
  ------------------------------
  */
  // items
  // gamma label
  QLabel* gammaName = new QLabel((QChar)(0x0263));
  // gamma slider
  m_GammaSlider = new QSlider(this);
  m_GammaSlider->setOrientation(Qt::Horizontal);
  m_GammaSlider->setMaximum(199);
  m_GammaSlider->setMinimum(1);
  m_GammaSlider->setValue(iLUTParameters[0]);
  // connect signals
  connect(m_GammaSlider, SIGNAL(valueChanged(int)), this, SLOT(UpdateLUT()));
  // add items to layout
  QHBoxLayout *gammaLayout = new QHBoxLayout;
  gammaLayout->addWidget(gammaName);
  gammaLayout->addWidget(m_GammaSlider);

  // add layout to the widget
  vbox->addLayout(gammaLayout);


  // fourth part:  the visibility checkboxes
  /*
  ------------------------------
  | + Color TF                 |
  | - Opacity TF               |
  | + Log Histogram            |
  ------------------------------
  */
  // items
  // color TF checkbox
  QCheckBox* tfCB = new QCheckBox("Color Transfer Function");
  tfCB->setChecked(true);
  QString style5 = "color: black; border: 1px solid rgb(0, 0, 0); background-color: rgba(255, 255, 255, 150); border-radius: 4px;";
  tfCB->setStyleSheet(style5);
  // opacity TF checkbox
  QCheckBox* tfoCB = new QCheckBox("Opacity Transfer Function");
  tfoCB->setChecked(false);
  QString style2 = "color: white; border: 1px solid rgb(0, 0, 0); background-color: rgba(0, 0, 0, 150); border-radius: 4px;";
  tfoCB->setStyleSheet(style2);
  // histogram checkbox
  QCheckBox* histogramCB = new QCheckBox("Log Histogram");
  histogramCB->setChecked(true);
  QString style3 = "border: 1px solid rgb(0, 0, 0); background-color: rgba(0, 0, 0, 0); border-radius: 4px;";
  histogramCB->setStyleSheet(style3);
  // connect signals
  connect(tfoCB, SIGNAL(clicked(bool)), m_TFWidget, SIGNAL(enableOpacityTF(bool)));
  connect(tfCB, SIGNAL(clicked(bool)), m_TFWidget, SIGNAL(enableLUTCurve(bool)));
  connect(histogramCB, SIGNAL(clicked(bool)), this, SLOT(ShowHistogram(bool)));

  // add items to the widget
  vbox->addWidget(tfCB);
  vbox->addWidget(tfoCB);
  vbox->addWidget(histogramCB);

  // fith part: apply/reset pushbutton
  /*
  ---------------------------
  |  _________   _________  |
  | |_A_PPL_Y_| |_R_ESE_T_| |
  |                         |
  ---------------------------
  */
  // items
  // apply pushbutton
  QPushButton *okPushButton = new QPushButton("Apply", this);
  // reset pushbutton
  QPushButton *ResetLUTPushButton = new QPushButton("Reset", this);
  // connect signals
  connect(okPushButton, SIGNAL(released()), this, SLOT(ApplyChanges()));
  connect(ResetLUTPushButton, SIGNAL(pressed()), this, SLOT(ResetLUT()));
  // layout
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(okPushButton);
  layout->addWidget(ResetLUTPushButton);

  // add layout to widget
  vbox->addLayout(layout);

  this->setLayout(vbox);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
UpdateLUT()
{
  if(m_LUT)
    {
    qreal side = m_GammaSlider->value()/(100);
    int ope = pow(-1, side + 1);
    qreal value = (qreal)(ope*(m_GammaSlider->value()-100) + ope);

    qreal gamma_value;
    if(value > 2)
      {
      qreal temp = log(value);
      gamma_value = pow(temp, ope);
      }
    else
      {
      gamma_value = 1;
      }

    // update the LUT
    m_TFWidget->UpdateLookupTable(m_LUT,
                                  gamma_value,
                                  (qreal)m_MinSlider->value(),
                                  (qreal)m_MaxSlider->value());

    // send signal to update the visualization
    emit updateVisualization();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddPoints( const std::map< unsigned int, unsigned int>& iPoints)
{
  // points
  QPolygonF points;
  // convert points
  ConvertSTDMapToQPolygonF(iPoints, points);
  // add points to the opacity TF
  m_TFWidget->AddPointsToOpacityTF(points);
  // add points to the LUT
  m_TFWidget->AddPointsToLUT(points);
  // update LUT in widget and visualization
  UpdateLUT();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
ConvertSTDMapToQPolygonF(const std::map< unsigned int, unsigned int>& iMap,
                         QPolygonF& iPoints)
{
  // all shades have same width and height
  qreal width = m_TFWidget->width();
  qreal height = m_TFWidget->height();

  std::map< unsigned int, unsigned int>::const_iterator it0;
  std::map< unsigned int, unsigned int>::const_iterator it255;
  it0 = iMap.begin();
  it255 = iMap.end();

  // check x and y
  while(it0!=it255)
    {
    iPoints << QPointF((qreal)(it0->first)*width/m_Max,
                         height*(1-(qreal)(it0->second)/m_Max));
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
  // get the x range
  int x_range = iHistogram->GetOutput()->GetNumberOfPoints();
  // get the y values
  vtkDataArray* scalars = iHistogram->GetOutput()->GetPointData()->GetScalars();
  // get the scalar values to normalize
  double* range = iHistogram->GetOutput()->GetScalarRange();

  QVector<qreal> histogram;

  for(int i=0; i<x_range; ++i)
    {
    double value;
    value = scalars->GetTuple1(i);
    histogram.push_back(log(value)/log(range[1]));
    }

  m_Histogram = histogram;

  // add histogram the the TF Widget
  m_TFWidget->SetHistogram(m_Histogram);

  // update the widget so the histogram is visible
  m_TFWidget->update();
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
ResetLUT()
{
  // reset the color
  m_Color = m_Color_original;
  // reset the opacity TF
  m_TFWidget->ResetOpacity();
  // reset the color
  m_TFWidget->setColor(m_Color);

  // reset button color
  QString style = "background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 black, stop: 1 rgb(%1, %2, %3)); border-radius: 4px;";
  m_ColorPushButton->setStyleSheet(
        style.arg(m_Color.red()).arg(m_Color .green()).arg(m_Color.blue()));

  // Reset Gamma, Min and Max
  m_GammaSlider->setValue(100);
  m_MinSlider->setValue(0);
  m_MaxSlider->setValue(m_Max);
  // Update LUT
  UpdateLUT();

  // might be buggy
  updateOpacityTF();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
ApplyChanges()
{
  // opacity
  std::map< unsigned int, unsigned int> pointsVector;
  QPolygonF opacityPoints = m_TFWidget->points();
  ConvertQPolygonFToSTDMap(opacityPoints, pointsVector);

  // send values to the image structure
  emit UpdateImageStructure(m_Channel,
                            pointsVector,
                            m_Color,
                            m_MinSlider->value(),
                            m_MaxSlider->value(),
                            m_GammaSlider->value());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
ConvertQPolygonFToSTDMap(const QPolygonF& iPoints, std::map< unsigned int, unsigned int>& iMap)
{
  qreal width = m_TFWidget->width();
  qreal height = m_TFWidget->height();
  int numberOfPoints = iPoints.size();

  if(numberOfPoints>m_Max)
  {
    qDebug() << "Too many points: " << numberOfPoints << " points";
    return;
  }

  for(int i=0; i<numberOfPoints; ++i)
    {
    unsigned int x = (iPoints.at(i).x())*m_Max/width;
    unsigned int y = (1-(iPoints.at(i).y())/height)*m_Max;
    iMap[x]  = y;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
updateOpacityTF()
{
  // all shades have same width and height
  qreal width = m_TFWidget->width();
  qreal height = m_TFWidget->height();
  int numberOfPoints = m_TFWidget->points().size();

  m_OpacityTF->Initialize();

  for(int i=0; i<numberOfPoints; ++i)
    {
    // x 0 to 255
    double x = (m_TFWidget->points().at(i).x())*m_Max/width;
    // y 0 to 1
    double y = (1-(m_TFWidget->points().at(i).y())/height);
    m_OpacityTF->AddPoint(x, y);
    }
  m_OpacityTF->Modified();

  emit updateVisualization();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
ChangeColor()
{
  QColor color = QColorDialog::getColor();
  // if we selected a color and clicked on "OK"
  if(color.isValid())
    {
    m_Color = color;
    }

  // update button
  QString style = "background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 black, stop: 1 rgb(%1, %2, %3)); border-radius: 4px;";
  m_ColorPushButton->setStyleSheet(
        style.arg(m_Color.red()).arg(m_Color .green()).arg(m_Color.blue()));

  // update shade
  m_TFWidget->setColor(m_Color);

  // update LUT
  UpdateLUT();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
ShowHistogram(bool iShow){

  if(iShow)
    {
    m_TFWidget->SetHistogram(this->m_Histogram);
    }
  else
    {
    QVector<qreal> removeHisto;
    m_TFWidget->SetHistogram(removeHisto);
    }

  // to be tested might be buggy
  // update might be better?
  m_TFWidget->repaint();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AdjustWindowLevel(double iMin, double iMax){

  this->m_MinSlider->blockSignals(true);
  this->m_MinSlider->setValue(iMin);
  this->m_MinSlider->blockSignals(false);
  this->m_MaxSlider->blockSignals(true);
  this->m_MaxSlider->setValue(iMax);
  this->m_MaxSlider->blockSignals(false);

  UpdateLUT();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
SetMaximumValue( double iMax)
{
  m_Max = iMax;
  m_TFWidget->setMax(m_Max);

  if(m_MaxSlider->maximum() > m_Max)
    {
    m_MaxSlider->setValue(iMax);
    }
  else
    {
    m_MaxSlider->setValue(m_MaxSlider->value());
    }

  m_MinSlider->setMaximum(m_Max);
  m_MaxSlider->setMaximum(m_Max);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
updateSliders( int iValue)
{
  // min and max must be separated by at least 2 to create a consitent LUT
  // since a consistent LUT need 2 values at least.
  /*
                      need value in between
                              ^
     LUT: MIN VALUE | color 1 | color 2 | MAX VALUE
  */
  if(m_MinSlider->value() > m_MaxSlider->value() - 2)
    {
    QString name = QObject::sender()->objectName();
    if(! name.compare("min") )
      {
      m_MinSlider->setValue(iValue - 1);
      }
    else
      {
      m_MaxSlider->setValue(iValue + 1);
      }
      return;
    }

  UpdateLUT();
}

