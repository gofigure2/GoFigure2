#include "QGoTabElementBase.h"

#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>

QGoTabElementBase::QGoTabElementBase( QWidget* parent ) : QWidget( parent )
{}

QGoTabElementBase::~QGoTabElementBase()
{}

std::vector< QMenu* > QGoTabElementBase::Menus()
{
  QMenu* ViewMenu = new QMenu( QObject::tr( "&View" ) );

  std::vector< QMenu* > oMenuVector;
  oMenuVector.push_back( ViewMenu );

  return oMenuVector;
}

std::list< QToolBar* > QGoTabElementBase::ToolBar()
{
  return std::list< QToolBar* >();
}
std::list< QDockWidget* > QGoTabElementBase::DockWidget()
{
  return std::list< QDockWidget* >();
}

std::list< QWidget* > QGoTabElementBase::AdditionalWidget()
{
  return std::list< QWidget* >();
}

// QStatusBar* QGoTabElementBase::StatusBar()
// {
//   return 0;
// }
