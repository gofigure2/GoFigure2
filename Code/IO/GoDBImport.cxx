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

#include "GoDBImport.h"
#include "SelectQueryDatabaseHelper.h"
#include "QueryDataBaseHelper.h"
#include "ConvertToStringHelper.h"
#include "GoDBColorRow.h"
#include "GoDBCellTypeRow.h"
#include "GoDBSubCellTypeRow.h"
#include "GoDBCoordinateRow.h"
#include "GoDBContourRow.h"
#include "GoDBMeshRow.h"
#include "GoDBTrackRow.h"
#include "GoDBLineageRow.h"

//--------------------------------------------------------------------------
GoDBImport::GoDBImport(std::string iServerName,std::string iLogin,
  std::string iPassword, int iImagingSessionID,std::string iFilename)
{
  this->m_ServerName = iServerName;
  this->m_Login = iLogin;
  this->m_Password = iPassword;
  this->m_ImagingSessionID = iImagingSessionID;
  this->m_InFile.open( iFilename.c_str(), std::ifstream::in );
}

//--------------------------------------------------------------------------
GoDBImport::~GoDBImport()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::ImportContours()
{
  this->OpenDBConnection();
  std::map<int,int> MapColorIDs;
  std::map<int,int> MapCellTypeIDs;
  std::map<int,int> MapSubCellTypeIDs;
  std::map<int,int> MapCoordIDs;
  std::map<int,int> MapContourIDs;
  std::map<int,int> MapMeshIDs;
  std::map<int,int> MapTrackIDs;
  std::map<int,int> MapLineageIDs;
  std::string       LineContent;
  LineContent = this->SaveNoTracesEntities(MapColorIDs,MapCellTypeIDs,
    MapSubCellTypeIDs,MapCoordIDs); 
  this->SaveTraces<GoDBLineageRow>(MapColorIDs,MapCoordIDs,
    MapLineageIDs,LineContent,MapLineageIDs);
  this->SaveTraces<GoDBTrackRow>(MapColorIDs,MapCoordIDs,MapLineageIDs,
    LineContent,MapTrackIDs);

  this->SaveTraces<GoDBMeshRow>(MapColorIDs,MapCoordIDs,MapTrackIDs,
    LineContent,MapMeshIDs,MapCellTypeIDs,MapSubCellTypeIDs);
  //this->SaveMeshes(MapColorIDs,MapCellTypeIDs,MapSubCellTypeIDs,
    //MapCoordIDs,LineContent,MapMeshIDs); 
  this->SaveTraces<GoDBContourRow>(MapColorIDs,MapCoordIDs,MapMeshIDs,
    LineContent,MapContourIDs);
  //this->SaveContours(MapColorIDs,MapCoordIDs,LineContent,MapMeshIDs);
  this->FillContourInfoForVisu(this->m_NewContourIDs);
  /** \todo make the new mesh,track and lineage appear in the tablewidget*/
  this->CloseDBConnection();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBImport::SaveNoTracesEntities(std::map<int,int> & ioMapColorIDs,
  std::map<int,int> & ioMapCellTypeIDs,std::map<int,int> & ioMapSubCellTypeIDs,
  std::map<int,int> & ioMapCoordIDs)
{
  std::string LineContent;
  getline(this->m_InFile, LineContent);
  while (!this->IsLineForNumberOfEntities(LineContent))
    {
    getline (this->m_InFile, LineContent);
    }
  while (this->FindFieldName(LineContent)!= "NumberOflineage")
    {
    int EntitiesNumber = atoi(this->GetValueForTheLine(LineContent).c_str());
    getline(this->m_InFile, LineContent);
    //if there is nothing to be saved in the database for this group, just go
    //to the next line in the file:
    if (EntitiesNumber != 0)
      {
      if(this->GetValueForTheLine(LineContent) != "NoValueOnTheLine")
        {
        std::cout<<"There was supposed to be only the name of the entity to save,the entity will not be saved";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        return LineContent;
        }
        std::string NameEntity = this->FindFieldName(LineContent);
      if (NameEntity == "color")
        {
        LineContent = this->SaveImportedEntitiesInDatabase<GoDBColorRow>(
          EntitiesNumber,ioMapColorIDs);
        }
      if (NameEntity == "celltype")
        {
        LineContent = this->SaveImportedEntitiesInDatabase<GoDBCellTypeRow>(
        EntitiesNumber,ioMapCellTypeIDs);
        }
      if (NameEntity == "subcellulartype")
        {
        LineContent = this->SaveImportedEntitiesInDatabase<GoDBSubCellTypeRow>(
        EntitiesNumber,ioMapSubCellTypeIDs);
        }
      if (NameEntity == "coordinate")
        {
        LineContent = this->SaveImportedEntitiesInDatabase<GoDBCoordinateRow>(
        EntitiesNumber,ioMapCoordIDs);
        }
      if (NameEntity != "color" && NameEntity != "celltype" && 
        NameEntity != "subcellulartype" && NameEntity != "coordinate")
        {
        std::cout<<"The name of the entity doesn't correspond to any of the no traces entity";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        }
      }
    }
  return LineContent;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void GoDBImport::SaveMeshes(std::map<int,int> iMapColorIDs,
  std::map<int,int> iMapCellTypeIDs,std::map<int,int> iMapSubCellTypeIDs,
  std::map<int,int> iMapCoordIDs,std::string & ioLineContent,
  std::map<int,int> & ioMapMeshIDs)
{
  GoDBMeshRow MeshToSave;
  int NumberOfMeshes = atoi(this->GetValueForTheLine(ioLineContent).c_str());
  getline(this->m_InFile, ioLineContent);
  for(int i = 0; i < NumberOfMeshes; i++)
    {
    ioLineContent = this->GetValuesFromInfile<GoDBMeshRow>(
      MeshToSave);
    this->ReplaceTheFieldWithNewIDs<GoDBMeshRow>(
      iMapCellTypeIDs,"CellTypeID",MeshToSave);
    this->ReplaceTheFieldWithNewIDs<GoDBMeshRow>(
      iMapSubCellTypeIDs,"SubCellularID",MeshToSave);
    this->ReplaceCommonFieldsForTraces(
      MeshToSave,iMapColorIDs,iMapCoordIDs);
    int OldMeshID = atoi(MeshToSave.GetMapValue(MeshToSave.GetTableIDName()).c_str());
    // in order the query works, the MeshID to be saved has to be set to 0 otherwise 
    // if the MeshID already exits,the query will return the error 
    // "Duplicate entry MeshID for key primary":
    // MeshToSave.SetField(MeshToSave.GetTableIDName(),"0");
    int NewMeshID = MeshToSave.DoesThisBoundingBoxExist(this->m_DatabaseConnector);
    if(NewMeshID == -1)
      {
      NewMeshID = MeshToSave.SaveInDB(this->m_DatabaseConnector);
      this->m_NewMeshIDs.push_back(NewMeshID);
      }
    else
      {
      std::cout<<"The mesh "<<OldMeshID<<" has the same bounding box as ";
      std::cout<<"the existing mesh "<<NewMeshID;
      std::cout<<"so the imported contours belonging to the mesh "<<OldMeshID;
      std::cout<<" will belong to the existing mesh "<<NewMeshID<<std::endl;
      }
    ioMapMeshIDs[OldMeshID]= NewMeshID;
    }
  std::string test = ioLineContent;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::SaveContours(std::map<int,int> iMapColorIDs,
    std::map<int,int> iMapCoordIDs,std::string & ioLineContent,
    std::map<int,int> iMapMeshIDs)
{
  GoDBContourRow ContourToSave; 
  int NumberOfContours = atoi(this->GetValueForTheLine(ioLineContent).c_str());
  getline(this->m_InFile, ioLineContent);
  for(int i = 0; i < NumberOfContours; i++)
    {
    ioLineContent = this->GetValuesFromInfile<GoDBContourRow>(ContourToSave);
    this->ReplaceCommonFieldsForContourAndMesh(
      ContourToSave,iMapColorIDs,iMapCoordIDs);
    this->ReplaceTheFieldWithNewIDs<GoDBContourRow>(iMapMeshIDs,"MeshID",ContourToSave);
    //in order the query works, the ContourID to be saved has to be set to 0 otherwise 
    //if the ContourID already exits,the query will return the error 
    //"Duplicate entry ContourID for key primary":
    int OldContourID = atoi(ContourToSave.GetMapValue(ContourToSave.GetTableIDName()).c_str());
    ContourToSave.SetField(ContourToSave.GetTableIDName(),"0");
    int ContourID = ContourToSave.DoesThisBoundingBoxExist(this->m_DatabaseConnector);
    if(ContourID != -1)
      {
      std::cout<<"The contour" << OldContourID;
      std::cout<<"won't be saved because there is already an existing contour with the same bounding box ";
      std::cout<<"The existing contourID is "<<ContourID <<std::endl;
      }
    else
      {
      this->m_NewContourIDs.push_back(ContourToSave.SaveInDB(this->m_DatabaseConnector));
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::SaveTracks(std::map<int,int> iMapColorIDs,
    std::map<int,int> iMapCoordIDs,std::string & ioLineContent,
    std::map<int,int> iMapLineageIDs)
{
  GoDBTrackRow TrackToSave; 
  int NumberOfTracks = atoi(this->GetValueForTheLine(ioLineContent).c_str());
  getline(this->m_InFile, ioLineContent);
  for(int i = 0; i < NumberOfTracks; i++)
    {
    ioLineContent = this->GetValuesFromInfile<GoDBContourRow>(TrackToSave);
    this->ReplaceCommonFieldsForTraces(
      TrackToSave,iMapColorIDs,iMapCoordIDs);
    this->ReplaceTheFieldWithNewIDs<GoDBTrackRow>(iMapLineageIDs,"LineageID",TrackToSave);
    //in order the query works, the TrackID to be saved has to be set to 0 otherwise 
    //if the TrackID already exits,the query will return the error 
    //"Duplicate entry ContourID for key primary":
    int OldTrackID = atoi(TrackToSave.GetMapValue(TrackToSave.GetTableIDName()).c_str());
    TrackToSave.SetField(TrackToSave.GetTableIDName(),"0");
    int TrackID = TrackToSave.DoesThisBoundingBoxExist(this->m_DatabaseConnector);
    if(TrackID != -1)
      {
      std::cout<<"The contour" << OldContourID;
      std::cout<<"won't be saved because there is already an existing contour with the same bounding box ";
      std::cout<<"The existing contourID is "<<ContourID <<std::endl;
      }
    else
      {
      this->m_NewTrackIDs.push_back(ContourToSave.SaveInDB(this->m_DatabaseConnector));
      }
    }
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::FillContourInfoForVisu(
  std::vector<int> iListContourIDs)
{
  for (unsigned int i = 0; i<iListContourIDs.size(); i++)
    {
    this->m_NewContourInfoForVisu.push_back(GetTraceInfoFromDB(
      this->m_DatabaseConnector,"contour",
      "mesh",iListContourIDs.at(i)));
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBImport::FindFieldName(std::string iLine)
{
  size_t BegName = iLine.find("<",0)+1;
  size_t EndName = iLine.find(">",0);
  size_t NameLength = EndName - BegName;
  std::string oName = iLine.substr(BegName,NameLength);
  if (oName.find("/",0)== 0)
    {
    oName = oName.substr(1);
    }
  return oName;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBImport::GetValueForTheLine(std::string iLine)
{
  size_t BegValue = iLine.find(">",0)+1;
  size_t EndValue = iLine.find("<",BegValue);
  if( EndValue != iLine.npos)
    {
    size_t ValueLength = EndValue - BegValue;
    return iLine.substr(BegValue,ValueLength);
    }
  return "NoValueOnTheLine";  
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool GoDBImport::IsLineForNumberOfEntities(std::string iLine)
{
  size_t BegValue = iLine.find("NumberOf",0);
  if (BegValue != iLine.npos)
    {
    return true;
    }
  return false;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::OpenDBConnection()
{
  this->m_DatabaseConnector = OpenDatabaseConnection(m_ServerName,
    m_Login,m_Password,"gofiguredatabase");
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::CloseDBConnection()
{
  CloseDatabaseConnection(m_DatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void GoDBExport::on_actionImportContour_triggered( )
{
  typedef std::vector<ContourMeshStructure> ContourMeshVectorType;
  typedef ContourMeshVectorType::iterator ContourMeshIteratorType;

  // get current tab widget
  QWidget* w = this->CentralTabWidget->currentWidget();

  QGoTabImageView3DwT* w3t = dynamic_cast< QGoTabImageView3DwT* >( w );

  //get default color
  double rgba[4];
  rgba[0] = rgba[1] = rgba[2] = rgba[3] = 1;

// Arnauds Suggestions
//   typedef std::list< ContourMeshStructure > ContourMeshListType;
//   typedef std::map< int, ContourMeshListType > MapOfContourMeshListType;
// 1. Fill Container
// 2. Iterate]
//    Create a new mesh
//    Get corresponding id
//    Add the list with the id
// Add subcellular type
// Add

  if( w3t )
    {
    if( w3t->m_DataBaseTables->IsDatabaseUsed() )
      {
      // Get file name
      QString p = QFileDialog::getOpenFileName(
      this,
      tr( "Open Contour Export File" ),"",
      tr( "TextFile (*.txt)" )
      );

      if ( ! p.isNull() )
        {
        std::string filename = p.toStdString();

        int contourId, meshId, timePt, NumberOfContours;
        //Open an xml file
        std::ifstream infile( filename.c_str(), std::ifstream::in );
        std::string line, nodes;
        if( infile.is_open() )
          {
          vtkPolyDataMySQLTextReader* convert_reader =
            vtkPolyDataMySQLTextReader::New();
          convert_reader->SetIsContour( true );

          // Get number of contours
          getline( infile, line );
          std::istringstream nC(line);
          nC >> NumberOfContours;
//           std::cout << "# of contours: " << NumberOfContours << std::endl;
          for( int i = 0; i < NumberOfContours; i++ )
            {
            //<contour>
            getline( infile, line );
            getline( infile, line );
            std::istringstream c(line);
            c >> contourId;
//             std::cout << contourId << std::endl;

            // <TCoord>
            getline( infile, line );
            getline( infile, line );
            std::istringstream t(line);
            t >> timePt;
            getline( infile, line );
//             std::cout << timePt << std::endl;

            // <MeshId>
            getline( infile, line );
            getline( infile, line );
            std::istringstream m(line);
            m >> meshId;
            getline( infile, line );
//             std::cout << meshId << std::endl;

            // <Nodes>
            getline( infile, line );
            getline( infile, nodes );
            getline( infile, line );
//             std::cout << nodes << std::endl;

            //create a vtkPolyData*
            vtkPolyData* ptr = convert_reader->GetPolyData( nodes );

// Fill container

            w3t->AddContourFromNodes(
                contourId,
                ptr,
                rgba, // what default color to set
                false,   // no highlighting
                timePt, // timepoint
                true ); // to be saved in the database

            getline( infile, line );
            }
         }
       }
     }
   }
}*/
