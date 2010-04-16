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

#ifndef __QGoTabElementBase_h
#define __QGoTabElementBase_h

#include <QWidget>
#include <QDir>

#include "QGoPlugin.h"
#include "QGoDockWidgetStatus.h"

#include "QGoGUILibConfigure.h"

class QAction;
class QActionGroup;
class QMenu;
class QToolBar;
class QDockWidget;
class QGoPluginManager;

/**
\class QGoTabElementBase
\brief Abstract class for one tab element in GoFigure2.
\example GUI/lib/qgotabelementbase.cxx
*/
class QGOGUILIB_EXPORT QGoTabElementBase : public QWidget
{
  Q_OBJECT
public:
  /** \brief Constructor */
  explicit QGoTabElementBase( QWidget* parent = 0 );

  /** \brief Destructor */
  virtual ~QGoTabElementBase();

  typedef std::pair< QGoDockWidgetStatus*, QDockWidget* > QGoDockWidgetStatusPair;

  /** \brief Get the dimension type of the underlying data set.*/
  virtual GoFigure::TabDimensionType GetTabDimensionType( ) const = 0;

  /** \brief Get all actions belonging to View Menu and Toolbar.*/
  virtual std::vector< QAction* > ViewActions();
  /** \brief Get all actions belonging to Segmentation Menu and Toolbar. */
  virtual std::vector< QAction* > SegmentationActions();
  /** \brief Get all actions belonging to Tools Menu and Toolbar. */
  virtual std::vector< QAction* > ToolsActions();
  /** \brief Get all actions belonging to Bookmark Menu and Toolbar. */
  virtual std::vector< QAction* > BookmarkActions();
  /** \brief Get all actions belonging to Mode Menu and Toolbar.*/
  virtual std::vector< QAction* > ModeActions();

  /** \brief Get all the DockWidgets with its status (visibility, location). */
  virtual std::list< QGoDockWidgetStatusPair >& DockWidget();
  virtual std::list< QWidget* > AdditionalWidget();
//   virtual QStatusBar* StatusBar();

  virtual std::list< QAction* > GetPluginActions();
  virtual void SetPluginActions( std::list< QAction* > iList );

  /** \brief Write Settings for the tab element. */
  virtual void WriteSettings() = 0;
  /** \brief Read Settings for the related tab element.*/
  virtual void ReadSettings() = 0;

protected:
  std::list< QAction* > m_PluginActionList;

  std::vector< QAction* > m_ViewActions;
  std::vector< QAction* > m_SegmentationActions;
  std::vector< QAction* > m_ToolsActions;
  std::vector< QAction* > m_BookmarkActions;
  std::vector< QAction* > m_ModeActions;

  std::list< QGoDockWidgetStatusPair > m_DockWidgetList;

private:
  Q_DISABLE_COPY( QGoTabElementBase );
};
#endif
