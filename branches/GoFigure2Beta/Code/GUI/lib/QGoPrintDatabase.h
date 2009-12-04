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
#include "QTableWidgetNumericalItem.h"
#include "vtkMySQLDatabase.h"
#include "vtkPolyData.h"
#include "GoDBCollectionOfTraces.h"
#include "GoDBTraceInfoForVisu.h"
#include "ContourMeshStructure.h"

/** \brief Ensure the connection with the Database*/
class QGoPrintDatabase : public QWidget,
  private Ui::WidgetPrintDatabase
{
  Q_OBJECT

public:

  QGoPrintDatabase( QWidget* iParent = 0 );
  virtual ~QGoPrintDatabase();
  
  typedef GoDBCollectionOfTraces::DBTableWidgetContainerType DBTableWidgetContainerType;
  /** \brief set all the values needed for the database*/
  void SetDatabaseVariables(
    const std::string& iNameDB, const std::string& iServer,
    const std::string& iUser, const std::string& iPassword,
    const unsigned int& iImgSessionID, const std::string& iImgSessionName );

  /** \brief Create the QTableWidgetChild,get the columns names and the values stored
  in the database, display them in the QTableWidgetChild and fill the info for the
  contours and meshes*/
  void FillTableFromDatabase();
  
  /** \brief Return a vector of all the contours for the given timepoint*/
  std::vector<ContourMeshStructure> GetContoursForAGivenTimepoint (
    unsigned int iTimePoint);
  
  /** \brief Return a vector of all the meshes for the given timepoint*/
  std::vector<ContourMeshStructure> GetMeshesForAGivenTimepoint (
    unsigned int iTimePoint);
  
  /** \brief Return a vector of all the contours with a bounding box 
  containing the given ZCoord*/
  std::vector<ContourMeshStructure> GetContoursForAGivenZCoord (
    unsigned int iZCoord);
  
  /** \brief Return a vector of all the meshes with a bounding box 
  containing the given ZCoord*/
  std::vector<ContourMeshStructure> GetMeshesForAGivenZCoord (
    unsigned int iZCoordPoint);

  /** \brief return a list containing the exisiting colornames with their corresponding rgba
  from the database*/
  std::list<std::pair<std::string,std::vector<int> > > GetColorComboBoxInfofromDB();

  void SaveNewColorInDB(std::vector<std::string> iDataNewColor);
  
  QTableWidgetChild* ContourTable;
  QTableWidgetChild* MeshTable;
  QTableWidgetChild* TrackTable;
  QTableWidgetChild* LineageTable;

  std::vector<ContourMeshStructure> m_ContoursInfo;
  std::vector<ContourMeshStructure> m_MeshesInfo;

  void UpdateTableFromDB();
  void SaveContoursFromVisuInDB(unsigned int iXCoordMin,
    unsigned int iYCoordMin,unsigned int iZCoordMin,unsigned int iTCoord,
    unsigned int iXCoordMax,unsigned int iYCoordMax,unsigned int iZCoordMax,
    vtkPolyData* iContourNodes);
  
  /** \brief return a bool to know if the user is using the database or not*/ 
  bool IsDatabaseUsed();

  QAction* toggleViewAction();

signals:
  void TableContentChanged();
  void SelectionContoursToHighLightChanged();
  void SelectionMeshesToHighLightChanged();
  void FillDatabaseFinished();

protected:
  GoDBCollectionOfTraces* m_CollectionOfContours;
  GoDBCollectionOfTraces* m_CollectionOfMeshes;
  GoDBCollectionOfTraces* m_CollectionOfTracks;
  GoDBCollectionOfTraces* m_CollectionOfLineages;

  vtkMySQLDatabase* m_DatabaseConnector;
  std::string       m_Server;
  std::string       m_User;
  std::string       m_Password;
  std::string       m_DBName;
  unsigned int      m_ImgSessionID;
  std::string       m_ImgSessionName;
  bool              m_IsDatabaseUsed;

  QAction* m_VisibilityAction;

  void QPrintColumnNames( QString TableName,
    std::map< std::string,std::string > ColumnNames, QTableWidgetChild* QTabTableName );

  void OpenDBConnection();
  void CloseDBConnection();

  /** \brief Return the Index of the tab currently used: */
  int InWhichTableAreWe();

  /** \brief initialize the m_ContoursInfo and m_MeshesInfo with the info from the
  database*/
  void LoadContoursAndMeshesFromDB(vtkMySQLDatabase* DatabaseConnector);
  
  std::vector<ContourMeshStructure> GetTracesForAGivenTimepoint(
    std::vector<ContourMeshStructure> iAllTraces, unsigned int iTimePoint);
  /**
    \brief get the columns names and the values of the table (type T) from the
    database, then display them in the QTableWidgetchild.
  */
  /** \brief return a vector of all the traces with a bounding box containing
  the given ZCoord*/
  std::vector<ContourMeshStructure> GetTracesForAGivenZCoord(
    std::vector<ContourMeshStructure> iAllTraces,unsigned int iZCoord, 
    GoDBCollectionOfTraces* iCollectionOfTraces);

  void GetContentAndDisplayFromDB( QString TableName, QTableWidgetChild* Table,
    GoDBCollectionOfTraces* iCollectionOfTraces);

  /**
    \brief get the values of the table (type T) from the
    database, then display them in the QTableWidgetchild.
    \todo check that the values hasn't been modified first, then update
    only in the database the modified ones*/
  template< class myT >
  void UpdateContentAndDisplayFromDB( QString TableName, QTableWidgetChild* Table,
    vtkMySQLDatabase* DatabaseConnector)
    {
    Table->setSortingEnabled(false);
    typedef GoDBRecordSet< myT >                  SetType;
    typedef typename SetType::InternalObjectType  InternalObjectType;
    typedef typename SetType::RowContainerType    RowContainerType;

    RowContainerType* RowContainer;

    SetType* mySet = new SetType;
    mySet->SetConnector(DatabaseConnector);
    mySet->SetTableName( TableName.toStdString() );
    std::stringstream WhereClause;
    WhereClause << "ImagingSessionID = ";
    WhereClause << m_ImgSessionID;
    WhereClause << ";";
    mySet->SetWhereString(WhereClause.str());
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
    Table->setSortingEnabled(true);
    }

  /** \brief Display the values stored in the RowContainer (list of type T)
  in the QTableWidgetChild TableToFill: */
  template<class myT>
  void PrintOutContentFromDB(
    typename GoDBRecordSet< myT >::RowContainerType *RowContainer,
    QTableWidgetChild* TableToFill )
    {
    unsigned int NbofRows = RowContainer->size()-1;
    TableToFill->setRowCount(NbofRows);
    unsigned int i = 0;
    while (i < NbofRows)
      {
      //get the column names from the settype:
      std::vector<std::string> VectorColumnNames =(*RowContainer)[i].second.GetVectorColumnNames();
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
        for( int j = 0; j< TableToFill->columnCount();j++)
          {
          QTableWidgetItem* HeaderCol = new QTableWidgetItem;
          HeaderCol = TableToFill->horizontalHeaderItem(j);
          //don't print the contents for the column points:
          if (HeaderCol->text().toStdString()!= "Points")
          //if (!ColumnName.empty()) //check that it is not the column empty (points):todo:make it cleaner
            {
            std::string ColumnName = HeaderCol->text().toStdString();
            std::string Value = (*RowContainer)[i].second.GetMapValue (HeaderCol->text().toStdString());
            if (Value == "noValue")
              {
              return;
              }
            else
              {
              QTableWidgetNumericalItem* CellTable = new QTableWidgetNumericalItem;
              CellTable->setText(Value.c_str());
              TableToFill->setItem(i,j,CellTable);
              }
            }//ENDIF
          }//ENDFOR
         }// ENDELSE
      i++;
      TableToFill->setRowHeight(i,18);
      }//ENDWHILE
    }


  void closeEvent(QCloseEvent* event);

protected slots:
  void CreateContextMenu(const QPoint &pos);
  void DeleteTraces();

  /** \brief Create a new Collection row in the collection table and change the
  collection ID of the selected contours to the new CollectionID created:*/
  void CreateCorrespondingCollection();

  void AddToExistingCollection();

  /** \brief Update the m_ContoursInfo or m_MeshesInfo depending on which table
  the user had clicked with the selected traces and emit a signal to say which m_tracesInfo has
  changed*/
  void ChangeTracesToHighLightInfoFromTableWidget();

  void ChangeContoursToHighLightInfoFromVisu(
  std::list<int> iListContoursHighLightedInVisu);

};

#endif
