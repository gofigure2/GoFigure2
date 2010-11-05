/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 1972 $  // Revision of last commit
  Date: $Date: 2010-08-16 12:23:05 -0400 (Mon, 16 Aug 2010) $  // Date of last commit
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

#include "QGoDBContourManager.h"
#include "GoDBContourRow.h"
#include <iostream>
#include <sstream>
#include <QMessageBox>
#include <QMenu>
#include "GoDBContourRow.h"

QGoDBContourManager::QGoDBContourManager(int iImgSessionID, QWidget *iparent):
  QGoDBTraceManager()
{
  this->SetInfo(iImgSessionID, iparent);
  this->m_TWContainer = new GoDBTWContainerForContourMesh(this->m_TraceName,
                                                          this->m_CollectionName, iImgSessionID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBContourManager::~QGoDBContourManager()
{
  if ( this->m_TWContainer )
    {
    delete this->m_TWContainer;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::SetCollectionsTraceNames()
{
  this->m_TraceName = "contour";
  this->m_CollectionName = "mesh";
  this->m_CollectionOf = "None";
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::DisplayInfoAndLoadVisuContainerForAllContours(
  vtkMySQLDatabase *iDatabaseConnector,unsigned int iTimePoint)
{
  //this->DisplayInfoForAllTraces(iDatabaseConnector);
  this->DisplayInfoAndLoadVisuContainerWithAllTraces< GoDBTWContainerForContourMesh >
    (this->m_TWContainer, iDatabaseConnector);
  this->UpdateTracesVisibilityForGivenTimePoint(iTimePoint);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::DisplayInfoForAllTraces(
  vtkMySQLDatabase *iDatabaseConnector)

{
  this->DisplayInfoForAllTracesTemplate< GoDBTWContainerForContourMesh >(
    this->m_TWContainer, iDatabaseConnector,Qt::Unchecked);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::UpdateTracesVisibilityForGivenTimePoint(unsigned int iTimePoint)
{
  std::list<unsigned int> ListContours = 
    this->m_TraceContainerInfoForVisu->GetElementsTraceIDForGivenTimePoint(
    iTimePoint);
  this->m_Table->SetVisibleStateForListTraceIDs(
    ListContours,Qt::Checked,this->m_TraceName);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

void QGoDBContourManager::DisplayInfoForLastCreatedTrace(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForLastCreatedTraceTemplate< GoDBTWContainerForContourMesh >(
    this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::DisplayInfoForExistingTrace(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  this->DisplayInfoForExistingTraceTemplate< GoDBTWContainerForContourMesh >(
    this->m_TWContainer, iDatabaseConnector, iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::AddActionsContextMenu(QMenu *iMenu)
{
  QGoDBTraceManager::AddActionsContextMenu(iMenu);
  this->AddSpecificActionsForContourMesh(iMenu);
  iMenu->addAction( tr("ReEdit the checked %1").arg( this->m_TraceName.c_str() ),
                    this, SLOT( ReEditTrace() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::AddActionForCreateNewCollectionFromCheckedTraces(
  QMenu *iMenu)
{
  iMenu->addAction( tr("Generate a new mesh from checked contours"),
                    this, SLOT( CreateCorrespondingCollection() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::SetDatabaseConnection(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->m_DatabaseConnector = iDatabaseConnector;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::SetSelectedColor(NameWithColorData iSelectedColor)
{
  this->m_SelectedColor = iSelectedColor;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::ChangeTraceColor()
{
  emit NeedToGetCurrentSelectedColor();
  emit NeedToGetDatabaseConnection();

  this->UpdateTheTracesColor(this->m_DatabaseConnector,
                             this->m_SelectedColor);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBContourManager::UpdateTheTracesColor(
  vtkMySQLDatabase *iDatabaseConnector, NameWithColorData iNewColor)
{
  return this->UpdateTheTracesColorTemplate< GoDBContourRow >(
           iDatabaseConnector, iNewColor);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBContourManager::SaveNewContourFromVisu(
  unsigned int iXCoordMin, unsigned int iYCoordMin, unsigned int iZCoordMin,
  unsigned int iTCoord, unsigned int iXCoordMax, unsigned int iYCoordMax,
  unsigned int iZCoordMax, vtkPolyData *iTraceNodes,
  vtkMySQLDatabase *iDatabaseConnector, NameWithColorData iColor,
  unsigned int iMeshID)

{
  GoDBContourRow NewContour(this->m_ImgSessionID);
  //NewContour.SetTheDataFromTheVisu(iDatabaseConnector, iTraceNodes);

  int NewContourID = this->CreateNewTraceInDBFromVisu< GoDBContourRow >(
    iXCoordMin, iYCoordMin, iZCoordMin, iTCoord, iXCoordMax, iYCoordMax, iZCoordMax, iTraceNodes,
    iColor, iDatabaseConnector, NewContour, iMeshID);

  double *rgba = this->GetVectorFromQColor(iColor.second);

  this->m_TraceContainerInfoForVisu->UpdateCurrentElementFromDB(
    NewContourID, rgba);
  delete[] rgba;

  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
  return NewContourID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBContourManager::SaveReeditedContourFromVisu(unsigned int iXCoordMin,
                                                              unsigned int iYCoordMin,
                                                              unsigned int iZCoordMin,
                                                              unsigned int iTCoord,
                                                              unsigned int iXCoordMax,
                                                              unsigned int iYCoordMax,
                                                              unsigned int iZCoordMax,
                                                              vtkPolyData *iContourNodes,
                                                              vtkMySQLDatabase *iDatabaseConnector)
{
  unsigned int TraceID =
    this->m_TraceContainerInfoForVisu->m_CurrentElement.TraceID;
  GoDBContourRow ReeditedContour;

  ReeditedContour.SetValuesForSpecificID(TraceID, iDatabaseConnector);
  this->SetTraceBoundingBoxAndPoints(iXCoordMin, iYCoordMin, iZCoordMin, iTCoord, iXCoordMax,
                                     iYCoordMax, iZCoordMax, iContourNodes, iDatabaseConnector, ReeditedContour);
  ReeditedContour.SaveInDB(iDatabaseConnector);
  this->DisplayInfoForExistingTrace(iDatabaseConnector, TraceID);
  return TraceID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::ReEditTrace()
{
  std::list< unsigned int > ListCheckedTraces =
    this->m_TraceContainerInfoForVisu->GetHighlightedElementsTraceID();
  if ( ListCheckedTraces.empty() )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select the %1 you want to reedit")
      .arg( this->m_TraceName.c_str() ) );
    msgBox.exec();
    }
  else
    {
    if ( ListCheckedTraces.size() != 1 )
      {
      QMessageBox msgBox;
      msgBox.setText(
        tr("Please select only one %1 to reedit")
        .arg( this->m_TraceName.c_str() ) );
      msgBox.exec();
      }
    else
      {
      emit TraceToReEdit( ListCheckedTraces.front() );
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::UpdateTWAndContainerForImportedTraces(
  std::vector< int > iVectorImportedTraces, vtkMySQLDatabase *iDatabaseConnector)
{
  this->UpdateTWAndContainerWithImportedTracesTemplate<
    GoDBTWContainerForContourMesh >(this->m_TWContainer,
                                    iVectorImportedTraces, iDatabaseConnector);
  //update the visualization and the data from visu in the container for visu:
  this->m_TraceContainerInfoForVisu->
  UpdateVisualizationForGivenIDs< std::vector< int > >(
    iVectorImportedTraces, true);
}
