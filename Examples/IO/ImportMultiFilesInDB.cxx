#include "itkMegaCaptureImport.h"
#include "GoFigureFileInfoHelper.h"
#include "GoDBRecordSet.h"
#include "GoDBExperimentRow.h"
#include "GoDBSeriesGridRow.h"

int main( int argc, char * argv[] )
{
  if( argc < 2 )
    {
    std::cout << "Usage: ";
    std::cout << "<bin> ImageName" << std::endl;
    return EXIT_FAILURE;
    }

  try
    {
    itk::MegaCaptureImport::Pointer  importFileInfoList = itk::MegaCaptureImport::New();
    importFileInfoList->SetFileName( argv[1] );
    importFileInfoList->Update();

    typedef GoDBRecordSet< GoDBSeriesGridRow > myRecordSetType;
    myRecordSetType* RecordSet = new myRecordSetType;
    RecordSet->SetServerName( "localhost" );
    RecordSet->SetUser( "gofigure" );
    RecordSet->SetPassword( "gofigure" );
    RecordSet->SetDataBaseName( "allnewdb" );
    RecordSet->SetTableName( "seriesgrid" );

    typedef FileListType::iterator myFilesIteratorType;
    myFilesIteratorType It  = importFileInfoList->GetOutput()->begin();
    myFilesIteratorType end = importFileInfoList->GetOutput()->end();
    while( It != end )
      {
      GoDBSeriesGridRow row;
      row.experimentID = 5;
      row.RCoord = (*It).m_RTile;
      row.CCoord = (*It).m_CTile;
      row.TCoord = (*It).m_TimePoint;
      row.YCoord = (*It).m_YOffset;
      row.XCoord = (*It).m_XOffset;
      row.ZCoord = (*It).m_ZDepth;
      row.filename = (*It).m_Filename.c_str();

      RecordSet->AddObject( row );

      It++;
      }

    if( RecordSet->SaveInDB() )
      {
      return EXIT_SUCCESS;
      }
    else
      {
      return EXIT_FAILURE;
      }

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

