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
#ifndef __QGoPrintDatabase_h
#define __QGoPrintDatabase_h

#include <QWidget>
#include <QTableWidget>
#include "ui_QGoPrintDatabase.h"
#include "MegaVTK2Configure.h"
#include "GoDBRecordSet.h"
#include "GoDBFigureRow.h"
#include "QTableWidgetChild.h"

class QGoPrintDatabase : public QWidget,
  private Ui::WidgetPrintDatabase
{
  Q_OBJECT

public:

  explicit QMEGAVTKADDON2_EXPORT QGoPrintDatabase();
  virtual QMEGAVTKADDON2_EXPORT ~QGoPrintDatabase();
  void QMEGAVTKADDON2_EXPORT Fill_Database(QString ServerName,QString login,
      QString Password, QString DBName,
      int ExpID,QString ExpName);

protected:
  QTableWidgetChild* QPrintColumnNames (QString TableName, std::vector< std::string > ColumnNames);

  /*get the columns names and the values of the table (type T) from the database, then display them in the
  QTableWidgetchild: */
  template< class myT >
  void GetContentAndDisplayFromDB( QString ServerName, QString User,
    QString Password, QString NameDB,QString TableName )
    {
    std::vector< std::string > ColumnNamesContainer;

    typedef GoDBRecordSet< myT >                  SetType;
    typedef typename SetType::InternalObjectType  InternalObjectType;
    typedef typename SetType::RowContainerType    RowContainerType;

    RowContainerType* RowContainer;

    SetType* mySet = new SetType;
    mySet->SetServerName( ServerName.toStdString() );
    mySet->SetDataBaseName( NameDB.toStdString() );
    mySet->SetTableName( TableName.toStdString() );
    mySet->SetUser( User.toStdString());
    mySet->SetPassword( Password.toStdString() );
    mySet->PopulateFromDB();

    myT myNewObject;
    mySet->AddObject( myNewObject );

    ColumnNamesContainer = mySet->GetColumnNamesContainer();
    QTableWidgetChild* NewTable = new QTableWidgetChild;
    NewTable = QPrintColumnNames( TableName, ColumnNamesContainer );
    RowContainer = mySet->GetRowContainer();
    if( RowContainer->size() < 2 )
      {
      std::cout<<"Table empty";
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      }
    else
      {
      PrintOutContentFromDB< myT >( RowContainer, NewTable );
      }
    delete mySet;
   };

  /*Display the values stored in the RowContainer (list of type T)
  in the QTableWidgetChild TableToFill: */
  template<class myT>
  void PrintOutContentFromDB(
    typename GoDBRecordSet< myT >::RowContainerType *RowContainer,
    QTableWidgetChild* TableToFill )
    {
    for( unsigned int i = 0; i < RowContainer->size()-1; ++i )
       {
       std::map<std::string,std::string> Map 
         = (*RowContainer)[i].second.LinkColumnNamesAndValues();
       if( TableToFill->columnCount() != (int)(Map.size()) )
         {
         std::cout << "Pb, row is not the same size as the number of col";
         std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
         std::cout << std::endl;
         return;
         }
       else
         {
         for( int j = 0; j< TableToFill->columnCount();j++)
           {
           QTableWidgetItem* HeaderCol = new QTableWidgetItem;
           HeaderCol = TableToFill->horizontalHeaderItem(j);
           std::map<std::string,std::string>::iterator it = Map.find(HeaderCol->text().toStdString());
           if (it == Map.end())
             {
             return;
             }
           else
             {
             QTableWidgetItem* CellTable = new QTableWidgetItem;
             CellTable->setText(it->second.c_str());
             TableToFill->setItem(i,j,CellTable);
             }

           } // ENDFOR

         }
       }
    };

  QString m_NameDB;
  QString m_Server;
  QString m_User;
  QString m_Password;

};

#endif
