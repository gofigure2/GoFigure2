/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

#include "CreateDataBaseHelper.h"
#include "QueryDataBaseHelper.h"
#include "vtkSQLQuery.h"
#include "vtkVariant.h"
#include <sstream>
#include <string>


//------------------------------------------------------------------------------
bool IsDatabaseOfGoFigureType(vtkMySQLDatabase * DatabaseConnector)
{
  return (  DoesTableExist( DatabaseConnector, "bookmark" )
    && DoesTableExist( DatabaseConnector, "contour" )
    && DoesTableExist( DatabaseConnector, "lineage" )
    && DoesTableExist( DatabaseConnector, "mesh" )
    && DoesTableExist( DatabaseConnector, "image" )
    && DoesTableExist( DatabaseConnector, "track" ) );

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DoesDatabaseExit(vtkMySQLDatabase * DataBaseConnector,std::string DBName)
{
  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  std::stringstream queryScript;
  queryScript << "SHOW DATABASES LIKE '";
  queryScript << DBName;
  queryScript << "';";

  query->SetQuery(queryScript.str().c_str());
  if (!query->Execute())
    {
    itkGenericExceptionMacro(
    << "Does database already exist query failed."
    << query->GetLastErrorText() );
    query->Delete();
    return true;
    }

  if( query->NextRow() )
    {
    query->Delete();
    return true;
    }

  query->Delete();
  return false;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateGoFigureDataBase(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName )
{
  /*if( ConnectToServer(ServerName, login, Password ) )
    {
    vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
    DataBaseConnector->SetHostName( ServerName.c_str() );
    DataBaseConnector->SetUser( login.c_str() );
    DataBaseConnector->SetPassword( Password.c_str() );

    if( !DataBaseConnector->Open() )
      {
      std::cerr << "Could not open Database Connector." << std::endl;
      std::cerr << "DB will not be created."  << std::endl;
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      DataBaseConnector->Delete();
      return;
      }*/
  std::pair<bool,vtkMySQLDatabase*> ConnectionServer = ConnectToServer(
    ServerName, login, Password );

  if (!ConnectionServer.first)
    {
    std::cout<<"Can not connect to the server"<<std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return;
    }

    vtkMySQLDatabase* ServerConnector = ConnectionServer.second;

  if (!DoesDatabaseExit(ServerConnector,DBName))
    {
    CreateDataBase(ServerConnector,DBName);
    ServerConnector->Close();
    ServerConnector->Delete();

    vtkMySQLDatabase * DataBaseConnector = ConnectToDatabase(ServerName, login,
      Password,DBName).second;
    CreateTables(DataBaseConnector);
    //CreateForeignKeys(DataBaseConnector);
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    }
   else
     {
     ServerConnector->Close();
     ServerConnector->Delete();
    }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateDataBase( vtkMySQLDatabase* DataBaseConnector, std::string DBName )
{
  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  std::stringstream insertQuery;
  insertQuery<< "CREATE DATABASE "<< DBName;
  query->SetQuery( insertQuery.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    query->Delete();
    return;
    }
  query->Delete();
 }
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateTables( vtkMySQLDatabase* DataBaseConnector )
{
  Query(DataBaseConnector,AuthorTable());
  Query(DataBaseConnector,BookmarkTable());
  Query(DataBaseConnector,CalculatedValueTable());
  Query(DataBaseConnector,CellTypeTable());
  Query(DataBaseConnector,ChannelTable());
  Query(DataBaseConnector,ColorTableScript());
  Query(DataBaseConnector,ContourTable());
  Query(DataBaseConnector,ContourValueTable());
  Query(DataBaseConnector,CoordinateTable());
  Query(DataBaseConnector,ImageTable());
  Query(DataBaseConnector,ImageValueTable());
  Query(DataBaseConnector,ImagingSessionTable());
  Query(DataBaseConnector,ImagingSessionValueTable());
  Query(DataBaseConnector,IntensityTable());
  Query(DataBaseConnector,LineageTable());
  Query(DataBaseConnector,LineageValueTable());
  Query(DataBaseConnector,MeshTable());
  Query(DataBaseConnector,MeshValueTable());
  Query(DataBaseConnector,MicroscopeTable());
  Query(DataBaseConnector,ProjectTable());
  Query(DataBaseConnector,SubCellularTypeTable());
  Query(DataBaseConnector,TrackFamilyTable());
  Query(DataBaseConnector,TrackTable());
  Query(DataBaseConnector,TrackValueTable());
  Query(DataBaseConnector,ValuePerVectorCoordTable());
  Query(DataBaseConnector,ValueTypeTable());
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CreateForeignKeys(vtkMySQLDatabase* DataBaseConnector)
{

  Query(DataBaseConnector,ProjectFK());
  Query(DataBaseConnector,ImagingSessionFKMicroscopeName());
  Query(DataBaseConnector,ImagingSessionFKProjectName());
 // Query(DataBaseConnector,ImagingSessionFKCoordIDMin());
 // Query(DataBaseConnector,ImagingSessionFKCoordIDMax());
  Query(DataBaseConnector,TrackFamilyFKTrackIDDaughter1());
  Query(DataBaseConnector,TrackFamilyFKTrackIDDaughter2());
  Query(DataBaseConnector,TrackFamilyFKTrackIDMother());
  Query(DataBaseConnector,TrackFKColor());
  Query(DataBaseConnector,TrackFKLineage());
  Query(DataBaseConnector,TrackFKCoordIDMax());
  Query(DataBaseConnector,TrackFKCoordIDMin());
  Query(DataBaseConnector,TrackFKTrackFamily());
  Query(DataBaseConnector,MeshFKImagingSession());
  Query(DataBaseConnector,MeshFKTrackID());
  Query(DataBaseConnector,MeshFKColor());
  Query(DataBaseConnector,MeshFKCoordIDMin());
  Query(DataBaseConnector,MeshFKCoordIDMax());
  Query(DataBaseConnector,MeshFKSubCellType());
  Query(DataBaseConnector,MeshFKCellType());
  Query(DataBaseConnector,ContourFKImagingSession());
  Query(DataBaseConnector,ContourFKCoordIDMin());
  Query(DataBaseConnector,ContourFKCoordIDMax());
  Query(DataBaseConnector,ContourFKMesh());
  Query(DataBaseConnector,ChannelFKColor());
  Query(DataBaseConnector,ChannelFKImagingSession());
  Query(DataBaseConnector,ImageFKChannel());
  Query(DataBaseConnector,ImageFKCoordIDMin());
  Query(DataBaseConnector,ImageFKImagingSession());
  Query(DataBaseConnector,LineageFKImagingSession());
  Query(DataBaseConnector,LineageFKTrackRoot());
  Query(DataBaseConnector,LineageFKColor());
  Query(DataBaseConnector,LineageFKCoordIDMin());
  Query(DataBaseConnector,LineageFKCoordIDMax());
  Query(DataBaseConnector,BookmarkFKCoord());
  Query(DataBaseConnector,BookmarkFKImagingSession());
  Query(DataBaseConnector,IntensityFKChannel());
  Query(DataBaseConnector,IntensityFKMesh());
  Query(DataBaseConnector,ValueperVectorCoordFKCalculatedValue());
  Query(DataBaseConnector,CalculatedValueFKValueType());
  Query(DataBaseConnector,MeshValueFKCalculatedValue());
  Query(DataBaseConnector,MeshValueFKMesh());
  Query(DataBaseConnector,TrackValueFKCalculatedValue());
  Query(DataBaseConnector,TrackValueFKTrack());
  Query(DataBaseConnector,ImageValueFKCalculatedValue());
  Query(DataBaseConnector,ImageValueFKImage());
  Query(DataBaseConnector,ImagingSessionValueFKCalculatedValue());
  Query(DataBaseConnector,ImagingSessionValueFKImagingSession());
  Query(DataBaseConnector,ContourValueFKCalculatedValue());
  Query(DataBaseConnector,ContourValueFKContour());
  Query(DataBaseConnector,LineageValueFKCalculatedValue());
  Query(DataBaseConnector,LineageValueFKLineage());
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void Query(vtkMySQLDatabase* DataBaseConnector,std::string queryScript)
{
  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  query->SetQuery( queryScript.c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    query->Delete();
    return;
    }
  query->Delete();

 }
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string CellTypeTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `celltype`(\
    `CellTypeID` INT NOT NULL AUTO_INCREMENT ,\
    `Name` TEXT NOT NULL ,\
    `Description` VARCHAR(1000) NULL ,\
    PRIMARY KEY (`CellTypeID`)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string AuthorTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `author` (\
    `AuthorID` INT NOT NULL AUTO_INCREMENT ,\
    `LastName` VARCHAR(45) NOT NULL ,\
    `FirstName` VARCHAR(45) NOT NULL ,\
    `MiddleName` VARCHAR(45) NOT NULL DEFAULT '<none>' ,\
    UNIQUE INDEX UniqueAuthor (`LastName`,`FirstName`,`MiddleName`),\
    PRIMARY KEY (`authorID`)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SubCellularTypeTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `subcellulartype` (\
    `SubCellularID` INT NOT NULL AUTO_INCREMENT ,\
    `Name` VARCHAR(45) NOT NULL ,\
    `Description` VARCHAR(1000) NULL ,\
    PRIMARY KEY (`SubCellularID`)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string CoordinateTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `coordinate` (\
    `CoordID` INT NOT NULL AUTO_INCREMENT ,\
    `PCoord` TINYINT UNSIGNED NOT NULL DEFAULT 0,\
    `RCoord` TINYINT UNSIGNED NOT NULL DEFAULT 0,\
    `CCoord` TINYINT UNSIGNED NOT NULL DEFAULT 0,\
    `XTileCoord` SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
    `YTileCoord` SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
    `ZTileCoord` SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
    `XCoord` FLOAT UNSIGNED NOT NULL DEFAULT 0,\
    `YCoord` FLOAT UNSIGNED NOT NULL DEFAULT 0,\
    `ZCoord` FLOAT UNSIGNED NOT NULL DEFAULT 0,\
    `TCoord` FLOAT UNSIGNED NOT NULL DEFAULT 0,\
    PRIMARY KEY (`CoordID`)\
    );";
}
  //if needed:
  // UNIQUE INDEX UniqueCoordinate (`PCoord`,`RCoord`,`CCoord`,`XTileCoord`,
  // `YTileCoord`,`ZTileCoord`,`XCoord`,`YCoord`,`ZCoord`,`TCoord`),
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ColorTableScript()
{
  return
    "CREATE  TABLE IF NOT EXISTS `color` (\
    `ColorID` INT NOT NULL AUTO_INCREMENT ,\
    `Name` VARCHAR(45) NULL ,\
    `Red` TINYINT UNSIGNED NOT NULL ,\
    `Green` TINYINT UNSIGNED NOT NULL ,\
    `Blue` TINYINT UNSIGNED NOT NULL ,\
    `Alpha` TINYINT UNSIGNED NOT NULL ,\
    `Description` VARCHAR(1000) NULL ,\
     UNIQUE INDEX UniqueColor (`Red`,`Green`,`Blue`,`Alpha`,`Name`),\
     PRIMARY KEY (`ColorID`)\
     );";
 }
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MicroscopeTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `microscope` (\
    `Name` VARCHAR(255) NOT NULL ,\
    PRIMARY KEY (`Name`)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ProjectTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `project` (\
    `Name` VARCHAR(255) NOT NULL ,\
    `Description` VARCHAR(1000) NULL ,\
    `AuthorID` INT NOT NULL ,\
    `CreationDate` DATE NOT NULL ,\
    `DatabaseVersion` VARCHAR(45) NOT NULL ,\
    PRIMARY KEY (`Name`) ,\
    INDEX `FK_Project_AuthorID` (`AuthorID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImagingSessionTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `imagingsession` (\
    `ImagingSessionID` INT NOT NULL AUTO_INCREMENT ,\
    `CoordIDMax` INT NOT NULL DEFAULT 0 ,\
    `CoordIDMin` INT NOT NULL DEFAULT 0,\
    `Name` VARCHAR(255) NOT NULL ,\
    `Description` VARCHAR(1000) NULL ,\
    `ImagesTimeInterval` FLOAT UNSIGNED NULL ,\
    `RealPixelDepth` FLOAT UNSIGNED NULL ,\
    `RealPixelHeight` FLOAT UNSIGNED NULL ,\
    `RealPixelWidth` FLOAT UNSIGNED NULL ,\
    `ProjectName` VARCHAR(255) NOT NULL ,\
    `MicroscopeName` VARCHAR(255) NOT NULL ,\
    `CreationDate` DATETIME NOT NULL ,\
    `XImageSize` INT NOT NULL,\
    `YImageSize` INT NOT NULL,\
    `XTileOverlap` FLOAT UNSIGNED DEFAULT 0,\
    `YTileOverlap` FLOAT UNSIGNED DEFAULT 0,\
    `ZTileOverlap` FLOAT UNSIGNED DEFAULT 0,\
    UNIQUE INDEX UniqueImagingSession (`MicroscopeName`,`CreationDate`),\
    PRIMARY KEY (`ImagingSessionID`) ,\
    INDEX `FK_ImagingSession_CoordIDMax` (`CoordIDMax` ASC) ,\
    INDEX `FK_ImagingSession_CoordIDMin` (`CoordIDMin` ASC) ,\
    INDEX `FK_ImagingSession_ProjectName` (`ProjectName` ASC) ,\
    INDEX `FK_ImagingSession_MicroscopeName` (`MicroscopeName` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackFamilyTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `trackfamily` (\
    `TrackFamilyID` INT NOT NULL AUTO_INCREMENT ,\
    `TrackIDMother` INT NOT NULL ,\
    `TrackIDDaughter1` INT NOT NULL ,\
    `TrackIDDaughter2` INT NOT NULL ,\
    PRIMARY KEY (`TrackFamilyID`) ,\
    INDEX `FK_TrackFamily_TrackIDMother` (`TrackIDMother` ASC) ,\
    INDEX `FK_TrackFamily_TrackIDDaughter1` (`TrackIDDaughter1` ASC) ,\
    INDEX `FK_TrackFamily_TrackIDDaughter2` (`TrackIDDaughter2` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `track` (\
    `TrackID` INT NOT NULL AUTO_INCREMENT ,\
    `LineageID` INT NULL ,\
    `ColorID` INT NOT NULL DEFAULT '2',\
    `CoordIDMax` INT NOT NULL ,\
    `CoordIDMin` INT NOT NULL ,\
    `TrackFamilyID` INT NULL ,\
    `Points` LONGTEXT NULL ,\
    `ImagingSessionID` INT NOT NULL ,\
    PRIMARY KEY (`TrackID`) ,\
    INDEX `FK_Track_ColorID` (`ColorID` ASC) ,\
    INDEX `FK_Track_LineageID` (`LineageID` ASC) ,\
    INDEX `FK_Track_CoordIDMax` (`CoordIDMax` ASC) ,\
    INDEX `FK_Track_CoordIDMin` (`CoordIDMin` ASC) ,\
    INDEX `FK_Track_TrackFamilyID` (`TrackFamilyID` ASC) ,\
    INDEX `FK_Track_ImagingSessionID` (`ImagingSessionID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MeshTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `mesh` (\
    `MeshID` INT NULL AUTO_INCREMENT ,\
    `CellTypeID` INT NOT NULL DEFAULT '1',\
    `SubCellularID` INT NOT NULL DEFAULT '1',\
    `CoordIDMax` INT NOT NULL ,\
    `CoordIDMin` INT NOT NULL ,\
    `ColorID` INT NOT NULL DEFAULT '1',\
    `TrackID` INT NULL ,\
    `ImagingSessionID` INT NOT NULL ,\
    `Points` LONGTEXT NULL ,\
    PRIMARY KEY (`MeshID`) ,\
    INDEX `FK_Mesh_CellTypeID` (`CellTypeID` ASC) ,\
    INDEX `FK_Mesh_CoordIDMax` (`CoordIDMax` ASC) ,\
    INDEX `FK_Mesh_CoordIDMin` (`CoordIDMin` ASC) ,\
    INDEX `FK_Mesh_ColorID` (`ColorID` ASC) ,\
    INDEX `FK_Mesh_TrackID` (`TrackID` ASC) ,\
    INDEX `FK_Mesh_ImagingSessionID` (`ImagingSessionID` ASC) ,\
    INDEX `FK_Mesh_SubCellularID` (`SubCellularID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ContourTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `contour` (\
    `ContourID` INT NOT NULL AUTO_INCREMENT ,\
    `MeshID` INT NULL ,\
    `ImagingSessionID` INT NULL ,\
    `ColorID` INT NULL ,\
    `CoordIDMax` INT NOT NULL ,\
    `CoordIDMin` INT NOT NULL ,\
    `Points` LONGTEXT NOT NULL ,\
    PRIMARY KEY (`ContourID`) ,\
    INDEX `FK_Contour_MeshID` (`MeshID` ASC) ,\
    INDEX `FK_Contour_CoordIDMax` (`CoordIDMax` ASC) ,\
    INDEX `FK_Contour_CoordIDMin` (`CoordIDMin` ASC) ,\
    INDEX `FK_Contour_ImagingSessionID` (`ImagingSessionID` ASC) \
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ChannelTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `channel` (\
    `ChannelID` INT NOT NULL AUTO_INCREMENT ,\
    `Name` VARCHAR(45) NULL ,\
    `ImagingSessionID` INT NOT NULL,\
    `ColorID` INT NOT NULL ,\
    `ChannelNumber` INT NOT NULL ,\
    `NumberOfBits` TINYINT UNSIGNED NOT NULL ,\
    UNIQUE INDEX UniqueChannel (`ImagingSessionID`,`ChannelNumber`),\
    PRIMARY KEY (`ChannelID`) ,\
    INDEX `FK_Channel_ColorID` (`ColorID` ASC), \
    INDEX `FK_Channel_ImagingSessionID`(`ImagingSessionID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImageTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `image` (\
    `ImageID` INT NOT NULL AUTO_INCREMENT ,\
    `ImagingSessionID` INT NOT NULL ,\
    `CoordIDMin` INT NOT NULL ,\
    `Filename` TEXT NOT NULL ,\
    `ChannelID` INT NOT NULL ,\
    PRIMARY KEY (`ImageID`) ,\
    INDEX `FK_Image_ImagingSessionID` (`ImagingSessionID` ASC) ,\
    INDEX `FK_Image_CoordIDMin` (`CoordIDMin` ASC) ,\
    INDEX `FK_Image_ChannelID` (`ChannelID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string LineageTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `lineage` (\
    `LineageID` INT NOT NULL AUTO_INCREMENT ,\
    `CoordIDMax` INT NOT NULL ,\
    `CoordIDMin` INT NOT NULL ,\
    `ColorID` INT NOT NULL DEFAULT '3',\
    `Points` LONGTEXT NOT NULL ,\
    `TrackIDRoot` INT NOT NULL ,\
    `ImagingSessionID` INT NOT NULL ,\
    PRIMARY KEY (`LineageID`) ,\
    INDEX `FK_Lineage_CoordIDMax` (`CoordIDMax` ASC) ,\
    INDEX `FK_Lineage_CoordIDMin` (`CoordIDMin` ASC) ,\
    INDEX `FK_Lineage_ColorID` (`ColorID` ASC) ,\
    INDEX `FK_Lineage_TrackIDRoot` (`TrackIDRoot` ASC) ,\
    INDEX `FK_Lineage_ImagingSessionID` (`ImagingSessionID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string BookmarkTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `bookmark` (\
    `BookmarkID` INT NOT NULL AUTO_INCREMENT ,\
    `ImagingSessionID` INT NOT NULL ,\
    `CoordID` INT NOT NULL ,\
    `Name` VARCHAR(45) NULL ,\
    `Description` VARCHAR(1000) NULL ,\
    `CreationDate` DATETIME NOT NULL,\
    PRIMARY KEY (`BookmarkID`) ,\
    INDEX `FK_Bookmark_ImagingSessionID` (`ImagingSessionID` ASC) ,\
    INDEX `FK_Bookmark_CoordID` (`CoordID` ASC) \
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string IntensityTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `intensity` (\
    `IntensityID` INT NOT NULL AUTO_INCREMENT ,\
    `Value` INT NOT NULL ,\
    `MeshID` INT NOT NULL ,\
    `ChannelID` INT NOT NULL ,\
    PRIMARY KEY (`IntensityID`) ,\
    INDEX `FK_Intensity_MeshID` (`MeshID` ASC) ,\
    INDEX `FK_Intensity_ChannelID` (`ChannelID` ASC) \
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ValueTypeTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `valuetype` (\
    `ValueTypeID` INT NOT NULL AUTO_INCREMENT ,\
    `Name` VARCHAR(45) NOT NULL ,\
    `Description` VARCHAR(1000) NULL ,\
    PRIMARY KEY (`ValueTypeID`) \
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string CalculatedValueTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `calculatedvalue` (\
    `CalculatedValueID` INT NOT NULL AUTO_INCREMENT ,\
    `ValueTypeID` INT NOT NULL ,\
    PRIMARY KEY (`CalculatedValueID`) ,\
    INDEX `FK_CalculatedValue_ValueTypeID` (`ValueTypeID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ValuePerVectorCoordTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `valuepervectorcoord` (\
    `ValuePerVectorCoordID` INT NOT NULL AUTO_INCREMENT ,\
    `Name` VARCHAR(45) NULL ,\
    `VectorCoordNumber` INT NOT NULL ,\
    `Value` DECIMAL(3) NOT NULL ,\
    `CalculatedValueID` INT NOT NULL ,\
    PRIMARY KEY (`ValuePerVectorCoordID`) ,\
    INDEX `FK_ValuePerVectorCoord_CalculatedValueID` (`CalculatedValueID` ASC) \
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MeshValueTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `meshvalue` (\
    `CalculatedValueID` INT NOT NULL ,\
    `MeshID` INT NOT NULL ,\
    PRIMARY KEY (`CalculatedValueID`, `MeshID`),\
    INDEX `FK_MeshValue_MeshID` (`MeshID` ASC) ,\
    INDEX `FK_MeshValue_CalculatedValueID` (`CalculatedValueID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackValueTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `trackvalue` (\
    `TrackID` INT NOT NULL ,\
    `CalculatedValueID` INT NOT NULL ,\
    PRIMARY KEY (`TrackID`, `CalculatedValueID`) ,\
    INDEX `FK_TrackValue_TrackID` (`TrackID` ASC) ,\
    INDEX `CalculatedValueID` (`CalculatedValueID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImageValueTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `imagevalue` (\
    `ImageID` INT NOT NULL ,\
    `CalculatedValueID` INT NOT NULL ,\
    PRIMARY KEY (`ImageID`, `CalculatedValueID`),\
    INDEX `FK_ImageValue_ImageID` (`ImageID` ASC) ,\
    INDEX `FK_ImageValue_CalculatedValueID` (`CalculatedValueID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImagingSessionValueTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `imagingsessionvalue` (\
    `ImagingSessionID` INT NOT NULL ,\
    `CalculatedValueID` INT NOT NULL ,\
    PRIMARY KEY (`ImagingSessionID`, `CalculatedValueID`) ,\
    INDEX `ImagingSessionID` (`ImagingSessionID` ASC) ,\
    INDEX `CalculatedValueID` (`CalculatedValueID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ContourValueTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `contourvalue` (\
    `ContourID` INT NOT NULL ,\
    `CalculatedValueID` INT NOT NULL ,\
    PRIMARY KEY (`ContourID`, `CalculatedValueID`),\
    INDEX `FK_ContourValue_ContourID` (`ContourID` ASC) ,\
    INDEX `FK_ContourValue_CalculatedValueID` (`CalculatedValueID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string LineageValueTable()
{
  return
    "CREATE  TABLE IF NOT EXISTS `lineagevalue` (\
    `LineageID` INT NOT NULL ,\
    `CalculatedValueID` INT NOT NULL ,\
    PRIMARY KEY (`LineageID`, `CalculatedValueID`),\
    INDEX `FK_LineageValue_LineageID` (`LineageID` ASC) ,\
    INDEX `FK_LineageValue_CalculatedValueID` (`CalculatedValueID` ASC)\
    );";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ProjectFK()
{
  return
    "ALTER TABLE `project` ADD\
     CONSTRAINT `FK_Project_AuthorID`\
     FOREIGN    KEY (`AuthorID`)\
     REFERENCES `author`(`AuthorID`)\
     ON DELETE NO ACTION\
     ON UPDATE NO ACTION\
     ;";
 }
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImagingSessionFKCoordIDMax()
{
  return
    "ALTER TABLE `imagingsession` ADD\
     CONSTRAINT `FK_ImagingSession_CoordIDMax`\
     FOREIGN KEY (`CoordIDMax`)\
     REFERENCES `coordinate`(`CoordID`)\
     ON DELETE NO ACTION\
     ON UPDATE NO ACTION\
     ;";
 }
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImagingSessionFKCoordIDMin()
{
  return
    "ALTER TABLE `imagingsession` ADD\
     CONSTRAINT `FK_ImagingSession_CoordIDMin`\
     FOREIGN KEY (`CoordIDMin`)\
     REFERENCES `coordinate`(`CoordID`)\
     ON DELETE NO ACTION\
     ON UPDATE NO ACTION\
     ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImagingSessionFKProjectName()
{
  return
     "ALTER TABLE `imagingsession` ADD\
      CONSTRAINT `FK_ImagingSession_ProjectName`\
      FOREIGN KEY (`ProjectName`)\
      REFERENCES `project`(`Name`)\
      ON DELETE NO ACTION\
      ON UPDATE NO ACTION\
      ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImagingSessionFKMicroscopeName()
{
  return
     "ALTER TABLE `imagingsession` ADD\
      CONSTRAINT `FK_ImagingSession_MicroscopeName`\
      FOREIGN KEY (`MicroscopeName`)\
      REFERENCES `microscope`(`Name`)\
      ON DELETE NO ACTION\
      ON UPDATE NO ACTION\
      ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackFamilyFKTrackIDMother()
{
  return
     "ALTER TABLE `trackfamily` ADD\
      CONSTRAINT `FK_TrackFamily_TrackIDMother`\
      FOREIGN KEY (`TrackIDMother`)\
      REFERENCES `track`(`TrackID`)\
      ON DELETE NO ACTION\
      ON UPDATE NO ACTION\
      ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackFamilyFKTrackIDDaughter1()
{
  return
     "ALTER TABLE `trackfamily` ADD\
      CONSTRAINT `FK_TrackFamily_TrackIDDaughter1`\
      FOREIGN KEY (`TrackIDDaughter1`)\
      REFERENCES `track`(`TrackID`)\
      ON DELETE NO ACTION\
      ON UPDATE NO ACTION\
      ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackFamilyFKTrackIDDaughter2()
{
  return
     "ALTER TABLE `trackfamily` ADD\
      CONSTRAINT `FK_TrackFamily_TrackIDDaughter2`\
      FOREIGN KEY (`TrackIDDaughter2`)\
      REFERENCES `track`(`TrackID`)\
      ON DELETE NO ACTION\
      ON UPDATE NO ACTION\
      ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackFKColor()
{
  return
   "ALTER TABLE `track` ADD\
    CONSTRAINT `FK_Track_ColorID`\
    FOREIGN KEY (`ColorID`)\
    REFERENCES `color`(`ColorID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackFKLineage()
{
  return
   "ALTER TABLE `track` ADD\
    CONSTRAINT `FK_Track_LineageID`\
    FOREIGN KEY (`LineageID`)\
    REFERENCES `lineage`(`LineageID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackFKCoordIDMax()
{
  return
   "ALTER TABLE `track` ADD\
    CONSTRAINT `FK_Track_CoordIDMax`\
    FOREIGN KEY (`CoordIDMax`)\
    REFERENCES `coordinate`(`CoordID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackFKCoordIDMin()
{
  return
   "ALTER TABLE `track` ADD\
    CONSTRAINT `FK_Track_CoordIDMin`\
    FOREIGN KEY (`CoordIDMin`)\
    REFERENCES `coordinate`(`CoordID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackFKTrackFamily()
{
  return
   "ALTER TABLE `track` ADD\
    CONSTRAINT `FK_Track_TrackFamilyID`\
    FOREIGN KEY (`TrackFamilyID`)\
    REFERENCES `trackfamily`(`TrackFamilyID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackFKImagingSession()
{
  return
   "ALTER TABLE `track` ADD\
    CONSTRAINT `FK_Track_ImagingSessionID`\
    FOREIGN KEY (`ImagingSessionID`)\
    REFERENCES `imagingsession`(`ImagingSessionID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MeshFKCellType()
{
  return
   "ALTER TABLE `mesh` ADD\
    CONSTRAINT `FK_Mesh_CellTypeID`\
    FOREIGN KEY (`CellTypeID`)\
    REFERENCES `celltype`(`CellTypeID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MeshFKSubCellType()
{
  return
   "ALTER TABLE `mesh` ADD\
    CONSTRAINT `FK_Mesh_SubCellularID`\
    FOREIGN KEY (`SubCellularID`)\
    REFERENCES `subcellulartype`(`SubCellularID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MeshFKCoordIDMax()
{
  return
   "ALTER TABLE `mesh` ADD\
    CONSTRAINT `FK_Mesh_CoordIDMax`\
    FOREIGN KEY (`CoordIDMax`)\
    REFERENCES `coordinate`(`CoordID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MeshFKCoordIDMin()
{
  return
   "ALTER TABLE `mesh` ADD\
    CONSTRAINT `FK_Mesh_CoordIDMin`\
    FOREIGN KEY (`CoordIDMin`)\
    REFERENCES `coordinate`(`CoordID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MeshFKColor()
{
  return
   "ALTER TABLE `mesh` ADD\
    CONSTRAINT `FK_Mesh_ColorID`\
    FOREIGN KEY (`ColorID`)\
    REFERENCES `color`(`ColorID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MeshFKTrackID()
{
  return
   "ALTER TABLE `mesh` ADD\
    CONSTRAINT `FK_Mesh_TrackID`\
    FOREIGN KEY (`TrackID`)\
    REFERENCES `track`(`TrackID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MeshFKImagingSession()
{
  return
   "ALTER TABLE `mesh` ADD\
    CONSTRAINT `FK_Mesh_ImagingSessionID`\
    FOREIGN KEY (`ImagingSessionID`)\
    REFERENCES `imagingSession`(`ImagingSessionID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ContourFKMesh()
{
  return
   "ALTER TABLE `contour` ADD\
    CONSTRAINT `FK_Contour_MeshID`\
    FOREIGN KEY (`MeshID`)\
    REFERENCES `mesh`(`MeshID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ContourFKCoordIDMax()
{
  return
   "ALTER TABLE `contour` ADD\
    CONSTRAINT `FK_Contour_CoordIDMax`\
    FOREIGN KEY (`CoordIDMax`)\
    REFERENCES `coordinate`(`CoordID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ContourFKCoordIDMin()
{
  return
   "ALTER TABLE `contour` ADD\
    CONSTRAINT `FK_Contour_CoordIDMin`\
    FOREIGN KEY (`CoordIDMin`)\
    REFERENCES `coordinate`(`CoordID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ContourFKImagingSession()
{
  return
   "ALTER TABLE `contour` ADD\
    CONSTRAINT `FK_Contour_ImagingSessionID`\
    FOREIGN KEY (`ImagingSessionID`)\
    REFERENCES `imagingsession`(`ImagingSessionID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ChannelFKColor()
{
  return
   "ALTER TABLE `channel` ADD\
    CONSTRAINT `FK_Channel_ColorID`\
    FOREIGN KEY (`ColorID`)\
    REFERENCES `color`(`ColorID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ChannelFKImagingSession()
{
  return
   "ALTER TABLE `channel` ADD\
    CONSTRAINT `FK_Channel_ImagingSessionID`\
    FOREIGN KEY (`ImagingSessionID`)\
    REFERENCES `imagingsession`(`ImagingSessionID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImageFKImagingSession()
{
  return
   "ALTER TABLE `image` ADD\
    CONSTRAINT `FK_Image_ImagingSessionID`\
    FOREIGN KEY (`ImagingSessionID`)\
    REFERENCES `imagingsession`(`ImagingSessionID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImageFKCoordIDMin()
{
  return
   "ALTER TABLE `image` ADD\
    CONSTRAINT `FK_Image_CoordIDMin`\
    FOREIGN KEY (`CoordIDMin`)\
    REFERENCES `coordinate`(`CoordID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImageFKChannel()
{
  return
   "ALTER TABLE `image` ADD\
    CONSTRAINT `FK_Image_ChannelID`\
    FOREIGN KEY (`ChannelID`)\
    REFERENCES `channel`(`ChannelID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string LineageFKCoordIDMax()
{
  return
   "ALTER TABLE `lineage` ADD\
    CONSTRAINT `FK_Lineage_CoordIDMax`\
    FOREIGN KEY (`CoordIDMax`)\
    REFERENCES `coordinate`(`CoordID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string LineageFKCoordIDMin()
{
  return
   "ALTER TABLE `lineage` ADD\
    CONSTRAINT `FK_Lineage_CoordIDMin`\
    FOREIGN KEY (`CoordIDMin`)\
    REFERENCES `coordinate`(`CoordID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string LineageFKColor()
{
  return
   "ALTER TABLE `lineage` ADD\
    CONSTRAINT `FK_Lineage_ColorID`\
    FOREIGN KEY (`ColorID`)\
    REFERENCES `color`(`ColorID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string LineageFKTrackRoot()
{
  return
   "ALTER TABLE `lineage` ADD\
    CONSTRAINT `FK_Lineage_TrackIDRoot`\
    FOREIGN KEY (`TrackIDRoot`)\
    REFERENCES `track`(`TrackID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string LineageFKImagingSession()
{
  return
   "ALTER TABLE `lineage` ADD\
    CONSTRAINT `FK_Lineage_ImagingSessionID`\
    FOREIGN KEY (`ImagingSessionID`)\
    REFERENCES `imagingsession`(`ImagingSessionID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string BookmarkFKImagingSession()
{
  return
   "ALTER TABLE `bookmark` ADD\
    CONSTRAINT `FK_Bookmark_ImagingSessionID`\
    FOREIGN KEY (`ImagingSessionID`)\
    REFERENCES `imagingsession`(`ImagingSessionID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string BookmarkFKCoord()
{
  return
   "ALTER TABLE `bookmark` ADD\
    CONSTRAINT `FK_Bookmark_CoordID`\
    FOREIGN KEY (`CoordID`)\
    REFERENCES `coordinate`(`CoordID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string IntensityFKMesh()
{
  return
   "ALTER TABLE `intensity` ADD\
    CONSTRAINT `FK_Intensity_MeshID`\
    FOREIGN KEY (`MeshID`)\
    REFERENCES `mesh`(`MeshID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string IntensityFKChannel()
{
  return
   "ALTER TABLE `intensity` ADD\
    CONSTRAINT `FK_Intensity_ChannelID`\
    FOREIGN KEY (`ChannelID`)\
    REFERENCES `channel`(`ChannelID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string CalculatedValueFKValueType()
{
  return
   "ALTER TABLE `calculatedvalue` ADD\
    CONSTRAINT `FK_CalculatedValue_ValueTypeID`\
    FOREIGN KEY (`ValueTypeID`)\
    REFERENCES `valuetype`(`ValueTypeID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ValueperVectorCoordFKCalculatedValue()
{
  return
   "ALTER TABLE `valuepervectorcoord` ADD\
    CONSTRAINT `FK_ValuePerVectorCoord_CalculatedValueID`\
    FOREIGN KEY (`CalculatedValueID`)\
    REFERENCES `calculatedvalue`(`CalculatedValueID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MeshValueFKMesh()
{
  return
   "ALTER TABLE `meshvalue` ADD\
    CONSTRAINT `FK_MeshValue_MeshID`\
    FOREIGN KEY (`MeshID`)\
    REFERENCES `mesh`(`MeshID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string MeshValueFKCalculatedValue()
{
  return
   "ALTER TABLE `meshvalue` ADD\
    CONSTRAINT `FK_MeshValue_CalculatedValueID`\
    FOREIGN KEY (`CalculatedValueID`)\
    REFERENCES `calculatedvalue`(`CalculatedValueID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackValueFKTrack()
{
  return
   "ALTER TABLE `trackvalue` ADD\
    CONSTRAINT `FK_TrackValue_TrackID`\
    FOREIGN KEY (`TrackID`)\
    REFERENCES `track`(`TrackID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string TrackValueFKCalculatedValue()
{
  return
   "ALTER TABLE `trackvalue` ADD\
    CONSTRAINT `FK_TrackValue_CalculatedValueID`\
    FOREIGN KEY (`CalculatedValueID`)\
    REFERENCES `calculatedvalue`(`CalculatedValueID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImageValueFKImage()
{
  return
   "ALTER TABLE `imagevalue` ADD\
    CONSTRAINT `FK_ImageValue_ImageID`\
    FOREIGN KEY (`ImageID`)\
    REFERENCES `image`(`ImageID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImageValueFKCalculatedValue()
{
  return
    "ALTER TABLE `imagevalue` ADD\
    CONSTRAINT `FK_ImageValue_CalculatedValueID`\
    FOREIGN KEY (`CalculatedValueID`)\
    REFERENCES `calculatedvalue`(`CalculatedValueID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImagingSessionValueFKImagingSession()
{
  return
   "ALTER TABLE `imagingsessionvalue` ADD\
    CONSTRAINT `FK_ImagingSessionValue_ImagingSessionID`\
    FOREIGN KEY (`ImagingSessionID`)\
    REFERENCES `imagingsession`(`ImagingSessionID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ImagingSessionValueFKCalculatedValue()
{
  return
   "ALTER TABLE `imagingsessionvalue` ADD\
    CONSTRAINT `FK_ImagingSessionValue_CalculatedValueID`\
    FOREIGN KEY (`CalculatedValueID`)\
    REFERENCES `calculatedvalue`(`CalculatedValueID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ContourValueFKContour()
{
  return
   "ALTER TABLE `contourvalue` ADD\
    CONSTRAINT `FK_ContourValue_ContourID`\
    FOREIGN KEY (`ContourID`)\
    REFERENCES `contour`(`ContourID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ContourValueFKCalculatedValue()
{
  return
   "ALTER TABLE `contourvalue` ADD\
    CONSTRAINT `FK_ContourValue_CalculatedValueID`\
    FOREIGN KEY (`CalculatedValueID`)\
    REFERENCES `calculatedvalue`(`CalculatedValueID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string LineageValueFKLineage()
{
  return
   "ALTER TABLE `lineagevalue` ADD\
    CONSTRAINT `FK_LineageValue_LineageID`\
    FOREIGN KEY (`LineageID`)\
    REFERENCES `lineage`(`LineageID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string LineageValueFKCalculatedValue()
{
  return
   "ALTER TABLE `lineagevalue` ADD\
    CONSTRAINT `FK_LineageValue_CalculatedValueID`\
    FOREIGN KEY (`CalculatedValueID`)\
    REFERENCES `calculatedvalue`(`CalculatedValueID`)\
    ON DELETE NO ACTION\
    ON UPDATE NO ACTION\
    ;";
}
