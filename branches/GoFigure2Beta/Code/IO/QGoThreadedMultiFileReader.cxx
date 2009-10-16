#include "QGoThreadedMultiFileReader.h"

#include <algorithm>

QGoThreadedMultiFileReader::
QGoThreadedMultiFileReader( QObject* parent ) : QThread( parent ),
  m_Image( 0 ),
  m_FileType( itk::MultiFileReader::JPEG ),
  m_TimeBased( true ),
  m_TimePoint( 0 ),
  m_ZDepth( 0 )
{
  m_MultiFileReader = itk::MultiFileReader::New();
}

QGoThreadedMultiFileReader::~QGoThreadedMultiFileReader()
{
}

void QGoThreadedMultiFileReader::SetInput( FileListType* iFileList,
  const FILETYPE& iFileType,
  const bool& iBool )
{
  m_FileList = iFileList;
  m_FileType = iFileType;
  m_TimeBased = iBool;

  m_MultiFileReader->SetFileType( m_FileType );
  m_MultiFileReader->SetTimeBased( m_TimeBased );
  m_MultiFileReader->SetDimensionality( 2 );
  m_MultiFileReader->MultiChannelImagesOn();
}

void QGoThreadedMultiFileReader::SetTimePoint( const int& iT )
{
  m_TimePoint = iT;
}

void QGoThreadedMultiFileReader::SetZDepth( const int& iZ )
{
  m_ZDepth = iZ;
}

vtkImageData* QGoThreadedMultiFileReader::GetOutput()
{
  return m_Image;
}

void QGoThreadedMultiFileReader::run()
{
  if( m_TimeBased )
    {
    std::sort( m_FileList->begin(), m_FileList->end(),
      GoFigureFileInfoHelperTimeBasedCompare() );
    }
  else
    {
    std::sort( m_FileList->begin(), m_FileList->end(),
      GoFigureFileInfoHelperZCoordBasedCompare() );
    }
//   m_Lock.lockForRead();
  m_MultiFileReader->SetInput( m_FileList );

  if( m_TimeBased )
    {
    m_MultiFileReader->SetTimePoint( m_TimePoint );
    }
  else
    {
    m_MultiFileReader->SetZDepth( m_ZDepth );
    }

  m_MultiFileReader->Update();
//   m_Lock.unlock();

  m_Image = m_MultiFileReader->GetOutput();
  std::cout <<"end" <<std::endl;
}
