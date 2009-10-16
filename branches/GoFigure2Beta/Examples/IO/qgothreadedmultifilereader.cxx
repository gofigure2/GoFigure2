#include <QApplication>
#include <QMessageBox>

#include "itkMegaCaptureImport.h"
#include "QGoThreadedMultiFileReader.h"
#include "vtkImageData.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cout <<"Usage: ./multifilereader(.exe) takes 2 arguments" <<std::endl;
    std::cout <<"1-filename (.jpg)" <<std::endl;
    std::cout <<"2-timebased (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }

  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  itk::MegaCaptureImport::Pointer importer = itk::MegaCaptureImport::New();
  importer->SetFileName( argv[1] );
  importer->Update();
  FileListType* listoffiles = importer->GetOutput();

  QMessageBox* messagebox = new QMessageBox;
  messagebox->setText( "Thread ended" );

  QGoThreadedMultiFileReader* thread = new QGoThreadedMultiFileReader;
  thread->SetInput( listoffiles,
      itk::MultiFileReader::JPEG,
      atoi( argv[2] ) == 0 );

  QObject::connect( thread, SIGNAL( finished() ),
    messagebox, SLOT( show() ) );

  thread->start();

  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();

  vtkImageData* image = thread->GetOutput();
  std::cout <<*image <<std::endl;

  image->Delete();

  delete thread;
  delete messagebox;

  return output;
}