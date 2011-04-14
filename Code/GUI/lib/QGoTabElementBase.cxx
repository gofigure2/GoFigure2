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

#include "QGoTabElementBase.h"

#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QPluginLoader>

#include <iostream>

//--------------------------------------------------------------------------
QGoTabElementBase::QGoTabElementBase(QWidget *iParent) : QWidget(iParent),
  m_StatusBar(NULL)
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabElementBase::~QGoTabElementBase()
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction * > QGoTabElementBase::ViewActions()
{
  return m_ViewActions;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction * > QGoTabElementBase::SegmentationActions()
{
  return m_SegmentationActions;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction * > QGoTabElementBase::ToolsActions()
{
  return m_ToolsActions;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction * > QGoTabElementBase::BookmarkActions()
{
  return m_BookmarkActions;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction * > QGoTabElementBase::ModeActions()
{
  return m_ModeActions;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction * > QGoTabElementBase::TracesActions()
{
  return m_TracesActions;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QWidget*  QGoTabElementBase::TraceSettingsWidget()
{
  return m_TraceSettingsWidget;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< QGoTabElementBase::QGoDockWidgetStatusPair > &
QGoTabElementBase::DockWidget()
{
  return m_DockWidgetList;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< QAction * > QGoTabElementBase::GetPluginActions()
{
  return m_PluginActionList;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabElementBase::SetPluginActions(std::list< QAction * > iList)
{
  m_PluginActionList = iList;
}

//--------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabElementBase::CreateModeActions(QActionGroup *group)
{
  //---------------------------------//
  //           default mode          //
  //---------------------------------//

  // Create/initialize the default action
  QAction *DefaultAction = new QAction(tr("Default"), this);

  DefaultAction->setCheckable(true);
  DefaultAction->setChecked(true);

  QIcon DefaultIcon;
  DefaultIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/mouse-cursor.png") ),
                        QIcon::Normal, QIcon::Off);
  DefaultAction->setIcon(DefaultIcon);

  group->addAction(DefaultAction);

  // it also updates the interactor behaviour
  QObject::connect( DefaultAction, SIGNAL( toggled(bool) ),
                    this, SLOT( DefaultInteractorBehavior(bool) ) );

  this->m_ModeActions.push_back(DefaultAction);

  //---------------------------------//
  //            Zoom mode            //
  //---------------------------------//

  QAction *ZoomAction = new QAction(tr("Zoom"), this);
  ZoomAction->setCheckable(true);
  ZoomAction->setChecked(false);

  QIcon ZoomIcon;
  ZoomIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/zoom.png") ),
                     QIcon::Normal, QIcon::Off);
  ZoomAction->setIcon(ZoomIcon);

  group->addAction(ZoomAction);

  this->m_ModeActions.push_back(ZoomAction);
  // it also updates the interactor behaviour
  QObject::connect( ZoomAction, SIGNAL( toggled(bool) ),
                    this, SLOT( ZoomInteractorBehavior(bool) ) );

  //---------------------------------//
  //            Pan  mode            //
  //---------------------------------//

  QAction *PanAction = new QAction(tr("Pan"), this);
  PanAction->setCheckable(true);
  PanAction->setChecked(false);

  QIcon PanIcon;
  PanIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/Hand.png") ),
                    QIcon::Normal, QIcon::Off);
  PanAction->setIcon(PanIcon);

  group->addAction(PanAction);

  this->m_ModeActions.push_back(PanAction);
  // it also updates the interactor behaviour
  QObject::connect( PanAction, SIGNAL( toggled(bool) ),
                    this, SLOT( PanInteractorBehavior(bool) ) );
}

//--------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabElementBase::SetStatusBarPointer(QStatusBar *iStatusbar)
{
  this->m_StatusBar = iStatusbar;
}