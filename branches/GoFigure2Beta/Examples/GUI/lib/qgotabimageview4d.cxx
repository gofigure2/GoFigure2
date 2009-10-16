#include <QApplication>

#include "QGoTabImageView4D.h"
#include "itkMegaCaptureImport.h"

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  typedef itk::MegaCaptureImport MegaCaptureImportType;
  MegaCaptureImportType::Pointer
    importFileInfoList = MegaCaptureImportType::New();
  importFileInfoList->SetFileName( argv[1] );
  importFileInfoList->Update();

  QGoTabImageView4D* tab = new QGoTabImageView4D;
  tab->SetMultiFiles( importFileInfoList->GetOutput(), 0, 0 );
  tab->Update();

//   tab->show();
  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();

  delete tab;

  return output;
}