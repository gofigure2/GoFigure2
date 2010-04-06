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

#include "GoDBExport.h"
#include "SelectQueryDatabaseHelper.h"
#include "QueryDataBaseHelper.h"
#include "ConvertToStringHelper.h"
#include "GoDBColorRow.h"
#include "GoDBCellTypeRow.h"
#include "GoDBSubCellTypeRow.h"
#include "GoDBCoordinateRow.h"


//--------------------------------------------------------------------------
GoDBExport::GoDBExport(std::string iServerName,std::string iLogin,
  std::string iPassword, int iImagingSessionID,std::string iFilename)
{
  this->m_ServerName = iServerName;
  this->m_Login = iLogin;
  this->m_Password = iPassword;
  this->m_ImagingSessionID = iImagingSessionID;
  this->m_outfile.open ( iFilename.c_str(), std::ios::out );
}

//--------------------------------------------------------------------------
GoDBExport::~GoDBExport()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::ExportContours( )
{
  std::string NameDocXml = "ExportTraces";
  int VersionNumber = 1;
  this->OpenDBConnection();
  this->m_outfile << "<?xml version=\"1.0\" ?>" << std::endl;
  this->m_outfile << "<";
  this->m_outfile << NameDocXml;
  this->m_outfile << " version=\"";
  this->m_outfile << VersionNumber;
  this->m_outfile << "\">"<<std::endl;

  this->WriteOnTheOutputFile("imagingsession",this->GetImagingSessionInfoFromDB());
  std::vector<std::string> ListMeshIDsWithContours 
    = this->GetListMeshIDsWithContours();  
  this->WriteTheColorsInfoFromDatabase(ListMeshIDsWithContours);
  //this->WriteTableInfoFromDB<GoDBColorRow>("","");
  //this->WriteTableInfoFromDB<GoDBCellTypeRow>("","");
  //this->WriteTableInfoFromDB<GoDBSubCellTypeRow>("","");
  this->WriteCellTypeAndSubCellTypeInfoFromDatabase(ListMeshIDsWithContours);
  this->WriteCoordinatesInfoFromDatabase();
  this->CloseDBConnection();
  this->m_outfile << this->GetNameWithSlashBrackets(NameDocXml);
  /*typedef std::vector<ContourMeshStructure> ContourMeshVectorType;
  typedef ContourMeshVectorType::iterator ContourMeshIteratorType;

  typedef std::vector < int > RGBAType;
  typedef std::pair < std::string, RGBAType > ColorType;
  //typedef std::list< ColorType > ColorComboBoxType;
  //typedef std::list< std::pair < std::string, std::vector < int > > >::iterator iter;

      ContourMeshVectorType* contours = w3t->m_DataBaseTables->GetTracesInfoListForVisu("contour");

      vtkPolyDataMySQLTextWriter* convert_writer =
        vtkPolyDataMySQLTextWriter::New();

      // Set file name
      QString p = QFileDialog::getSaveFileName(
      this,
      tr( "Save Contour Export File" ),"",
      tr( "TextFile (*.txt)" )
      );

      if ( ! p.isNull() )
      {
        QFileInfo pathInfo( p );
        std::string filename = p.toStdString();

        // Create an xml file
        std::fstream outfile;
        outfile.open ( filename.c_str(), std::ios::out );

        // Missing information in the export
//         ColorComboBoxType colorComboBoxInfo = w3t->m_DataBaseTables->GetColorComboBoxInfofromDB ();
//         for( std::list< ColorType >::iterator it = colorComboBoxInfo.begin();
//           it != colorComboBoxInfo.end();
//           ++it )
//         {
//           std::cout << (*it) << std::endl;
//         }

        // 2. GetCurrentCollectionData ()
        // 3. GetListExistingCollectionIDFromDB (std::string TraceName, int iTimePoint)

        unsigned int contourId, meshId, timePt;
        ContourMeshIteratorType It = contours->begin();
        outfile << contours->size() << std::endl;
        while( It != contours->end() )
        {
          contourId = (*It).TraceID;
          meshId = (*It).CollectionID;
          timePt = (*It).TCoord;
          outfile << "<contour>" << std::endl;
          outfile << contourId << std::endl;
          outfile << "<TCoord> " << std::endl;
          outfile << timePt << std::endl;
          outfile << "</TCoord>" << std::endl;
          outfile << "<MeshId> " << std::endl;
          outfile << meshId << std::endl;
          outfile << "</MeshId>" << std::endl;

          outfile << "<Nodes>" << std::endl;
          //with the class here
          std::string str = convert_writer->GetMySQLText( (*It).Nodes );
          outfile << str << std::endl;
          outfile << "</Nodes>" << std::endl;
          outfile << "</contour>" << std::endl;
          ++It;
        }
        outfile.close();
      }*/
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
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<std::pair<std::string,std::string> > 
  GoDBExport::GetImagingSessionInfoFromDB()
{
  std::vector<std::pair<std::string,std::string> > infoImgSession;
  infoImgSession.push_back(this->GetOneInfoFromDBForImgSession("Name"));
  infoImgSession.push_back(this->GetOneInfoFromDBForImgSession("CreationDate"));
  infoImgSession.push_back(this->GetOneInfoFromDBForImgSession("MicroscopeName"));

  return infoImgSession;  
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::pair<std::string,std::string> GoDBExport::GetOneInfoFromDBForImgSession(
  std::string iNameInfo)
{
  std::pair<std::string,std::string> OneInfo;
  OneInfo.first = iNameInfo;
  OneInfo.second = ListSpecificValuesForOneColumn(
    this->m_DatabaseConnector,"imagingsession", iNameInfo,"ImagingSessionID",
    ConvertToString<int>(this->m_ImagingSessionID)).at(0);
  return OneInfo;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<std::string> GoDBExport::GetListMeshIDsWithContours()
{
  return ListSpecificValuesForOneColumn(this->m_DatabaseConnector,
    "contour", "meshID","imagingsessionID",
    ConvertToString<int>(this->m_ImagingSessionID),true);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteTheColorsInfoFromDatabase(
  std::vector<std::string> iListMeshIDsWithContours)
{
  std::vector<std::string> ListColorIDs = GetSamefieldFromTwoTables(
    this->m_DatabaseConnector,"contour","mesh","ColorID",
    "ImagingSessionID",ConvertToString<int>(this->m_ImagingSessionID),
    "MeshID",iListMeshIDsWithContours);
  this->WriteTableInfoFromDB<GoDBColorRow>(ListColorIDs);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteCellTypeAndSubCellTypeInfoFromDatabase(
  std::vector<std::string> iListMeshIDsWithContours)
{
  std::vector<std::string> ListCellTypeIDs = ListSpecificValuesForOneColumn(
    this->m_DatabaseConnector,"mesh", "CellTypeID","MeshID",
    iListMeshIDsWithContours,true);
  this->WriteTableInfoFromDB<GoDBCellTypeRow>(ListCellTypeIDs);
  std::vector<std::string> ListSubCellTypeIDs = 
    ListSpecificValuesForOneColumn(this->m_DatabaseConnector,
    "mesh", "SubCellularID","MeshID",iListMeshIDsWithContours,true);
  this->WriteTableInfoFromDB<GoDBSubCellTypeRow>(ListSubCellTypeIDs);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteCoordinatesInfoFromDatabase()
{
  std::vector<std::string> ListCoordIDs = GetSamefieldsFromTwoTables(
    this->m_DatabaseConnector,"contour","mesh","CoordIDMax","CoordIDMin",
    "ImagingSessionID",ConvertToString<int>(this->m_ImagingSessionID));
  this->WriteTableInfoFromDB<GoDBCoordinateRow>(ListCoordIDs);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteOnTheOutputFile(std::string iNameOfEntity,
  std::vector<std::pair<std::string,std::string> > iInfoToWrite)
{  
  this->AddTabulation();
  this->m_outfile << GetNameWithBrackets(iNameOfEntity)<<std::endl;
  std::vector<std::pair<std::string,std::string> >::iterator iter =
    iInfoToWrite.begin();
  while(iter!=iInfoToWrite.end())
    {
    this->AddTabulation();
    this->AddTabulation();
    this->m_outfile << this->GetNameWithBrackets(iter->first);
    this->m_outfile << iter->second;
    this->m_outfile << this->GetNameWithSlashBrackets(iter->first)<<std::endl;
    iter++;
    }
  this->AddTabulation();
  this->m_outfile << GetNameWithSlashBrackets(iNameOfEntity)<<std::endl;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBExport::GetNameWithBrackets(std::string iName)
{
  std::stringstream NameWithBrackets;
  NameWithBrackets << "<";
  NameWithBrackets << iName;
  NameWithBrackets << ">";
  return NameWithBrackets.str();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBExport::GetNameWithSlashBrackets(std::string iName)
{
  std::stringstream NameWithBrackets;
  NameWithBrackets << "</";
  NameWithBrackets << iName;
  NameWithBrackets << ">";
  return NameWithBrackets.str();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::AddTabulation()
{
  this->m_outfile << "  ";
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::OpenDBConnection()
{
  this->m_DatabaseConnector = OpenDatabaseConnection(m_ServerName,
    m_Login,m_Password,"gofiguredatabase");
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::CloseDBConnection()
{
  CloseDatabaseConnection(m_DatabaseConnector);
}
