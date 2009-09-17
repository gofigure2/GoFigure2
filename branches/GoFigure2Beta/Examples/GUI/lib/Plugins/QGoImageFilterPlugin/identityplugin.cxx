#include "identityplugin.h"

#include <QAction>

identityplugin::identityplugin()
{}
identityplugin::~identityplugin()
{}

std::vector< QAction* > identityplugin::Actions()
{
  QAction* action = new QAction( tr( "&Identity" ), this );

  QObject::connect( action, SIGNAL( triggered ),
    this, SLOT( Process( ) ) );

  std::vector< QAction* > oActionList;
  oActionList.push_back( action );

  return oActionList;
}

QToolBar* identityplugin::ToolBar()
{
  return 0;
}
QDockWidget* identityplugin::DockWidget()
{
  return 0;
}
QWidget* identityplugin::AdditionalWidget()
{
  return 0;
}

void identityplugin::WriteSettings()
{}

void identityplugin::ReadSettings()
{}

void identityplugin::Process( )
{
  m_VTKOutput = m_VTKInput;
}
