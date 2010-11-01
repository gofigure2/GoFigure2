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
#include "vtkMySQLDatabase.h"
#include "vtkPolyData.h"
#include "GoDBTraceInfoForVisu.h"
#include "ContourMeshStructure.h"
#include "TraceInfoStructure.h"
#include "QGoDBBookmarkManager.h"
#include "QGoGUILibConfigure.h"
#include "GoFigureMeshAttributes.h"
#include "QGoTraceManualEditingDockWidget.h"
#include "QGoTraceManualEditingWidget.h"
#include "QGoDBCellTypeManager.h"
#include "QGoDBSubCellTypeManager.h"
#include "QGoDBColorManager.h"
#include "QGoDBMeshManager.h"
#include "QGoDBContourManager.h"
#include "QGoDBTrackManager.h"
#include "ContourMeshContainer.h"
#include "TrackContainer.h"
/**
\defgroup DB Database
\defgroup GUI GUI
*/

/**
\class QGoPrintDatabase
\brief manages all the database components: table widget, trace manual editing widdet,
QGoDBTraceManager...
\ingroup DB GUI
*/
class QGOGUILIB_EXPORT QGoPrintDatabase:public QWidget,
  private Ui::WidgetPrintDatabase
{
  Q_OBJECT
public:

  /** \brief Constructor */
  explicit QGoPrintDatabase(QWidget *iParent = 0);

  /** \brief Destructor */
  virtual ~QGoPrintDatabase();

  typedef GoDBCollectionOfTraces::TWContainerType            TWContainerType;
  typedef QGoDBBookmarkManager::NamesDescrContainerType      NamesDescrContainerType;
  typedef QGoTraceManualEditingWidget::ItemColorComboboxData ItemColorComboboxData;
  typedef std::pair< int, QColor >                           IDWithColorData;

  /** \brief set all the values needed for the database*/
  void SetDatabaseVariables(
    const std::string & iNameDB, const std::string & iServer,
    const std::string & iUser, const std::string & iPassword,
    const unsigned int & iImgSessionID, const std::string & iImgSessionName);

  /** \brief Create the QTableWidgetChild,get the columns names and the
 * values stored in the database, display them in the QTableWidgetChild
 * and fill the info for the contours and meshes*/
  void FillTableFromDatabase();

  /** \brief Add the contours in the traceinfoForvisu and return all the
  contours data for the given timepoint, included the new ones*/
  //void AddContoursFromDBForAGivenTimePoint(
    //std::vector< int > iListIDs = std::vector< int >() );

  /** \brief return the multi index container for the contours with the
  data from the database corresponding to iTimePoint and to the list
  of given IDs,if no list of IDs is given, will return the info for all
  the contours*/
  // ContourMeshStructureMultiIndexContainer *
  // GetContoursMultiIndexFromDBForAGivenTimePoint(
  //  int iTimePoint, std::vector< int > iListIDs = std::vector< int >() );

  /** \brief return the meshes info for the visu with the data from
  the database corresponding to iTimePoint*/
  // ContourMeshStructureMultiIndexContainer * GetMeshesFromDBForAGivenTimePoint(
  //  int iTimePoint);

  /** \brief return the multi index container for the meshes with the
   data from the database corresponding to iTimePoint and to the list
   of given IDs,if no list of IDs is given, will return the info for all
   the meshes*/
  //ContourMeshStructureMultiIndexContainer *
  //GetMeshesMultiIndexFromDBForAGivenTimePoint( int iTimePoint,
  //                                             std::vector< int > iListIDs = std::vector< int >() );

  /** \brief Add the meshes in the trcaeinfoForvisu and return all the
  meshes data for the given timepoint, included the new ones*/
  //void
 // AddMeshesFromDBForAGivenTimePoint(std::vector< int > iListIDs);

  /** \brief Return a vector of all the contours for the given timepoint*/
  std::vector< ContourMeshStructure > GetContoursForAGivenTimepoint(
    unsigned int iTimePoint);

  /** \brief Return a vector of all the meshes for the given timepoint*/
  std::vector< ContourMeshStructure > GetMeshesForAGivenTimepoint(
    unsigned int iTimePoint);

  /** \brief Return a vector of all the contours with a bounding box
  containing the given ZCoord*/
  //std::list< unsigned int > GetContoursForAGivenZCoord(unsigned int iZCoord);

  /** \brief Return a vector of all the meshes with a bounding box
  containing the given ZCoord*/
  //std::list< unsigned int > GetMeshesForAGivenZCoord(unsigned int iZCoordPoint);

  //useful ???
  std::pair< std::string, QColor > GetSelectedCollectionData();

  /**
  \brief save a new contour from the visu into the database, update
  the table widget and the CurrentElement of the visu container if reeditMode
  is set to false; if not, update the contour in the DB and in the TW, for
  both modes, update the mesh the contour is part of.
  */
  void SaveContoursFromVisuInDB(unsigned int iXCoordMin,
                                unsigned int iYCoordMin, unsigned int iZCoordMin, unsigned int iTCoord,
                                unsigned int iXCoordMax, unsigned int iYCoordMax, unsigned int iZCoordMax,
                                vtkPolyData *iContourNodes);

  /**
  \brief save the mesh into the database for a mesh generated in the
  visualization, if the mesh is an updated mesh which already exists(for
  example a new contour is added to this mesh, the NewMesh has to be set
  to false
  */
  void SaveMeshFromVisuInDB(unsigned int iXCoordMin,
                            unsigned int iYCoordMin, unsigned int iZCoordMin,
                            unsigned int iXCoordMax, unsigned int iYCoordMax, unsigned int iZCoordMax,
                            vtkPolyData *iMeshNodes, GoFigureMeshAttributes *iMeshAttributes);

  /**
  \brief save a new contour in the database, the TW and the container for the contours to sphere
  action
  \param[in] iXCoordMin xcoord of the minimum for the boundingbox
  \param[in] iYCoordMin ycoord of the minimum for the boundingbox
  \param[in] iZCoordMin zcoord of the minimum for the boundingbox
  \param[in] iXCoordMax xcoord of the maximum for the boundingbox
  \param[in] iYCoordMax ycoord of the maximum for the boundingbox
  \param[in] iZCoordMax zcoord of the maximum for the boundingbox
  \param[in] iTraceNodes nodes to be saved as points in the database
  \param[in] iMeshID meshID for the mesh that has just been created
  \return    unsigned int ID of the new created contour
  */
  unsigned int SaveNewContourForMeshToContours(unsigned int iXCoordMin,
                                               unsigned int iYCoordMin,
                                               unsigned int iZCoordMin,
                                               unsigned int iXCoordMax,
                                               unsigned int iYCoordMax,
                                               unsigned int iZCoordMax,
                                               vtkPolyData *iTraceNodes);

  void AddBookmark(int iXCoord, int iYCoord,
                   int iZCoord, int iTCoord);

  /** \brief Return the Name of the tab currently used in the table widget,
 * which correspond to the TraceName of the CollectionOfTraces: */
  std::string InWhichTableAreWe();

  NamesDescrContainerType GetListBookmarks();

  GoDBCoordinateRow GetCoordinateForBookmark(std::string iName);

  /** \brief return a bool to know if the user is using the database or
 * not*/
  bool IsDatabaseUsed();

  QAction * toggleViewAction();

  void ImportContours();

  void ImportMeshes();

  void PrintVolumeAreaForMesh(GoFigureMeshAttributes *
                              iMeshAttributes, unsigned int iMeshID);

  /** \brief return the TraceManualEditingDockWidget*/
  QGoTraceManualEditingDockWidget * GetTraceManualEditingDockWidget();

  /**
  \brief update the tracemanualeditingwidget for the trace with the
  corresponding list of collectionID and set the tablewidget for the
  trace table
  \param[in] iTraceName name of the corresponding trace
  \param[in] iCollectionName name of the corresponding collection
  \param[in] UpdateTableWidget true if the tablewidget has to be
  updated also
  */
  void UpdateWidgetsForCorrespondingTrace(std::string iTraceName,
                                          std::string iCollectionName, bool UpdateTableWidget = true);

  /** \brief Initialize or reinitialized the celltype,subcelltype
  and color list from the database into the tracemanualeditingwidget*/
  void InitializeTheComboboxesNotTraceRelated();

  /**
  \brief set the pointer m_TraceInfoForVisu of the ContoursManager to
  iContoursContainer
  \param[in] iContoursContainer pointer for the container of contours
  for the visu
  */
  void SetContoursContainer(ContourMeshContainer *iContoursContainer);

  /**
  \brief set the pointer m_TraceInfoForVisu of the MeshesManager to
  iMeshesContainer
  \param[in] iMeshesContainer pointer for the container of meshes
  for the visu
  */
  void SetMeshesContainer(ContourMeshContainer *iMeshesContainer);

  /**
  \brief set the pointer m_TrackInfoForVisu of the TracksManager to
  iTracksContainer
  \param[in] iContainer pointer for the container of tracks
  for the visu
  */
  void SetTracksContainer(TrackContainer *iContainer);

public slots:
  void DeleteBookmarks();

  void SetTable(std::string iTablename);

  void ExportContours();

  void ExportMeshes();

  void UpdateSelectedTimePoint(int iTimePoint);

  void SaveNewMeshForMeshToContours(int iNumberOfContours);

  void SaveTrackCurrentElement();

signals:
  void PrintDBReady();

  void DBVariablesSet();

  void TraceToReEdit(unsigned int);

  void OpenBookmarksToUpdate();

  void TableWidgetTabChanged();

  void NewMeshToGenerate(std::list< unsigned int > ListContourIDs, int iNewMeshID);

  /**
  \brief signal emitted when a signal is emitted from a m_TraceManager for
  "go to the trace " from the context menu
  */
  void NeedToGoToTheLocation(int XCoord, int YCoord, int ZCoord, int TCoord);

protected:
  //updated by the TraceManualEditing Widget:
  ItemColorComboboxData m_SelectedColorData;
  ItemColorComboboxData m_SelectedCollectionData;
  std::string           m_SelectedCellType;
  std::string           m_SelectedSubCellType;

  //related to 3dwt:
  int                   m_SelectedTimePoint;
  QGoDBBookmarkManager *m_BookmarkManager;

  //related to TraceManualEditing Widget:
  QGoDBCellTypeManager *           m_CellTypeManager;
  QGoDBSubCellTypeManager *        m_SubCellTypeManager;
  QGoDBColorManager *              m_ColorManager;
  QGoTraceManualEditingDockWidget *m_TraceManualEditingDockWidget;
  QGoTraceManualEditingWidget *    m_TraceWidget;

  QGoDBContourManager *m_ContoursManager;
  QGoDBMeshManager *   m_MeshesManager;
  QGoDBTrackManager *  m_TracksManager;

  //Database variables:
  vtkMySQLDatabase *m_DatabaseConnector;
  std::string       m_Server;
  std::string       m_User;
  std::string       m_Password;
  std::string       m_DBName;
  unsigned int      m_ImgSessionID;
  std::string       m_ImgSessionName;
  bool              m_IsDatabaseUsed;

  bool m_ReeditMode;
  bool m_MeshGenerationMode;

  QAction *m_VisibilityAction;

  void OpenDBConnection();

  void CloseDBConnection();

  /**
  \brief create the m_ContoursManager and its SLOT/SIGNAL connection
  */
  void SetContoursManager();

  /**
 \brief create the m_MeshesManager and its SLOT/SIGNAL connection
 */
  void SetMeshesManager();

  /**
 \brief create the m_TracksManager and its SLOT/SIGNAL connection
 */
  void SetTracksManager();

  //******************Methods related to Trace Manual Editing Widget***********

  /**
  \brief create all the connections between the QGoPrintDatabase and the
  QGoTraceManaualEditingWidget (TM)
  */
  void CreateConnectionsForTraceManualEditingWidget();

  /**
  \brief get the list of celltypes from the database, put them in
  the Trace Manual combobox and if the string is not empty, the combobox will have as
  selected item the string
  \param[in] iCellTypeToSelect name of the celltype to be selected
  */
  void SetTMListCellTypes(std::string iCellTypeToSelect = "");

  /**
  \brief get the list of subcelltypes from the database, put them in
  the Trace Manual combobox and if the string is not empty, the combobox will have as
  selected item the string
  \param[in] iSubCellTypeToSelect name of the subcelltype to be selected
  */
  void SetTMListSubCellTypes(std::string iSubCellTypeToSelect = "");

  /**
  \brief get a list of the IDs with their colors for the collection corresponding to
  the tracename, for the given timepoint if the
  collection is a mesh or for all timepoints for tracks and lineages,
  update the Trace Manual colorcombobox and select the corresponding ID in the combobox
  if the string is not empty
  \param[in] iIDToSelect ID for the item to be selected in the combobox
  */
  void SetTMListCollectionID(std::string iIDToSelect = "");

  /**
  \brief get the data for the colorcombobox from the database,
  put them in the Trace Manual colorcombobox and if the string is not empty,
  the combobox will have as selected item the string
  \param[in] iColorToSelect name of the color to be selected in the combobox
  */
  void SetTMListColors(std::string iColorToSelect = "");

  //******************End of Methods related to Trace Manual Editing
  // Widget***********

  /**
  \brief get the collection id with their corresponding color from the database
  \param[in] iDatabaseConnector connection to the database
  */
  std::list< ItemColorComboboxData > GetListCollectionIDFromDB(
    vtkMySQLDatabase *iDatabaseConnector);

  //std::vector< ContourMeshStructure > GetTracesForAGivenTimepoint(
  //  ContourMeshStructureMultiIndexContainer iAllTraces, unsigned int iTimePoint);

  void closeEvent(QCloseEvent *event);

  /**
  \brief set all the traces manager
  */
  void SetTracesManager();

  /**
  \brief get the data from the database for contours, meshes and tracks, display
  them in the TableWidget and load the different containers for the visu with
  the info from the database
  */
  void GetContentAndDisplayAllTracesInfo(vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief get the RGB Alpha values from the iTraceRow and set a QColor with them
  \tparam T any children of GoDBTraceRow
  \param[in] iTraceRow the trace from which the QColor is created  
  \param[in] iDatabaseConnector connection to the database
  \return QColor with the values corresponding to the color values of the iTraceRow
  */
  template< typename T >
  QColor GetQColorFromTraceRow(T iTraceRow, vtkMySQLDatabase *iDatabaseConnector)
  {
    GoDBColorRow ColorRow;

    ColorRow.SetValuesForSpecificID(atoi( iTraceRow.GetMapValue("ColorID").c_str() ),
                                    iDatabaseConnector);
    QColor Color( atoi( ColorRow.GetMapValue("Red").c_str() ),
                  atoi( ColorRow.GetMapValue("Green").c_str() ),
                  atoi( ColorRow.GetMapValue("Blue").c_str() ),
                  atoi( ColorRow.GetMapValue("Alpha").c_str() ) );
    return Color;
  }

  /**
  \brief set the color of the traceRow according to the iColor
  \param[in,out] ioRow traceRow with the color to be set up
  \param[in] iColor color for the traceRow
  \tparam T children of GoDBTraceRow
  */
  template< typename T >
  void SetTheColorForTheRow(T & ioRow, QColor iColor)
  {
    ioRow.SetField( "Red", iColor.red() );
    ioRow.SetField( "Green", iColor.green() );
    ioRow.SetField( "Blue", iColor.blue() );
    ioRow.SetField( "Alpha", iColor.alpha() );
  }

  /**
  \brief update the color for the checked traces and the rows in the table widget
  for the collectionOf
  \param[in] iTraceManager manager for the trace (expl: mesh)
  \param[in] iCollectionOfManager manager for the collectionOf (expl:contour)
  \tparam TTrace children of QGoDBTraceManager
  \tparam TCollection children of QGoDBTraceManager
  */
  template< typename TTrace, typename TCollection >
  void ChangeTraceColor(TTrace *iTraceManager,
                        TCollection *iCollectionOfManager)
  {
    this->OpenDBConnection();
    //update everything for the traces and get the list of collection ID they
    //are collection of:
    std::list< unsigned int > ListCollectionOfIDsToUpdate =
      iTraceManager->UpdateTheTracesColor(this->m_DatabaseConnector, this->m_SelectedColorData);
    iCollectionOfManager->DisplayInfoForExistingTraces(this->m_DatabaseConnector,
                                                       ListCollectionOfIDsToUpdate);
    this->CloseDBConnection();
  }

  /** todo once lineage container is set up, the bool track needs to be
    removed*/
  /**
  \brief delete the traces from the database,TW,visu container, udpate the collectionof
  collectionID in database and TW and update the bounding box of the collection
  \param[in] iTraceManager the manager for the trace expl: mesh_manager
  \param[in] iCollectionManager the manager for the collection expl: track
  \param[in] iCollectionOfManager the manager for the collectioof expl: contour
  \param[in] track if the trace is track, track is set to true
  \tparam TTrace children of QGoDBTraceManager
  \tparam TCollection children of QGoDBTraceManager
  */
  template< typename TTrace, typename TCollection, typename TCollectionOf >
  void DeleteTraces(TTrace *iTraceManager,
                    TCollection *iCollectionManager, TCollectionOf *iCollectionOfManager,
                    bool track = false)
  {
    this->OpenDBConnection();
    //need to get all the needed data from the traces before deleting them:
    std::list< unsigned int > ListTracesToDelete =
      iTraceManager->GetListHighlightedIDs();
    std::list< unsigned int > ListCollectionsIDs =
      iTraceManager->GetListCollectionIDs(this->m_DatabaseConnector, ListTracesToDelete);
    std::list< unsigned int > ListTracesAsCollectionOf =
      iTraceManager->GetListTracesIDsFromThisCollectionOf(this->m_DatabaseConnector,
                                                          ListTracesToDelete);

    iTraceManager->DeleteTraces(this->m_DatabaseConnector);

    if ( !ListTracesAsCollectionOf.empty() )
      {
      iCollectionOfManager->UpdateCollectionID(this->m_DatabaseConnector,
                                               ListTracesAsCollectionOf, 0);
      }

    if ( !ListCollectionsIDs.empty() || track )
      {
      iCollectionManager->UpdateBoundingBoxes(this->m_DatabaseConnector, ListCollectionsIDs);
      }

    this->CloseDBConnection();
  }

  /**
  \brief change the collectionIDs to iCollectionID for the traces in iListCheckedTraces,
  and update the bounding boxes of the previous collection the traces were part of
  and of iCollectionID in the database, the TW and the container for visu
  \param[in] iTraceManager the manager for the trace expl: mesh_manager
  \param[in] iCollectionManager the manager for the collection expl: track
  \param[in] iCollectionID the ID of the collection the checked traces will be part of
  \param[in] iListCheckedTraces List of the tracesIDs checked in the TW
  \tparam TTrace children of QGoDBTraceManager
  \tparam TCollection children of QGoDBTraceManager
  */
  template< typename TTrace, typename TCollection >
  void AddCheckedTracesToCollection(
    TTrace *iTraceManager,
    TCollection *iCollectionManager, unsigned int iCollectionID,
    std::list< unsigned int > iListCheckedTraces)
  {
    //get the list of CollectionIDs that will be updated:
    std::list< unsigned int > ListCollectionIDsToUpdate =
      iTraceManager->GetListCollectionIDs(this->m_DatabaseConnector,
                                          iListCheckedTraces);

    iTraceManager->UpdateCollectionID(this->m_DatabaseConnector,
                                      iListCheckedTraces, iCollectionID);

    ListCollectionIDsToUpdate.push_back(iCollectionID);
    iCollectionManager->UpdateBoundingBoxes(this->m_DatabaseConnector,
                                            ListCollectionIDsToUpdate);
  }

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
protected slots:
  void CreateContextMenu(const QPoint & iPos);

  void TheTabIsChanged(int iIndex);

  void PassDBConnectionToContoursManager();

  void PassSelectedColorToContoursManager();

  /**
  \brief slot connected to the TraceColorToChange(0 emitted by the
  m_MeshesManager
  */
  void ChangeMeshColor();

  /**
  \brief slot connected to the TraceColorToChange(0 emitted by the
  m_MeshesManager
  */
  void ChangeTrackColor();

  /**
  \brief slot connected to the signal TracesToDelete() emitted by the
  m_ContoursManager
  */
  void DeleteContours();

  /**
  \brief slot connected to the signal Traces DeleteTracesToDelete() emitted by the
  m_MeshesManager
  */
  void DeleteMeshes();

  /**
  \brief slot connected to the signal TracesToDelete() emitted by the
  m_TracksManager
  */
  void DeleteTracks();

  /**
  \brief slot connected to the signal NewCollectionFromCheckedTraces() emitted by
  the m_MeshesManager, create a new track and call the the
  AddCheckedTracesToCollection template method.
  \param[in] iListCheckedMeshes list of the meshIDs of the checked meshes in the TW
  */
  void CreateNewTrackFromCheckedMeshes(std::list< unsigned int > iListCheckedMeshes);

  /**
  \brief slot connected to the signal NewCollectionFromCheckedTraces() emitted by
  the m_ContoursManager, emit the signal NewMeshToGenerate for the visu
  after creating a new mesh and calling the AddCheckedTracesToCollection
  template method.
  \param[in] iListCheckedContours list of the meshIDs of the checked meshes in the TW
  */
  void CreateNewMeshFromCheckedContours(std::list< unsigned int > iListCheckedContours);

  /**
  \brief slot connected to the the signal CheckedTracesToAddToSelectedCollection
  emitted by m_ContoursManager, which call the AddCheckedTracesToCollection template
  \param[in] iListCheckedContours list of the checked contours to be part of the
  selected meshID
  */
  void AddCheckedContoursToSelectedMesh(std::list< unsigned int > iListCheckedContours);

  /**
  \brief slot connected to the the signal CheckedTracesToAddToSelectedCollection
  emitted by m_MeshesManager, which call the AddCheckedTracesToCollection template
  \param[in] iListCheckedMeshes list of the checked meshes to be part of the
  selected trackID
  */
  void AddCheckedMeshesToSelectedTrack(std::list< unsigned int > iListCheckedMeshes);

  /**
  \brief emit a signal TraceToReedit and set m_ReeditMode to true
  \param[in] iTraceID ID of the trace to reedit
  */
  void ReEditTrace(unsigned int iTraceID);

  //*********************Slots for
  // TraceManualEditingWidget:**************************
  /**
  \brief set the m_SelectedColorData to iSelectedColorData
  \param[in] iSelectedColorData name and QColor of the color
  */
  void UpdateSelectedColorData(ItemColorComboboxData iSelectedColorData);

  /**
  \brief set the m_SelectedCollectionData to iSelectedCollectionData
  \param[in] iSelectedCollectionData ID and QColor of the collection
  */
  void UpdateSelectedCollectionID(ItemColorComboboxData iSelectedCollectionData);

  /**
  \brief set the m_SelectedCelltype to iSelectedCelltype
  \param[in] iSelectedCellType name of the celltype
  */
  void UpdateSelectedCellType(std::string iSelectedCellType);

  /**
  \brief set the m_SelectedSubCelltype to iSelectedSubCelltype
  \param[in] iSelectedSubCellType name of the subcelltype
  */
  void UpdateSelectedSubCellType(std::string iSelectedSubCellType);

  /**
  \brief Add the new collection in the database,add a row in TW
  and update the colorcombobox
  */
  void SaveNewCollectionFromTraceWidgetInDBAndTW();

  /**
  \brief call the m_CellTypeManager to add a new celltype (ask the user to enter
  name+description and save it in the database) and call the setListCelltype to
  update the list in the TraceManualEditingWidget
  */
  void AddNewCellType();

  /**
  \brief call the m_SubCellTypeManager to add a new subcelltype (ask the user to enter
  name+description and save it in the database) and call the setListSubCelltype to
  update the list in the TraceManualEditingWidget
  */
  void AddNewSubCellType();

  /**
  \brief call the m_ColorManager to add a new color (ask the user to choose the
  color +enter name+description and save it in the database) and call the
  SetColorComboBoxInfofromDB to update the list in the TraceManualEditingWidget
  */
  void AddNewColor();

  /**
  \brief call the m_CellTypeManager to delete celltypes (display all the
  celltypes for the user to select the ones he wants to delete and delete them
  from the database) and call the setListCelltype to update the list in
  the TraceManualEditingWidget
  */
  void DeleteCellType();

  /**
  \brief call the m_SubCellTypeManager to delete subcelltypes (display all the
  subcelltypes for the user to select the ones he wants to delete and delete them
  from the database) and call the setListSubCelltype to update the list in
  the TraceManualEditingWidget
  */
  void DeleteSubCellType();

  /**
  \brief call the m_ColorManager to delete colors (display all the
  colors for the user to select the ones he wants to delete and delete them
  from the database) and call the SetColorComboBoxInfofromDB to update the
  list in the TraceManualEditingWidget
  */
  void DeleteColor();

  //**********************End TraceManualEditingWidget slots
  // related****************
private:
  Q_DISABLE_COPY(QGoPrintDatabase);
};

#endif
