#include <qapplication.h>
#include "QGoWizardDB.h"

 
 int main(int argc, char *argv[])
 {
  //Q_INIT_RESOURCE(qgocreatedb);
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QGoWizardDB* wizard = new QGoWizardDB;
  wizard->show();

  int output = app.exec();

  app.closeAllWindows();

  std::vector<std::vector<std::string> > filenames = wizard->GetFilenamesFromDB();

  for (unsigned int i =0; i < filenames.size(); i++)
    {
    for (unsigned int j = 0; j < filenames[i].size();j++)
      {
      std::cout<<"image filename with channel "
        <<i <<" " <<filenames[i][j].c_str()<<std::endl;
      }
    }

  delete wizard;

  return output;
 }

