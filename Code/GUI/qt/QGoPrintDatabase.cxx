#include <qstringlist.h>
#include <qwidget.h>
//#include <QtGui>
#include <QTableWidgetItem>
#include "QGoPrintDatabase.h"
#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStringArray.h"
#include "vtkStdString.h"

QGoPrintDatabase::QGoPrintDatabase(QString ServerName,QString login,
    QString Password, QString DBName)
{
  m_Server = ServerName;
  m_User = login;
  m_Password = Password;
  m_NameDB=DBName;

  this->setupUi( this );
  this->setWindowTitle(DBName);

  DBTabWidget->setTabPosition(QTabWidget::West);
  DBTabWidget->setTabShape(QTabWidget::Triangular);


  QPrintColumnNames ("figure");
  QPrintColumnNames ("mesh");
  QPrintColumnNames ("track");
  DBTabWidget->removeTab(0);
  QPrintColumnNames ("lineage");
 /* QPrintColumnNames ("figure", QFigureTable);
  QPrintColumnNames ("mesh", QMeshTable);
  QPrintColumnNames ("track", QTrackTable);
  QPrintColumnNames ("lineage", QLineageTable);*/

}

QGoPrintDatabase::~QGoPrintDatabase()
{
}


QStringList QGoPrintDatabase::GetTableContentFromDB(QString TableName)
{
  QStringList List;
  QString SQLquery;

  // CONFIGURE CONNECTOR
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  //DataBaseConnector->SetHostName("localhost");
  DataBaseConnector->SetHostName(m_Server.toAscii().data());
  DataBaseConnector->SetUser(m_User.toAscii().data());
  DataBaseConnector->SetPassword(m_Password.toAscii().data());
  DataBaseConnector->SetDatabaseName(m_NameDB.toStdString().c_str());


  // CONNECT TO SERVER
  DataBaseConnector->Open("");
  if( ! DataBaseConnector->IsOpen() )
  {
    DataBaseConnector->Delete();
    exit(0);
  }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  SQLquery=QString("select * from %1;").arg(TableName);
  vtkStdString createQuery(SQLquery.toStdString());

  query->SetQuery( createQuery.c_str() );

  if( !query->Execute() )
  {
    std::cerr << "Create query failed" << std::endl;
    DataBaseConnector->Delete();
    query->Delete();
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

void QGoPrintDatabase::QPrintColumnNames (QString TableName)
{
  QStringList listColumnNames;
  listColumnNames=GetTableContentFromDB(TableName);
  int numberCol=listColumnNames.size();
  QTableWidget* QTabName = new QTableWidget;
  this->DBTabWidget->addTab(QTabName,TableName);
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
