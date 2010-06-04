#include "qgocomparegui.h"

#include <sstream>

#include <QtGui>
#include "qinputdialog.h"

#include "QGoComparer.h"
#include "QGoComparer3D.h"
#include "QGoCompareOrchestra.h"


#include "itkImageFileReader.h"

#include "vtkImageReader2Factory.h"
#include "vtkImageReader2.h"
#include "vtkImageData.h"

QGoCompareGUI::QGoCompareGUI()
{
  m_CompareOrchestra = new QGoCompareOrchestra(this);

  mdiArea = new QMdiArea;

  setCentralWidget(mdiArea);
  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));

  windowMapper = new QSignalMapper(this);
  connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));


  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  updateMenus();

  setWindowTitle(tr("QGoCompareGUI"));
  setUnifiedTitleAndToolBarOnMac(true);

  //mdiArea->setViewMode(QMdiArea::TabbedView);


}

QGoCompareGUI::~QGoCompareGUI()
{

}



QGoCompareOrchestra* QGoCompareGUI::GetCompareOrchestra()
{
  return m_CompareOrchestra;
}

void QGoCompareGUI::Update()
{
  m_CompareOrchestra->Update();
  m_CompareOrchestra->show();
}

QGoComparer* QGoCompareGUI::newComparer2D(QString iComparerName, vtkImageData* iImage)
{
  QGoComparer2D *comparer = m_CompareOrchestra->newComparer2D( iComparerName, iImage);

  mdiArea->addSubWindow(comparer,Qt::SubWindow);
  comparer->parentWidget()->resize(300,300);
  comparer->show();
  tileAct->trigger();
  return comparer;
}

QGoComparer3D* QGoCompareGUI::newComparer3D(QString iComparerName, vtkImageData* iImage)
{
  QGoComparer3D *comparer = m_CompareOrchestra->newComparer3D( iComparerName, iImage);

  mdiArea->addSubWindow(comparer);
  comparer->parentWidget()->resize(300,300);
  comparer->show();
  tileAct->trigger();

  return comparer;
}

/*
QGoComparer3D* QGoCompareGUI::newComparer3D(QString iComparerName, QString iImagePath)
{





    QGoComparer3D *comparer = m_CompareOrchestra->newComparer3D( iComparerName, iImage);

    mdiArea->addSubWindow(comparer);

    return comparer;

}

*/

void QGoCompareGUI::deleteComparer2D(const int& iId)
{
  m_CompareOrchestra->deleteComparer2D(iId);
}

void QGoCompareGUI::deleteComparer3D(const int& iId)
{
  m_CompareOrchestra->deleteComparer3D(iId);
}



void QGoCompareGUI::closeEvent(QCloseEvent *event)
{

    if (m_CompareOrchestra!= NULL)
      delete m_CompareOrchestra;
    mdiArea->closeAllSubWindows();

    if (mdiArea->currentSubWindow())
      {
      event->ignore();
      }
    else
      {
      event->accept();
      }
}


void QGoCompareGUI::openfile()
{

  QString filename = QFileDialog::getOpenFileName(
    this,
    tr( "Select Image" ),"",
    tr( "Images (*.png *.bmp *.jpg *.jpeg *.tiff *.mha *.mhd *.img *.lsm)" )
    );

  if( !filename.isEmpty( ) )
    {
    this->OpenComparerForFile( filename );
    }

}

void QGoCompareGUI::OpenComparerForFile(QString& iFile)
{
  if( QFile::exists( iFile ) )
    {

    // parse extension
    QString ext = QFileInfo( iFile ).suffix();
    //if( ext.compare( "lsm", Qt::CaseInsensitive ) == 0 )
    //  {
    //  this->OpenLSMImage( m_CurrentFile, 0 );
    //  }
    //else
      {
      vtkImageReader2Factory* r_factory = vtkImageReader2Factory::New();
      vtkImageReader2* reader = r_factory->CreateImageReader2( iFile.toAscii().data() );

      reader->SetFileName( iFile.toAscii().data() );
      reader->Update();

      vtkImageData* image = reader->GetOutput();

      int dim[3];
      image->GetDimensions( dim );

      if( ( dim[0] != 1 ) && ( dim[1] != 1 ) && ( dim[2] != 1 ) )
        {
        std::cout<<"opening2"<<std::endl;
        newComparer3D( iFile, image );
        }
      else
        {
        newComparer2D( iFile, image );
        }
      reader->Delete();
      r_factory->Delete();
      }
    }

}



