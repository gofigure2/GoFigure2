#ifndef __QGoThreadedMultiFileReader_h
#define __QGoThreadedMultiFileReader_h

#include <QThread>

#include "vtkImageData.h"
#include "itkMultiFileReader.h"


class QGoThreadedMultiFileReader : public QThread
{
  Q_OBJECT
  public:
    QGoThreadedMultiFileReader( QObject* iParent = 0 );
    ~QGoThreadedMultiFileReader();

    typedef itk::MultiFileReader::FILETYPE FILETYPE;

    void SetInput( FileListType iFileList,
      const FILETYPE& iFileType,
      const bool& iBool );
    void SetTimePoint( const int& iT );
    void SetZDepth( const int& iZ );

    vtkImageData* GetOutput();

  protected:
    void run();

  private:
    itk::MultiFileReader::Pointer   m_MultiFileReader;
    vtkImageData*                   m_Image;
    FileListType                    m_FileList;
    FILETYPE                        m_FileType;
    bool                            m_TimeBased;
    int                             m_TimePoint;
    int                             m_ZDepth;

};

#endif
