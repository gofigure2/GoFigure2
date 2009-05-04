#ifndef __CreateDataBaseHelper_h_
#define __CreateDataBaseHelper_h_

#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStdString.h"

void CreateDataBase( const char * DBName );
void CreateBookmarksTable( const char * DBName );
void CreateCollectionInfoTable( const char * DBName );
void CreateExperimentTable( const char * DBName );
void CreateFigureTable( const char * DBName );
void CreateFigureFlavorTable( const char * DBName );
void CreateLineageTable( const char * DBName );
void CreateLineageFlavorTable( const char * DBName );
void CreateMeshTable( const char * DBName );
void CreateMeshFlavor( const char * DBName );
void CreateSeriesGridTable( const char * DBName );
void CreateTrackTable( const char * DBName );
void CreateTrackFlavor( const char * DBName );

void CreateDataBaseMain( const char * DBName);

#endif