void QGoCompareGUI::snapshotAs()
{
QGoComparer* ScreenshotComparer = activeComparer();
  QString filename = QFileDialog::getSaveFileName(
    this,
    tr( "Select Image" ),"",
    tr( "Images (*.png *.bmp *.jpg *.jpeg *.tiff)" )
    );

  if( !filename.isEmpty( ) )
    {
    this->SaveSnapshotInFile( filename, ScreenshotComparer);
    }
}


void
QGoCompareGUI::SaveSnapshotInFile( QString& iFile, QGoComparer* Comparer)
{

QGoComparer3D* temp3DComparer = NULL;
QGoComparer2D* temp2DComparer = NULL;
GoFigure::FileType iType;

QString extension = iFile.section('.', -1);
QString nameOfScreenshot = iFile.section('/', -1);

if (extension.isEmpty() || nameOfScreenshot.isEmpty())
  {
  std::cerr << "QGoCompareGUI::SaveSnapshotInFile incorrect name of file"<< std::endl;
  return;
  }


std::cout << iFile.toStdString() <<std::endl;
std::cout << extension.toStdString() <<std::endl;

std::cout << nameOfScreenshot.toStdString() <<std::endl;

// file extension  parsing
  if ( (extension.contains("jpg",Qt::CaseInsensitive))
       || (extension.contains("jpeg",Qt::CaseInsensitive)) )
    iType = GoFigure::JPEG;
  else
  if (extension.contains("bmp",Qt::CaseInsensitive))
    iType = GoFigure::BMP;
  else
  if (extension.contains("png",Qt::CaseInsensitive))
    iType = GoFigure::PNG;
  else
  if (extension.contains("eps",Qt::CaseInsensitive))
    iType = GoFigure::EPS;
  else
  if (extension.contains("tiff",Qt::CaseInsensitive))
    iType = GoFigure::TIFF;
  else
    {
    std::cerr << "QGoCompareGUI::SaveSnapshotInFile couldn't find appropriate extension"<< std::endl;
    return;
    }


if (Comparer != 0)
  if (Comparer->GetComparerType()==3) // if we take a snapshot of a 3D comparer
    {
    temp3DComparer = static_cast<QGoComparer3D*>(Comparer);

    temp3DComparer->GetFullScreenView();

    switch (temp3DComparer->GetFullScreenView())
      {
      case 0 :
        temp3DComparer->SnapshotViewXYZ(iType,iFile);
        break;

      case 1 :
        temp3DComparer->SnapshotViewXY(iType,iFile);
        break;

      case 2 :
        temp3DComparer->SnapshotView2(iType,iFile);
        break;

      case 3 :
        temp3DComparer->SnapshotView3(iType,iFile);
        break;

      case 4 :
        temp3DComparer->SnapshotViewXYZ(iType,iFile);
        break;

      default:
        std::cerr << "QGoCompareGUI::SaveSnapshotInFile can't access fullscreen view"<< std::endl;
        return;
        break; // facultative
      }
    }
  else // if we take a snapshot of a 2D comparer
    {
    temp2DComparer = static_cast<QGoComparer2D*>(Comparer);
    temp2DComparer->SnapshotViewXY(iType,iFile);
    }
else
  {
  std::cerr << "QGoCompareGUI::SaveSnapshotInFile comparer pointer error"<< std::endl;
  return;
  }


}








