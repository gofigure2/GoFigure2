#include <QApplication>
#include "QGoPrintDatabase.h"
#include "ContourMeshStructure.h"
#include "ContourMeshStructureHelper.h"
#include "vtkPolyDataWriter.h"
#include "vtkSmartPointer.h"
#include <sstream>
#include <string>

int main( int argc, char** argv )
{
  if( argc < 7 )
    {
    std::cout <<"Usage: " << argv[0] << std::endl;
    std::cout << " nameDB server user password ImgSessionID ImgSessionName" << std::endl;
    return EXIT_FAILURE;
    }

  typedef std::vector<ContourMeshStructure> ContourMeshVectorType;
  typedef ContourMeshVectorType::iterator ContourMeshIteratorType;

  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QGoPrintDatabase* win = new QGoPrintDatabase();
  win->SetDatabaseVariables( argv[1], argv[2], argv[3],argv[4], atoi( argv[5] ), argv[6] );
  win->FillTableFromDatabase();
  ContourMeshVectorType* contours = win->GetTracesInfoListForVisu( "contour" );
  std::cout << contours->size() << std::endl;

  unsigned int contourId, meshId, timePt;
  ContourMeshIteratorType It = contours->begin();
  while( It != contours->end() )
  {
    contourId = (*It).TraceID;
    meshId = (*It).CollectionID;
    timePt = (*It).TCoord;

    std::stringstream out;
    out << meshId << '_' << timePt;

    std::string name = out.str() + ".vtk";
    vtkSmartPointer<vtkPolyDataWriter> writer =
      vtkSmartPointer<vtkPolyDataWriter>::New();
    writer->SetInput( (*It).Nodes );
    writer->SetFileName( name.c_str() );
    writer->Write();

    ++It;
  }

  app.closeAllWindows();
  delete win;

  return EXIT_SUCCESS;
}
