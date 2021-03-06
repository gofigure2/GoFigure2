/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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

#include "QGoDBLineageManager.h"
#include "GoDBLineageRow.h"
#include "GoDBTrackRow.h"
#include "GoDBTrackFamilyRow.h"
#include <iostream>
#include <sstream>

// directory for export
 #include <QFileDialog>
#include <QString>

//writer
#include "vtkSmartPointer.h"
#include "vtkTree.h"
#include "vtkTreeWriter.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkGraphLayoutView.h"
#include "LineageStructure.h"

QGoDBLineageManager::QGoDBLineageManager(int iImgSessionID, QWidget *iparent) :
  QGoDBTraceManager(), m_LineageContainerInfoForVisu(NULL), m_TrackContainerInfoForVisu(NULL)
{
  this->SetInfo(iImgSessionID, iparent);
  this->m_TWContainer = new GoDBTWContainerForLineage(iImgSessionID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBLineageManager::~QGoDBLineageManager()
{
  if ( this->m_TWContainer )
    {
    delete this->m_TWContainer;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::SetLineagesInfoContainersForVisu(
  LineageContainer *iContainerForVisu, TrackContainer *iTrackContainerInfoForVisu)
{
  this->SetTracesInfoContainerForVisuTemplate< LineageContainer >(
    iContainerForVisu, &this->m_LineageContainerInfoForVisu);

  this->m_TrackContainerInfoForVisu = iTrackContainerInfoForVisu;

  // Connect signals
  // actor picking in visualization
  QObject::connect( m_TrackContainerInfoForVisu,
                    SIGNAL(UpdateLineageHighlightingFromTrackRootID(unsigned int)),
                    this,
                    SLOT( UpdateElementHighlighting(unsigned int) ) );

  // for a list of lineages
  QObject::connect( m_LineageContainerInfoForVisu,
                    SIGNAL( HighlightLineage(const unsigned int&, const bool&) ),
                    m_TrackContainerInfoForVisu,
                    SLOT( HighlightCollection(const unsigned int&, const bool&) ) );

  QObject::connect( m_LineageContainerInfoForVisu,
                    SIGNAL( ShowLineage(const unsigned int&, const bool&) ),
                    m_TrackContainerInfoForVisu,
                    SLOT( ShowCollection(const unsigned int&, const bool&) ) );

  QObject::connect( m_LineageContainerInfoForVisu,
                    SIGNAL( ShowCurrentLineage(std::list<unsigned int>&, const unsigned int&) ),
                    m_TrackContainerInfoForVisu,
                    SLOT( ShowCurrentCollection(std::list<unsigned int>&, const unsigned int&) ) );

  QObject::connect( m_TrackContainerInfoForVisu,
                    SIGNAL(UpdateTWCollectionStatus(std::list<unsigned int>, std::list<unsigned int>)),
                    this,
                    SLOT( UpdateStatus(std::list<unsigned int>, std::list<unsigned int>) ) );

  // export lineage
  QObject::connect( m_LineageContainerInfoForVisu,
                    SIGNAL( ExportLineages() ),
                    this,
                    SLOT( ExportLineages() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::SetCollectionsTraceNames()
{
  this->m_TraceName = "lineage";
  this->m_CollectionName = "None";
  this->m_CollectionOf = "track";
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DisplayInfoForAllTraces(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForAllTracesTemplate< GoDBTWContainerForLineage >(
    this->m_TWContainer, iDatabaseConnector, Qt::Unchecked);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DisplayInfoForTracesForSpecificTPs(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::list<unsigned int> & iListTPs)
{
  (void) iListTPs;
  this->DisplayInfoForAllTraces(iDatabaseConnector);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

void QGoDBLineageManager::DisplayInfoAndLoadVisuContainerForAllLineages(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->m_Table->setSortingEnabled(false);

  std::vector< int > VectorIDs = this->m_TWContainer->GetAllTraceIDsInContainer();
  std::list<unsigned int> ListIDs(VectorIDs.begin(), VectorIDs.end());

  std::list<LineageStructure> list_of_traces =
      this->m_CollectionOfTraces->GetListStructureFromDB<LineageStructure>(
      iDatabaseConnector, this->m_ImgSessionID, ListIDs);

  this->m_Table->DisplayColumnNames(
    this->m_TWContainer->GetListColumnsNamesAndToolTipsForTableWidget() );

  std::list<LineageStructure>::iterator it = list_of_traces.begin();
  while ( it != list_of_traces.end() )
    {
    LineageStructure Lineage = *it;
    GoFigureLineageAttributes Attributes =
      m_TrackContainerInfoForVisu->UpdateDivisionsForALineage(
      Lineage.TrackRootID, Lineage.rgba);
    this->m_LineageContainerInfoForVisu->Insert(*it);
    this->m_TWContainer->SetLineageAttributes(Attributes);
    this->InsertLineageInTW(iDatabaseConnector, Lineage.TraceID);
    ++it;
    }

  this->m_Table->setSortingEnabled(true);
  this->m_Table->resizeColumnsToContents();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::InsertLineageInTW(vtkMySQLDatabase *iDatabaseConnector,
  unsigned int iTraceID)
  {
    TWContainerType RowContainer =
      this->m_TWContainer->GetContainerForOneSpecificTrace(iDatabaseConnector,
                                                    iTraceID);


    // insert is buggy on sorted table
    // 1- unsort (if sorted)
    // 2- insert
    // 3- sort (if sorted)
    bool sorting = this->m_Table->isSortingEnabled();
    if(sorting)
      {
      this->m_Table->setSortingEnabled(false);
      }
    this->m_Table->InsertOnlyOneNewRow(RowContainer,
                                this->m_TWContainer->GetIndexForGroupColor(this->m_TraceName),
                                this->m_TWContainer->GetIndexForGroupColor(this->m_CollectionName),
                                this->m_TraceName, this->m_CollectionName, Qt::Unchecked);
    if(sorting)
      {
      this->m_Table->setSortingEnabled(true);
      }
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DisplayInfoForLastCreatedTrace(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForLastCreatedTraceTemplate< GoDBTWContainerForLineage >(
    this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DisplayInfoForExistingTrace(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  this->DisplayInfoForExistingTraceTemplate< GoDBTWContainerForLineage >(
    this->m_TWContainer, iDatabaseConnector, iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBLineageManager::CreateNewLineageWithTrackRoot(
      vtkMySQLDatabase *iDatabaseConnector, unsigned int iTrackRoot)
{
  GoDBLineageRow NewLineage;
  unsigned int NewLineageID =
    this->m_CollectionOfTraces->CreateCollectionWithNoTracesNoPoints< GoDBLineageRow >(
      iDatabaseConnector, *this->m_SelectedColorData, NewLineage);
  this->UpdateTrackRootSelectedLineage(iDatabaseConnector, NewLineageID, iTrackRoot);
  // pointer to double has to be deleted after usage...
  double* color = this->GetVectorFromQColor(this->m_SelectedColorData->second);
  this->m_LineageContainerInfoForVisu->InsertNewLineage(NewLineageID, color
    , iTrackRoot,
    true);
  delete[] color;
  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);

  return NewLineageID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBLineageManager::UpdateTheTracesColor(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->UpdateTheTracesColorTemplate< GoDBLineageRow,
      LineageContainer >(iDatabaseConnector, this->m_LineageContainerInfoForVisu);

  std::list< unsigned int > oList = this->GetListHighlightedIDs();

  std::list< unsigned int >::iterator it = oList.begin();
  while( it != oList.end() )
    {
    // pointer to double has to be deleted after usage...
    double* color = this->GetVectorFromQColor(this->m_SelectedColorData->second);
    unsigned int trackRoot = this->m_LineageContainerInfoForVisu->GetLineageTrackRootID(*it);
     m_TrackContainerInfoForVisu->UpdateCollectionColorsData( trackRoot, color);
    delete[] color;
    ++it;
    }

  return oList;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateTWAndContainerForImportedTraces(
  const std::vector< int > & iVectorImportedTraces,
  vtkMySQLDatabase *iDatabaseConnector)
{
  //this->UpdateTWAndContainerWithImportedTracesTemplate<
  //  GoDBTWContainerForLineage >(this->m_TWContainer,
                             // iVectorImportedTraces, iDatabaseConnector);

}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DeleteCheckedTraces(vtkMySQLDatabase *iDatabaseConnector)
{
  this->DeleteListTraces(iDatabaseConnector, this->GetListHighlightedIDs() );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::
DeleteListTraces(vtkMySQLDatabase *iDatabaseConnector,
                 const std::list< unsigned int > & iListTraces)
{
  //delete the lineages from the visu, the database and the TW:
  this->DeleteTracesTemplate< LineageContainer >(iDatabaseConnector,
                                               this->m_LineageContainerInfoForVisu, iListTraces, false);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBLineageManager::GetListHighlightedIDs()
{
  return this->m_LineageContainerInfoForVisu->GetHighlightedElementsTraceID();
}

//-------------------------------------------------------------------------
// From table widget
//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateHighlightedElementsInVisuContainer(
  int iTraceID)
{
  // update lineage container element (invert highlighted boolean)
  this->m_LineageContainerInfoForVisu->
    UpdateElementHighlightingWithGivenTraceID(iTraceID);
  //get root track id
  unsigned int trackRootID = this->m_LineageContainerInfoForVisu->
    GetLineageTrackRootID(iTraceID);
  // is the lineage highlighted?
  bool highlighted = this->m_LineageContainerInfoForVisu->
      GetLineageHighlighted(iTraceID);
  // update divisions
  this->m_TrackContainerInfoForVisu->HighlightCollection(trackRootID, highlighted);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateVisibleElementsInVisuContainer(int iTraceID)
{
  // update container element (invert visible bool)
  this->m_LineageContainerInfoForVisu->
    UpdateElementVisibilityWithGivenTraceID(iTraceID);
  //get root track id
  unsigned int trackRootID = this->m_LineageContainerInfoForVisu->
    GetLineageTrackRootID(iTraceID);
  // is the lineage visible?
  bool visible = this->m_LineageContainerInfoForVisu->
      GetLineageVisibile(iTraceID);
  // update divisions
  this->m_TrackContainerInfoForVisu->ShowCollection(trackRootID, visible);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::GetTracesInfoFromDBAndModifyContainerForVisu(
  vtkMySQLDatabase *iDatabaseConnector,
  std::list< unsigned int > iListTraceIDs)
{
    std::list<LineageStructure> list_of_traces =
      this->m_CollectionOfTraces->GetListStructureFromDB<LineageStructure>(
      iDatabaseConnector, this->m_ImgSessionID, iListTraceIDs);
    std::list<LineageStructure>::iterator it = list_of_traces.begin();
    while ( it != list_of_traces.end() )
      {
      LineageStructure Lineage = *it;
      m_TrackContainerInfoForVisu->UpdateDivisionsForALineage(
        Lineage.TrackRootID, Lineage.rgba);
      this->m_LineageContainerInfoForVisu->Insert(*it);
      ++it;
      }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::SetColorCoding(bool IsChecked)
{
  std::string ColumnName = "";
  std::map<unsigned int, std::string> Values;
  std::map<unsigned int, std::string> NewValues;
  m_IsColorCodingOn = IsChecked;
  //create map track ID/field
  if (IsChecked)
    {
    Values = this->m_Table->GetTraceIDAndColumnsValues(
          this->m_TraceNameID, ColumnName);

    // change lineage id by track root id
    std::map<unsigned int, std::string>::iterator trackRootIt = Values.begin();
    while(trackRootIt != Values.end())
      {
      unsigned int trackRoot =
          m_LineageContainerInfoForVisu->GetLineageTrackRootID(trackRootIt->first);
      NewValues.insert(
          std::pair<unsigned int,std::string>(trackRoot, trackRootIt->second) );
      ++trackRootIt;
      }

      vtkLookupTable* LUT = NULL;

    bool IsRandomIncluded =
      (ColumnName == this->m_TraceNameID) ||
      (ColumnName == this->m_CollectionNameID);

    QGoColorCodingDialog::ColorWay UserColorway =
      QGoColorCodingDialog::GetColorWay( this->m_TraceName, &LUT,
      IsRandomIncluded, this->m_Table );

    switch ( UserColorway )
      {
      case QGoColorCodingDialog::Default:
        m_TrackContainerInfoForVisu->SetCollectionColorCode( ColumnName,NewValues );
        break;

      case QGoColorCodingDialog::Random:
        m_TrackContainerInfoForVisu->SetDivisionRandomColor(ColumnName,NewValues );
        break;

      case QGoColorCodingDialog::LUT:
        m_TrackContainerInfoForVisu->SetCollectionColorCode( ColumnName,NewValues );
        m_TrackContainerInfoForVisu->SetLookupTableForAllDivisionsColorCoding(LUT);
        break;

      default:
      case QGoColorCodingDialog::Nothing:
        m_IsColorCodingOn = !IsChecked;
        break;
      }
    }
  else
    {
    m_TrackContainerInfoForVisu->SetCollectionColorCode( ColumnName, NewValues );
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateTrackRootSelectedLineage(
  vtkMySQLDatabase* iDatabaseConnector,
  unsigned int iLineageID, unsigned int iTrackIDRoot)
{
  GoDBLineageRow LastLineage;
  LastLineage.SetValuesForSpecificID(iLineageID, iDatabaseConnector);
  LastLineage.SetField("TrackIDRoot", iTrackIDRoot);
  LastLineage.SaveInDB(iDatabaseConnector);
}
//-------------------------------------------------------------------------
// actor picking
//-------------------------------------------------------------------------
void
QGoDBLineageManager::
UpdateElementHighlighting(unsigned int iTraceRootID)
{
  unsigned int lineageID = this->m_LineageContainerInfoForVisu->GetTraceIDFromTrackRootID(
          iTraceRootID);
  this->m_LineageContainerInfoForVisu->UpdateElementHighlighting(lineageID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoDBLineageManager::
ExportLineages()
{
  //get path to export somewhere
  QString dir = QFileDialog::getExistingDirectory(NULL, tr("Choose Directory"));
  //get lineages info
  std::list<unsigned int> rootIDs =
      this->m_LineageContainerInfoForVisu->GetListOfTrackRootIDs();

  std::list<unsigned int> lineageIDs =
      this->m_LineageContainerInfoForVisu->GetListOfLineageIDs();

  std::list<unsigned int>::iterator itLineage = lineageIDs.begin();
  std::list<unsigned int>::iterator itTrack = rootIDs.begin();

  // export all the lineages
  while(itLineage != lineageIDs.end() )
    {
    vtkMutableDirectedGraph* graph =
      m_TrackContainerInfoForVisu->ExportLineage(*itTrack);

    vtkSmartPointer<vtkTree> tree =
      vtkSmartPointer<vtkTree>::New();
    tree->CheckedDeepCopy(graph);

    //save tree
    vtkSmartPointer<vtkTreeWriter> writer =
        vtkSmartPointer<vtkTreeWriter>::New();
    writer->SetInput(tree);
    QString name(dir);
    name.append("/lineage_");
    name.append( QString::number(*itLineage, 10) );
    name.append(".vtk");
    writer->SetFileName(name.toLocal8Bit().data());
    writer->Write();

    graph->Delete();

    ++itLineage;
    ++itTrack;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoDBLineageManager::
UpdateBoundingBoxes(vtkMySQLDatabase *iDatabaseConnector,
                    const std::list< unsigned int > & iListTracesIDs,
                    bool UpdateTW)
{
  this->m_CollectionOfTraces->RecalculateDBBoundingBox(
    iDatabaseConnector, iListTracesIDs);

  std::list<unsigned int>::const_iterator iter = iListTracesIDs.begin();

  while(iter != iListTracesIDs.end() )
    {
    std::list<unsigned int> Listiter;
    Listiter.push_back(*iter);
    //need to check first that the lineage does exist, which corresponds to check if there are tracks that have this lineageid:
    if (!this->m_CollectionOfTraces->GetListTracesIDsFromThisCollectionOf(iDatabaseConnector, Listiter).empty())
      {
      this->UpdateDivisionsInTrackContainer(*iter);
      if ( UpdateTW )
        {
        this->DisplayInfoForExistingTrace(iDatabaseConnector, *iter);
        }
      }
    ++iter;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateDivisionsInTrackContainer(unsigned int iLineageID)
{
  unsigned int root =
      this->m_LineageContainerInfoForVisu->GetLineageTrackRootID(iLineageID);
  double* color = this->m_LineageContainerInfoForVisu->GetLineageColor(iLineageID);

  GoFigureLineageAttributes Attributes;
  if(color)
    {
    Attributes = m_TrackContainerInfoForVisu->UpdateDivisionsForALineage(root, color);
    }
  else
    {
    Attributes = m_TrackContainerInfoForVisu->UpdateCollectionScalars( root );
    }

  this->m_TWContainer->SetLineageAttributes(Attributes);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DeleteTracesFromContextMenu()
{
  std::list<unsigned int> ListLineagesToDelete =
    this->GetListHighlightedIDs();
  if ( QGoDBTraceManager::CheckThatThereAreTracesToDelete(ListLineagesToDelete) )
    {
    emit NeedToGetDatabaseConnection();
    this->DeleteDivisionsForLineages(
      this->m_DatabaseConnector, ListLineagesToDelete );
    DBConnectionNotNeededAnymore();

    emit CheckedTracesToDelete();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DeleteDivisionsForLineages(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::list<unsigned int> & iLineagesID)
{
  std::list<unsigned int> TrackFamiliesToDelete =
    this->m_CollectionOfTraces->GetTrackFamiliesForLineages(iDatabaseConnector, iLineagesID);

  std::list<unsigned int>::iterator iter = TrackFamiliesToDelete.begin();
  while(iter != TrackFamiliesToDelete.end() )
    {
     if (*iter != 0)
       {
       this->DeleteADivision(iDatabaseConnector, *iter);
       }
    ++iter;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DeleteADivision(
  vtkMySQLDatabase *iDatabaseConnector, unsigned int iTrackFamilyID)
{
  GoDBTrackFamilyRow TrackFamily(iTrackFamilyID, iDatabaseConnector);

  //update the trackfamilyID of the daughter:
  GoDBTrackRow Daughter(TrackFamily.GetMapValue<unsigned int>("TrackIDDaughter1"), iDatabaseConnector);
  Daughter.SetField("TrackFamilyID", 0);
  Daughter.SaveInDB(iDatabaseConnector);
  Daughter.SetValuesForSpecificID(TrackFamily.GetMapValue<unsigned int>("TrackIDDaughter2"), iDatabaseConnector);
  Daughter.SetField("TrackFamilyID", 0);
  Daughter.SaveInDB(iDatabaseConnector);

  //delete the division in the visu
  this->m_TrackContainerInfoForVisu->DeleteADivision(
    TrackFamily.GetMapValue<unsigned int>("TrackIDMother") );

  //delete the division from the database:
  TrackFamily.DeleteFromDB(iDatabaseConnector);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoDBLineageManager::
UpdateStatus(std::list<unsigned int> check, std::list<unsigned int> unckeck)
{
  // show
  std::list<unsigned int>::iterator itcheck = check.begin();
  while(itcheck != check.end())
    {
    unsigned int lineageID =
        this->m_LineageContainerInfoForVisu->GetTraceIDFromTrackRootID(*itcheck);
    // check lineage in tw
    ShowTheTraceInTW(lineageID, Qt::Checked);
    // update visibility in the container to true
    this->m_LineageContainerInfoForVisu->SetTraceVisibility(lineageID, true);
    ++itcheck;
    }

  // hide
  std::list<unsigned int>::iterator ituncheck = unckeck.begin();
  while(ituncheck != unckeck.end())
    {
    unsigned int lineageID =
        this->m_LineageContainerInfoForVisu->GetTraceIDFromTrackRootID(*ituncheck);
    // uncheck lineage in tw
    ShowTheTraceInTW(lineageID, Qt::Unchecked);
    // update visibility in the container to false
    this->m_LineageContainerInfoForVisu->SetTraceVisibility(lineageID, false);
    ++ituncheck;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
