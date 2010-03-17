/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Revision: 535 $  // Revision of last commit
  Date: $Date: 2009-08-06 11:56:52 -0400 (Thu, 06 Aug 2009) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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
#include <QColor>
#include <string>
#include "ui_QGoPrintDatabase.h"
#include "MegaVTK2Configure.h"
#include "GoDBRecordSet.h"
#include "GoDBContourRow.h"
#include "QTableWidgetChild.h"
#include "vtkMySQLDatabase.h"
#include "vtkPolyData.h"
#include "GoDBCollectionOfTraces.h"
#include "GoDBTraceInfoForVisu.h"
#include "ContourMeshStructure.h"
#include "TraceInfoStructure.h"
#include "QGoDBBookmarkManager.h"
#include "QGoDBCellTypeManager.h"
#include "QGoDBSubCellTypeManager.h"

/** \brief Ensure the connection with the Database*/
class QGoPrintDatabase : public QDockWidget,
  private Ui::WidgetPrintDatabase
{
  Q_OBJECT

public:

  explicit QGoPrintDatabase( QWidget* iParent = 0 );
  virtual ~QGoPrintDatabase();

  typedef GoDBCollectionOfTraces::DBTableWidgetContainerType DBTableWidgetContainerType;
  typedef QGoDBBookmarkManager::NamesDescrContainerType       NamesDescrContainerType;

  /** \brief set all the values needed for the database*/
  void SetDatabaseVariables(
    const std::string& iNameDB, const std::string& iServer,
    const std::string& iUser, const std::string& iPassword,
    const unsigned int& iImgSessionID, const std::string& iImgSessionName );

  /** \brief Create the QTableWidgetChild,get the columns names and the values stored
  in the database, display them in the QTableWidgetChild and fill the info for the
  contours and meshes*/
  void FillTableFromDatabase(int iTimePoint);

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

  /** \brief return a list containing the existing colornames with their corresponding rgba
  from the database*/
  std::list<std::pair<std::string,std::vector<int> > > GetColorComboBoxInfofromDB();
  
  /** \brief return a list of the IDs for the collection corresponding to the tracename
  with their colors for all the timepoints*/
 // std::list<std::pair<std::string,QColor> > GetListExistingCollectionIDFromDB(
  //  std::string TraceName);

  /** \brief return a list of the IDs for the collection corresponding to the tracename
  with their colors for the given timepoint if the collection is a mesh or for 
  all timepoint for tracks and lineages*/
  std::list<std::pair<std::string,QColor> > GetListExistingCollectionIDFromDB(
    std::string TraceName,int iTimePoint);

  std::pair<std::string,QColor> GetCurrentCollectionData();
  
  /** \brief Save the new collection in the database and return the corresponding ID with
  the QColor*/
  std::pair<std::string,QColor> SaveNewCollectionInDB(
    std::pair<std::string,QColor> iColorNewCollection,std::string iTraceName);

  void UpdateCurrentColorData(std::pair<std::string,QColor> iCurrentColorData);
  void SetCurrentCollectionID(std::pair<std::string,QColor> iCurrentCollectionData);

  /** \brief return the info for the visu for the related traces*/
  std::vector<ContourMeshStructure>* GetTracesInfoListForVisu(std::string iTraceName);

  /** \brief save a new contour from the visu into the database,
  update the table widget with the row container and the m_ContoursInfo*/
  int SaveContoursFromVisuInDB( unsigned int iXCoordMin,
    unsigned int iYCoordMin, unsigned int iZCoordMin, unsigned int iTCoord,
    unsigned int iXCoordMax, unsigned int iYCoordMax, unsigned int iZCoordMax,
    vtkPolyData* iContourNodes, std::pair<std::string, QColor> iColorData,
    unsigned int iMeshID );

  /** \brief Update the data for the reedited contour into the database and
  update the table widget*/
  int UpdateContourFromVisuInDB(unsigned int iXCoordMin,
    unsigned int iYCoordMin, unsigned int iZCoordMin, unsigned int iTCoord,
    unsigned int iXCoordMax, unsigned int iYCoordMax, unsigned int iZCoordMax,
    vtkPolyData* iContourNodes, int ContourID );

  void AddBookmark(int iXCoord, int iYCoord, 
    int iZCoord, int iTCoord);
  /** \brief return a bool to know if the user is using the database or not*/

  NamesDescrContainerType GetListBookmarks();
  NamesDescrContainerType GetListCellTypes();
  NamesDescrContainerType GetListSubCellTypes();
  GoDBCoordinateRow GetCoordinateForBookmark(std::string iName);
  bool IsDatabaseUsed();

  QAction* toggleViewAction();

public slots:
  void ChangeContoursToHighLightInfoFromVisu(
  std::list<int> iListContoursHighLightedInVisu, bool Reedit);

  void ChangeMeshesToHighLightInfoFromVisu(
  std::list<int> iListMeshesHighLightedInVisu);

  /** \brief Save a new color in the database with rgba and the name of the color*/
  void SaveNewColorInDB(std::vector<std::string> iDataNewColor);
  void DeleteBookmarks();
  void AddNewCellType();
  void AddNewSubCellType();
  void DeleteCellType();
  void DeleteSubCellType();

signals:
  void PrintDBReady();
  void TableContentChanged();
  void SelectionContoursToHighLightChanged();
  void SelectionMeshesToHighLightChanged();
  void PrintExistingColorsFromDB(
    std::list<std::pair<std::string,std::vector<int> > >);
  void PrintExistingCollectionIDsFromDB(
    std::list<std::pair<std::string,QColor> >);
  void NeedToGetCurrentSelectedColor();
  void NewCreatedCollection(QColor,QString );
  void NeedCurrentSelectedCollectionID();
  void TraceToReEdit( unsigned int );
  void DeletedCollection( unsigned int );
  void TracesToDeleteInVisu( std::list< int > );
  void OpenBookmarksToUpdate();
  void ListCellTypesToUpdate(QStringList);
  void ListSubCellTypesToUpdate(QStringList);

protected:
  std::pair<std::string,QColor> m_CurrentColorData;
  std::pair<std::string,QColor> m_CurrentCollectionData;

  TraceInfoStructure* m_ContoursData;
  TraceInfoStructure* m_MeshesData;
  TraceInfoStructure* m_TracksData;
  TraceInfoStructure* m_LineagesData;

  vtkMySQLDatabase* m_DatabaseConnector;
  std::string       m_Server;
  std::string       m_User;
  std::string       m_Password;
  std::string       m_DBName;
  unsigned int      m_ImgSessionID;
  std::string       m_ImgSessionName;
  bool              m_IsDatabaseUsed;

  QAction* m_VisibilityAction;

  QGoDBBookmarkManager*  m_BookmarkManager;
  QGoDBCellTypeManager*  m_CellTypeManager;
  QGoDBSubCellTypeManager*  m_SubCellTypeManager;

  void OpenDBConnection();
  void CloseDBConnection();

  /** \brief Return the Name of the tab currently used in the table widget,
  which correspond to the TraceName of the CollectionOfTraces: */
  std::string InWhichTableAreWe();
  /** \brief Return the TraceInfoStructure corresponding to the trace name*/
  TraceInfoStructure* GetTraceInfoStructure(std::string iTraceName);

  /** \brief initialize the m_ContoursInfo and m_MeshesInfo with the info from the
  database*/
  void LoadContoursAndMeshesFromDB(vtkMySQLDatabase* DatabaseConnector);

  std::vector<ContourMeshStructure> GetTracesForAGivenTimepoint(
    std::vector<ContourMeshStructure> iAllTraces, unsigned int iTimePoint);
  
  /** \brief return a vector of all the traces with a bounding box containing
  the given ZCoord*/
  std::vector<ContourMeshStructure> GetTracesForAGivenZCoord(
    std::vector<ContourMeshStructure> iAllTraces,unsigned int iZCoord,
    GoDBCollectionOfTraces* iCollectionOfTraces);
  /**\brief get the columns names and the values of the table (type T) from the
    database, then display them in the QTableWidgetchild.*/
  void GetContentAndDisplayFromDB( std::string iTableName);
  void closeEvent(QCloseEvent* event);

  /** \brief Insert a row in the table widget and in the row container with the
  corresponding values of the new created Trace*/
  void UpdateTableWidgetAndRowContainerWithNewCreatedTrace(
    std::string iTraceName);

  /** \brief Update the IDs in the CollectionID column for the selected traces*/
  void UpdateTableWidgetAndRowContainerWithNewCollectionID(
   std::string iTraceName,vtkMySQLDatabase* DatabaseConnector,
   unsigned int iNewCollectionID,QColor iColorNewCollection,
   std::list<int> iListSelectedTraces);
  /** \brief Get the data for the corresponding trace from the database and 
  replace the printed ones in the tablewidget with them*/
  void UpdateTableWidgetForAnExistingTrace(std::string iTraceName, int iTraceID);

  void DeleteTraceInContourMeshStructure(int iTraceID,
    std::vector<ContourMeshStructure>* iTraceInfo);

  /** \brief take care of the traces belonging to the collection that are going
  to be deleted and are collection of these belonging traces*/
  void UpdateInfoForTracesToBeDeletedAsCollectionOf(std::string iTraceName, 
    std::vector<std::string> iTracesAsCollectionToBeDeleted);

  /** \brief delete the corresponding traces, update the related information
  and recalculate the bounding box of the corresponding collections*/
  void DeleteTracesAsPartOfACollection(std::string iTraceName,
  std::list<int> iTracesToDelete);

  /** \brief update the database and the table widget for the selected traces,
  the new collection and the previous collection the traces were part of*/
  void AddListTracesToACollection(std::list<int> iListSelectedTraces,
    std::pair<std::string,QColor> iCollection, std::string iTraceName,
    bool IsANewCollection);

  /** \brief create the contour row with the related data provided by
  the visu*/
  GoDBContourRow GetContourRowFromVisu(
    unsigned int iXCoordMin,unsigned int iYCoordMin,unsigned int iZCoordMin,
    unsigned int iTCoord,unsigned int iXCoordMax,unsigned int iYCoordMax,
    unsigned int iZCoordMax,vtkPolyData* iContourNodes, 
    vtkMySQLDatabase* iDatabaseConnector);


protected slots:
  void CreateContextMenu(const QPoint &pos);
  void DeleteTraces();
  void CheckSelectedRows();
  void UncheckSelectedRows();
  void ChangeTraceColor();

  /** \brief Create a new Collection row in the collection table and change the
  collection ID of the selected contours to the new CollectionID created:*/
  void CreateCorrespondingCollection();

  void AddToSelectedCollection();
  void ReEditTrace();
  /** \brief Update the m_ContoursInfo or m_MeshesInfo depending on which table
  the user had clicked with the selected traces and emit a signal to say which m_tracesInfo has
  changed*/
  void ChangeTracesToHighLightInfoFromTableWidget();
  /** \brief Add the new created trace in the vector of ContourMeshInfo*/
  /** \todo once we know more for the visualization of meshes, need to do the
  same for add a collection*/
  void AddATraceToContourMeshInfo(std::string iTraceName,int iTraceID);

};

#endif
