/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Revision: 602 $  // Revision of last commit
  Date: $Date: 2009-08-27 07:47:13 -0400 (Thu, 27 Aug 2009) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include <qstringlist.h>
#include <qwidget.h>
#include <QDialog>
#include <QInputDialog>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QSettings>
#include <QMenu>

#include "QGoPrintDatabase.h"
#include "QTableWidgetChild.h"
#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStringArray.h"
#include "vtkStdString.h"
#include "GoDBRecordSet.h"
#include "GoDBRecordSetHelper.h"
#include "GoDBContourRow.h"
#include "GoDBMeshRow.h"
//#include "CreateDataBaseHelper.h"
#include "QueryDataBaseHelper.h"
#include "ConvertToStringHelper.h"
#include <iostream>
#include <QCloseEvent>

QGoPrintDatabase::QGoPrintDatabase()
{
  this->setupUi( this );
  DBTabWidget->setTabPosition(QTabWidget::West);
  DBTabWidget->setTabShape(QTabWidget::Triangular);
  DBTabWidget->removeTab(0);
  ContourTable = new QTableWidgetChild( this );
  MeshTable = new QTableWidgetChild( this );
  CollectionOfContours = new GoDBCollectionOfTraces("mesh",
  "meshID","contour", "ContourID");
  CollectionOfMeshes = new GoDBCollectionOfTraces( "track",
  "trackID", "mesh", "meshID");
  this->setContextMenuPolicy(Qt::CustomContextMenu);


  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
    this, SLOT(CreateContextMenu(const QPoint &)));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoPrintDatabase::~QGoPrintDatabase()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::QPrintColumnNames (QString TableName,
  std::vector< std::string > ColumnNames, QTableWidgetChild* QTabTableName )
{
  int numberCol=ColumnNames.size();
  this->DBTabWidget->addTab(QTabTableName,TableName);
  QTabTableName->setColumnCount(numberCol);

  for ( int i = 0; i < numberCol; i++ )
    {
    QTableWidgetItem* HeaderCol=new QTableWidgetItem;
    std::string NameHeader;
    NameHeader =ColumnNames[i];
    HeaderCol->setText(NameHeader.c_str());
    QFont serifFont("Arial", 10, QFont::Bold);
    HeaderCol->setFont(serifFont);
    QTabTableName->setHorizontalHeaderItem(i,HeaderCol);
    }

  QTabTableName->horizontalHeader()->setSortIndicatorShown(true);
  QTabTableName->setSortingEnabled(true);
  QTabTableName->horizontalHeader()->setMovable(true);

  QObject::connect( QTabTableName->horizontalHeader(),
    SIGNAL( sortIndicatorChanged(int,Qt::SortOrder) ),
    QTabTableName,SLOT( sortItems(int,Qt::SortOrder)) );

  QSettings settings( "MegasonLab", "Gofigure2" );
  QByteArray stateTableWidget = settings.value("StateTableWidget").toByteArray();
  QTabTableName->horizontalHeader()->restoreState(stateTableWidget);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::FillTableFromDatabase( std::string iNameDB,
  std::string iServer, std::string iUser,std::string iPassword,
  unsigned int iImgSessionID, std::string iImgSessionName )
{
  m_DatabaseConnector = OpenDatabaseConnection(iServer,iUser,iPassword,iNameDB);

  m_ImgSessionID = iImgSessionID;
  m_ImgSessionName = iImgSessionName;

  this->setWindowTitle(QString("Imaging Session: %1 ").arg(m_ImgSessionName.c_str()));

  GetContentAndDisplayFromDB< GoDBContourRow >("contour", ContourTable);
  //CollectionOfContours->SetDatabaseVariables(m_DatabaseConnector);
  GetContentAndDisplayFromDB< GoDBMeshRow   >("mesh", MeshTable);
  //CollectionOfMeshes->SetDatabaseVariables(m_DatabaseConnector);

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::closeEvent(QCloseEvent* iEvent)
{
  int r = QMessageBox::warning(this, tr(""),
                        tr("Are you sure you want to close\n"
                           "permanently the table?"),
                          QMessageBox::Yes,
                          QMessageBox::No|QMessageBox::Default);
  if (r == QMessageBox::Yes)
    {
    iEvent->accept();
    QByteArray stateContourTable = ContourTable->horizontalHeader()->saveState();
    QByteArray stateMeshTable = MeshTable->horizontalHeader()->saveState();
    QSettings settings( "MegasonLab", "Gofigure2" );
    settings.setValue("StateContourTable", stateContourTable);
    settings.setValue("StateMeshTable", stateMeshTable);
    }
  else
    {
    iEvent->ignore();
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::UpdateTableFromDB()
{
  UpdateContentAndDisplayFromDB< GoDBContourRow >("contour", ContourTable);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateContextMenu(const QPoint &iPos)
{
  QMenu* ContextMenu = new QMenu;
  ContextMenu->addAction(tr("Delete Traces"),this,SLOT(DeleteTraces()));
  ContextMenu->addAction(tr("Create New Collection"),this,SLOT(CreateCorrespondingCollection()));
  ContextMenu->addAction(tr("Add to existing Collection"),this,SLOT(AddToExistingCollection()));
  ContextMenu->exec(this->mapToGlobal(iPos));

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTraces()
{
  int TabIndex = InWhichTableAreWe();

  switch (TabIndex)
    {
    case 0: //contour
        {
        //add the tableWidgetChild in the CollectionOfTraces?
        QStringList ContoursToDelete = this->ContourTable->ValuesForSelectedRows("contourID");
        CollectionOfContours->DeleteTraces(ContoursToDelete);
        this->UpdateContentAndDisplayFromDB<GoDBContourRow>("contour", ContourTable);
        break;
        }
    case 1: //mesh
        {
        QStringList MeshesToDelete = this->MeshTable->ValuesForSelectedRows("meshID");
        CollectionOfMeshes->DeleteTraces(MeshesToDelete);
        this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("mesh",MeshTable);
        break;
        }
    default:
      {
      std::cout<<"error, tab doesn't exist";
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      break;
      }
    }

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateCorrespondingCollection()
{
  int TabIndex = InWhichTableAreWe();

  switch (TabIndex)
    {
    case 0: //contour
        {
        //add the tableWidgetChild in the CollectionOfTraces?
        QStringList ListSelectedTraces = this->ContourTable->ValuesForSelectedRows("contourID");
        CollectionOfContours->CreateNewCollectionFromSelection<GoDBMeshRow>(ListSelectedTraces);
        this->UpdateContentAndDisplayFromDB<GoDBContourRow>("contour", ContourTable);
        this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("mesh",MeshTable);
        break;
        }
    case 1: //mesh
        {
        //QStringList ListSelectedMeshes = this->MeshTable->ValuesForSelectedRows("meshID");
        //CollectionOfMeshes->CreateNewCollectionFromSelection<GoDBTrackRow>();
        //this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("Mesh",MeshTable);
        //this->UpdateContentAndDisplayFromDB<GoDBTrackRow>("Track",TrackTable);
        break;
        }
    default:
      {
      std::cout<<"error, tab doesn't exist";
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      break;
      }
    }

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//
//  Return negative value on error
//  Return 0 if we are in the FIgure Table
//  Return 1 if we are in the Mesh   Table
//
int QGoPrintDatabase::InWhichTableAreWe ()
{
  int CurrentIndex = this->DBTabWidget->currentIndex();
  QString TabName = this->DBTabWidget->tabText(CurrentIndex);

  int TabIndex = -1; // Default value.

  if (TabName == "contour")
    {
    TabIndex = 0;
    }
  if (TabName == "mesh")
    {
    TabIndex = 1;
    }

  return TabIndex;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::AddToExistingCollection()
{
  QStringList items;
  QString LabelDialog;
  int TabIndex = InWhichTableAreWe();

  switch (TabIndex)
    {
    case 0: //contour
        {
        items = CollectionOfContours->ListCollectionID();
        LabelDialog = tr("Choose the Mesh ID you want\n the selected contours to be part of: ");
        break;
        }
    case 1: //mesh
        {
        items = CollectionOfMeshes->ListCollectionID();
        LabelDialog = tr("Choose the Track ID you want\n the selected meshes to be part of: ");
        break;
        }
    default:
      {
      std::cout<<"error, tab doesn't exist";
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      break;
      }
    }
  bool ok;
  QString CollectionID = QInputDialog::getItem(this, tr("Collection ID"),
                         LabelDialog, items,0,false,&ok);

  if (ok && !CollectionID.isEmpty())
    {
    switch (TabIndex)
      {
      case 0: //contour
          {
          QStringList ListContours = this->ContourTable->ValuesForSelectedRows("contourID");
          CollectionOfContours->AddSelectedTracesToCollection(ListContours,CollectionID.toInt());
          this->UpdateContentAndDisplayFromDB<GoDBContourRow>("Contour", ContourTable);
          break;
          }
      case 1: //mesh
          {
          //QStringList ListMeshes = this->ContourTable->ValuesForSelectedRows("meshID");
          //CollectionOfMeshes->AddSelectedTracesToCollection(ListMeshes,CollectionID.toInt());
          //this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("Mesh", MeshTable);
          break;
          }
      default:
        {
        std::cout<<"error, tab doesn't exist";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        break;
        }
      }
    }

}
