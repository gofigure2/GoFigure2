/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Revision: 270 $  // Revision of last commit
  Date: $Date: 2009-06-11 15:11:56 -0400 (Thu, 11 Jun 2009) $  // Date of last commit
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
#include "GoDBFigureRow.h"
#include "CreateDataBaseHelper.h"
#include <iostream>
#include <QCloseEvent>

QGoPrintDatabase::QGoPrintDatabase()
{
  this->setupUi( this );
  DBTabWidget->setTabPosition(QTabWidget::West);
  DBTabWidget->setTabShape(QTabWidget::Triangular);
  DBTabWidget->removeTab(0);
  FigureTable = new QTableWidgetChild;
  this->setContextMenuPolicy(Qt::CustomContextMenu);

  
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
    this, SLOT(createContextMenu(const QPoint &)));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QGoPrintDatabase::~QGoPrintDatabase()
{
}

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
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QGoPrintDatabase::FillTableFromDatabase(QString iNameDB,QString iServer,QString iUser,
  QString iPassword,int iExpID,QString iExpName)
{
  m_NameDB = iNameDB;
  m_Server = iServer;
  m_User = iUser;
  m_Password = iPassword;
  m_ExpID = iExpID;
  m_ExpName = iExpName;
  
  this->setWindowTitle(QString("DB: %1 - Exp: %2").arg(m_NameDB).arg(m_ExpName));
  GetContentAndDisplayFromDB< GoDBFigureRow     >("figure", FigureTable);

  //Need to create GoDBMeshRow,etc...first
  //QPrintTable ("mesh");
  //QPrintTable ("track");
  //QPrintTable ("lineage");

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QGoPrintDatabase::closeEvent(QCloseEvent* event)
{
  int r = QMessageBox::warning(this, tr(""),
                        tr("Are you sure you want to close\n"
                           "permanently the table?"),
                          QMessageBox::Yes,
                          QMessageBox::No|QMessageBox::Default);
  if (r == QMessageBox::Yes)
    {
    event->accept();
    QByteArray state = FigureTable->horizontalHeader()->saveState();
    QSettings settings( "MegasonLab", "Gofigure2" );
    settings.setValue("StateTableWidget", state);
    }
  else
    {
    event->ignore();
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QGoPrintDatabase::UpdateTableFromDB()
{  
  UpdateContentAndDisplayFromDB< GoDBFigureRow >("figure", FigureTable);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QGoPrintDatabase::createContextMenu(const QPoint &pos)
{
  QMenu* ContourMenu = new QMenu;
  ContourMenu->addAction(tr("Delete Contour"));
  ContourMenu->addAction(tr("Create Mesh"));
  //ContourMenu->addAction(tr("Delete Contour"),this,slot());
  ContourMenu->exec(this->mapToGlobal(pos));

}
void QGoPrintDatabase::DeleteContour()
{
  
}