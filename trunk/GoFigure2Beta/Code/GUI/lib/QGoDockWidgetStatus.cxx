#include "QGoDockWidgetStatus.h"

#include <iostream>

QGoDockWidgetStatus::QGoDockWidgetStatus( QDockWidget* iW ) : QObject( iW ),
    m_DockWidget( iW ),
    m_Area( Qt::LeftDockWidgetArea ), m_DefaultArea( Qt::LeftDockWidgetArea ),
    m_Visibility( true ), m_Attached( true )
    {
    QObject::connect( m_DockWidget, SIGNAL( dockLocationChanged( Qt::DockWidgetArea ) ),
      this, SLOT( SetArea( Qt::DockWidgetArea ) ) );

    QObject::connect( m_DockWidget, SIGNAL( visibilityChanged( bool ) ),
      this, SLOT( SetVisibility( bool ) ) );
    }

QGoDockWidgetStatus::QGoDockWidgetStatus( const QGoDockWidgetStatus& iS ) :
    QObject( iS.m_DockWidget ), m_Area( iS.m_Area ), m_DefaultArea( iS.m_Area ),
    m_Visibility( iS.m_Visibility ), m_Attached( iS.m_Attached )
    {
    QObject::connect( m_DockWidget, SIGNAL( dockLocationChanged( Qt::DockWidgetArea ) ),
      this, SLOT( SetArea( Qt::DockWidgetArea ) ) );

    QObject::connect( m_DockWidget, SIGNAL( visibilityChanged( bool ) ),
      this, SLOT( SetVisibility( bool ) ) );
    }

QGoDockWidgetStatus::QGoDockWidgetStatus( QDockWidget* iW,
    Qt::DockWidgetArea iArea,
    const bool& iVisibility,
    const bool& iAttached ) : QObject( iW ), m_DockWidget( iW ), m_Area( iArea ),
                              m_DefaultArea( iArea ),
                              m_Visibility( iVisibility ),
                              m_Attached( iAttached )
    {
    QObject::connect( m_DockWidget, SIGNAL( dockLocationChanged( Qt::DockWidgetArea ) ),
      this, SLOT( SetArea( Qt::DockWidgetArea ) ) );

    QObject::connect( m_DockWidget, SIGNAL( visibilityChanged( bool ) ),
      this, SLOT( SetVisibility( bool ) ) );
    }

QGoDockWidgetStatus::~QGoDockWidgetStatus() {}

void QGoDockWidgetStatus::SetArea( Qt::DockWidgetArea iArea )
{
//   switch( iArea )
//     {
//     case Qt::LeftDockWidgetArea:
//       std::cout <<"Left" <<std::endl;
//       break;
//     case Qt::RightDockWidgetArea:
//       std::cout <<"Right" <<std::endl;
//       break;
//     case Qt::TopDockWidgetArea:
//       std::cout <<"Top" <<std::endl;
//       break;
//     case Qt::BottomDockWidgetArea:
//       std::cout <<"Bottom" <<std::endl;
//       break;
//     case Qt::AllDockWidgetAreas:
//       std::cout <<"All Area" <<std::endl;
//       break;
//     default:
//     case Qt::NoDockWidgetArea:
//       std::cout <<"No dockWidget" <<std::endl;
//       break;
//     }
  m_Area = iArea;
}

void QGoDockWidgetStatus::SetVisibility( bool iVisibility )
{
//   if( iVisibility )
//     {
//     std::cout <<"Visible" <<std::endl;
//     }
//   else
//     {
//     std::cout <<"Invisible" <<std::endl;
//     }
  m_Visibility = iVisibility;
}

void QGoDockWidgetStatus::SetAttached( bool iAttached )
{
  m_Attached = iAttached;
}
