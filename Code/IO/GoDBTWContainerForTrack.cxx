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
#include <vtkUnicodeString.h>
#include "GoDBTWContainerForTrack.h"

GoDBTWContainerForTrack::GoDBTWContainerForTrack(int iImgSessionID) :
  GoDBTWContainerForTrackLineage("track", "lineage", iImgSessionID),
  m_TrackAttributes(NULL)
{
  this->SetSpecificInfoForTrackTable();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBTWContainerForTrack::~GoDBTWContainerForTrack()
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForTrack::SetSpecificInfoForTrackTable()
{
  GoDBTraceInfoForTableWidget temp;

  std::pair< GoDBTraceInfoForTableWidget,
             std::vector< std::string > > PairTemp;

  //Get the info for the Deplacement:
  temp.InfoName = "Deplacement";
  temp.ColumnNameTableWidget = "Deplacement";
  temp.ToolTip = "micron";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the Distance:
  temp.InfoName = "Distance";
  temp.ColumnNameTableWidget = "Distance";
  temp.ToolTip = "micron";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for theta:
  temp.InfoName = "Theta";
  temp.ColumnNameTableWidget = "Theta";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for phi:
  temp.InfoName = "Phi";
  temp.ColumnNameTableWidget = "Phi";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for AvgSpeed:
  temp.InfoName = "AvgSpeed";
  temp.ColumnNameTableWidget = "AvgSpeed";
  temp.ToolTip = "micron/s";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for MaxSpeed:
  temp.InfoName = "MaxSpeed";
  temp.ColumnNameTableWidget = "MaxSpeed";
  temp.ToolTip = "micron/s";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForTrack::SetTrackAttributes(GoFigureTrackAttributes *iTrackAttributes)
{
  this->m_TrackAttributes = iTrackAttributes;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBTableWidgetContainer::TWContainerType
GoDBTWContainerForTrack::GetContainerForOneSpecificTrace(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  GoDBTableWidgetContainer::GetContainerForOneSpecificTrace(iDatabaseConnector,
                                                            iTraceID);
  this->FillRowContainerForTrackComputedValues();
  return this->m_RowContainer;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForTrack::FillRowContainerForTrackComputedValues()
{
  std::vector< std::string >                VectorNames;
  std::vector< std::vector< std::string > > VectorValues;
  this->GetValuesAndNamesForTrackComputedValues(this->m_TrackAttributes, VectorValues,
                                                VectorNames);
  this->FillRowContainer(VectorValues, VectorNames, "ColumnNameTableWidget");
  this->m_TrackAttributes = 0;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForTrack::GetValuesAndNamesForTrackComputedValues(
  GoFigureTrackAttributes *iTrackAttributes,
  std::vector< std::vector< std::string > > & ioValues,
  std::vector< std::string > & ioNames)
{
  if ( iTrackAttributes != 0 )
    {
    std::vector< std::string > temp;
    ioNames.push_back("Deplacement");
    temp.push_back( ConvertToString< double >(iTrackAttributes->total_length) );
    ioNames.push_back("Distance");
    temp.push_back( ConvertToString< double >(iTrackAttributes->distance) );
    ioNames.push_back("Theta");
    temp.push_back( ConvertToString< double >(iTrackAttributes->theta) );
    ioNames.push_back("Phi");
    temp.push_back( ConvertToString< double >(iTrackAttributes->phi) );
    ioNames.push_back("AvgSpeed");
    temp.push_back( ConvertToString< double >(iTrackAttributes->avg_speed) );
    ioNames.push_back("MaxSpeed");
    temp.push_back( ConvertToString< double >(iTrackAttributes->max_speed) );
    ioValues.push_back(temp);
    }
}