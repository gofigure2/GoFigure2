/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Revision: 1188 $  // Revision of last commit
  Date: $Date: 2010-04-06 15:24:27 -0400 (Tue, 06 Apr 2010) $  // Date of last commit
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
  
  this->SaveNoTracesEntities(MapColorIDs,MapCellTypeIDs,MapSubCellTypeIDs,
  MapCoordIDs); 
  this->CloseDBConnection();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::SaveNoTracesEntities(std::map<int,int> & ioMapColorIDs,
  std::map<int,int> & ioMapCellTypeIDs,std::map<int,int> & ioMapSubCellTypeIDs,
  std::map<int,int> & ioMapCoordIDs)
{
  std::string LineContent;
  getline(this->m_InFile, LineContent);
  while (!this->IsLineForNumberOfEntities(LineContent))
    {
    getline (this->m_InFile, LineContent);
    }
  while (this->FindFieldName(LineContent)!= "Number Of mesh")
    {
    int EntitiesNumber = atoi(this->GetValueForTheLine(LineContent).c_str());
    getline(this->m_InFile, LineContent);
      if(this->GetValueForTheLine(LineContent) != "NoValueOnTheLine")
        {
        std::cout<<"There was supposed to be only the name of the entity to save,the entity will not be saved";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        return;
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
  //size_t BegValue = iFieldName.size() + 2;
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
  size_t BegValue = iLine.find("Number Of",0);
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