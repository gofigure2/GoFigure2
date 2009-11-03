#ifndef __itkMegaCaptureReader_h
#define __itkMegaCaptureReader_h

#include "itkLightProcessObject.h"
#include "GoFigureFileInfoMultiIndexContainerHelper.h"

class vtkImageData;

namespace GoFigure
{
  enum FILETYPE
    {
      BMP = 0,
      JPEG,
      PNG,
      TIFF,
      MHA,
      LSM
    };
}

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

    /** \brief  */
    itkSetMacro( FileType, GoFigure::FILETYPE );
    itkSetMacro( TimeBased, bool );

    void SetTimePoint( const unsigned int& iTm );
    itkGetConstMacro( UpdateTimePoint, unsigned int );

    void SetZSlice( const unsigned int& iZs );
    itkGetConstMacro( UpdateZSlice, unsigned int );

    void SetChannel( const unsigned int& iCh );
    itkGetConstMacro( UpdateChannel, unsigned int );

    void Update();

    vtkImageData* GetOutput();

  protected:
    MegaCaptureReader();
    ~MegaCaptureReader();

    void ComputeBounds();

    vtkImageData*  m_OutputImage;
    GoFigureFileInfoHelperMultiIndexContainer  m_FileList;
    GoFigure::FILETYPE m_FileType;

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
