/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 542 $  // Revision of last commit
  Date: $Date: 2009-08-06 16:08:10 -0400 (Thu, 06 Aug 2009) $  // Date of last commit
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

#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStdString.h"
#include "vtkVariant.h"
#include <sstream>
#include <string>


//------------------------------------------------------------------------------
bool IsDatabaseOfGoFigureType(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName )
{
  if(  DoesTableExist( ServerName, login, Password, DBName, "bookmarks" )
    && DoesTableExist( ServerName, login, Password, DBName, "figure" )
    && DoesTableExist( ServerName, login, Password, DBName, "lineage" )
    && DoesTableExist( ServerName, login, Password, DBName, "mesh" )
    && DoesTableExist( ServerName, login, Password, DBName, "seriesgrid" )
    && DoesTableExist( ServerName, login, Password, DBName, "track" ) )
    {
    return true;
    }
  return false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateDataBaseMain(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName )
{
  if( CanConnectToServer(    ServerName, login, Password ) )
    {
    CreateDataBase(            ServerName, login, Password, DBName );
    CreateBookmarksTable(      ServerName, login, Password, DBName );
    CreateCollectionInfoTable( ServerName, login, Password, DBName );
    CreateExperimentTable(     ServerName, login, Password, DBName );
    CreateFigureTable(         ServerName, login, Password, DBName );
    CreateFigureFlavorTable(   ServerName, login, Password, DBName );
    CreateLineageTable(        ServerName, login, Password, DBName );
    CreateLineageFlavorTable(  ServerName, login, Password, DBName );
    CreateMeshTable(           ServerName, login, Password, DBName );
    CreateMeshFlavor(          ServerName, login, Password, DBName );
    CreateSeriesGridTable(     ServerName, login, Password, DBName );
    CreateTrackTable(          ServerName, login, Password, DBName );
    CreateTrackFlavor(         ServerName, login, Password, DBName );
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateDataBase(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  std::ostringstream insertQuery;
  insertQuery<< "CREATE DATABASE "<< DBName;
  query->SetQuery( insertQuery.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateBookmarksTable(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery =
    "CREATE TABLE bookmarks ( \
    `BookmarkID` INTEGER NOT NULL  AUTO_INCREMENT, \
    `BookmarkName` varchar (50), \
    `BookmarkDesc` varchar (50), \
    `experimentID` INTEGER NOT NULL  DEFAULT -1, \
    `RCoord` INTEGER NOT NULL  DEFAULT -1, \
    `CCoord` INTEGER NOT NULL  DEFAULT -1, \
    `TCoord` INTEGER NOT NULL  DEFAULT -1, \
    `YCoord` INTEGER NOT NULL  DEFAULT -1, \
    `XCoord` INTEGER NOT NULL  DEFAULT -1, \
    `ZCoord` INTEGER NOT NULL  DEFAULT -1, \
    `DateCreated` datetime, \
    PRIMARY KEY (BookmarkID) \
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateCollectionInfoTable(
  std::string ServerName,
  std::string login,
  std::string Password,
  std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery =
    "CREATE TABLE `collectioninfo` \
    (\
    `CollectionInfoID` INTEGER (11) NOT NULL  AUTO_INCREMENT , \
    `AuthorName` varchar (50), \
    `CreationDate` varchar (50), \
    `CollectionName` varchar (50), \
    `CollectionDescription` varchar (50), \
    `GoFigureVersion` DOUBLE DEFAULT 0, \
    `GoFigureProductID` varchar (50),\
    PRIMARY KEY (CollectionInfoID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateExperimentTable(
  std::string ServerName,
  std::string login,
  std::string Password,
  std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery =
    "CREATE TABLE `experiment` \
    (\
    `experimentID` INTEGER NOT NULL  AUTO_INCREMENT , \
    `name` varchar (50), \
    `description` text, \
    `timeInterval` INTEGER NOT NULL  DEFAULT 0, \
    `tileHeight` INTEGER NOT NULL  DEFAULT 1024, \
    `tileWidth` INTEGER NOT NULL  DEFAULT 1024, \
    `pixelDepth` DOUBLE DEFAULT 0, \
    `pixelHeight` DOUBLE DEFAULT 0, \
    `pixelWidth` DOUBLE DEFAULT 0, \
    `colorDepth` INTEGER DEFAULT 8, \
    `nTimePoints` INTEGER DEFAULT -1, \
    `nYTiles` INTEGER DEFAULT -1, \
    `nXTiles` INTEGER DEFAULT -1, \
    `nSlices` INTEGER NOT NULL  DEFAULT -1, \
    `nRows` INTEGER DEFAULT -1, \
    `nColumns` INTEGER DEFAULT -1, \
    `filePattern` varchar (250),\
    PRIMARY KEY (experimentID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateFigureTable(
  std::string ServerName,
  std::string login,
  std::string Password,
  std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery =
  "CREATE TABLE `figure` \
    (\
    `figureID` INTEGER NOT NULL  NOT NULL  AUTO_INCREMENT , \
    `imageID` INTEGER NOT NULL  DEFAULT 0, \
    `meshID` INTEGER NOT NULL  DEFAULT 0, \
    `points` TEXT, \
    `cellTypeID` INTEGER NOT NULL  DEFAULT 0, \
    `flavorID` INTEGER NOT NULL  DEFAULT 0, \
    `meanRed` SMALLINT DEFAULT 0, \
    `meanGreen` SMALLINT DEFAULT 0, \
    `meanBlue` SMALLINT DEFAULT 0, \
    `area` INTEGER NOT NULL  DEFAULT 0, \
    `perimeter` INTEGER NOT NULL  DEFAULT 0, \
    `AP` INTEGER NOT NULL  DEFAULT 0, \
    `score` INTEGER NOT NULL  DEFAULT 0, \
    `xCenter` INTEGER NOT NULL  DEFAULT 0, \
    `yCenter` INTEGER NOT NULL  DEFAULT 0,\
    `experimentID` INTEGER NOT NULL  DEFAULT -1, \
    `RCoord` INTEGER NOT NULL  DEFAULT -1, \
    `CCoord` INTEGER NOT NULL  DEFAULT -1, \
    `TCoord` INTEGER NOT NULL  DEFAULT -1, \
    `YCoord` INTEGER NOT NULL  DEFAULT -1, \
    `XCoord` INTEGER NOT NULL  DEFAULT -1, \
    `ZCoord` INTEGER NOT NULL  DEFAULT -1, \
    PRIMARY KEY (figureID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateFigureFlavorTable(
  std::string ServerName,
  std::string login,
  std::string Password,
  std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery = "\
    CREATE TABLE `figureflavor` \
    (\
    `flavorID` INTEGER NOT NULL  AUTO_INCREMENT , \
    `FlavorName` varchar (50), \
    `Colorref` INTEGER NOT NULL  DEFAULT 0, \
    `ShowInCombo` tinyint (4),\
    PRIMARY KEY (flavorID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateLineageTable(
  std::string ServerName,
  std::string login,
  std::string Password,
  std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery = "\
    CREATE TABLE `lineage` \
    (\
    `lineageID` INTEGER NOT NULL  NOT NULL  AUTO_INCREMENT , \
    `tEnd` INTEGER NOT NULL  DEFAULT -1, \
    `tStart` INTEGER NOT NULL  DEFAULT -1,\
    `xStart` INTEGER NOT NULL  DEFAULT -1, \
    `yStart` INTEGER NOT NULL  DEFAULT -1, \
    `zStart` INTEGER NOT NULL  DEFAULT -1, \
    `points` blob, \
    `xEnd` INTEGER NOT NULL  DEFAULT -1, \
    `yEnd` INTEGER NOT NULL  DEFAULT -1, \
    `zEnd` INTEGER NOT NULL  DEFAULT -1, \
    `cellTypeID` INTEGER NOT NULL  DEFAULT 0, \
    `experimentID` INTEGER NOT NULL  DEFAULT -1, \
    `RCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `CCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `TCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `YCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `XCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `ZCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `RCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `CCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `TCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `YCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `XCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `ZCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `flavorLineageID` INTEGER NOT NULL  DEFAULT -1,\
    PRIMARY KEY (lineageID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateLineageFlavorTable(
  std::string ServerName,
  std::string login,
  std::string Password,
  std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery = "\
    CREATE TABLE `lineageflavor` \
    (\
    `flavorID` INTEGER NOT NULL  NOT NULL  AUTO_INCREMENT , \
    `FlavorName` varchar (50), \
    `Colorref` INTEGER NOT NULL  DEFAULT 0, \
    `ShowInCombo` tinyint (4),\
    PRIMARY KEY (flavorID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateMeshTable(
  std::string ServerName,
  std::string login,
  std::string Password,
  std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery = "\
    CREATE TABLE `mesh` \
    (\
    `meshID` INTEGER NOT NULL  AUTO_INCREMENT , \
    `flavorMeshID` INTEGER NOT NULL  DEFAULT 0, \
    `cellTypeID` INTEGER NOT NULL  DEFAULT 0, \
    `width` SMALLINT DEFAULT 0, \
    `height` SMALLINT DEFAULT 0, \
    `depth` SMALLINT DEFAULT 0, \
    `xCenter` INTEGER DEFAULT 0, \
    `yCenter` INTEGER DEFAULT 0, \
    `zCenter` INTEGER DEFAULT 0, \
    `trackID` bigint DEFAULT 0, \
    `meanBlue` SMALLINT DEFAULT 0, \
    `meanGreen` SMALLINT DEFAULT 0, \
    `meanRed` SMALLINT DEFAULT 0, \
    `score` INTEGER DEFAULT 0, \
    `surfaceArea` INTEGER DEFAULT 0, \
    `volume` INTEGER DEFAULT 0,\
    `experimentID` INTEGER NOT NULL  DEFAULT -1, \
    `RCoord` INTEGER NOT NULL  DEFAULT -1, \
    `CCoord` INTEGER NOT NULL  DEFAULT -1, \
    `TCoord` INTEGER NOT NULL  DEFAULT -1, \
    `YCoord` INTEGER NOT NULL  DEFAULT -1, \
    `XCoord` INTEGER NOT NULL  DEFAULT -1, \
    `ZCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `ZCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `points` TEXT,\
    PRIMARY KEY (meshID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateMeshFlavor(
  std::string ServerName,
  std::string login,
  std::string Password,
  std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery = "\
    CREATE TABLE `meshflavor` \
    (\
    `flavorID` INTEGER NOT NULL  NOT NULL  AUTO_INCREMENT , \
    `FlavorName` varchar (50), \
    `Colorref` INTEGER NOT NULL  DEFAULT 0, \
    `ShowInCombo` tinyint (4),\
    PRIMARY KEY (flavorID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateSeriesGridTable(
  std::string ServerName,
  std::string login,
  std::string Password,
  std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery = "\
    CREATE TABLE `seriesgrid` \
    (\
    `imageID` INTEGER NOT NULL  NOT NULL  AUTO_INCREMENT , \
    `experimentID` INTEGER NOT NULL  DEFAULT -1, \
    `RCoord` INTEGER NOT NULL  DEFAULT -1, \
    `CCoord` INTEGER NOT NULL  DEFAULT -1, \
    `TCoord` INTEGER NOT NULL  DEFAULT -1, \
    `YCoord` INTEGER NOT NULL  DEFAULT -1, \
    `XCoord` INTEGER NOT NULL  DEFAULT -1, \
    `ZCoord` INTEGER NOT NULL  DEFAULT -1, \
    `filename` varchar (250),\
    PRIMARY KEY (imageID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateTrackTable(
  std::string ServerName,
  std::string login,
  std::string Password,
  std::string DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery = "\
    CREATE TABLE `track` \
    (\
    `trackID` INTEGER NOT NULL  NOT NULL  AUTO_INCREMENT , \
    `lineageID` INTEGER NOT NULL  DEFAULT 0, \
    `flavorTrackID` INTEGER NOT NULL  DEFAULT 0, \
    `tEnd` INTEGER NOT NULL  DEFAULT 0, \
    `tStart` INTEGER NOT NULL  DEFAULT 0, \
    `xEnd` INTEGER DEFAULT 0, \
    `xStart` INTEGER DEFAULT 0, \
    `yEnd` INTEGER DEFAULT 0, \
    `yStart` INTEGER DEFAULT 0, \
    `zStart` INTEGER DEFAULT 0, \
    `zEnd` INTEGER DEFAULT 0, \
    `score` INTEGER DEFAULT 0, \
    `distance` INTEGER DEFAULT 0, \
    `directionTheta` INTEGER DEFAULT 0, \
    `directionPhi` INTEGER DEFAULT 0, \
    `displacement` INTEGER DEFAULT 0, \
    `speed` INTEGER DEFAULT 0, \
    `cellTypeID` INTEGER NOT NULL  DEFAULT 0, \
    `experimentID` INTEGER NOT NULL  DEFAULT -1, \
    `RCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `CCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `TCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `YCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `XCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `ZCoordMin` INTEGER NOT NULL  DEFAULT -1, \
    `RCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `CCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `TCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `YCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `XCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `ZCoordMax` INTEGER NOT NULL  DEFAULT -1, \
    `points` blob,\
    PRIMARY KEY (trackID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateTrackFlavor(
  std::string ServerName,
  std::string login,
  std::string Password,
  std::string DBName )
{
  (void) ServerName;
  (void) login;
  (void) Password;
  (void) DBName;

  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName(ServerName.c_str());
  DataBaseConnector->SetUser(login.c_str());
  DataBaseConnector->SetPassword(Password.c_str());
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery = "\
    CREATE TABLE `trackflavor` \
    (\
    `flavorID` INTEGER NOT NULL  NOT NULL  AUTO_INCREMENT , \
    `FlavorName` varchar (50), \
    `Colorref` INTEGER NOT NULL  DEFAULT 0, \
    `ShowInCombo` tinyint (4),\
    PRIMARY KEY (flavorID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Create query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
