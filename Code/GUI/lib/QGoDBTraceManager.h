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

#ifndef __QGoDBTraceManager_h
#define __QGoDBTraceManager_h

#include <QObject>
#include <QMessageBox>
#include "QGoTableWidget.h"
#include "GoDBCollectionOfTraces.h"
#include "GoDBTableWidgetContainer.h"
#include "QGoGUILibConfigure.h"
#include "ContourMeshContainer.h"

/**
\class QGoDBTraceManager
\brief Abstract class inherited by QGoDBContourManager,Mesh,Track,Lineage
\ingroup DB GUI
*/
class QGOGUILIB_EXPORT QGoDBTraceManager:public QObject
{
  Q_OBJECT
public:
  QGoDBTraceManager();
  ~QGoDBTraceManager();

  typedef GoDBTableWidgetContainer::TWContainerType TWContainerType;
  typedef std::pair< std::string, QColor >          NameWithColorData;
  typedef std::pair< unsigned int, QColor >         IDWithColorData;

  /**
  \brief get the m_Table
  \return a QGoTableWidget
  */
  QGoTableWidget * GetTableWidget();


  /**
  \brief set the m_DatabaseConnection to iDatabaseConnector
  */
  void SetDatabaseConnection(vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief return the distinct traces with their color for the imagingsession,
  for all timepoints if the timepoint is set to the default one or for the
  corresponding timepoint if not
  \param[in] iDatabaseConnector connection to the database
  \param[in] iTimePoint timepoint to get the tracesIDs for if different from -1
  \return a list of the tracesIDs with their
  corresponding QColor
  */
  std::list< NameWithColorData > GetAllTraceIDsWithColor(
    vtkMySQLDatabase *iDatabaseConnector, int iTimePoint = -1);

  /**
  \brief get the data needed from the database,
  display them in the m_Table for all traces corresponding to the imagingsession
  and update the container for the visu
  \param[in] iDatabaseConnector connection to the database
  */
  //void UpdateTWAndContainerForAllTraces(vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief get the data needed from the database for the
  existing traces,display them in a new inserted row of the m_Table and
  update the container for the visu.
  \param[in] iDatabaseConnector connection to the database
  \param[in] iTraceIDs vector of the IDs for which the objects need to be updated
  */
  //void UpdateTWAndContainerForExistingTraces(
  //  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iTraceIDs);

  /**
  \overload UpdateTWAndContainerForExistingTraces(
    vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iTraceIDs)
  */
  //void UpdateTWAndContainerForExistingTraces(
  //  vtkMySQLDatabase *iDatabaseConnector, int iTraceID);

  /**
  \brief delete the corresponding traces in the table widget and in the
  container for visu
  \param[in] iTraceIDs list of the IDs for the traces to be deleted
  */
  void UpdateTWAndContainerForDeletedTraces(
    std::list< unsigned int > iTraceIDs);

  /**
  \brief get the data needed from the database for the
  imported traces,display them in new inserted rows of the m_Table and
  update the container for the visu.
  \param[in] iVectorImportedTraces IDs of the imported traces
  \param[in] iDatabaseConnector connection to the database
  */
  virtual void UpdateTWAndContainerForImportedTraces(std::vector< int > iVectorImportedTraces,
                                                     vtkMySQLDatabase *iDatabaseConnector) = 0;

  /**
  \brief get from the database the list of all the IDs for the traces with the
  given ZCoord within the bounding box
  \param[in] iDatabaseConnector connection to the database
  \param[in] iZCoord given Z coordinate
  return std::list<unsigned int> list of the Traces IDs
  */
  //std::list< unsigned int > GetAllTracesIDForAGivenZCoord(
  //  vtkMySQLDatabase *iDatabaseConnector, int iZCoord);

  /**
  \brief
  \return the list of traceIDs that have highlighted set to true in the
  ContainerForVisu.
  */
  virtual std::list< unsigned int > GetListHighlightedIDs()= 0;

  /**
  \brief set the m_TraceContainerInfoForVisu to the iContainerForVisu
  \param[in] iContainerForVisu common container for the visu and database
  \tparam T ContourMeshContainer or TrackContainer
  */
  //template<typename T>
  //virtual void SetTracesInfoContainerForVisu(T *iContainerForVisu) = 0;

  /**
  \brief virtual pure. update the color of the checked traces in the database,
  the visu container and the TW and return the collectionOf IDs. (i.e traces that
  belongs to these traces as collection: contourIDs belonging to these meshes
  if the trace is a mesh)
  \param[in] iDatabaseConnector connection to the database
  \param[in] iColor new color for the traces
  \return a list of the tracesIDs, part of the collection
  represented by the checked traces
  */
  virtual std::list< unsigned int > UpdateTheTracesColor(
    vtkMySQLDatabase *iDatabaseConnector, NameWithColorData iColor) = 0;

  /**
  \brief delete the selected traces from the database, the TW and the
  container for visu
  \param[in] iDatabaseConnector connection to the database
  */
  virtual void DeleteTraces(vtkMySQLDatabase *iDatabaseConnector) = 0;

  /**
  \brief update the collectionID of the tracesIDs in the list with
  the iCollectionID in the database and the TW
  \param[in] iDatabaseConnector connection to the database
  \param[in] iListTracesIDs list of the tracesIDs that the collectionID
  need to be changed
  \param[in] iCollectionID collectionID that needs to be changed for
  the traces
  */
  void UpdateCollectionID(vtkMySQLDatabase *iDatabaseConnector,
                          std::list< unsigned int > iListTracesIDs, int iCollectionID);

  /**
  \brief update in the database the bounding boxes corresponding to the
  TracesIDs and update the corresponding rows in the TW.
  \param[in] iDatabaseConnector connection to the database
  \param[in] iListTracesIDs list of the tracesIDs with a bounding box
  to update
  */
  virtual void UpdateBoundingBoxes(vtkMySQLDatabase *iDatabaseConnector,
                                   std::list< unsigned int > iListTracesIDs);

  /**
  \brief return the list of collection IDs distinct and different from zero
  corresponding to the tracesIDs
  \param[in] iDatabaseConnector connection to the database
  \param[in] iListTracesIDs list of the tracesIDs from which the
  collectionID is needed
  \return a list of the collectionIDs distinct and
  different from zero found in the database for these tracesIDs
  */
  std::list< unsigned int > GetListCollectionIDs(
    vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTracesIDs);

  /**
  \brief get the collectionOf IDs for the TracesIDs found in iListTraces
  \param[in] iDatabaseConnector connection to the database
  \param[in] iListTraces list of the tracesIDs from which the collectionOf IDs
  are needed
  \return a list of the corresponding CollectionOf IDs
  (exp: return a list of ContourIDs belonging to the meshesIDs listed in iListTraces)
  */
  std::list< unsigned int > GetListTracesIDsFromThisCollectionOf(
    vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTraces);

  /**
  \brief get the data from the database corresponding to the iListTraces and display
  them in the Table Widget
  \param[in] iDatabaseConnector connection to the database
  \param[in] iListTraces list of the TraceIDs the rows in the TW need to be updated
  */
  void DisplayInfoForExistingTraces(vtkMySQLDatabase *iDatabaseConnector,
                                    std::list< unsigned int > iListTraces);

  /**
  \brief create the context menu when the user clicks on the table widget
  \param[in] iPos position of the context menu event
  */
  void CreateContextMenu(const QPoint & iPos);

  /**
  \brief get the last saved traces in the database
  \param[in] iDatabaseConnector connection to the database
  \param[in] iNumberOfTracesIDs number of the last tracesIDs required
  \return a list of the IDs
  */
  std::list< unsigned int > GetLastCreatedTracesIDs(
    vtkMySQLDatabase *iDatabaseConnector, int iNumberOfTraceIDs);


signals:
  /**
  \brief signal emitted when the user click on the action "change color" from
  the context menu as it can impact different TraceManagers
  */
  void TraceColorToChange();

  /**
  \brief signal emitted when the user click on the action "DeleteTraces" from
  the context menu as it can impact different TraceManagers
  */
  void TracesToDelete();

  /**
  \brief signal emitted when the user clicks on "go to the trace" in the context
  menu and return the coordinates for the center of the bounding box of the trace
  */
  void NeedToGoToTheLocation(int XCoord, int YCoord, int ZCoord, int TCoord);

  /**
  \brief signal emitted when the user clicks on "create a new collection from
  checked traces" in the context menu and return the list of checked tracesIDs
  */
  void NewCollectionFromCheckedTraces(std::list< unsigned int > );

  void CheckedTracesToAddToSelectedCollection(std::list< unsigned int > );

  void NeedToGetDatabaseConnection();

  void DBConnectionNotNeededAnymore();

protected:
  std::string m_TraceName;
  std::string m_TraceNameID;
  std::string m_CollectionName;
  std::string m_CollectionNameID;
  std::string m_CollectionOf;
  std::string m_CollectionOfID;

  int                     m_SelectedCollectionID;
  int                     m_ImgSessionID;
  QGoTableWidget *        m_Table;
  GoDBCollectionOfTraces *m_CollectionOfTraces;
  vtkMySQLDatabase *      m_DatabaseConnector;

  /**
  \brief Virtual pure method: get the data needed from the database and
  display them in the m_Table for all traces corresponding to the imagingsession
  \param[in] iDatabaseConnector connection to the database
  */
  virtual void DisplayInfoForAllTraces(vtkMySQLDatabase *iDatabaseConnector) = 0;

  /**
  \brief Virtual pure method: get the data needed from the database for the
  last created trace and display them in a new inserted row of the m_Table.
  \param[in] iDatabaseConnector connection to the database
  */
  virtual void DisplayInfoForLastCreatedTrace(vtkMySQLDatabase *iDatabaseConnector) = 0;

  /**
  \brief Virtual pure method: get the data needed from the database for the
  existing trace with iTraceID and update the corresponding row in the m_Table.
  \param[in] iDatabaseConnector connection to the database
  \param[in] iTraceID ID of the trace
  */
  virtual void DisplayInfoForExistingTrace(vtkMySQLDatabase *iDatabaseConnector,
                                           int iTraceID) = 0;

  /**
  \brief transform the iName into iNameID
  \param[in] iName
  \return iNameID
  */
  std::string GetTheNameIDFromName(std::string iName);

  /**
  \brief set the variables class members
  \param[in] iImgSessionID to set the m_ImgSessionID
  \param[in] iParent parent for the m_Table
  */
  void SetInfo(unsigned int iImgSessionID, QWidget *iParent);

  /**
  \brief virtual pure method: set the std::string class members
  */
  virtual void SetCollectionsTraceNames() = 0;

  /**
  \brief get the ID for the last created trace in the database
  \param[in] iDatabaseConnector connection to the database
  \return the ID corresponding to the trace
  */
  int GetLastCreatedTraceID(vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief return a double rgba[4] from a QColor
  \param[in] iColor QColor
  \return a double* rgba[4]
  */
  double * GetVectorFromQColor(QColor iColor);

  /**
  \brief create a GoDBCoordinateRow and set its fields X,Y,Z,Tcoord
  \param[in] iXCoord
  \param[in] iYCoord
  \param[in] iZCoord
  \param[in] iTCoord
  \return a new created GoDBCooordinateRow with coordinates
  */
  GoDBCoordinateRow GetCoordinateFromInt(int iXCoord, int iYCoord, int iZCoord,
                                         int iTCoord);

  /**
  \brief fill the TWContainer with all the data needed from the database
  and display them into the m_Table
  \param[in] iTWContainer contains all the description of its columns to
  get the data from the database and to display them in the m_Table but has
  no value yet
  \param[in] iDatabaseConnector connection to the database
  \tparam T this method takes only children of GoDBTableWidgetContainer as type
  */
  template< typename T >
  void DisplayInfoForAllTracesTemplate(T *iTWContainer,
                                       vtkMySQLDatabase *iDatabaseConnector,
                                       Qt::CheckState iState)
  {
    TWContainerType RowContainer =
    iTWContainer->GetContainerLoadedWithAllFromDB(iDatabaseConnector);

    std::list< std::string > ColumnNames =
      iTWContainer->GetListColumnsNamesForTableWidget();
    this->m_Table->DisplayContent(
      RowContainer,
      iTWContainer->GetIndexForGroupColor(this->m_TraceName),
      iTWContainer->GetIndexForGroupColor(this->m_CollectionName),
      this->m_TraceName, this->m_CollectionName, ColumnNames,iState);
    this->m_Table->setSortingEnabled(true);
  }

  /**
  \brief fill the TWContainer with the data needed from the database for
  the last created trace and insert a new row into the m_Table to display them
  \param[in] iTWContainer contains all the description of its columns to
  get the data from the database and to display them in the m_Table but has
  no value yet
  \param[in] iDatabaseConnector connection to the database
  \tparam T this method takes only children of GoDBTableWidgetContainer as type
  */
  template< typename T >
  void DisplayInfoForLastCreatedTraceTemplate(T *iTWContainer,
                                              vtkMySQLDatabase *iDatabaseConnector)
  {
    int             TraceID = this->GetLastCreatedTraceID(iDatabaseConnector);
    TWContainerType RowContainer =
      iTWContainer->GetContainerForOneSpecificTrace(iDatabaseConnector,
                                                    TraceID);

    this->m_Table->setSortingEnabled(false);
    this->m_Table->InsertNewRow(RowContainer,
                                iTWContainer->GetIndexForGroupColor(this->m_TraceName),
                                iTWContainer->GetIndexForGroupColor(this->m_CollectionName),
                                this->m_TraceName, this->m_CollectionName);
    this->m_Table->setSortingEnabled(true);
  }

  /**
  \brief fill the TWContainer with the data needed from the database for
  the trace with the iTraceID and update the corresponding row into the m_Table
  \param[in] iTWContainer contains all the description of its columns to
  get the data from the database and to display them in the m_Table but has
  no value yet
  \param[in] iDatabaseConnector connection to the database
  \param[in] iTraceID ID for the trace the info will be displayed
  \tparam T this method takes only children of GoDBTableWidgetContainer as type
  */
  template< typename T >
  void DisplayInfoForExistingTraceTemplate(T *iTWContainer,
                                           vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
  {
    TWContainerType RowContainer =
      iTWContainer->GetContainerForOneSpecificTrace(iDatabaseConnector,
                                                    iTraceID);

    this->m_Table->UpdateRow(RowContainer,
                             iTWContainer->GetIndexForGroupColor(this->m_TraceName),
                             iTWContainer->GetIndexForGroupColor(this->m_CollectionName),
                             this->m_TraceName, this->m_CollectionName, iTraceID);
  }

  /**
  \brief create the trace row with the related data provided by
  the visu, iTCoordMax is equal to 0 as for contour and mesh, it is the
  same as TCoord
  \param[in] iXCoordMin X coord min of the bounding box
  \param[in] iYCoordMin Y coord min of the bounding box
  \param[in] iZCoordMin Z coord min of the bounding box
  \param[in] iTCoord    T coord min of the bounding box and max if iTCoordMax = 0
  \param[in] iXCoordMax X coord max of the bounding box
  \param[in] iYCoordMax Y coord max of the bounding box
  \param[in] iZCoordMax Z coord max of the bounding box
  \param[in] iTraceNodes polydata from which the points will be extracted
  \param[in] iColor color of the trace
  \param[in] iDatabaseConnector connection to the database
  \param[in,out] iTrace GoDBTraceRow with the fields to be set
  \param[in] iCollectionID collection ID of the trace
  \param[in] iTCoordMax T coord max of the bounding box
  \tparam
  */
  template< typename T >
  unsigned int CreateNewTraceInDBFromVisu(
    unsigned int iXCoordMin, unsigned int iYCoordMin, unsigned int iZCoordMin,
    unsigned int iTCoord, unsigned int iXCoordMax, unsigned int iYCoordMax,
    unsigned int iZCoordMax, vtkPolyData *iTraceNodes, NameWithColorData iColor,
    vtkMySQLDatabase *iDatabaseConnector, T & iTrace, unsigned int iCollectionID,
    unsigned int iTCoordMax = 0)
  {
    this->SetTraceBoundingBoxAndPoints< T >(iXCoordMin, iYCoordMin, iZCoordMin, iTCoord,
                                            iXCoordMax, iYCoordMax, iZCoordMax, iTraceNodes, iDatabaseConnector, iTrace,
                                            iTCoordMax);

    return this->m_CollectionOfTraces->CreateNewTraceInDB< T >(iTrace,
                                                               iDatabaseConnector, iColor, iCollectionID);
  }

  template< typename T >
  void SetTraceBoundingBoxAndPoints(unsigned int iXCoordMin, unsigned int iYCoordMin,
                                    unsigned int iZCoordMin, unsigned int iTCoord, unsigned int iXCoordMax,
                                    unsigned int iYCoordMax, unsigned int iZCoordMax, vtkPolyData *iTraceNodes,
                                    vtkMySQLDatabase *iDatabaseConnector, T & iTrace, unsigned int iTCoordMax = 0)
  {
    GoDBCoordinateRow coord_min = this->GetCoordinateFromInt(iXCoordMin,
                                                             iYCoordMin, iZCoordMin, iTCoord);
    unsigned int TCoord;

    if ( iTCoordMax == 0 )
      {
      TCoord = iTCoord;
      }
    else
      {
      TCoord = iTCoordMax;
      }
    GoDBCoordinateRow coord_max = this->GetCoordinateFromInt(iXCoordMax,
                                                             iYCoordMax, iZCoordMax, TCoord);
    iTrace.SetTheDataFromTheVisu(iDatabaseConnector, iTraceNodes,
                                 coord_min, coord_max);
  }

  /**
  \brief get all the data from the database to load all the traces for the imagingsession
  into the table widget and the container for the visu
  \param[in] iTWContainer contains all the description of its columns to
  get the data from the database and to display them in the m_Table but has
  no value yet
  \param[in] iDatabaseConnector connection to the database
  \tparam T  only children of GoDBTableWidgetContainer as type T
  */
  template< typename T >
  void DisplayInfoAndLoadVisuContainerWithAllTraces(T *iTWContainer,
                                                    vtkMySQLDatabase *iDatabaseConnector)
  {
    this->DisplayInfoForAllTraces(iDatabaseConnector);
    std::vector< int >           VectorIDs = iTWContainer->GetAllTraceIDsInContainer();
    std::vector< int >::iterator iter = VectorIDs.begin();

    this->GetTracesInfoFromDBAndModifyContainerForVisu(iDatabaseConnector,VectorIDs);
  }

  /**
  \brief update for the imported traces the table widget and the database info
  of the container for visu.
  \param[in] iTWContainer contains all the description of its columns to
  get the data from the database and to display them in the m_Table but has
  no value yet
  \param[in] iVectorTraceIDs vector of the imported TracesIDs
  \param[in] iDatabaseConnector connection to the database
  \tparam T this method takes only children of GoDBTableWidgetContainer
  */
  template< typename T >
  void UpdateTWAndContainerWithImportedTracesTemplate(T *iTWContainer,
                                                      std::vector< int > iVectorTraceIDs,
                                                      vtkMySQLDatabase *iDatabaseConnector)
  {
    //insert the info from the database for the traces into the container
    //for visu:
    this->GetTracesInfoFromDBAndModifyContainerForVisu(
      iDatabaseConnector,iVectorTraceIDs);
    //insert the new rows into the TW:
    std::vector< int >::iterator iter = iVectorTraceIDs.begin();
    this->m_Table->setSortingEnabled(false);
    while ( iter != iVectorTraceIDs.end() )
      {
      TWContainerType RowContainer =
        iTWContainer->GetContainerForOneSpecificTrace(iDatabaseConnector,
                                                      *iter);
      this->m_Table->InsertNewRow(RowContainer,
                                  iTWContainer->GetIndexForGroupColor(this->m_TraceName),
                                  iTWContainer->GetIndexForGroupColor(this->m_CollectionName),
                                  this->m_TraceName, this->m_CollectionName);
      iter++;
      }
    this->m_Table->setSortingEnabled(true);
  }

  /**
  \brief update the visu container, the database and the TW with the new color
  for the highlighted traces
  \param[in] iDatabaseConnector connection to the database
  \param[in] iContainerInfoForVisu info needed for the visu
  \param[in] iNewColor color to be the new one for the highlighted traces
  \return a list of the tracesIDs, part of the collection
  represented by the checked traces
  \tparam T children of GoDBTraceRow
  \tparam C  ContourMeshContainer or TrackContainer
  */
  template< typename T,typename C >
  std::list< unsigned int > UpdateTheTracesColorTemplate(
    vtkMySQLDatabase *iDatabaseConnector, C *iContainerForVisu,
    NameWithColorData iNewColor)
  {
    std::list< unsigned int > oListOfCollectionOfIDs = std::list< unsigned int >();
    std::list< unsigned int > ListTracesIDs;
    ListTracesIDs = iContainerForVisu->
                    UpdateAllHighlightedElementsWithGivenColor(iNewColor.second);
    if ( ListTracesIDs.empty() )
      {
      QMessageBox msgBox;
      msgBox.setText(
        tr("Please select at least one %1 for the color to be changed")
        .arg( this->m_TraceName.c_str() ) );
      msgBox.exec();
      }
    else
      {
      std::list< unsigned int >::iterator iter = ListTracesIDs.begin();
      while ( iter != ListTracesIDs.end() )
        {
        this->m_CollectionOfTraces->ChangeColorForTrace< T >(*iter, iNewColor,
                                                             iDatabaseConnector);
        this->DisplayInfoForExistingTrace(iDatabaseConnector, *iter);
        ++iter;
        }
      oListOfCollectionOfIDs = this->m_CollectionOfTraces->
                               GetListTracesIDsFromThisCollectionOf(iDatabaseConnector, ListTracesIDs);
      }
    return oListOfCollectionOfIDs;
  }

  /**
  \brief set the iMemberContainerInfoForVisu to the iContainerForVisu and
  create the connections SLOT/SIGNAL
  \param[in] iContainerForVisu common container for the visu and database
  \param[in] iMemberContainerForVisu m_TraceContainerInfoForVisu
  \tparam T ContourMeshContainer or TrackContainer
  */
  template< typename T >
  void SetTracesInfoContainerForVisuTemplate(T *iContainerForVisu,
    T **iMemberContainerForVisu)
  {
    *iMemberContainerForVisu = iContainerForVisu;
    QObject::connect( *iMemberContainerForVisu,
                      SIGNAL( TracePicked(uint, Qt::CheckState) ),
                      this,
                      SLOT( CheckTheTraceInTW(uint, Qt::CheckState) ) );

    QObject::connect( *iMemberContainerForVisu,
                      SIGNAL( TraceVisibilityChanged(uint, Qt::CheckState) ),
                      this,
                      SLOT ( ShowTheTraceInTW(uint, Qt::CheckState) ) );

    QObject::connect( this->m_Table,
                      SIGNAL( ModifyHighlightListTraces(QStringList,Qt::CheckState) ),
                      *iMemberContainerForVisu,
                      SLOT ( UpdateElementHighlightingWithGivenTraceIDs(QStringList,
                                                   Qt::CheckState) ) );
    QObject::connect( this->m_Table,
                      SIGNAL( ModifyVisibilityListTraces(QStringList,Qt::CheckState) ),
                      *iMemberContainerForVisu,
                      SLOT ( UpdateElementVisibilityWithGivenTraceIDs(QStringList,
                                                   Qt::CheckState) ) );
  }

  /**
  \brief delete the selected traces from the database, the TW and the
  container for visu
  \param[in] iDatabaseConnector connection to the database
  \param[in] iContainerForVisu common container for the visu and database
  \tparam T ContourMeshContainer or TrackContainer
  */
  template<typename T>
  void DeleteTracesTemplate(vtkMySQLDatabase *iDatabaseConnector,
    T *iContainerForVisu)
  {
    std::list< unsigned int > ListTracesIDs =
      iContainerForVisu->GetHighlightedElementsTraceID();
    this->m_CollectionOfTraces->DeleteTracesInDB(
      ListTracesIDs, iDatabaseConnector);
    iContainerForVisu->DeleteAllHighlightedElements();
    this->m_Table->DeleteCheckedRows(this->m_TraceNameID, ListTracesIDs);
  }

  /**
  \brief get a map with the tracesIDs as keys and the values of the
  selected columns as values for all traces in the table widget and
  update the color of the traces in the visu
  \param[in] iContainerForVisu common container for the visu and database
  \tparam ContourMeshContainer or TrackContainer
  */
  template<typename T>
  void SetColorCodingTemplate( T* iContainerForVisu,bool IsChecked)
  {
    std::string ColumnName = "";
    std::map<unsigned int, double> Values = std::map<unsigned int, double>();
	if (IsChecked)
	  {
    Values = this->m_Table->GetTraceIDAndColumnsValues(
		  this->m_TraceNameID,ColumnName);
	  }
    iContainerForVisu->SetColorCode<double>( ColumnName,Values );
  }

  /**
  \brief return to the color saved in the database for the traces
  \param[in] iContainerForVisu common container for the visu and database
  \tparam ContourMeshContainer or TrackContainer
  */
  /*template<typename T>
  void SetBackFromColorCodingTemplate( T* iContainerForVisu)
  {
    std::string ColumnName = "";
	std::map<unsigned int, double> Values = std::map<unsigned int, double>();
    iContainerForVisu->SetColorCode( ColumnName,Values );
  }*/

  virtual void AddActionsContextMenu(QMenu *iMenu);

  void AddGeneralActionsContextMenu(QMenu *iMenu);

  void AddSpecificActionsForContourMesh(QMenu *iMenu);

  virtual void AddActionForCreateNewCollectionFromCheckedTraces(QMenu *iMenu);

  /**
  \brief get the info needed from the database to update the container
  for visu
  */
  virtual void GetTracesInfoFromDBAndModifyContainerForVisu(
    vtkMySQLDatabase* iDatabaseConnector,std::vector<int> iVectIDs = std::vector< int >())= 0;

protected slots:
  //context menu:
  void CheckSelectedRows();

  void UncheckSelectedRows();

  void ShowSelectedRows();

  void HideSelectedRows();

  /**
  \brief is called when the user choses the change color action in the
  context menu, emit the signal TraceColorToChange as it impacts the
  collectionOf also
  */
  virtual void ChangeTraceColor();

  void DeleteTracesFromContextMenu();

  /**
  \brief slot called when a trace has been picked from the visu, to
  update the TW "checked/unchecked" column correspondingly
  \param[in] iTraceID TraceID that has been picked
  \param[in] iState state to which the TW needs to be updated
  */
  void CheckTheTraceInTW(unsigned int iTraceID, Qt::CheckState iState);

  /**
  \brief slot called when a trace has been shown/hidden from the visu, to
  update the TW "show/hide" column correspondingly
  \param[in] iTraceID TraceID that has been shown/hidden
  \param[in] iState state to which the TW needs to be updated
  */
  void ShowTheTraceInTW(unsigned int iTraceID, Qt::CheckState iState);

  void GoToTheTrace();

  /**
  \brief slot called when the user click on "create a collection from
  checked traces" in the context menu, emit the signal NewCollectionFromCheckedTraces
  as it impact the collection also
  */
  void CreateCorrespondingCollection();

  void AddToSelectedCollection();

  /**
  \brief modify to the opposite one the highlighted property of the corresponding
  trace base on traceID in the container for visu
  \param[in] iTraceID ID of the trace for the property to be modified
  */
  virtual void UpdateHighlightedElementsInVisuContainer(int iTraceID) = 0;

  /**
  \brief modify to the opposite one the Visible property of the corresponding
  trace base on traceID in the container for visu
  \param[in] iTraceID ID of the trace for the property to be modified
  */
  virtual void UpdateVisibleElementsInVisuContainer(int iTraceID) = 0;


  /**
  \brief ColorCode the traces in the visualization base on a selected column
  in the table widget
  */
  virtual void SetColorCoding(bool IsChecked)= 0;

  /**
  \brief return to the color saved in the database for the traces in the
  visualization
  */
  //virtual void BackFromColorCoding() = 0;

};
#endif
