/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Revision: 535 $  // Revision of last commit
  Date: $Date: 2009-08-06 11:56:52 -0400 (Thu, 06 Aug 2009) $  // Date of last commit
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
#include <string>
#include "ui_QGoPrintDatabase.h"
#include "MegaVTK2Configure.h"
#include "GoDBRecordSet.h"
#include "GoDBContourRow.h"
#include "QTableWidgetChild.h"
#include "vtkMySQLDatabase.h"
#include "GoDBCollectionOfTraces.h"

class QGoPrintDatabase : public QWidget,
  private Ui::WidgetPrintDatabase
{
  Q_OBJECT

public:

  QMEGAVTKADDON2_EXPORT QGoPrintDatabase();
  virtual QMEGAVTKADDON2_EXPORT ~QGoPrintDatabase();

  /** \brief Create the QTableWidgetChild,get the columns names and the values stored
  in the database and display them in the QTableWidgetChild */
  //void QMEGAVTKADDON2_EXPORT FillTableFromDatabase(QString ServerName,QString login,
    //  QString Password, QString DBName,
    //  int ExpID,QString ExpName);
  void QMEGAVTKADDON2_EXPORT FillTableFromDatabase(std::string iNameDB,
    std::string iServer,std::string iUser,std::string iPassword,
    unsigned int iImgSessionID,std::string iImgSessionName);

  QMEGAVTKADDON2_EXPORT QTableWidgetChild* ContourTable;
  QMEGAVTKADDON2_EXPORT QTableWidgetChild* MeshTable;
  void QMEGAVTKADDON2_EXPORT UpdateTableFromDB();

signals:
  void TableContentChanged();


protected:
  void QPrintColumnNames( QString TableName,
    std::vector< std::string > ColumnNames, QTableWidgetChild* QTabTableName );
  GoDBCollectionOfTraces* CollectionOfContours;
  GoDBCollectionOfTraces* CollectionOfMeshes;

  /** \brief Return the Index of the tab currently used: */
  int InWhichTableAreWe ();

  /**
    \brief get the columns names and the values of the table (type T) from the
    database, then display them in the QTableWidgetchild.
  */
  template< class myT >
  void GetContentAndDisplayFromDB( QString TableName, QTableWidgetChild* Table )
    {
    std::vector< std::string > ColumnNamesContainer;

    typedef GoDBRecordSet< myT >                  SetType;
    typedef typename SetType::InternalObjectType  InternalObjectType;
    typedef typename SetType::RowContainerType    RowContainerType;

    RowContainerType* RowContainer;

    SetType* mySet = new SetType;
    mySet->SetConnector(m_DatabaseConnector);
    mySet->SetTableName(TableName.toStdString());
    mySet->PopulateFromDB();

    myT myNewObject;
    mySet->AddObject( myNewObject );

    //Get the column names from the database:
    ColumnNamesContainer = mySet->GetColumnNamesContainer();
    QPrintColumnNames( TableName, ColumnNamesContainer, Table );
    RowContainer = mySet->GetRowContainer();
    if( RowContainer->size() < 2 ) //because the first row is for the column names
      {
      std::cout<<"Table empty";
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      }
    else
      {
      PrintOutContentFromDB< myT >( RowContainer, Table );
      Table->setSortingEnabled(true);
      }
    delete mySet;
    }

  /**
    \brief get the values of the table (type T) from the
    database, then display them in the QTableWidgetchild.*/
  /**\todo check that the values hasn't been modified first, then update
  only in the database the modified ones*/
  template< class myT >
  void UpdateContentAndDisplayFromDB( QString TableName, QTableWidgetChild* Table )
    {
    typedef GoDBRecordSet< myT >                  SetType;
    typedef typename SetType::InternalObjectType  InternalObjectType;
    typedef typename SetType::RowContainerType    RowContainerType;

    RowContainerType* RowContainer;

    SetType* mySet = new SetType;
    mySet->SetConnector(m_DatabaseConnector);   
    mySet->SetTableName( TableName.toStdString() );
    mySet->PopulateFromDB();

    myT myNewObject;
    mySet->AddObject( myNewObject );
    RowContainer = mySet->GetRowContainer();
    if( RowContainer->size() < 2 )
      {
      std::cout<<"Table empty";
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      }
      PrintOutContentFromDB< myT >( RowContainer, Table );
    delete mySet;
    emit TableContentChanged();
    }

  /** \brief Display the values stored in the RowContainer (list of type T)
  in the QTableWidgetChild TableToFill: */
  template<class myT>
  void PrintOutContentFromDB(
    typename GoDBRecordSet< myT >::RowContainerType *RowContainer,
    QTableWidgetChild* TableToFill )
    {
    TableToFill->setRowCount(RowContainer->size()-1);
    int NbofRows = RowContainer->size()-1; //for test purpose, to delete
    unsigned int i = 0;
    while (i < NbofRows)
    //for( unsigned int i = 0; i < RowContainer->size()-1; ++i )
      {
      //get the column names from the settype:
      std::vector<std::string> VectorColumnNames =(*RowContainer)[i].second.GetVectorColumnNames();
      //std::map<std::string,std::string> Map
        // = (*RowContainer)[i].second.LinkColumnNamesAndValues();
      //compare if the number of columns found in the database is the same as the one defined in the set type:
      if( TableToFill->columnCount() != (int)VectorColumnNames.size())//(int)(Map.size()) )
        {
        std::cout << "Pb, row is not the same size as the number of col";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        return;
        }
      else
        {
        int NbCol = TableToFill->columnCount();//for test purpose, to delete
        for( int j = 0; j< TableToFill->columnCount();j++)
          {
          QTableWidgetItem* HeaderCol = new QTableWidgetItem;
          HeaderCol = TableToFill->horizontalHeaderItem(j);
          QString NameCol = HeaderCol->text();//for test purpose, to delete
          std::string Value = (*RowContainer)[i].second.GetMapValue (HeaderCol->text().toStdString());
          /*std::map<std::string,std::string>::iterator it = Map.find(HeaderCol->text().toStdString());
          if (it == Map.end())
            {
            return;
            }
          else
            {
            QTableWidgetItem* CellTable = new QTableWidgetItem;
            CellTable->setText(it->second.c_str());
            TableToFill->setItem(i,j,CellTable);
            }*/
          if (Value == "noValue")
            {
            return;
            }
          else
            {
            QTableWidgetItem* CellTable = new QTableWidgetItem;
            CellTable->setText(Value.c_str());
            TableToFill->setItem(i,j,CellTable);
            }          
          }//ENDFOR       
         }// ENDELSE 
      i++;
      TableToFill->setRowHeight(i,18);
      }//ENDWHILE
    }


  void closeEvent(QCloseEvent* event);
  
  vtkMySQLDatabase* m_DatabaseConnector;
  unsigned int      m_ImgSessionID;
  std::string       m_ImgSessionName;

protected slots:
  void CreateContextMenu(const QPoint &pos);
  void DeleteTraces();

  /** brief Create a new Collection row in the collection table and change the collection ID of the
  selected contours to the new CollectionID created:*/
  void CreateCorrespondingCollection();

  void AddToExistingCollection();

};

#endif
