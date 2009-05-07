#include "itkMegaCaptureImport.h"
#include "GoFigureFileInfoHelper.h"
#include "GoDBRecordSet.h"
#include "GoDBExperimentRow.h"
#include "GoDBSeriesGridRow.h"

int main( int argc, char * argv[] )
{
  if( argc < 6 )
    {
    std::cout << "Usage: ";
    std::cout << "<bin> ImageName ServerName Login Pass ExperimentID" << std::endl; 
    return EXIT_FAILURE;
    }

  try
    {
    itk::MegaCaptureImport::Pointer  importFileInfoList = itk::MegaCaptureImport::New();
    importFileInfoList->SetFileName( argv[1] );
    importFileInfoList->Update();

    typedef GoDBRecordSet< GoDBSeriesGridRow > myRecordSetType;
    myRecordSetType* RecordSet = new myRecordSetType;
    RecordSet->SetServerName( argv[2] );
    RecordSet->SetUser( argv[3] );
    RecordSet->SetPassword( argv[4] );
    RecordSet->SetTableName( "seriesgrid" );

    typedef FileListType::iterator myFilesIteratorType;
    myFilesIteratorType It  = importFileInfoList->GetOutput()->begin();
    myFilesIteratorType end = importFileInfoList->GetOutput()->end();    
    while( It != end )
      {
      GoDBSeriesGridRow row;
      row.experimentID = atoi( argv[5] );
      row.RCoord = (*It).RTile;
      row.CCoord = (*It).CTile;
      row.TCoord = (*It).TimePoint;
      row.YCoord = (*It).YOffset;
      row.XCoord = (*It).XOffset;
      row.ZCoord = (*It).ZDepth;
      row.filename = (*It).Filename.c_str();

      RecordSet->AddObject( row );
      }

    RecordSet->SaveInDB();

    }
  catch( const itk::ExceptionObject& e )
    {
    std::cerr << " caught an ITK exception: " << std::endl;
    e.Print( std::cerr);
    return EXIT_FAILURE;
    }
  catch( const std::exception& e )
    {
    std::cerr << " caught an std exception: " << std::endl;
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
    }
  catch( ... )
    {
    std::cerr << " caught an unknown exception!" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

