#ifndef __itkMegaCaptureReader_h
#define __itkMegaCaptureReader_h

#include "GoFigureGlobalDefinition.h"
#include "itkLightProcessObject.h"
#include "GoFigureFileInfoMultiIndexContainerHelper.h"
#include "MegaCaptureHeaderReader.h"

class vtkImageData;

namespace itk
{
  class MegaCaptureReader : public LightProcessObject
  {
  public:
    /** Standard class typedefs.      */
    typedef MegaCaptureReader           Self;
    typedef LightProcessObject          Superclass;
    typedef SmartPointer< Self >        Pointer;
    typedef SmartPointer< const Self >  ConstPointer;

    itkNewMacro( Self );

    itkTypeMacro( MegaCaptureReader, LightProcessObject );

    /** \brief set the input as a GoFigure format file list */
    void SetInput( const GoFigureFileInfoHelperMultiIndexContainer& UserFileList );

    void SetMegaCaptureHeader( const std::string& iHeader );

    /** \brief  */
    itkSetMacro( FileType, GoFigure::FileType );
    itkSetMacro( TimeBased, bool );

    void SetTimePoint( const unsigned int& iTm );
    itkGetConstMacro( UpdateTimePoint, unsigned int );

    itkGetConstMacro( MinTimePoint, unsigned int );
    itkGetConstMacro( MaxTimePoint, unsigned int );

    void SetZSlice( const unsigned int& iZs );
    itkGetConstMacro( UpdateZSlice, unsigned int );

    itkGetConstMacro( MinZSlice, unsigned int );
    itkGetConstMacro( MaxZSlice, unsigned int );

    void SetChannel( const unsigned int& iCh );
    itkGetConstMacro( UpdateChannel, unsigned int );

    itkGetConstMacro( MinChannel, unsigned int );
    itkGetConstMacro( MaxChannel, unsigned int );

    void Update();

    vtkImageData* GetOutput();

  protected:
    MegaCaptureReader();
    ~MegaCaptureReader();

    void ComputeBounds();

    vtkImageData*                               m_OutputImage;
    GoFigureFileInfoHelperMultiIndexContainer   m_FileList;
    GoFigure::FileType                          m_FileType;
    MegaCaptureHeaderReader*                    m_HeaderReader;

    unsigned int m_MinTimePoint;
    unsigned int m_MaxTimePoint;
    unsigned int m_UpdateTimePoint;

    unsigned int m_MinZSlice;
    unsigned int m_MaxZSlice;
    unsigned int m_UpdateZSlice;

    unsigned int m_MinChannel;
    unsigned int m_MaxChannel;
    unsigned int m_UpdateChannel;

    bool m_TimeBased;

  private:
    MegaCaptureReader( const Self& );
    void operator = ( const Self& );
  };
}
#endif
