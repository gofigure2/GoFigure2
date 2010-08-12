/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
  Date: $Date$  // Date of last commit
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
#include "ContourMeshStructureHelper.h"
#include "QGoGUILibConfigure.h"
#include "GoFigureMeshAttributes.h"
#include "QGoTraceManualEditingDockWidget.h"
#include "QGoTraceManualEditingWidget.h"
#include "QGoDBCellTypeManager.h"
#include "QGoDBSubCellTypeManager.h"
#include "QGoDBColorManager.h"

/** \class QGoPrintDatabase
 * \brief Ensure the connection with the Database
 * */
class QGOGUILIB_EXPORT QGoPrintDatabase : public QWidget,
  private Ui::WidgetPrintDatabase
  {
  Q_OBJECT

public:

  /** \brief Constructor */
  explicit QGoPrintDatabase(QWidget* iParent = 0);

  /** \brief Destructor */
  virtual ~QGoPrintDatabase();

  typedef GoDBCollectionOfTraces::DBTableWidgetContainerType DBTableWidgetContainerType;
  typedef QGoDBBookmarkManager::NamesDescrContainerType      NamesDescrContainerType;
  typedef QGoTraceManualEditingWidget::ItemColorComboboxData ItemColorComboboxData;
  typedef std::pair<int,QColor>                              IDWithColorData;

  /** \brief set all the values needed for the database*/
  void SetDatabaseVariables(
    const std::string& iNameDB, const std::string& iServer,
    const std::string& iUser, const std::string& iPassword,
    const unsigned int& iImgSessionID, const std::string& iImgSessionName);

  /** \brief Create the QTableWidgetChild,get the columns names and the
 * values stored in the database, display them in the QTableWidgetChild
 * and fill the info for the contours and meshes*/
  void FillTableFromDatabase(int iTimePoint);

  /** \brief Add the contours in the trcaeinfoForvisu and return all the
  contours data for the given timepoint, included the new ones*/
  ContourMeshStructureMultiIndexContainer* AddContoursFromDBForAGivenTimePoint(
    int iTimePoint, std::vector<int> iListIDs = std::vector<int>());

  /** \brief return the multi index container for the contours with the
  data from the database corresponding to iTimePoint and to the list
  of given IDs,if no list of IDs is given, will return the info for all
  the contours*/
  ContourMeshStructureMultiIndexContainer*
  GetContoursMultiIndexFromDBForAGivenTimePoint(
    int iTimePoint, std::vector<int> iListIDs = std::vector<int>());

  /** \brief return the meshes info for the visu with the data from
  the database corresponding to iTimePoint*/
  ContourMeshStructureMultiIndexContainer* GetMeshesFromDBForAGivenTimePoint(
    int iTimePoint);

  /** \brief return the multi index container for the meshes with the
   data from the database corresponding to iTimePoint and to the list
   of given IDs,if no list of IDs is given, will return the info for all
   the meshes*/
  ContourMeshStructureMultiIndexContainer*
  GetMeshesMultiIndexFromDBForAGivenTimePoint(int iTimePoint,
                                              std::vector<int> iListIDs = std::vector<int>());

  /** \brief Add the meshes in the trcaeinfoForvisu and return all the
  meshes data for the given timepoint, included the new ones*/
  ContourMeshStructureMultiIndexContainer*
  AddMeshesFromDBForAGivenTimePoint(int iTimePoint, std::vector<int> iListIDs);

  /** \brief Return a vector of all the contours for the given timepoint*/
  std::vector<ContourMeshStructure> GetContoursForAGivenTimepoint(
    unsigned int iTimePoint);

  /** \brief Return a vector of all the meshes for the given timepoint*/
  std::vector<ContourMeshStructure> GetMeshesForAGivenTimepoint(
    unsigned int iTimePoint);

  /** \brief Return a vector of all the contours with a bounding box
  containing the given ZCoord*/
  std::vector<ContourMeshStructure> GetContoursForAGivenZCoord(
    unsigned int iZCoord);

  /** \brief Return a multiIndex of all the contours with a bounding box
  containing the given ZCoord*/
  /*ContourMeshStructureMultiIndexContainer*
    GetContoursMultiIndexFromDBForAGivenZCoord(unsigned int iZCoord,
    std::vector<int> iListIDs= std::vector<int>());*/

  /** \brief Return a vector of all the meshes with a bounding box
  containing the given ZCoord*/
  std::vector<ContourMeshStructure> GetMeshesForAGivenZCoord(
    unsigned int iZCoordPoint);

  /** \brief return a list of the IDs for the collection corresponding to
 * the tracename with their colors for all the timepoints*/
  // std::list<std::pair<std::string,QColor> > GetListExistingCollectionIDFromDB(
  //  std::string TraceName);

  //useful ???
  std::pair<std::string, QColor> GetSelectedCollectionData();

  /** \brief return the info for the visu for the related traces*/
  ContourMeshStructureMultiIndexContainer* GetTracesInfoListForVisu(std::string iTraceName);

  /** \brief save a new contour from the visu into the database, update
 * the table widget with the row container and the m_ContoursInfo*/
  IDWithColorData SaveContoursFromVisuInDB(unsigned int iXCoordMin,
                               unsigned int iYCoordMin, unsigned int iZCoordMin, unsigned int iTCoord,
                               unsigned int iXCoordMax, unsigned int iYCoordMax, unsigned int iZCoordMax,
                               vtkPolyData* iContourNodes);//, std::pair<std::string, QColor> iColorData,
                               //unsigned int iMeshID = 0);

  /** 
  \brief Update the data for the reedited contour into the database and
  update the table widget
  */
  IDWithColorData UpdateContourFromVisuInDB(unsigned int iXCoordMin,
                                unsigned int iYCoordMin, unsigned int iZCoordMin, unsigned int iTCoord,
                                unsigned int iXCoordMax, unsigned int iYCoordMax, unsigned int iZCoordMax,
                                vtkPolyData* iContourNodes, int ContourID);

  /**
  \brief save the mesh into the database for a mesh generated in the
  visualization, if the mesh is an updated mesh which already exits(for
  example a new contour is added to this mesh, the NewMesh has to be set
  to false
  \return IDWithColorData the ID and the color of the new created mesh
  */
  IDWithColorData SaveMeshFromVisuInDB(unsigned int iXCoordMin,
                           unsigned int iYCoordMin, unsigned int iZCoordMin, unsigned int iTCoord,
                           unsigned int iXCoordMax, unsigned int iYCoordMax, unsigned int iZCoordMax,
                           vtkPolyData* iMeshNodes, GoFigureMeshAttributes* iMeshAttributes,
                           bool NewMesh = true, int iMeshID = 0);

  int CreateMeshFromOneClickSegmentation(std::list<int> iListContoursIDs);

  void AddTracesInTableWidgetFromDB(std::vector<int> ListTracesIDs,
                                    std::string iTraceName);

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

  QAction* toggleViewAction();

  ContourMeshStructureMultiIndexContainer* ImportContours(int iTimePoint);
  ContourMeshStructureMultiIndexContainer* ImportMeshes(int iTimePoint);

  void PrintVolumeAreaForMesh(double iVolume, double iArea,
                              unsigned int iMeshID);
  /** \brief return the TraceManualEditingDockWidget*/
  QGoTraceManualEditingDockWidget* GetTraceManualEditingDockWidget();

  /** \brief update the tracemanualeditingwidget for the trace with the 
  corresponding list of collectionID and set the tablewidget for the
  trace table*/
  void UpdateWidgetsForCorrespondingTrace(std::string iTraceName,
    std::string iCollectionName,bool UpdateTableWidget = true);
  
  /** \brief Initialize or reinitialized the celltype,subcelltype
  and color list from the database into the tracemanualeditingwidget*/
  void InitializeTheComboboxesNotTraceRelated();

public slots:
  void ChangeContoursToHighLightInfoFromVisu(
    std::list<int> iListContoursHighLightedInVisu, bool Reedit);

  void ChangeMeshesToHighLightInfoFromVisu(
    std::list<int> iListMeshesHighLightedInVisu);

  void DeleteBookmarks();
 
  void SetTable(std::string iTablename);
  void ExportContours();
  void ExportMeshes();

  void UpdateSelectedTimePoint(int iTimePoint);

signals:
  void PrintDBReady();
  void TableContentChanged();
  void SelectionContoursToHighLightChanged();
  void SelectionMeshesToHighLightChanged();
  void TraceToReEdit(unsigned int);
  void TracesToDeleteInVisu(std::list<int> );
  void OpenBookmarksToUpdate();
  void ColorChangedForSelectedTraces(std::pair<std::list<int>, QColor>);
  void TableWidgetTabChanged();
  void NewMeshToGenerate(std::list<int> ListContourIDs,int iNewMeshID);
  void MeshGenerationToUpdate(std::list<int> ListContourIDs);
  void NeedToGoToTheLocation(int XCoord, int YCoord, int ZCoord, int TCoord);
  void ShowCheckedTracesActivated();
  void SelectionContoursToShowChanged();
  void SelectionMeshesToShowChanged();

protected:
  //updated by the TraceManualEditing Widget:
  ItemColorComboboxData            m_SelectedColorData;
  ItemColorComboboxData            m_SelectedCollectionData;
  std::string                      m_SelectedCellType;
  std::string                      m_SelectedSubCellType;

  QGoDBBookmarkManager*            m_BookmarkManager;
  QGoDBCellTypeManager*            m_CellTypeManager;
  QGoDBSubCellTypeManager*         m_SubCellTypeManager;
  QGoDBColorManager*               m_ColorManager;

  int                              m_SelectedTimePoint;

  TraceInfoStructure*              m_ContoursData;
  TraceInfoStructure*              m_MeshesData;
  TraceInfoStructure*              m_TracksData;
  TraceInfoStructure*              m_LineagesData;
  QGoTraceManualEditingDockWidget* m_TraceManualEditingDockWidget;
  QGoTraceManualEditingWidget*     m_TraceWidget;


  vtkMySQLDatabase*                m_DatabaseConnector;
  std::string                      m_Server;
  std::string                      m_User;
  std::string                      m_Password;
  std::string                      m_DBName;
  unsigned int                     m_ImgSessionID;
  std::string                      m_ImgSessionName;
  bool                             m_IsDatabaseUsed;

  QAction* m_VisibilityAction;


  void OpenDBConnection();
  void CloseDBConnection();

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

  //******************End of Methods related to Trace Manual Editing Widget***********

  /** \brief get the info from the database for the collectionid, set the list in the
  combobox and select the last activated item (i.e after delete)*/
 // void UpdateListCollectionIDFromDB();

  /** \brief get the collection id with their corresponding color from the database*/
  std::list<ItemColorComboboxData> GetListCollectionIDFromDB();

  
  /** \brief Return the TraceInfoStructure corresponding to the trace name*/
  TraceInfoStructure* GetTraceInfoStructure(std::string iTraceName);

  /** \brief initialize the m_ContoursInfo and m_MeshesInfo with the info
 * from the database*/
  void LoadContoursAndMeshesFromDB(vtkMySQLDatabase* DatabaseConnector);

  std::vector<ContourMeshStructure> GetTracesForAGivenTimepoint(
    ContourMeshStructureMultiIndexContainer iAllTraces, unsigned int iTimePoint);

  /** \brief return a vector of all the traces with a bounding box
 * containing the given ZCoord*/
  std::vector<ContourMeshStructure> GetTracesForAGivenZCoord(
    ContourMeshStructureMultiIndexContainer iAllTraces, unsigned int iZCoord,
    GoDBCollectionOfTraces* iCollectionOfTraces);
  /**\brief get the columns names and the values of the table (type T)
 * from the database, then display them in the QTableWidgetchild.*/
  void GetContentAndDisplayFromDB(std::string iTableName);
  void closeEvent(QCloseEvent* event);

  /** \brief Insert a row in the table widget
 * with the corresponding values of the new created Trace and update the
 database with collectionID if needed*/
  void UpdateTableWidgetAndDBWithNewCreatedTrace(
    std::string iTraceName, GoFigureMeshAttributes* iMeshAttributes = 0);

  /** \brief Update the IDs in the CollectionID column for the selected traces*/
  /*void UpdateTableWidgetAndRowContainerWithNewCollectionID(
   std::string iTraceName,unsigned int iNewCollectionID,
   QColor iColorNewCollection,std::list<int> iListSelectedTraces);*/

  /** \brief Get the data for the corresponding trace from the database and
  replace the printed ones in the tablewidget with them*/
  void UpdateTableWidgetForAnExistingTrace(std::string iTraceName, int iTraceID,
    GoFigureMeshAttributes* iMeshAttributes = 0);

  void DeleteTraceInContourMeshStructure(int iTraceID,
                                         ContourMeshStructureMultiIndexContainer* iTraceInfo);

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
                                  std::pair<std::string, QColor> iCollection, std::string iTraceName,
                                  bool IsANewCollection, int iCollectionID = -1);

  /** \brief set all the tracesinfostructures */
  void SetTraceInfoStructures();

  /** \brief create the trace row with the related data provided by
  the visu, iTCoordMax is equal to 0 as for contour and mesh, it is the
  same as TCoord*/
  template<typename T>
  void GetTraceRowFromVisu(
    unsigned int iXCoordMin, unsigned int iYCoordMin, unsigned int iZCoordMin,
    unsigned int iTCoord, unsigned int iXCoordMax, unsigned int iYCoordMax,
    unsigned int iZCoordMax, vtkPolyData* iTraceNodes,
    vtkMySQLDatabase* iDatabaseConnector, T& iTrace, unsigned int iTCoordMax = 0,
    GoFigureMeshAttributes* iMeshAttributes = 0)
  {
    GoDBCoordinateRow coord_min;
    coord_min.SetField<unsigned int>("XCoord", iXCoordMin);
    coord_min.SetField<unsigned int>("YCoord", iYCoordMin);
    coord_min.SetField<unsigned int>("ZCoord", iZCoordMin);
    coord_min.SetField<unsigned int>("TCoord", iTCoord);

    GoDBCoordinateRow coord_max;
    coord_max.SetField<unsigned int>("XCoord", iXCoordMax);
    coord_max.SetField<unsigned int>("YCoord", iYCoordMax);
    coord_max.SetField<unsigned int>("ZCoord", iZCoordMax);

    if (iTCoordMax == 0)
      {
      coord_max.SetField<unsigned int>("TCoord", iTCoord);
      }
    else
      {
      coord_max.SetField<unsigned int>("TCoordMax", iTCoord);
      }

    iTrace.SetTheDataFromTheVisu(iDatabaseConnector, iTraceNodes,
                                 coord_min, coord_max, iMeshAttributes);

    //T trace_row( iDatabaseConnector, iTraceNodes, coord_min, coord_max,
    //this->m_ImgSessionID, iMeshAttributes );
  }

  /**
  \brief get the RGB Alpha values from the iTraceRow and set a QColor with them
  \tparam T any children of GoDBTraceRow
  \param[in] iTraceRow the trace from which the QColor is created
  \return QColor with the values corresponding to the color values of the iTraceRow
  */
  template<typename T>
  QColor GetQColorFromTraceRow(T iTraceRow)
  {
    QColor Color(atoi(iTraceRow.GetMapValue("Red").c_str()),
                           atoi(iTraceRow.GetMapValue("Green").c_str()),
                           atoi(iTraceRow.GetMapValue("Blue").c_str()),
                           atoi(iTraceRow.GetMapValue("Alpha").c_str()));
    return Color;
  }

  template<typename T>
  void SetTheColorForTheRow(T &ioRow, QColor iColor)
  {
    ioRow.SetField<int>("Red",iColor.red());
    ioRow.SetField<int>("Green",iColor.green());
    ioRow.SetField<int>("Blue",iColor.blue());
    ioRow.SetField<int>("Alpha",iColor.alpha());
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
protected slots:
  void CreateContextMenu(const QPoint& pos);
  void DeleteTraces();
  void GoToTheTrace();
  void CheckSelectedRows();
  void UncheckSelectedRows();
  void ShowSelectedRows();
  void HideSelectedRows();
  void ChangeTraceColor();
  void TheTabIsChanged(int iIndex);

  
  /** \brief Create a new Collection row in the collection table and change the
  collection ID of the selected contours to the new CollectionID created:*/
  void CreateCorrespondingCollection();

  void AddToSelectedCollection();
  void ReEditTrace();
  /** \brief Update the m_ContoursInfo or m_MeshesInfo depending on which
 * table the user had clicked with the selected traces and emit a signal
 * to say which m_tracesInfo has changed*/
  void ChangeTracesToHighLightInfoFromTableWidget();

  /** \brief Update the m_ContoursInfo or m_MeshesInfo depending on which
 * table the user had clicked with the selected traces to show and emit a signal
 * to say which m_tracesInfo has changed*/
  void ChangeTracesToShowInfoFromTableWidget();
  /** \brief Add the new created trace in the vector of ContourMeshInfo
 * \todo once we know more for the visualization of meshes, need to do the
 * same for add a collection*/
  void AddATraceToContourMeshInfo(std::string iTraceName, int iTraceID);

  //*********************Slots for TraceManualEditingWidget:**************************
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
  \param[in] iSelectedCelltype name of the celltype
  */
  void UpdateSelectedCellType(std::string iSelectedCellType);
  /**
  \brief set the m_SelectedSubCelltype to iSelectedSubCelltype
  \param[in] iSelectedSubCelltype name of the subcelltype
  */
  void UpdateSelectedSubCellType(std::string iSelectedSubCellType);

  /** \brief Save the new collection in the database and update the
  colorcombobox*/
  void SaveNewCollectionInDB();

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
  //**********************End TraceManualEditingWidget slots related****************


private:
  Q_DISABLE_COPY(QGoPrintDatabase);
  };

#endif
