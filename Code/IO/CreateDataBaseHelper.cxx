#include "CreateDataBaseHelper.h"

#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStdString.h"
#include "vtkVariant.h"

// NOTE ALEX
//
// use exception for the following problems:
// could not open database
// database with same name already exists.

bool CanConnectToServer(
  const char* ServerName, const char* login,
  const char* Password)
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->Open("");
  return DataBaseConnector->IsOpen();
};

std::vector<std::string> ListDataBases(
  const char* ServerName, const char* login,
  const char* Password)
{
  std::vector< std::string > result;

  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    return result;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  query->SetQuery( "Show Databases;" );
  if ( !query->Execute() )
    {
    cerr << "Show Databases query failed" << endl;
    DataBaseConnector->Close();
    return result;
    }

  // all set, proceed

  // iterate over lines, we know there is only one column
  // and as many rows as there is databases
  while( query->NextRow() )
    {
    result.push_back( query->DataValue( 0 ).ToString() );
    }

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();

  return result;
};

std::vector<std::string> ListTables(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName )
{
  std::vector< std::string > result;

  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    return result;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  query->SetQuery( "Show tables;" );
  if ( !query->Execute() )
    {
    cerr << "Show tables query failed" << endl;
    DataBaseConnector->Close();
    return result;
    }

  // all set, proceed

  // iterate over lines, we know there is only one column
  // and as many rows as there is databases
  while( query->NextRow() )
    {
    result.push_back( query->DataValue( 0 ).ToString() );
    }

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();

  return result;
};

void DropDatabase( 
  const char* ServerName, const char* login,
  const char* Password, const char* DBName )
{
  // if( CanOpenDatabase && DoesDataBaseExist )
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  char insertQuery[200];
  sprintf( insertQuery, "DROP DATABASE %s", DBName );
  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    cerr << "Drop query failed" << endl;
    DataBaseConnector->Close();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
};

void DropTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName,
  const char* TableName )
{
  // if( CanOpenDatabase && DoesDataBaseExist && DoesTableExist )
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  char insertQuery[200];
  sprintf( insertQuery, "DROP TABLE %s", TableName );
  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    cerr << "Drop query failed" << endl;
    DataBaseConnector->Close();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
};

bool DoesDataBaseExist(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName )
{
  std::vector< std::string > list;
  list = ListDataBases( ServerName, login, Password );
  std::string myString( DBName );
  std::vector< std::string >::iterator start = list.begin();
  std::vector< std::string >::iterator end   = list.end();
  while( start != end )
    {
	if( (*start) == myString ) return true;
	start++;
    }
  return false;

};

bool DoesTableExist(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName,
  const char* TableName )
{
  std::vector< std::string > list;
  list = ListTables( ServerName, login, Password, DBName );
  std::string myString( TableName );
  std::vector< std::string >::iterator start = list.begin();
  std::vector< std::string >::iterator end   = list.end();
  while( start != end )
    {
	if( (*start) == myString ) return true;
	start++;
    }
  return false;

};

bool IsDatabaseOfGoFigureType(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName )
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
};


void CreateDataBaseMain(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName )
{
  if( CanConnectToDatabase(    ServerName, login, Password, DBName ) )
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
  else
    {
	// throw exception
    }
}

void CreateDataBase(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  char insertQuery[200];
  sprintf( insertQuery, "CREATE DATABASE %s", DBName );
  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}

void CreateBookmarksTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
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
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}

void CreateCollectionInfoTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
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
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}

void CreateExperimentTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
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
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}

void CreateFigureTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
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
    `points` blob, \
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
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}

void CreateFigureFlavorTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
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
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}

void CreateLineageTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
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
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}

void CreateLineageFlavorTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
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
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}

void CreateMeshTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
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
    `points` blob,\
    PRIMARY KEY (meshID)\
    )";

  query->SetQuery( insertQuery );
  if ( !query->Execute() )
    {
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}

void CreateMeshFlavor(
  const char* ServerName, const char* login, const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  vtkStdString insertQuery = "\
    CREATE TABLE `mesh flavor` \
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
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}

void CreateSeriesGridTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
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
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}

void CreateTrackTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName );
  DataBaseConnector->SetUser( login );
  DataBaseConnector->SetPassword( Password );
  DataBaseConnector->SetDatabaseName( DBName );
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
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}


void CreateTrackFlavor(
  const char* ServerName, const char* login, const char* Password, const char* DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName("localhost");
  DataBaseConnector->SetUser("gofigure");
  DataBaseConnector->SetPassword("gofigure");
  DataBaseConnector->SetDatabaseName( DBName );
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
    cerr << "Create query failed" << endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
