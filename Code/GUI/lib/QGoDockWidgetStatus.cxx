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

#include "QGoDockWidgetStatus.h"

#include <iostream>

QGoDockWidgetStatus::QGoDockWidgetStatus(QGoDockWidget *iW) : QObject(iW),
  m_DockWidget(iW),
  m_Area(Qt::LeftDockWidgetArea), m_DefaultArea(Qt::LeftDockWidgetArea),
  m_Visibility(true), m_Attached(true)
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoDockWidgetStatus::QGoDockWidgetStatus(const QGoDockWidgetStatus & iS) :
  QObject(iS.m_DockWidget), m_Area(iS.m_Area), m_DefaultArea(iS.m_Area),
  m_Visibility(iS.m_Visibility), m_Attached(iS.m_Attached), 
  m_MainWindow(iS.m_MainWindow)
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoDockWidgetStatus::QGoDockWidgetStatus(QGoDockWidget *iW, Qt::DockWidgetArea iArea,
                                         const bool & iVisibility, const bool & iAttached,
                                         QMainWindow* iMainWindow) :
  QObject(iW), m_DockWidget(iW), m_Area(iArea),
  m_DefaultArea(iArea), m_Visibility(iVisibility),
  m_Attached(iAttached),
  m_MainWindow(iMainWindow)
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoDockWidgetStatus::
~QGoDockWidgetStatus()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoDockWidgetStatus::SetConnections()
{
  QObject::connect( m_DockWidget, SIGNAL( dockLocationChanged(Qt::DockWidgetArea) ),
                    this, SLOT( SetArea(Qt::DockWidgetArea) ) );

  //QObject::connect( m_DockWidget, SIGNAL( visibilityChanged(bool) ),
  //                  this, SLOT( SetVisibility(bool) ) );
  QObject::connect(m_DockWidget->toggleViewAction(), SIGNAL(toggled(bool) ),
                    this, SLOT(SetVisibility(bool) ) );

  QObject::connect(m_DockWidget, SIGNAL(topLevelChanged (bool) ),
                    this, SLOT( SetAttached(bool) ) );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoDockWidgetStatus::SetArea(Qt::DockWidgetArea iArea)
{
  m_Area = iArea;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoDockWidgetStatus::SetVisibility(bool iVisibility)
{
   m_Visibility = iVisibility;
   this->m_DockWidget->setVisible(iVisibility);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoDockWidgetStatus::SetAttached(bool iAttached)
{
  m_Attached = iAttached;
}