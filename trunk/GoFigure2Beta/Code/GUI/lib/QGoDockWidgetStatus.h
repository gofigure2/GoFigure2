#ifndef __QGoDockWidgetStatus_h
#define __QGoDockWidgetStatus_h

#include <Qt>
#include <QObject>
#include <QDockWidget>

class QGoDockWidgetStatus : public QObject
{
  Q_OBJECT

public:
  explicit QGoDockWidgetStatus( QDockWidget* iW );

  explicit QGoDockWidgetStatus( const QGoDockWidgetStatus& iS );

  explicit QGoDockWidgetStatus( QDockWidget* iW,
    Qt::DockWidgetArea iArea,
    const bool& iVisibility,
    const bool& iAttached );

  virtual ~QGoDockWidgetStatus();

  QDockWidget*        m_DockWidget;

  /** \brief Position */
  Qt::DockWidgetArea  m_Area;
  Qt::DockWidgetArea  m_DefaultArea;

  /** \brief is Visible */
  bool                m_Visibility;

  /** \brief Attached to QGoMainWindow*/
  bool                m_Attached;

public slots:
  void SetArea( Qt::DockWidgetArea iArea );
  void SetVisibility( bool iVisibility );
  void SetAttached( bool iAttached );

};
#endif