void QGoCompareGUI::imageinfo()
{

  std::stringstream imageinfo;
    if (activeComparer())
    {
    activeComparer()->PrintOs(imageinfo);
    QMessageBox::about(this, tr("Image Informations"),
            QString::fromStdString(imageinfo.str()));
    }

}





void QGoCompareGUI::synchronize()
{
  if (m_CompareOrchestra->isSynchronizing())
    m_CompareOrchestra->unSynchronizeOpenComparers();
  else
    m_CompareOrchestra->synchronizeOpenComparers();

}

void QGoCompareGUI::FullscreenXY()
{
if (activeComparer() != 0)
  if (activeComparer()->GetComparerType()==3)
    static_cast<QGoComparer3D*>(activeComparer())->SetFullXYScreenView();

}

void QGoCompareGUI::FullscreenXZ()
{
if (activeComparer() != 0)
  if (activeComparer()->GetComparerType()==3)
    static_cast<QGoComparer3D*>(activeComparer())->SetFullXZScreenView();

}

void QGoCompareGUI::FullscreenYZ()
{
if (activeComparer() != 0)
  if (activeComparer()->GetComparerType()==3)
    static_cast<QGoComparer3D*>(activeComparer())->SetFullYZScreenView();

}

void QGoCompareGUI::FullscreenXYZ()
{
if (activeComparer() != 0)
  if (activeComparer()->GetComparerType()==3)
    static_cast<QGoComparer3D*>(activeComparer())->SetFullXYZScreenView();
}

void QGoCompareGUI::Quadscreen()
{
if (activeComparer() != 0)
  if (activeComparer()->GetComparerType()==3)
    static_cast<QGoComparer3D*>(activeComparer())->SetQuadView();

}





void QGoCompareGUI::about()
{
   QMessageBox::about(this, tr("About QGoCompare"),
            tr("<b>QGoCompare</b> lets you open multiple VTK/ITK images from a "
               "VTK or ITK pipeline and compare them."
               "This program uses Qt, VTK, ITK and GoFigure2 libraries"));
}

void QGoCompareGUI::aboutGF2()
{
   QMessageBox::about(this, tr("About GoFigure2"),
            tr("<b>GoFigure2</b> is a cross-platform, free open source software (FOSS),"
               "for visualizing, processing and analysing of bioimages"
               "http://gofigure2.sourceforge.net/"));
}

void QGoCompareGUI::updateMenus()
{
    bool hasComparer = (activeComparer() != 0);
    syncAct->setEnabled(hasComparer);
    closeAct->setEnabled(hasComparer);
    closeAllAct->setEnabled(hasComparer);
    tileAct->setEnabled(hasComparer);
    cascadeAct->setEnabled(hasComparer);

    // if it is a 3D view, we activate the change view actions
    bool has3DComparer = ( (hasComparer)
                            && (activeComparer()->GetComparerType()==3) );
        XYviewAct->setEnabled(has3DComparer);
        XZviewAct->setEnabled(has3DComparer);
        YZviewAct->setEnabled(has3DComparer);
        XYZviewAct->setEnabled(has3DComparer);
        QuadviewAct->setEnabled(has3DComparer);
        View3DToolBar->setVisible(has3DComparer);

}

void QGoCompareGUI::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);


}






