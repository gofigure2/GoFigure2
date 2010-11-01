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
//#include <QWidget>

/**
\class QGoDBTraceManager
\brief Abstract class
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
  \brief return the distinct traces with their color for the imagingsession,
  for all timepoints if the timepoint is set to the default one or for the
  corresponding timepoint if not
  \param[in] iDatabaseConnector connection to the database
  \return std::list<NameWithColorData> list of the tracesIDs with their
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
  void UpdateTWAndContainerForAllTraces(vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief get the data needed from the database for the
  existing traces,display them in a new inserted row of the m_Table and
  update the container for the visu.
  \param[in] iDatabaseConnector connection to the database
  \param[in] iTraceIDs vector of the IDs for which the objects need to be updated
  */
  void UpdateTWAndContainerForExistingTraces(
    vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iTraceIDs);

  /**
  \overload
  */
  void UpdateTWAndContainerForExistingTraces(
    vtkMySQLDatabase *iDatabaseConnector, int iTraceID);

  void UpdateTWAndContainerForDeletedTraces(
    std::list< unsigned int > iTraceIDs);

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
  \brief return the list of IDs for the current selected traces
  \return std::list<unsigned int> list of selected TraceIDs
  */
  std::list< unsigned int > GetListHighlightedIDs();

  /**
  \brief set the m_TraceContainerInfoForVisu to the iContainerForVisu
  \param[in] iContainerForVisu common container for the visu and database
  */
  void SetTracesInfoContainerForVisu(ContourMeshContainer *iContainerForVisu);

  /**
  \brief virtual pure. update the color of the checked traces in the database,
  the visu container and the TW and return the collectionOf IDs. (i.e traces that
  belongs to these traces as collection: contourIDs belonging to these meshes
  if the trace is a mesh)
  \param[in] iDatabaseConnector connection to the database
  \param[in] iColor new color for the traces
  \return std::list<unsigned int> list of the tracesIDs, part of the collection
  represented by the checked traces
  */
  virtual std::list< unsigned int > UpdateTheTracesColor(
    vtkMySQLDatabase *iDatabaseConnector, NameWithColorData iColor) = 0;

  /**
  \brief delete the selected traces from the database, the TW and the
  container for visu
  \param[in] iDatabaseConnector connection to the database
  */
  void DeleteTraces(vtkMySQLDatabase *iDatabaseConnector);

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
  \return std::list<unsigned int> list of the collectionIDs distinct and
  different from zero found in the database for these tracesIDs
  */
  std::list< unsigned int > GetListCollectionIDs(
    vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTracesIDs);

  /**
  \brief get the collectionOf IDs for the TracesIDs found in iListTraces
  \param[in] iDatabaseConnector connection to the database
  \param[in] iListTraces list of the tracesIDs from which the collectionOf IDs
  are needed
  return std::list<unsigned int> list of the corresponding CollectionOf IDs
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

protected:
  std::string m_TraceName;
  std::string m_TraceNameID;
  std::string m_CollectionName;
  std::string m_CollectionNameID;
  std::string m_CollectionOf;
  std::string m_CollectionOfID;
  //IDWithColorData            m_SelectedColor;
  int                     m_SelectedCollectionID;
  int                     m_ImgSessionID;
  QGoTableWidget *        m_Table;
  GoDBCollectionOfTraces *m_CollectionOfTraces;

  //to be changed:
  //ContourMeshStructureMultiIndexContainer* ListTracesInfoForVisu;
  ContourMeshContainer *m_TraceContainerInfoForVisu;

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
  \return std::string iNameID
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
  \return double* rgba[4]
  */
  double * GetVectorFromQColor(QColor iColor);

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
                                       vtkMySQLDatabase *iDatabaseConnector)
  {
    TWContainerType RowContainer =
      iTWContainer->GetContainerLoadedWithAllFromDB(iDatabaseConnector);

    std::list< std::string > ColumnNames =
      iTWContainer->GetListColumnsNamesForTableWidget();
    this->m_Table->DisplayContent(
      RowContainer,
      iTWContainer->GetIndexForGroupColor(this->m_TraceName),
      iTWContainer->GetIndexForGroupColor(this->m_CollectionName),
      this->m_TraceName, this->m_CollectionName, ColumnNames);
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

  /** \brief create the trace row with the related data provided by
  the visu, iTCoordMax is equal to 0 as for contour and mesh, it is the
  same as TCoord*/
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

  template< typename T >
  void DisplayInfoAndLoadVisuContainerWithAllTraces(T *iTWContainer,
                                                    vtkMySQLDatabase *iDatabaseConnector)
  {
    this->DisplayInfoForAllTraces(iDatabaseConnector);
    std::vector< int >           VectorIDs = iTWContainer->GetAllTraceIDsInContainer();
    std::vector< int >::iterator iter = VectorIDs.begin();

    std::list< ContourMeshStructure > list_of_traces;
    GetTracesInfoFromDBAndModifyContainer(
      list_of_traces,
      iDatabaseConnector, this->m_TraceName, this->m_CollectionName,
      this->m_ImgSessionID, -1, VectorIDs);

    std::list< ContourMeshStructure >::iterator it = list_of_traces.begin();

    while ( it != list_of_traces.end() )
      {
      this->m_TraceContainerInfoForVisu->Insert(*it);
      ++it;
      }
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
    std::list< ContourMeshStructure > ListOfTraces;
    GetTracesInfoFromDBAndModifyContainer(
      ListOfTraces, iDatabaseConnector, this->m_TraceName, this->m_CollectionName,
      this->m_ImgSessionID, -1, iVectorTraceIDs);
    std::list< ContourMeshStructure >::iterator it = ListOfTraces.begin();
    while ( it != ListOfTraces.end() )
      {
      this->m_TraceContainerInfoForVisu->Insert(*it);
      ++it;
      }
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
  \param[in] iNewColor color to be the new one for the highlighted traces
  \return std::list<unsigned int> list of the tracesIDs, part of the collection
  represented by the checked traces
  */
  template< typename T >
  std::list< unsigned int > UpdateTheTracesColorTemplate(
    vtkMySQLDatabase *iDatabaseConnector, NameWithColorData iNewColor)
  {
    std::list< unsigned int > oListOfCollectionOfIDs = std::list< unsigned int >();
    std::list< unsigned int > ListTracesIDs;

    ListTracesIDs = this->m_TraceContainerInfoForVisu->
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

  virtual void AddActionsContextMenu(QMenu *iMenu);

  void AddGeneralActionsContextMenu(QMenu *iMenu);

  void AddSpecificActionsForContourMesh(QMenu *iMenu);

  virtual void AddActionForCreateNewCollectionFromCheckedTraces(QMenu *iMenu);

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

  void UpdateHighlightedElementsInVisuContainer(int iTraceID);

  void UpdateVisibleElementsInVisuContainer(int iTraceID);

  /**
  \brief ColorCode the traces base on a selected column in the table widget
  */
  void ColorCoding();

};
#endif
