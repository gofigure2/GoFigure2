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
#include "GoDBTWContainerForLineage.h"

GoDBTWContainerForLineage::GoDBTWContainerForLineage(int iImgSessionID) :
  GoDBTWContainerForTrackLineage("lineage", "None", iImgSessionID)
{
  this->SetSpecificInfoForLineageTable();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBTWContainerForLineage::~GoDBTWContainerForLineage()
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForLineage::SetSpecificInfoForLineageTable()
{
  GoDBTraceInfoForTableWidget temp;

  std::pair< GoDBTraceInfoForTableWidget,
             std::vector< std::string > > PairTemp;

  //Get the info for the TrackIDRoot:
  temp.InfoName = "TrackIDRoot";
  temp.ColumnNameDatabase = "TrackIDRoot";
  temp.TableNameDatabase = "lineage";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the Max Depth:
  temp.InfoName = "MaxDepth";
  temp.ColumnNameTableWidget = "MaxDepth";
  temp.ToolTip = "TBD";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the Min Depth:
  temp.InfoName = "MinDepth";
  temp.ColumnNameTableWidget = "MinDepth";
  temp.ToolTip = "TBD";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the number of divisions:
  temp.InfoName = "NbDivisions";
  temp.ColumnNameTableWidget = "NbDivisions";
  temp.ToolTip = "Number of divisions";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the number of leaves:
  temp.InfoName = "NbLeaves";
  temp.ColumnNameTableWidget = "NbLeaves";
  temp.ToolTip = "Number of leaves";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForLineage::SetLineageAttributes(GoFigureLineageAttributes *iLineageAttributes)
{
  this->m_LineageAttributes = iLineageAttributes;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForLineage::FillRowContainerForLineageComputedValues()
{
  std::vector< std::string >                VectorNames;
  std::vector< std::vector< std::string > > VectorValues;
  this->GetValuesAndNamesForLineageComputedValues(this->m_LineageAttributes, VectorValues,
                                                VectorNames);
  this->FillRowContainer(VectorValues, VectorNames, "ColumnNameTableWidget");
  this->m_LineageAttributes = 0;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBTableWidgetContainer::TWContainerType
GoDBTWContainerForLineage::GetContainerForOneSpecificTrace(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  GoDBTableWidgetContainer::GetContainerForOneSpecificTrace(iDatabaseConnector,
                                                            iTraceID);
  this->FillRowContainerForLineageComputedValues();
  return this->m_RowContainer;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForLineage::GetValuesAndNamesForLineageComputedValues(
  GoFigureLineageAttributes *iLineageAttributes,
  std::vector< std::vector< std::string > > & ioValues,
  std::vector< std::string > & ioNames)
{
  if ( iLineageAttributes != 0 )
    {
    std::vector< std::string > temp;
    ioNames.push_back("MaxDepth");
    temp.push_back( ConvertToString< unsigned int >(iLineageAttributes->MaxDepth) );
    ioNames.push_back("MinDepth");
    temp.push_back( ConvertToString< unsigned int >(iLineageAttributes->MinDepth) );
    ioNames.push_back("NbDivisions");
    temp.push_back( ConvertToString< unsigned int >(iLineageAttributes->NumberOfDivisions) );
    ioNames.push_back("NbLeaves");
    temp.push_back( ConvertToString< unsigned int >(iLineageAttributes->NumberOfLeaves) );
    ioValues.push_back(temp);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------