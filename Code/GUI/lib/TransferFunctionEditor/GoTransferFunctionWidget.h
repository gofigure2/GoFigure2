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

#ifndef __GoTransferFunctionWidget_h
#define __GoTransferFunctionWidget_h

#include <QtGui>

class HoverPoints;

// vtk
class vtkLookupTable;


class GoTransferFunctionWidget : public QWidget
{
    Q_OBJECT
public:

    GoTransferFunctionWidget(QColor iColor,
                             double iMax,
                             QWidget *parent);

    /**
      * \brief Paint event:
      1- generate new shade is size of the widget changed
      2- draw the shade
      3- draw the histogram
      */
    void paintEvent(QPaintEvent *e);

    QSize sizeHint() const { return QSize(150, 40); }
    QPolygonF points() const;


    /**
      * \brief Add points to the opacity transfer function.
      * Called at initialization or reset.
      * \param[in] iPoints points to create the opacity transfer function
      */
    void AddPointsToOpacityTF(const QPolygonF& iPoints);


    /**
      * \brief Add points to the LUT.
      * Called at initialization or reset.
      * \param[in] iPoints points to create the opacity transfer function
      */
    void AddPointsToLUT(const QPolygonF& iPoints);

    /**
      * \brief Modify LUT with given parameters.
      * \param[in] iLUT pointere to the LUT
      * \param[in] iGamma gamma value
      * \param[in] iMin min window value
      * \param[in] iMin max window value
      */
    void UpdateLookupTable(vtkLookupTable* iLUT,
                           qreal iGamma, qreal iMin, qreal iMax);

    /**
      * \brief Set the histogram
      */
    void SetHistogram(QVector<qreal> iHistogram);

    /**
      * \brief Reset the opacity TF from min to max, from 0 to 1
      */
    void ResetOpacity();

    /**
      * \brief Set the color of the channel.
      * 1- Modify the color
      * 2- Update the shade
      * 3- Update the visualization
      */
    void setColor(QColor iColor);

    /**
      * \brief Set maximum pixel intensity for current channel at current T point
      */
    void setMax(double iMax);

signals:
    /**
      * \brief Point added in the Opacity TF then update the visualization
      */
    void opacityChanged();

    /**
      * \brief enable/disable opacity TF
      */
    void enableOpacityTF(bool);

    /**
      * \brief enable/disable LUT curve
      */
    void enableLUTCurve(bool);

private:
    /**
      * \brief Generate the shade when the color changed.
      */
    void generateShade();

    /**
      * \brief Color of the current channel
      */
    QColor m_color;

    /**
      * \brief Shade generated, based on the color
      */
    QImage m_shade;

    /**
      * \brief Opacity transfer function points
      */
    HoverPoints *m_OpacityTFPoints;

    /**
      * \brief LUT points
      */
    HoverPoints *m_LUTPoints;

    /**
      * \brief the histogram
      */
    QVector<qreal> m_Histogram;

    /**
      * \brief Maximum pixel value in the current channel at given time point
      */
    double m_Max;
};

#endif
