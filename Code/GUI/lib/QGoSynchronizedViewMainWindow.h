#ifndef QGOSYNCHRONIZEDVIEWMAINWINDOW_H
#define QGOSYNCHRONIZEDVIEWMAINWINDOW_H


#include <QMainWindow>

class QGoSynchronizedView;
class QGoSynchronizedView3D;
class QGoSynchronizedViewManager;
class vtkImageData;

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
QT_END_NAMESPACE

class QGoSynchronizedViewMainWindow : public QMainWindow
{
// QT macro
  Q_OBJECT

public:

  QGoSynchronizedViewMainWindow();
  ~QGoSynchronizedViewMainWindow();

  QGoSynchronizedViewManager* GetSynchronizedViewManager();

  void Update();

  QGoSynchronizedView* newSynchronizedView2D(
    QString iSynchronizedViewName, 
    vtkImageData* iImage);
  QGoSynchronizedView3D* newSynchronizedView3D(
    QString iSynchronizedViewName, 
    vtkImageData* iImage);

  void OpenSynchronizedViewForFile(QString& iFile);

  void deleteSynchronizedView2D(const int& iId);
  void deleteSynchronizedView3D(const int& iId);


protected:
  void closeEvent(QCloseEvent *event);


private slots:

  void openfile();
  void snapshotAs();
  void about();
  void aboutGF2();
  void updateMenus();
  void updateWindowMenu();
  void setActiveSubWindow(QWidget *window);
  void synchronize();

  void FullscreenXY();
  void FullscreenXZ();
  void FullscreenYZ();
  void FullscreenXYZ();
  void Quadscreen();

private:

  void SaveSnapshotInFile( QString& iFile, QGoSynchronizedView* SynchronizedView);
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void readSettings();
  void writeSettings();

  void imageinfo();

  QGoSynchronizedView *activeSynchronizedView();
  QMdiSubWindow *findSynchronizedView(const QString &iSynchronizedViewName);

  QMdiArea *mdiArea;
  QSignalMapper *windowMapper;

  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *windowMenu;
  QMenu *helpMenu;

  QToolBar *ToolBar;
  QToolBar *View3DToolBar;
  QAction *syncAct;
  QAction *openfileAct;
  QAction *openmemAct;
  QAction *snapshotAsAct;
  QAction *exitAct;
  QAction *closeAct;
  QAction *closeAllAct;
  QAction *tileAct;
  QAction *cascadeAct;
  QAction *aboutAct;
  QAction *aboutQtAct;
  QAction *aboutGF2Act;
  QAction *XYviewAct;
  QAction *XZviewAct;
  QAction *YZviewAct;
  QAction *XYZviewAct;
  QAction *QuadviewAct;

  QGoSynchronizedViewManager* m_SynchronizedViewManager;
};

#endif // QGOSYNCHRONIZEDVIEWMAINWINDOW_H
