#ifndef __itkMultiFileReader_h__
#define __itkMultiFileReader_h__

#include "itkLightProcessObject.h"

#include "vtkJPEGReader.h"
#include "vtkImageData.h"
#include "vtkImageAppend.h"

#include "GoFigureFileInfoHelper.h"
#include "MegaVTK2Configure.h"

enum FILETYPE
  {
  BMP = 0,
  JPEG,
  PNG,
  TIFF,
  MHA,
  LSM
  };

namespace itk
{
/**
*  \class MultiFileReader
*
*  \brief
*  Class that implement a reader for a stack of files
*
*  files can be of any dimensionality (we expect 2, 3 or 4D)
*  files can be of the following types: JPG, PNG, BMP, TIFF, MHA, LSM
*
*  \todo  Add Support for ProgressEvent
*
*
*/
class ITK_EXPORT MultiFileReader : public LightProcessObject
{
public:


  /** Standard class typedefs.      */
  typedef MultiFileReader Self;

  typedef LightProcessObject Superclass;

  typedef SmartPointer< Self >  Pointer ;
  typedef SmartPointer< const Self >  ConstPointer ;

  /** Run-time type information (and related methods). */
  itkTypeMacro( MultiFileReader, LightProcessObject ) ;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \brief set the time point you want to extract
      and load in memory.                              */
  void SetTimePoint( const int& UserTimePoint );

  /** \brief set the channel you want to extract
      and load in memory. -1 for all channels.         */
  void SetChannel( const int& UserChannel );
  void UpdateChannel();

  /** \brief set the input as a GoFigure format file list */
  void SetInput( FileListType* UserFileList );

  /** \brief  */
  void SetFileType( const FILETYPE UserFileType );

  /** \brief  */
  void SetDimensionality( int UserDimensionality );

  int GetNumberOfTimePoints( ) const { return m_NumberOfTimePoints; };

  /** \brief  */
  void SetMultiChannelImages( int value );
  void SetMultiChannelImagesON( )  { SetMultiChannelImages( 1 ); };
  void SetMultiChannelImagesOFF( ) { SetMultiChannelImages( 0 ); };

  /** \brief  */
  vtkImageData* GetOutput( ) const { return( m_OutputImage ); };

  // Fake It. The output is not of itkDataObject type
  // so pipeline mechanism would not work.
  void Update(void);

// protected:

  /** \brief Mandatory PrintSelf */
  void PrintSelf( std::ostream& os, Indent indent) const;

  MultiFileReader( );
  ~MultiFileReader();

private:

  void ComputeUpdateFileList();

  MultiFileReader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  vtkImageData*  m_OutputImage;
  FileListType*  m_FileList;
  FileListType   m_UpdateFileList;
  FILETYPE       m_FileType;        // suppose same file format for all files
  int            m_Dimensionality;  // suppose same dimensionality for all files
  int            m_DataScalarType;  // suppose same type for all files
  int            m_NumberOfChannels;// OfScalarComponents;
  int            m_NumberOfTimePoints;
  int            m_UpdateTimePoint;
  int            m_UpdateChannel;
  bool           m_AreImagesMultiChannel;
};

} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultiFileReader.txx"
#endif

#endif
