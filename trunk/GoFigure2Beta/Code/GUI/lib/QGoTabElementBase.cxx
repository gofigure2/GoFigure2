#include "QGoTabElementBase.h"

#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QPluginLoader>

#include <iostream>

//--------------------------------------------------------------------------
QGoTabElementBase::
QGoTabElementBase( QWidget* iParent ) : QWidget( iParent )
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
  return m_ViewActions;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction* > QGoTabElementBase::SegmentationActions()
{
  return m_SegmentationActions;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction* > QGoTabElementBase::ToolsActions()
{
  return m_ToolsActions;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< std::pair< Qt::DockWidgetArea, QDockWidget* > > QGoTabElementBase::DockWidget()
{
  return std::list< std::pair< Qt::DockWidgetArea, QDockWidget* > >();
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
