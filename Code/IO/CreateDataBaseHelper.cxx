#include "CreateDataBaseHelper.h"

void CreateDataBaseMain( char * DBName)
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName("localhost");
  DataBaseConnector->SetUser("gofigure");
  DataBaseConnector->SetPassword("gofigure");
  DataBaseConnector->Open("");
  std::cout << "IsOpen ? " << DataBaseConnector->IsOpen()  << std::endl;

  CreateDataBase(            DBName );
  CreateBookmarksTable(      DBName );
  CreateCollectionInfoTable( DBName );
  CreateExperimentTable(     DBName );
  CreateFigureTable(         DBName );
  CreateFigureFlavorTable(   DBName );
  CreateLineageTable(        DBName );
  CreateLineageFlavorTable(  DBName );
  CreateMeshTable(           DBName );
  CreateMeshFlavor(          DBName );
  CreateSeriesGridTable(     DBName );
  CreateTrackTable(          DBName );
  CreateTrackFlavor(         DBName );

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
}

void CreateDataBase( char * DBName )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName("localhost");
  DataBaseConnector->SetUser("gofigure");
  DataBaseConnector->SetPassword("gofigure");
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

void CreateBookmarksTable( char * DBName )
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

void CreateCollectionInfoTable( char * DBName )
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

void CreateExperimentTable( char * DBName )
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
    `filePattern` varchar (50),\
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

void CreateFigureTable( char * DBName )
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

void CreateFigureFlavorTable( char * DBName )
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

void CreateLineageTable( char * DBName )
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

void CreateLineageFlavorTable( char * DBName )
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

void CreateMeshTable( char * DBName )
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

void CreateMeshFlavor( char * DBName )
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

void CreateSeriesGridTable( char * DBName )
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
    `filename` varchar (50),\
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

void CreateTrackTable( char * DBName )
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

void CreateTrackFlavor( char * DBName )
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
