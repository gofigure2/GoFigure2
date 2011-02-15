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

#include "QGoPluginManager.h"

#include <iostream>

#include <QApplication>
#include <QDir>
#include <QMap>
#include <QPluginLoader>
#include <QAction>
#include <QDockWidget>
#include <QMainWindow>
#include <QToolBar>

#include "QGoPlugin.h"
#include "QGoPluginHelper.h"
// #include "QGoMainWindow.h"

// class QGoPluginManager::Interior
// {
// public:
//   Interior() {}
//   ~Interior() {}
//
//   /** \todo check later on if it is already loaded. */
//   bool AddPlugin( QGoPlugin* plugin )
//   {
//     if( plugin )
//       {
//       m_Plugins.push_back( plugin );
//       return true;
//       }
//     else
//       {
//       return false;
//       }
//   }
//
//   std::list< QGoPlugin* > m_Plugins;
// };

QGoPluginManager::QGoPluginManager() : QObject()
{
//   m_InternalManager = new Interior();
}

QGoPluginManager::~QGoPluginManager()
{
//   delete m_InternalManager;
}

//--------------------------------------------------------------------------
// Tab events to be handled by plugins
//--------------------------------------------------------------------------
void QGoPluginManager::NotifyTabActivated(const int & iId)
{
  for ( std::list< QGoPlugin * >::iterator
        it = m_PluginList.begin();
        it != m_PluginList.end();
        ++it )
    {
    if ( ( *it ) )
      {
      ( *it )->OnTabActivated(iId);
      }
    }
}

void QGoPluginManager::NotifyTabClosed(const int & iId)
{
  for ( std::list< QGoPlugin * >::iterator
        it = m_PluginList.begin();
        it != m_PluginList.end();
        ++it )
    {
    if ( ( *it ) )
      {
      ( *it )->OnTabClosed(iId);
      }
    }
}

void QGoPluginManager::NotifyTabMoved(const int & from, const int & to)
{
  for ( std::list< QGoPlugin * >::iterator
        it = m_PluginList.begin();
        it != m_PluginList.end();
        ++it )
    {
    if ( ( *it ) )
      {
      ( *it )->OnTabMoved(from, to);
      }
    }
}

//--------------------------------------------------------------------------

bool QGoPluginManager::IsPluginAlreadyLoaded(const QString & iName)
{
  for ( std::list< QGoPlugin * >::iterator
        it = m_PluginList.begin();
        it != m_PluginList.end();
        ++it )
    {
    if ( ( *it ) )
      {
      if ( ( *it )->Name() == iName )
        {
        return true;
        }
      }
    }
  return false;
}

void QGoPluginManager::LoadPlugin(const QString & path)
{
  QPluginLoader loader(path);

  if ( !loader.load() )
    {
    std::cout << "Plugin was not loaded" << std::endl;
    return;
    }

  QObject *obj = loader.instance();
  if ( obj )
    {
    QGoPlugin *plugin = qobject_cast< QGoPlugin * >(obj);
    if ( plugin )
      {
      //	Check if plugin with the same name was already loaded.
      //	If is was then exit.
      if ( IsPluginAlreadyLoaded( plugin->Name() ) )
        {
        return;
        }

      if ( plugin )
        {
        m_PluginList.push_back(plugin);
        std::cout << "Plugin LOADED" << std::endl;
        }
      else
        {
        //note that for now this case can never happen!
        loader.unload();
        }
      }
    else
      {
      std::cout << "Error while casting" << std::endl;
      }
    }
  else
    {
    std::cout << "Null plugin" << std::endl;
    }
}

void QGoPluginManager::LoadPlugins()
{
  //	global plugins
  QDir gPluginDir = FindPluginDirectory("plugins");

  foreach ( QString fileName, gPluginDir.entryList(QDir::Files) )
    {
    QString path = gPluginDir.absoluteFilePath(fileName);

    LoadPlugin(path);
    }

//   std::list< QDockWidget* >::iterator it
//     = m_InternalManager->m_Docks.begin();
//   std::list< QDockWidget* >::iterator end
//     = m_InternalManager->m_Docks.end();
//
//   for( ; it != end; ++it )
//     {
//     m_InternalManager->m_MainWindow->AddDockWidget( *it );
//     }
}

void QGoPluginManager::ApplySettings()
{
  std::list< QGoPlugin * >::iterator it = m_PluginList.begin();
  std::list< QGoPlugin * >::iterator end = m_PluginList.end();

  for (; it != end; ++it )
    {
    ( *it )->ReadSettings();
    }
}

std::list< QMenu * > QGoPluginManager::GetMenus()
{
  std::list< QMenu * > oMenuList;

  std::list< QGoPlugin * >::iterator it = m_PluginList.begin();
  std::list< QGoPlugin * >::iterator end = m_PluginList.end();

  for (; it != end; ++it )
    {
    if ( ( *it )->Menu() )
      {
      oMenuList.push_back( ( *it )->Menu() );
      }
    }

  return oMenuList;
}