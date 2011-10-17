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
#ifndef __GoTransferFunctionEditorWidget_h
#define __GoTransferFunctionEditorWidget_h

#include "QGoGUILibConfigure.h"

// Qt
#include <QWidget>
#include <QBrush>

//std
#include <vector>
#include <map>

// gofigure
class HoverPoints;
class GoTransferFunctionWidget;

// qt
class QPolygonF;
class QTextStream;
class QPushButton;
class QSlider;

//vtk
class vtkLookupTable;
class vtkImageAccumulate;
class vtkPiecewiseFunction;

class GoTransferFunctionEditorWidget : public QWidget
{
  Q_OBJECT
public:
  GoTransferFunctionEditorWidget(QWidget *parent, QString iChannel,
                                 const std::vector<double>& iColor,
                                 std::vector<int> iLUTParameters,
                                 double iMax);

  /**
   * \brief Add points to both LUT and Opacity transfer function
   * \param[in] iPoints map containing the points. The key is the "X" poisition,
   * the value is the "Y" position.
   */
  void AddPoints( const std::map<unsigned int, unsigned int >& iPoints);

  /**
   * \brief Add LUT to the GoTransferFunctionEditorWidget
   * \param[in] iLUT the vtk Look Up Table
   */
  void AddLookupTable(vtkLookupTable* iLUT);

  /**
   * \brief 1- Add Histogram to the GoTransferFunctionEditorWidget
   * 2- Convert it to QVector<qreal>
   * 3- Add it to the m_TFWidget
   * 4- Update the m_TFWidget
   * \param[in] iHistogram a vtkImage Accumulate
   */
  void AddHistogram(vtkImageAccumulate* iHistogram);

  /**
   * \brief Add color of the channel to the GoTransferFunctionEditorWidget.
   It modifies m_Color. m_Color is necessary for reset and to initialize the
   m_TFWidget;
   \param[in] iColor is a vector of size 4 (RGBA), with values between 0 and 255.
   Add color then normalize the values between 0 and 1.
   */
  void AddColor(const std::vector<double>& iColor);

  /**
   * \brief Add name of the channel to the GoTransferFunctionEditorWidget.
   * \param[in] iChannel name of the channel
   */
  void AddName(QString iChannel);

  /**
    * \brief Add pointer to the opacity TF for a direct access.
    */
  void AddOpacityTransferFunction(vtkPiecewiseFunction* iOpacity);

  /**
    * \bried Set maximum value of the current channel. Useful to adjust
    the range of the LUT.
    */
  void SetMaximumValue( double iMax);

public slots:
  /**
    * \brief Update the LUT based on the min, max and gamma.
    * It updates the m_TFWidget and the visualition.
    */
  void UpdateLUT();
  /**
    * \brief Opens a QColorDialog. If chosen color is valid, then:
    1- Modify push button color
    2- Modify m_TFWidget color
    3- Update LUT
    */
  void ChangeColor();

  void resetLUT();

  void saveAll();

  // opacity TF
  void updateOpacityTF();
  void showHistogram(bool iEnable);
  void AdjustWindowLevel(double, double);
  void updateSliders(int);

signals:
  void updateVisualization();
  void updatePoints(QString,
                    std::map< unsigned int, unsigned int>,
                    QColor,
                    int,
                    int,
                    int);

private:

  bool eventFilter(QObject *object, QEvent *event);

  void computeMapFromPoints(
    std::map< unsigned int, unsigned int>& iMap, const QPolygonF& iPoints);
  void ConvertSTDMapToQPolygonF(
    const std::map< unsigned int, unsigned int>& iMap, QPolygonF& iPoints);

  void WriteLUTComponent(GoTransferFunctionWidget* iTFWidget,
                         QTextStream& iStream);
  void ReadLUTComponent(GoTransferFunctionWidget* iTFWidget,
                        QTextStream& iStream,
                        const QString& iBalise);

  QPushButton* m_ColorPushButton;

  GoTransferFunctionWidget *m_TFWidget;

  QSlider* m_GammaSlider;
  QSlider* m_MinSlider;
  QSlider* m_MaxSlider;

  QColor                    m_Color;
  // for reset
  QColor                    m_Color_original;
  QString                   m_Channel;

  vtkLookupTable           *m_LUT;
  vtkPiecewiseFunction     *m_OpacityTF;

  QVector<qreal>            m_Histogram;

  double m_Max;
};

#endif
