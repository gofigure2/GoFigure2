#include <QApplication>
#include <QTimer>

#include "vtkSmartPointer.h"
#include "vtkPNGReader.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkMetaImageReader.h"
/*
#include "QGoSynchronizedView.h"
#include  "QGoSynchronizedViewManager.h"
#include "vtkMetaImageReader.h"
#include "QGoSynchronizedView3D.h"
*/
#include "QGoSynchronizedViewMainWindow.h"

#include <QStringList>
#include <QString>

#include "itkImageFileReader.h"
#include "itkImage.h"

int main(int argc, char** argv)
{

  if (argc != 4)
    {
    std::cout << "Usage : QGoSynchronizedViewGUITest(.exe) " << std::endl;
    std::cout << "1-file.png" << std::endl;
    std::cout << "2-file.mhd" << std::endl;
    std::cout << "3-test (boolean)" << std::endl;
    return EXIT_FAILURE;
    }

  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain("http://gofigure2.sourceforge.net");

  // ITK Typedefs
  // ITK Reader Typedef
  typedef double InputPixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image<InputPixelType, Dimension> InputImage3DType;
  typedef InputImage3DType::Pointer             InputImage3DPointer;

  //itk reader
  typedef itk::ImageFileReader<InputImage3DType> itkReaderType;
  itkReaderType::Pointer itkReader = itkReaderType::New();
  itkReader->SetFileName(argv[2]);
  itkReader->Update();

  // create 2 2D images from 1
  vtkSmartPointer<vtkPNGReader> reader = vtkSmartPointer<vtkPNGReader>::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkSmartPointer<vtkImageGaussianSmooth> filter1 =
    vtkSmartPointer<vtkImageGaussianSmooth>::New();
  filter1->SetInputConnection(reader->GetOutputPort());
  filter1->Update();

  // create 2 3D images from 1
  vtkSmartPointer<vtkMetaImageReader> reader3D = vtkSmartPointer<vtkMetaImageReader>::New();
  reader3D->SetFileName(argv[2]);
  reader3D->Update();

  vtkSmartPointer<vtkImageGaussianSmooth> filter13D =
    vtkSmartPointer<vtkImageGaussianSmooth>::New();
  filter13D->SetInputConnection(reader3D->GetOutputPort());
  filter13D->Update();

  QString cp0 = "comp0";
  QString cp1 = "comp1";

  QString cp03D = "comp03D";
  QString cp13D = "comp13D";
  QString cp23D = "compITK13D";
  QTimer* timer = new QTimer;
  timer->setSingleShot(true);

  QGoSynchronizedViewMainWindow* SynchronizedViewMainWindow
    = new QGoSynchronizedViewMainWindow();

  QObject::connect(timer, SIGNAL(timeout()), qApp, SLOT(closeAllWindows()));

  if (atoi(argv[3]) == 1) timer->start(2000);

  // open some images for the test
  SynchronizedViewMainWindow->newSynchronizedView(
    cp0, reader->GetOutput());
  SynchronizedViewMainWindow->newSynchronizedView(
    cp1, filter1->GetOutput());
  SynchronizedViewMainWindow->newSynchronizedView(
    cp03D, reader3D->GetOutput());
  SynchronizedViewMainWindow->newSynchronizedView(
    cp13D, filter13D->GetOutput());
  SynchronizedViewMainWindow->newSynchronizedView<InputPixelType>(
    cp23D, itkReader->GetOutput());

  SynchronizedViewMainWindow->Update();
  SynchronizedViewMainWindow->show();

  app.processEvents();
  int output = app.exec();
  return output;

  delete timer;
}
