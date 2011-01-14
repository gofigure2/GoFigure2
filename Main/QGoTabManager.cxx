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

#include "QGoTabManager.h"

#include <QTabWidget>

#include "QGoMainWindow.h"
#include "QGoTabElementBase.h"

//--------------------------------------------------------------------------
QGoTabManager::QGoTabManager(QGoMainWindow *iMW, QTabWidget *iTW):m_MainWindow(iMW),
  m_TabWidget(iTW), m_PreviousTabIndex(-1)
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabManager::~QGoTabManager()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabManager::SetMainWindow(QGoMainWindow *iMW)
{
  m_MainWindow = iMW;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabManager::SetTabWidget(QTabWidget *iTW)
{
  m_TabWidget = iTW;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabManager::ClearTabElement(QGoTabElementBase *iE)
{
  if ( iE )
    {
    // First remove all toolbar related to the previous tab
    m_MainWindow->m_ViewToolBar->clear();
    m_MainWindow->m_ModeToolBar->clear();
    // Then remove all actions related to the previous tab from menuView
    m_MainWindow->menuView->clear();

    // Then remove all actions from the segmentation menu
    m_MainWindow->menuSegmentation->clear();

    // Then remove all actions from the tools menu
    m_MainWindow->menuTools->clear();

    // Then remove all actions from the bookmark menu
    m_MainWindow->menuBookmarks->clear();

    //then remove all actions from the mode menu:
    m_MainWindow->menuMode->clear();

    std::list< QGoTabElementBase::QGoDockWidgetStatusPair > & dock_list = iE->DockWidget();

    for ( std::list< QGoTabElementBase::QGoDockWidgetStatusPair >::iterator
          dck_it = dock_list.begin();
          dck_it != dock_list.end();
          ++dck_it )
      {
      dck_it->first->m_Area = m_MainWindow->dockWidgetArea(dck_it->second);
      bool temp = dck_it->second->isVisible();

      m_MainWindow->removeDockWidget(dck_it->second);

      dck_it->first->m_Visibility = temp;
      }

    GoFigure::TabDimensionType dim = iE->GetTabDimensionType();

    std::map< GoFigure::TabDimensionType, std::list< QAction * > >::iterator
      map_it = m_MainWindow->m_TabDimPluginActionMap.find(dim);

    if ( map_it != m_MainWindow->m_TabDimPluginActionMap.end() )
      {
      for ( std::list< QAction * >::iterator list_it = ( map_it->second ).begin();
            list_it != ( map_it->second ).end();
            list_it++ )
        {
        ( *list_it )->setDisabled(true);
        }
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabManager::SetUpTabElement(QGoTabElementBase *iE)
{
  if ( iE )
    {
    // Then add all actions related to the new tab from menuView
    std::vector< QAction * > action_vector2 = iE->ViewActions();

    for ( std::vector< QAction * >::iterator it = action_vector2.begin();
          it != action_vector2.end();
          ++it )
      {
      m_MainWindow->menuView->addAction(*it);
      m_MainWindow->m_ViewToolBar->addAction(*it);
      }

    action_vector2 = iE->ModeActions();

    for ( std::vector< QAction * >::iterator it = action_vector2.begin();
          it != action_vector2.end();
          ++it )
      {
      m_MainWindow->menuMode->addAction(*it);
      m_MainWindow->m_ModeToolBar->addAction(*it);
      }

    action_vector2 = iE->SegmentationActions();

    for ( std::vector< QAction * >::iterator it = action_vector2.begin();
          it != action_vector2.end();
          ++it )
      {
      m_MainWindow->menuSegmentation->addAction(*it);
      }

    action_vector2 = iE->ToolsActions();

    for ( std::vector< QAction * >::iterator it = action_vector2.begin();
          it != action_vector2.end();
          ++it )
      {
      m_MainWindow->menuTools->addAction(*it);
      }

    action_vector2 = iE->BookmarkActions();

    for ( std::vector< QAction * >::iterator it = action_vector2.begin();
          it != action_vector2.end();
          ++it )
      {
      m_MainWindow->menuBookmarks->addAction(*it);
      }

    std::list< QGoTabElementBase::QGoDockWidgetStatusPair > dock_list = iE->DockWidget();

    for ( std::list< QGoTabElementBase::QGoDockWidgetStatusPair >::iterator
          dck_it = dock_list.begin();
          dck_it != dock_list.end();
          ++dck_it )
      {
      if ( dck_it->first->m_Attached )
        {
        if ( dck_it->first->m_Area == Qt::NoDockWidgetArea )
          {
          dck_it->first->m_Area = dck_it->first->m_DefaultArea;
          }
        m_MainWindow->addDockWidget(dck_it->first->m_Area, dck_it->second);
        }
      dck_it->second->setVisible(dck_it->first->m_Visibility);
      }

    GoFigure::TabDimensionType dim = iE->GetTabDimensionType();

    if ( dim == GoFigure::THREE_D_WITH_T )
      {
      ///\todo: Check if there is a connection with database to update
      // import/export in menu
      //std::cout << "Check if there is a connection with database to update import/export in menu" << std::endl;
      }

    std::map< GoFigure::TabDimensionType, std::list< QAction * > >::iterator
      map_it = m_MainWindow->m_TabDimPluginActionMap.find(dim);

    if ( map_it != m_MainWindow->m_TabDimPluginActionMap.end() )
      {
      for ( std::list< QAction * >::iterator list_it = ( map_it->second ).begin();
            list_it != ( map_it->second ).end();
            list_it++ )
        {
        ( *list_it )->setEnabled(true);
        }
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabManager::ChangeCurrentTab(int iIdx)
{
  if ( m_PreviousTabIndex != -1 )
    {
    QGoTabElementBase *w =
      dynamic_cast< QGoTabElementBase * >(
        m_TabWidget->widget(m_PreviousTabIndex) );

    ClearTabElement(w);
    }

  if ( iIdx != -1 )
    {
    QGoTabElementBase *w2 =
      dynamic_cast< QGoTabElementBase * >(
        m_TabWidget->widget(iIdx) );

    SetUpTabElement(w2);
    }

  m_PreviousTabIndex = iIdx;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabManager::UpdateBookmarkMenu(std::vector< QAction * > iBookmarkActions)
{
  m_MainWindow->menuBookmarks->clear();

  for ( std::vector< QAction * >::iterator it = iBookmarkActions.begin();
        it != iBookmarkActions.end();
        ++it )
    {
    m_MainWindow->menuBookmarks->addAction(*it);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabManager::CloseTab(int idx)
{
  if ( idx >= 0 )
    {
    QGoTabElementBase *w =
      dynamic_cast< QGoTabElementBase * >( m_TabWidget->widget(idx) );
    if ( w )
      {
      w->WriteSettings();
      ClearTabElement(w);
      delete w;
      w = 0;
      }
    //     m_TabWidget->removeTab( idx );
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabManager::CloseAllTabs()
{
  int NumberOfTabs = m_TabWidget->count();

  for ( int i = 0; i < NumberOfTabs; i++ )
    {
    int                k = NumberOfTabs - 1 - i;
    QGoTabElementBase *w =
      dynamic_cast< QGoTabElementBase * >( m_TabWidget->widget(k) );

    if ( w )
      {
      ClearTabElement(w);

      w->WriteSettings();
      delete w;
      }
    }

  m_MainWindow->m_ViewToolBar->clear();
  m_MainWindow->m_ModeToolBar->clear();
  m_TabWidget->clear();
}

//--------------------------------------------------------------------------
