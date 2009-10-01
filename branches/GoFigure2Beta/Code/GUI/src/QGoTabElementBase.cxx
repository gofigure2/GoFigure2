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
std::vector< QToolBar* > QGoTabElementBase::ToolBar()
{
  return std::vector< QToolBar* >();
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

