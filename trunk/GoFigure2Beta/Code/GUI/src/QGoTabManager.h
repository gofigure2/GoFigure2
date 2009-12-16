#ifndef __QGoTabManager_h
#define __QGoTabManager_h

#include <QObject>

class QTabWidget;
class QGoMainWindow;
class QGoTabElementBase;

class QGoTabManager : public QObject
{
  Q_OBJECT
  public:

    QGoTabManager( QGoMainWindow* iMW = 0, QTabWidget* iTW = 0 );
    ~QGoTabManager();

    void SetMainWindow( QGoMainWindow* iMW );
    void SetTabWidget( QTabWidget* iTW );

  public slots:
    void ChangeCurrentTab( int iIdx );
    void CloseTab( int idx );
    void CloseAllTabs( );

  private:
    QGoMainWindow*  m_MainWindow;
    QTabWidget*     m_TabWidget;
    int             m_PreviousTabIndex;

    void ClearTabElement( QGoTabElementBase* iE );
    void SetUpTabElement( QGoTabElementBase* iE );
};
#endif
