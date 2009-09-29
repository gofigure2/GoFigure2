#include "identityplugin.h"

#include <QAction>
#include <iostream>

Q_EXPORT_PLUGIN2( QGoIdentityImageFilterPlugin, identityplugin )

identityplugin::identityplugin() : QGoImageFilterPluginBase()
{
  this->SetAllRequiredInformation();
}
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

QMenu* identityplugin::Menu()
{
  return 0;
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
  std::cout <<"identityplugin::Process" <<std::endl;
}

void identityplugin::SetAllRequiredInformation()
{
  this->m_Information.Name = "Identity";
  this->m_Information.Version = "1.0";
  this->m_Information.GoFigureCompatibilityVersion = "1.0";
  this->m_Information.Distributor = "Megason Lab";
  this->m_Information.Copyright = "Harvard Medical School";
  this->m_Information.License = "BSD";
  this->m_Information.Description = "Test: plugin which copy the input to the output";
}
