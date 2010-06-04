#ifndef QGOCOMPAREGUI_H
#define QGOCOMPAREGUI_H


#include <QMainWindow>

class QGoComparer;
class QGoComparer3D;
class QGoCompareOrchestra;
class vtkImageData;

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
QT_END_NAMESPACE

class QGoCompareGUI : public QMainWindow
{
// QT macro
  Q_OBJECT

public:

  QGoCompareGUI();
  ~QGoCompareGUI();

  QGoCompareOrchestra* GetCompareOrchestra();

  void Update();

  QGoComparer* newComparer2D(QString iComparerName, vtkImageData* iImage);
  QGoComparer3D* newComparer3D(QString iComparerName, vtkImageData* iImage);

  void OpenComparerForFile(QString& iFile);

  void deleteComparer2D(const int& iId);
  void deleteComparer3D(const int& iId);


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
    
  void SaveSnapshotInFile( QString& iFile, QGoComparer* Comparer);
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void readSettings();
  void writeSettings();

  void imageinfo();

  QGoComparer *activeComparer();
  QMdiSubWindow *findComparer(const QString &iComparerName);

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

  QGoCompareOrchestra* m_CompareOrchestra;
};

#endif // QGOCOMPAREGUI_H
