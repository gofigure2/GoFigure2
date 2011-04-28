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

  // for a list of lineages - NOT TESTED
  QObject::connect( m_LineageContainerInfoForVisu,
                    SIGNAL( HighlightLineage(unsigned int, bool) ),
                    m_TrackContainerInfoForVisu,
                    SLOT( HighlightCollection(unsigned int, bool) ) );

  QObject::connect( m_LineageContainerInfoForVisu,
                    SIGNAL( ShowLineage(unsigned int, bool) ),
                    m_TrackContainerInfoForVisu,
                    SLOT( ShowCollection(unsigned int, bool) ) );

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
void QGoDBLineageManager::DisplayInfoAndLoadVisuContainerForAllLineages(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoAndLoadVisuContainerWithAllTraces< GoDBTWContainerForLineage >
    (this->m_TWContainer, iDatabaseConnector);

  this->UpdateDivisionsColors();
  this->UpdateDivisionsScalars();
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

  this->m_LineageContainerInfoForVisu->InsertNewLineage(NewLineageID, 
    this->GetVectorFromQColor(this->m_SelectedColorData->second), iTrackRoot,
    true);
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

    std::list< unsigned int > oList =
        this->m_LineageContainerInfoForVisu->GetHighlightedElementsTraceID();

  std::list< unsigned int >::iterator it = oList.begin();

  while( it != oList.end() )
    {
    unsigned int trackRoot = this->m_LineageContainerInfoForVisu->GetLineageTrackRootID(*it);
    
    /*double* color = new double[4];
    color[0] = this->m_SelectedColorData->second.redF();
    color[1] = this->m_SelectedColorData->second.greenF();
    color[2] = this->m_SelectedColorData->second.blueF();
    color[3] = this->m_SelectedColorData->second.alphaF();
    //change the data color
    m_TrackContainerInfoForVisu->UpdateCollectionColorsData( trackRoot, color );
    delete[] color;*/
     m_TrackContainerInfoForVisu->UpdateCollectionColorsData( trackRoot, 
       this->GetVectorFromQColor(this->m_SelectedColorData->second) );

    ++it;
    }

  return oList;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateTWAndContainerForImportedTraces(
  std::vector< int > iVectorImportedTraces, vtkMySQLDatabase *iDatabaseConnector)
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
void QGoDBLineageManager::DeleteListTraces(vtkMySQLDatabase *iDatabaseConnector,
                                         std::list< unsigned int > iListTraces)
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
  this->GetTracesInfoFromDBAndModifyContainerForVisuTemplate< LineageContainer >(
    this->m_LineageContainerInfoForVisu, iDatabaseConnector, iListTraceIDs);
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
UpdateDivisionsScalars()
{
  // Get track root IDs
  std::list<unsigned int> rootIDs =
      this->m_LineageContainerInfoForVisu->GetListOfTrackRootIDs();

  std::list<unsigned int>::iterator it = rootIDs.begin();
  while( it != rootIDs.end() )
    {
    m_TrackContainerInfoForVisu->UpdateCollectionScalars( *it );
    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoDBLineageManager::
UpdateDivisionsScalars( unsigned int iLineageID )
{
  unsigned int root =
      this->m_LineageContainerInfoForVisu->GetLineageTrackRootID(iLineageID);
  m_TrackContainerInfoForVisu->UpdateCollectionScalars( root );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoDBLineageManager::
UpdateDivisionsColors()
{
  // Get track root IDs
  std::list<unsigned int> rootIDs =
      this->m_LineageContainerInfoForVisu->GetListOfTrackRootIDs();

  std::list<unsigned int> lineageIDs =
      this->m_LineageContainerInfoForVisu->GetListOfLineageIDs();

  std::list<unsigned int>::iterator itTrack = rootIDs.begin();
  std::list<unsigned int>::iterator itLineage = lineageIDs.begin();

  while( itTrack != rootIDs.end() )
    {
    double* color = this->m_LineageContainerInfoForVisu->GetLineageColor(*itLineage);
    m_TrackContainerInfoForVisu->UpdateCollectionColors( *itTrack, color );
    ++itTrack;
    ++itLineage;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoDBLineageManager::
UpdateDivisionsColors( unsigned int iLineage)
{
    unsigned int root =
        this->m_LineageContainerInfoForVisu->GetLineageTrackRootID(iLineage);
    double* color = this->m_LineageContainerInfoForVisu->GetLineageColor(iLineage);
    if(color)
      {
      m_TrackContainerInfoForVisu->UpdateCollectionColors( root, color );
      }
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
QGoDBLineageManager::UpdateBoundingBoxes(vtkMySQLDatabase *iDatabaseConnector,
                                   std::list< unsigned int > iListTracesIDs,
                                   bool UpdateTW)
{
  QGoDBTraceManager::UpdateBoundingBoxes(iDatabaseConnector, iListTracesIDs, UpdateTW);
  std::list<unsigned int>::iterator iter = iListTracesIDs.begin();
  while(iter != iListTracesIDs.end() )
    {
    // update color data and actors
    this->UpdateDivisionsColors(*iter);
    this->UpdateDivisionsScalars(*iter);
    ++iter;
    }
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
  vtkMySQLDatabase *iDatabaseConnector, std::list<unsigned int> iLineagesID)
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
