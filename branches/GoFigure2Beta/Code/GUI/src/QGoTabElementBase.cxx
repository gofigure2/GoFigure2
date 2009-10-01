#include "QGoTabElementBase.h"

#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QPluginLoader>

#include <iostream>

//--------------------------------------------------------------------------
QGoTabElementBase::QGoTabElementBase( QWidget* parent ) : QWidget( parent )
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabElementBase::~QGoTabElementBase()
{}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction* > QGoTabElementBase::ViewActions()
{
  return std::vector< QAction* > ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< QDockWidget* > QGoTabElementBase::DockWidget()
{
  return std::list< QDockWidget* >();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< QWidget* > QGoTabElementBase::AdditionalWidget()
{
  return std::list< QWidget* >();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< QAction* > QGoTabElementBase::GetPluginActions()
{
  return m_PluginActionList;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabElementBase::SetPluginActions( std::list< QAction* > iList )
{
  m_PluginActionList = iList;
}
//--------------------------------------------------------------------------