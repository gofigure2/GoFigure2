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
#include "GoDBTWContainerForContourMesh.h"

GoDBTWContainerForContourMesh::GoDBTWContainerForContourMesh(
  std::string iTraceName, std::string iCollectionName, int iImgSessionID):
  GoDBTableWidgetContainer(iTraceName, iCollectionName, iImgSessionID)
{
  m_ColumnsInfos = GetColumnsInfoForTraceTable();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBTWContainerForContourMesh::~GoDBTWContainerForContourMesh()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForContourMesh::GetCommonInfoForTwoTracesTable()
{
  GoDBTraceInfoForTableWidget temp;

  std::pair< GoDBTraceInfoForTableWidget, std::vector< std::string > > PairTemp;

  //Get the info for the Time Point:
  temp.InfoName = "TimePoint";
  temp.ColumnNameDatabase = "TCoord";
  temp.ColumnNameTableWidget = "TimePoint";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  this->GetInfoForColumnIsVisible();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForContourMesh::FillRowContainerWithDBValues(
    vtkMySQLDatabase *iDatabaseConnector, std::string iRestrictionName,
    std::string iRestrictionValue)
{
  GoDBTableWidgetContainer::FillRowContainerWithDBValues(iDatabaseConnector,
    iRestrictionName,iRestrictionValue);
  this->FillColumnShowHide(iDatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForContourMesh::FillColumnShowHide(vtkMySQLDatabase* iDatabaseConnector)
{
  //get the current timepoint which is also the min timepoint for the imagingsession:
  std::vector<std::string> SelectedFields;
  SelectedFields.push_back("TCoord");
  std::string JoinCondition = "imagingsession.CoordIDMin = coordinate.CoordID";
  std::vector<std::string> VectorMinTimePoint = GetAllSelectedValuesFromTwoTables(
    iDatabaseConnector, "imagingsession", "coordinate",SelectedFields, JoinCondition,
    "imagingsessionID", ConvertToString<int>(this->m_ImgSessionID));

  std::string MinTimePoint = VectorMinTimePoint.at(0);
  std::vector<std::vector<std::string> > Values;
  std::vector<std::string> ShowHideValue;

  int IndexTimePoint = this->GetIndexInsideRowContainer("TimePoint");
  std::vector<std::string>::iterator iter = this->m_RowContainer.at(IndexTimePoint).second.begin();
  while(iter != this->m_RowContainer.at(IndexTimePoint).second.end() )
    {
      if (*iter == MinTimePoint)
        {
        ShowHideValue.push_back("true");
        }
      else
        {
         ShowHideValue.push_back("false");
        }
      Values.push_back(ShowHideValue);
      ShowHideValue.clear();
      iter++;
    }

  std::vector<std::string> Fields;
  Fields.push_back("Show");
  if (!Values.empty())
    {
    this->FillRowContainer(Values,Fields,"ColumnNameTableWidget");
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBTWContainerForContourMesh::GetIndexShowColumn()
{
  return this->GetIndexInsideRowContainer("Show");
}
