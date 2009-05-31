#include <qstringlist.h>
#include <qwidget.h>
//#include <QtGui>
#include <QTableWidgetItem>
#include "QPrintDatabase.h"
#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStringArray.h"
#include "vtkStdString.h"

QPrintDatabase::QPrintDatabase(QString NameDB)
{
  this->setupUi( this );
  this->setWindowTitle(NameDB);
  DBTabWidget->setTabPosition(QTabWidget::West);
  DBTabWidget->setTabShape(QTabWidget::Triangular);
  m_NameDB=NameDB;

  QPrintColumnNames ("figure", QFigureTable);
  QPrintColumnNames ("mesh", QMeshTable);
  QPrintColumnNames ("track", QTrackTable);
  QPrintColumnNames ("lineage", QLineageTable);

}

QPrintDatabase::~QPrintDatabase()
{
}


QStringList QPrintDatabase::GetTableContentFromDB(QString TableName)
{
  QStringList List;
  QString SQLquery;

  // CONFIGURE CONNECTOR
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName("localhost");
  DataBaseConnector->SetUser("gofigure");
  DataBaseConnector->SetPassword("gofigure");
  DataBaseConnector->SetDatabaseName(m_NameDB.toStdString().c_str());


  // CONNECT TO SERVER
  DataBaseConnector->Open("");
  if( ! DataBaseConnector->IsOpen() )
  {
    exit(0);
  }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  SQLquery=QString("select * from %1;").arg(TableName);
  vtkStdString createQuery(SQLquery.toStdString());

  query->SetQuery( createQuery.c_str() );

  if( !query->Execute() )
  {
    std::cerr << "Create query failed" << std::endl;
    exit(0);
  }

  for( int col = 0; col < query->GetNumberOfFields(); ++col )
  {
    List <<query->GetFieldName( col );
  }

  while ( query->NextRow() )
  {
    for ( int field = 0; field < query->GetNumberOfFields(); ++ field )
    {
      if ( field > 0 ) std::cout << ", ";
      List << query->DataValue( field ).ToString().c_str();
    }
    std::cout << endl;
  }

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();

  return List;
}

void QPrintDatabase::QPrintColumnNames (QString TableName, QTableWidget* QTabName)
{
  QStringList listColumnNames;
  listColumnNames=GetTableContentFromDB(TableName);
  int numberCol=listColumnNames.size();

  QTabName->setRowCount(15);
  QTabName->setColumnCount(numberCol);

  for ( int i = 0; i < numberCol; i++ )
  {
    QTableWidgetItem* HeaderCol=new QTableWidgetItem;
    QString NameHeader;
    NameHeader=listColumnNames[i];
    HeaderCol->setText(NameHeader);
    QFont serifFont("Arial", 10, QFont::Bold);
    HeaderCol->setFont(serifFont);
    QTabName->setHorizontalHeaderItem(i,HeaderCol);
  }
}
