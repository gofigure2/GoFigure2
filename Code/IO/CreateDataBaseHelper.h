#ifndef __CreateDataBaseHelper_h_
#define __CreateDataBaseHelper_h_

#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStdString.h"

void CreateDataBase( char * DBName );
void CreateBookmarksTable( char * DBName );
void CreateCollectionInfoTable( char * DBName );
void CreateExperimentTable( char * DBName );
void CreateFigureTable( char * DBName );
void CreateFigureFlavorTable( char * DBName );
void CreateLineageTable( char * DBName );
void CreateLineageFlavorTable( char * DBName );
void CreateMeshTable( char * DBName );
void CreateMeshFlavor( char * DBName );
void CreateSeriesGridTable( char * DBName );
void CreateTrackTable( char * DBName );
void CreateTrackFlavor( char * DBName );

void CreateDataBaseMain( char * DBName);

#endif