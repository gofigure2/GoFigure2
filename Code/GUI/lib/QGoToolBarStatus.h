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

#ifndef __QGoToolBarStatus_h
#define __QGoToolBarStatus_h

#include <Qt>
#include <QObject>
#include <QAction>
#include <QToolBar>
#include <QMenu>

class QGoToolBarStatus:public QObject
{
  Q_OBJECT
public:
  explicit QGoToolBarStatus(QWidget* iParent = 0);

  explicit QGoToolBarStatus(const QGoToolBarStatus & iS);

  explicit QGoToolBarStatus(  Qt::ToolBarArea iArea,
                               const bool & iVisibility,
                               const bool & iAttached,
                               QWidget* iParent = 0);

  explicit QGoToolBarStatus(QToolBar* iToolBar, QMenu* iMenu, 
                            Qt::ToolBarArea iArea,
                            const bool & iVisibility, 
                            const bool & iAttached,
                            QWidget* iParent = 0,
                            QWidget* iWidget = 0);

  virtual ~QGoToolBarStatus();

  void InitializeToolBarAndMenu(QToolBar* iToolBar, QMenu* iMenu);

  /** \brief clear the toolbar and disconnect all the slots 
  inside this class connected to the toolbar*/
  void ClearToolBar();

  /** \brief add all the actions coontained in the m_VectorAction
  into the toolbar and connect the slots*/
  void SetUpToolBar();

  /*\\todo Nicolas why public??
    */
  // variables - why public....?
  QToolBar * m_ToolBar;
  QMenu *    m_Menu;
  /** \brief Position */
  Qt::ToolBarArea m_Area;
  Qt::ToolBarArea m_DefaultArea;
  /** \brief is Visible */
  bool m_Visibility;
  /** \brief Attached to QGoMainWindow*/
  bool m_Attached;

  std::vector< QAction* > m_VectorAction;

public slots:
  /** \brief set the area of the m_ToolBar*/
  void SetArea(Qt::ToolBarArea iArea);

  /** \brief set the visibility of the m_ToolBar*/
  void SetVisibility(bool iVisibility);

  /** \brief set the floated status of the m_ToolBar*/
  void SetAttached(bool iAttached);

protected:
  QWidget*      m_Widget;
  /** \brief set the signal slots connections to update m_visibility, m_Attached and 
  m_Area with the toolbar */
  //void Connect();
  /** \brief disconnect all the signal slots with the toolbar*/
  //void Disconnect();
};
#endif
