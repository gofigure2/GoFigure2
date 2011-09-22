/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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

#ifndef __QGoNavigationDockWidget_h
#define __QGoNavigationDockWidget_h

#include <QComboBox>
#include "ui_NavigationDockWidget.h"
#include "GoFigureGlobalDefinition.h"
#include "QGoDockWidget.h"

#include "QGoGUILibConfigure.h"

class QHBoxLayout;
class QVBoxLayout;

class QPushButton;
class QCheckBox;

/** \class QGoNavigationDockWidget
 *  \brief Dock Widget for browsing images (changing slice, time point...)
 *  \ingroup GUI
 */
class QGOGUILIB_EXPORT QGoNavigationDockWidget:
  public QGoDockWidget,
  private Ui::NavigationDockWidget
{
  Q_OBJECT
public:
  /** \brief Constructor */
  explicit QGoNavigationDockWidget(
    QWidget *parent = 0,
    const GoFigure::TabDimensionType & iDim = GoFigure::THREE_D_WITH_T);

  /** \brief Destructor */
  ~QGoNavigationDockWidget();

 /** \brief Set the extent of the images in the X direction
 * \param[in] iMin XMin
 * \param[in] iMax XMax
 */
  void SetXMinimumAndMaximum(const int & iMin, const int & iMax);

 /** \brief Set the extent of the images in the Y direction
 * \param[in] iMin YMin
 * \param[in] iMax YMax
 */
  void SetYMinimumAndMaximum(const int & iMin, const int & iMax);

  /** \brief Set the extent of the images in the Z direction
 * \param[in] iMin ZMin
 * \param[in] iMax ZMax
 */
 void SetZMinimumAndMaximum(const int & iMin, const int & iMax);

 /** \brief Set the extent of the images in the Time
 * \param[in] iMin TMin
 * \param[in] iMax TMax
 */
  void SetTMinimumAndMaximum(const int & iMin, const int & iMax);

  // new channels representation
  void AddChannel(const QString& iName, const QColor& iColor,const unsigned int& iNumber,
                  const bool& iChecked);

  void ModifyChannel(QString iName, QColor iColor);

  void VisibilityListChannels(const bool& iVisibility);

  void AddDoppler(const QString& iName, const QColor& iColor,const unsigned int& iNumber,
                  const bool& iChecked);

  void VisibilityListDoppler(const bool& iVisibility);

  void setChannelName(QString iChannelName);

  void DeleteDopplerWidgets();

public slots:
  /** Set X Slice */
  void SetXSlice(int iSlice);

  /** Set Y Slice */
  void SetYSlice(int iSlice);

  /** Set Z Slice */
  void SetZSlice(int iSlice);

  /** Set Time Point */
  void SetTSlice(int iSlice);

  /** Move to the previous time point using shortcuts */
  void MoveToPreviousTimePoint();

  /** Move to the next time point using shortcuts */
  void MoveToNextTimePoint();

  void UpdateWidget(int);

  void visibilityChanged(bool);

  void changeColor();

signals:
  void XSliceChanged(int Slice);

  void YSliceChanged(int Slice);

  void ZSliceChanged(int Slice);

  void TSliceChanged(int Slice);

  void ModeChanged(int Mode);

  void StepChanged(int Step);

  void visibilityChanged(QString, bool);

  void openTransferFunctionEditor(QString);

  void DopplerSizeChanged(int iSize);

protected:
  GoFigure::TabDimensionType m_Dimension;

private:
  QList<QPushButton*> m_ListPushButtons;
  QList<QCheckBox*> m_ListCheckBoxes;
  QList<QWidget*> m_ListDoppler;
};
#endif
