#include <qapplication.h>
#include "QGoWizardDB.h"

 int main(int argc, char *argv[])
 {
     //Q_INIT_RESOURCE(qgocreatedb);

     QApplication app(argc, argv);
     QGoWizardDB wizard;
     wizard.show();
     return app.exec();
 }
