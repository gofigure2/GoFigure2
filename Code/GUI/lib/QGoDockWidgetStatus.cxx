/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

QGoDockWidgetStatus::
QGoDockWidgetStatus( QDockWidget* iW ) : QObject( iW ),
    m_DockWidget( iW ),
    m_Area( Qt::LeftDockWidgetArea ), m_DefaultArea( Qt::LeftDockWidgetArea ),
    m_Visibility( true ), m_Attached( true )
{
  QObject::connect( m_DockWidget, SIGNAL( dockLocationChanged( Qt::DockWidgetArea ) ),
  this, SLOT( SetArea( Qt::DockWidgetArea ) ) );

  QObject::connect( m_DockWidget, SIGNAL( visibilityChanged( bool ) ),
    this, SLOT( SetVisibility( bool ) ) );
}

QGoDockWidgetStatus::
QGoDockWidgetStatus( const QGoDockWidgetStatus& iS ) :
  QObject( iS.m_DockWidget ), m_Area( iS.m_Area ), m_DefaultArea( iS.m_Area ),
  m_Visibility( iS.m_Visibility ), m_Attached( iS.m_Attached )
{
  QObject::connect( m_DockWidget, SIGNAL( dockLocationChanged( Qt::DockWidgetArea ) ),
    this, SLOT( SetArea( Qt::DockWidgetArea ) ) );

QObject::connect( m_DockWidget, SIGNAL( visibilityChanged( bool ) ),
  this, SLOT( SetVisibility( bool ) ) );
}

QGoDockWidgetStatus::
QGoDockWidgetStatus( QDockWidget* iW, Qt::DockWidgetArea iArea,
  const bool& iVisibility, const bool& iAttached ) :
  QObject( iW ), m_DockWidget( iW ), m_Area( iArea ),
  m_DefaultArea( iArea ), m_Visibility( iVisibility ),
  m_Attached( iAttached )
{
  QObject::connect( m_DockWidget, SIGNAL( dockLocationChanged( Qt::DockWidgetArea ) ),
    this, SLOT( SetArea( Qt::DockWidgetArea ) ) );

  QObject::connect( m_DockWidget, SIGNAL( visibilityChanged( bool ) ),
    this, SLOT( SetVisibility( bool ) ) );
}

QGoDockWidgetStatus::
~QGoDockWidgetStatus()
{}

void
QGoDockWidgetStatus::
SetArea( Qt::DockWidgetArea iArea )
{
//   switch( iArea )
//     {
//     case Qt::LeftDockWidgetArea:
//       std::cout <<"Left" <<std::endl;
//       break;
//     case Qt::RightDockWidgetArea:
//       std::cout <<"Right" <<std::endl;
//       break;
//     case Qt::TopDockWidgetArea:
//       std::cout <<"Top" <<std::endl;
//       break;
//     case Qt::BottomDockWidgetArea:
//       std::cout <<"Bottom" <<std::endl;
//       break;
//     case Qt::AllDockWidgetAreas:
//       std::cout <<"All Area" <<std::endl;
//       break;
//     default:
//     case Qt::NoDockWidgetArea:
//       std::cout <<"No dockWidget" <<std::endl;
//       break;
//     }
  m_Area = iArea;
}

void
QGoDockWidgetStatus::
SetVisibility( bool iVisibility )
{
//   if( iVisibility )
//     {
//     std::cout <<"Visible" <<std::endl;
//     }
//   else
//     {
//     std::cout <<"Invisible" <<std::endl;
//     }
  m_Visibility = iVisibility;
}

void
QGoDockWidgetStatus::
SetAttached( bool iAttached )
{
  m_Attached = iAttached;
}
