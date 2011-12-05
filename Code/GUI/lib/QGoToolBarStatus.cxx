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

#include "QGoToolBarStatus.h"

#include <iostream>

QGoToolBarStatus::QGoToolBarStatus(QWidget* iParent) :
  QObject(iParent),
  m_ToolBar(NULL), m_Menu(NULL), m_Area(Qt::TopToolBarArea),
  m_DefaultArea(Qt::TopToolBarArea), m_Visibility(true), m_Attached(true),
  m_Widget(NULL)
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoToolBarStatus::QGoToolBarStatus(const QGoToolBarStatus & iS) :
    m_ToolBar(iS.m_ToolBar), m_Menu(iS.m_Menu), m_Area(iS.m_Area),
    m_DefaultArea(iS.m_Area), m_Visibility(iS.m_Visibility),
    m_Attached(iS.m_Attached), m_VectorAction(iS.m_VectorAction),
    m_Widget(iS.m_Widget)
{
  this->setParent(iS.parent());
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoToolBarStatus::QGoToolBarStatus(Qt::ToolBarArea iArea,
                                    const bool & iVisibility, const bool & iAttached,
                                    QWidget* iParent) :
  QObject(iParent),
  m_ToolBar(NULL), m_Menu(NULL), m_Area(iArea),
  m_DefaultArea(iArea), m_Visibility(iVisibility),
  m_Attached(iAttached), m_Widget(NULL)
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoToolBarStatus::QGoToolBarStatus(QToolBar* iToolBar, QMenu* iMenu,
  Qt::ToolBarArea iArea,
    const bool & iVisibility, const bool & iAttached, QWidget* iParent, QWidget* iWidget):
  QObject(iParent), m_ToolBar(iToolBar), m_Menu(iMenu),
  m_Area(iArea), m_Visibility(iVisibility), m_Attached(iAttached),
  m_Widget(iWidget)
{
  if(m_Widget && m_ToolBar)
    {
    QAction* Action = this->m_ToolBar->addWidget(iWidget);
    Action->setVisible(true);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoToolBarStatus::
~QGoToolBarStatus()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoToolBarStatus::SetArea(Qt::ToolBarArea iArea)
{
  m_Area = iArea;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoToolBarStatus::SetVisibility(bool iVisibility)
{
   m_Visibility = iVisibility;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoToolBarStatus::SetAttached(bool iAttached)
{
  m_Attached = iAttached;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoToolBarStatus::SetUpToolBar()
{
  //if (this->m_ToolBar)
  //  {
    for ( std::vector< QAction * >::iterator it = this->m_VectorAction.begin();
      it != this->m_VectorAction.end();
            ++it )
      {
      if (this->m_ToolBar)
        {
        this->m_ToolBar->addAction(*it);
        }
      if (this->m_Menu)
        {
        this->m_Menu->addAction(*it);
        }
      }

    if (this->m_Widget)
      {
      QAction* Action = this->m_ToolBar->addWidget(this->m_Widget);
      Action->setVisible(true);
      }

    if(this->m_ToolBar)
      {
      this->m_ToolBar->setVisible(this->m_Visibility);
      }

    if (!this->m_ToolBar && !this->m_Menu)
      {
      std::cout<<"initialize first the toolbar and/or the qmenu ";
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      }

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoToolBarStatus::ClearToolBar()
{
  if (this->m_ToolBar)
    {
    //this->Disconnect();
    this->m_Visibility = this->m_ToolBar->isVisible();
    //this->m_Area = this->m_ToolBar->geometry();
    this->m_Attached = this->m_ToolBar->isFloating();
    this->m_ToolBar->clear();
    }
  if (this->m_Menu)
    {
    this->m_Menu->clear();
    }
  if (!this->m_ToolBar && !this->m_Menu)
    {
    std::cout<<"initialize first the toolbar and the qmenu ";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoToolBarStatus::InitializeToolBarAndMenu(QToolBar* iToolBar, QMenu* iMenu)
{
  this->m_ToolBar = iToolBar;
  this->m_Menu = iMenu;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void QGoToolBarStatus::Connect()
{
  //QObject::connect( m_ToolBar, SIGNAL( dockLocationChanged(Qt::DockWidgetArea) ),
  //                  this, SLOT( SetArea(Qt::DockWidgetArea) ) );

 // QObject::connect(m_ToolBar->toggleViewAction(), SIGNAL(toggled(bool) ),
  //                  this, SLOT(SetVisibility(bool) ) );

  QObject::connect(m_ToolBar, SIGNAL(topLevelChanged (bool) ),
                    this, SLOT( SetAttached(bool) ) );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoToolBarStatus::Disconnect()
{
  if (this->m_ToolBar)
    {
    this->m_ToolBar->disconnect(this);
    this->m_ToolBar->toggleViewAction()->disconnect(this);
    }
  else
    {
    std::cout<<"initialize first the toolbar and the qmenu ";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
}*/