void QGoCompareGUI::createActions()
{
    openfileAct = new QAction(/*QIcon(":/images/open.png"),*/ tr("&Open an image file"), this);
    openfileAct->setShortcuts(QKeySequence::Open);
    openfileAct->setStatusTip(tr("Open an image from file"));
    connect(openfileAct, SIGNAL(triggered()), this, SLOT(openfile()));


    openmemAct = new QAction(/*QIcon(":/images/open.png"), */tr("Open from &memory"), this);
//    openmemAct->setShortcuts(QKeySequence::Open);
    openmemAct->setStatusTip(tr("Open an image from memory"));
    connect(openmemAct, SIGNAL(triggered()), this, SLOT(openfile()));


    syncAct = new QAction(tr("&Synchronize images"), this);
//    syncAct->setShortcuts(QKeySequence::SaveAs);
    syncAct->setStatusTip(tr("Synchronize open images for point-to-point comparison"));
    connect(syncAct , SIGNAL(triggered()), this, SLOT(synchronize()));


    snapshotAsAct = new QAction(tr("Sna&pshot..."), this);
    snapshotAsAct->setShortcuts(QKeySequence::SaveAs);
    snapshotAsAct->setStatusTip(tr("Save a Snapshot as.."));
    connect(snapshotAsAct, SIGNAL(triggered()), this, SLOT(snapshotAs()));


//! [0]
    exitAct = new QAction(tr("E&xit"), this);
#if ( ( QT_MAJOR_VERSION == 4 ) && ( QT_MINOR_VERSION >= 6 ) )
    exitAct->setShortcuts(QKeySequence::Quit);
#endif
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
//! [0]


    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active image"));
    connect(closeAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeActiveSubWindow()));


    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all images"));
    connect(closeAllAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeAllSubWindows()));


    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the images"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the images"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));


    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutGF2Act = new QAction(tr("About &GoFigure2"), this);
    aboutGF2Act->setStatusTip(tr("Show the Gofigure2 About box"));
    connect(aboutGF2Act, SIGNAL(triggered()), this, SLOT(aboutGF2()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    XYviewAct = new QAction(tr("XY view"), this);
    aboutQtAct->setStatusTip(tr("Shows XY view"));
    connect(XYviewAct, SIGNAL(triggered()), this, SLOT(FullscreenXY()));

    XZviewAct = new QAction(tr("XZ view"), this);
    aboutQtAct->setStatusTip(tr("Shows XZ view"));
    connect(XZviewAct, SIGNAL(triggered()), this, SLOT(FullscreenXZ()));

    YZviewAct = new QAction(tr("YZ view"), this);
    aboutQtAct->setStatusTip(tr("Shows YZ view"));
    connect(YZviewAct , SIGNAL(triggered()), this, SLOT(FullscreenYZ()));

    XYZviewAct = new QAction(tr("3D view"), this);
    aboutQtAct->setStatusTip(tr("Show 3D view"));
    connect(XYZviewAct, SIGNAL(triggered()), this, SLOT(FullscreenXYZ()));

    QuadviewAct = new QAction(tr("Quad-view"), this);
    aboutQtAct->setStatusTip(tr("Show 3D view and XY,XZ,YZ projections"));
    connect(QuadviewAct, SIGNAL(triggered()), this, SLOT(Quadscreen()));

}

void QGoCompareGUI::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openfileAct);
    fileMenu->addAction(openmemAct);
    fileMenu->addAction(snapshotAsAct);
    fileMenu->addSeparator();


    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));


    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    helpMenu->addAction(aboutGF2Act);
}

void QGoCompareGUI::createToolBars()
{
    ToolBar = addToolBar(tr("ImageActions"));
    ToolBar->addAction(openmemAct);
    ToolBar->addAction(openfileAct);
    ToolBar->addAction(snapshotAsAct);
    ToolBar->addAction(syncAct);

    View3DToolBar = addToolBar(tr("Image 3D View selection"));
    View3DToolBar ->addAction(XYviewAct);
    View3DToolBar ->addAction(XZviewAct);
    View3DToolBar ->addAction(YZviewAct);
    View3DToolBar ->addAction(XYZviewAct);
    View3DToolBar ->addAction(QuadviewAct);
}

void QGoCompareGUI::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}



QGoComparer *QGoCompareGUI::activeComparer()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return static_cast<QGoComparer*>(activeSubWindow->widget());
    return 0;
}


QMdiSubWindow *QGoCompareGUI::findComparer(const QString& iComparerName)
{
  foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        QGoComparer *Comparer = qobject_cast<QGoComparer *>(window->widget());
        if (Comparer->GetName() == iComparerName)
            return window;
    }
    return 0;
}



void QGoCompareGUI::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}
