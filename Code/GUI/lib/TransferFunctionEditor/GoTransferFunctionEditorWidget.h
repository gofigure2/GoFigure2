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
  explicit GoTransferFunctionEditorWidget(  QString iChannel,
                                            const std::vector<double>& iColor,
                                            std::vector<int> iLUTParameters,
                                            double iMax,
                                            QWidget *parent = 0 );

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

  /**
    * \brief Reset the channel to its original state (color, LUT, opacity)
    */
  void ResetLUT();

  /**
    * \brief Apply changes (color, min, manx, gamma, opacity TF) to the channel.
    It modifies informations relative to the given channel in the QGoImageStructure.
    */
  void ApplyChanges();

  // opacity TF
  // might be buggy, to be checked
  void updateOpacityTF();

  /**
    * \brief Show/hide histogram in m_TFWidget
    * \param[in] iShow true: paint current histogram, false paint an empty histogram
    */
  void ShowHistogram(bool iShow);

  /**
    * \brief Adjust window/level between the input values.
    * LUT (using the gamma value) will be calculated between those 2 points.
    * Before iMin, LUT = (0, 0, 0)
    * After iMax, LUT = ChannelColor.
    * It modifies the values of the sliders then update the LUT.
    * \param[in] iMin pixel intensity below which the pixels will be mapped to black
    * \param[in] iMax pixel intensity above which the pixels will be mapped to the channel color
    */
  void AdjustWindowLevel(double iMin, double iMax);


  /**
    * \brief When Min and Max sliders change, update the value of the one which
    * sent the signal, if it doesn't overlap with the other one.
    * param[in] iValue new value. Else modify its value back to original.
    */
  void updateSliders(int iValue);

signals:
  /**
    * \brief Modify the visualization when the LUT or opacity changed
    */
  void updateVisualization();

  /**
  * \brief Modify the image structure when apply clicked
  */
  void UpdateImageStructure(QString,
                            std::map< unsigned int, unsigned int>,
                            QColor,
                            int,
                            int,
                            int);

private:
  /**
    * \brief Convenience method to convert a QPolygonF to a std::map
    * \param[in] iPoints QPolygonF to be converted
    * \param[out] iMap std::map to be filled
    */
  void ConvertQPolygonFToSTDMap(
    const QPolygonF& iPoints, std::map< unsigned int, unsigned int>& iMap);

  /**
    * \brief Convenience method to convert a std::map to a QPolygonF
    * \param[in] iMap std::map to be converted
    * \param[out] iPoints QPolygonF to be filled
    */
  void ConvertSTDMapToQPolygonF(
    const std::map< unsigned int, unsigned int>& iMap, QPolygonF& iPoints);

  /**
    * \brief Push button to modify the color
    */
  QPushButton* m_ColorPushButton;

  /**
    * \brief The widget containing the shade, histogram, LUT and opacity TF
    */
  GoTransferFunctionWidget *m_TFWidget;

  /**
    * \brief Slider to modify the value of gamma
    */
  QSlider* m_GammaSlider;

  /**
    * \brief Slider to modify the min value
    */
  QSlider* m_MinSlider;

  /**
    * \brief Slider to modify the max value
    */
  QSlider* m_MaxSlider;

  /**
    * \brief Current color of the channel
    */
  QColor                    m_Color;

  /**
    * \brief Original color of the channel
    */
  QColor                    m_Color_original;

  /**
    * \brief Name of the channel
    */
  QString                   m_Channel;

  /**
    * \brief Pointer to the channel LUT
    */
  vtkLookupTable           *m_LUT;

  /**
    * \brief Pointer to the channel opacity LUT
    */
  vtkPiecewiseFunction     *m_OpacityTF;

  /**
    * \brief Histogram of the current channel for current time point
    */
  QVector<qreal>            m_Histogram;

  /**
    * \brief Maximum pixel intensity for the channel at current time point
    */
  double m_Max;
};

#endif
