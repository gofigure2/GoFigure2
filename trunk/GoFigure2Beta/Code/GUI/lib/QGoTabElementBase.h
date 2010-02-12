/*=========================================================================
  Author: $Author:$  // Author of last commit
  Version: $Rev:$  // Revision of last commit
  Date: $Date:$  // Date of last commit
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

#ifndef __QGoTabElementBase_h
#define __QGoTabElementBase_h

#include <QWidget>
#include <QDir>

#include "QGoPlugin.h"
#include "QGoDockWidgetStatus.h"

class QAction;
class QActionGroup;
class QMenu;
class QToolBar;
class QDockWidget;
class QGoPluginManager;

/**
\class QGoTabElementBase
\brief
\example GUI/lib/qgotabelementbase.cxx
*/
class QGoTabElementBase : public QWidget
{
  Q_OBJECT
public:
  QGoTabElementBase( QWidget* parent = 0 );
  virtual ~QGoTabElementBase();

  typedef std::pair< QGoDockWidgetStatus*, QDockWidget* > QGoDockWidgetStatusPair;

  virtual GoFigure::TabDimensionType GetTabDimensionType( ) const = 0;

  virtual std::vector< QAction* > ViewActions();
  virtual std::vector< QAction* > SegmentationActions();
  virtual std::vector< QAction* > ToolsActions();
  virtual std::vector< QAction* > BookmarkActions();

  virtual std::list< QGoDockWidgetStatusPair >& DockWidget();
  virtual std::list< QWidget* > AdditionalWidget();
//   virtual QStatusBar* StatusBar();

  virtual std::list< QAction* > GetPluginActions();
  virtual void SetPluginActions( std::list< QAction* > iList );

  virtual void WriteSettings() = 0;
  virtual void ReadSettings() = 0;

protected:
  std::list< QAction* > m_PluginActionList;

  std::vector< QAction* > m_ViewActions;
  std::vector< QAction* > m_SegmentationActions;
  std::vector< QAction* > m_ToolsActions;
  std::vector< QAction* > m_BookmarkActions;

  std::list< QGoDockWidgetStatusPair > m_DockWidgetList;

private:
  QGoTabElementBase( const QGoTabElementBase& );
  void operator = ( const QGoTabElementBase& );
};
#endif
