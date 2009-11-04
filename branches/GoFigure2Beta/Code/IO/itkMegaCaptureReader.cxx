#include "itkMegaCaptureReader.h"

#include "vtkImageData.h"
#include "vtkImageAppend.h"

#include "vtkJPEGReader.h"
#include "vtkBMPReader.h"
#include "vtkPNGReader.h"
#include "vtkTIFFReader.h"
#include "vtkMetaImageReader.h"

#include "VolumeBuilderHelper.h"

namespace itk
{

/**
 *
 */
MegaCaptureReader::
MegaCaptureReader() : m_FileType( GoFigure::PNG ),
  m_MinTimePoint( 0 ), m_MaxTimePoint( 0 ), m_UpdateTimePoint( 0 ),
  m_MinZSlice( 0 ), m_MaxZSlice( 0 ), m_UpdateZSlice( 0 ),
  m_MinChannel( 0 ), m_MaxChannel( 0 ), m_UpdateChannel( 0 ),
  m_TimeBased( true )
{
  m_OutputImage = vtkImageData::New();
}

/**
 *
 */
MegaCaptureReader::
~MegaCaptureReader()
{}

/**
 *
 * \param iT
 */
void
MegaCaptureReader::
SetTimePoint( const unsigned int& iT )
{
  if( iT < m_MinTimePoint )
    {
    m_UpdateTimePoint = m_MinTimePoint;
    }
  else
    {
    if( iT > m_MaxTimePoint )
      {
      m_UpdateTimePoint = m_MaxTimePoint;
      }
    else
      {
      m_UpdateTimePoint = iT;
      }
    }
}

/**
 *
 * \param iZs
 */
void
MegaCaptureReader::
SetZSlice( const unsigned int& iZs )
{
  if( iZs < m_MinZSlice )
    {
    m_UpdateZSlice = m_MinZSlice;
    }
  else
    {
    if( iZs > m_MaxZSlice )
      {
      m_UpdateZSlice = m_MaxZSlice;
      }
    else
      {
      m_UpdateZSlice = iZs;
      }
    }
}

/**
 *
 * \param iCh
 */
void
MegaCaptureReader::
SetChannel( const unsigned int& iCh )
{
  if( iCh < m_MinChannel )
    {
    m_UpdateChannel = m_MinChannel;
    }
  else
    {
    if( iCh > m_MaxChannel )
      {
      m_UpdateChannel = m_MaxTimePoint;
      }
    else
      {
      m_UpdateChannel = iCh;
      }
    }
}

/**
 *
 * \param iUserFileList
 */
void
MegaCaptureReader::
SetInput( const GoFigureFileInfoHelperMultiIndexContainer& iUserFileList )
{
  if( iUserFileList.empty() )
    {
    std::cerr <<"iUserFileList.empty()" <<std::endl;
    return;
    }

  m_FileList = iUserFileList;

  ComputeBounds();
}


/**
 *
 */
void
MegaCaptureReader::
ComputeBounds()
{
  // get min time point
  GoFigureFileInfoHelperMultiIndexContainer::index< m_TCoord >::type::iterator
    tm_it = m_FileList.get< m_TCoord >().begin();

  m_MinTimePoint = (*tm_it).m_TCoord;

  // get max time point
  GoFigureFileInfoHelperMultiIndexContainer::index< m_TCoord >::type::reverse_iterator
    r_tm_it = m_FileList.get< m_TCoord >().rbegin();

  m_MaxTimePoint = (*r_tm_it).m_TCoord;

  // get min Z slices
  GoFigureFileInfoHelperMultiIndexContainer::index< m_ZCoord >::type::iterator
    zs_it = m_FileList.get< m_ZCoord >().begin();

  m_MinZSlice = (*zs_it).m_ZCoord;

  // get max Z slices
  GoFigureFileInfoHelperMultiIndexContainer::index< m_ZCoord >::type::reverse_iterator
    r_zs_it = m_FileList.get< m_ZCoord >().rbegin();

  m_MaxZSlice = (*r_zs_it).m_ZCoord;

  // get min channel
  GoFigureFileInfoHelperMultiIndexContainer::index< m_Channel >::type::iterator
    ch_it = m_FileList.get< m_Channel >().begin();

  m_MinChannel = (*ch_it).m_Channel;

  // (note that the real one could be different)
  GoFigureFileInfoHelperMultiIndexContainer::index< m_Channel >::type::reverse_iterator
    r_ch_it = m_FileList.get< m_Channel >().rbegin();

  m_MaxChannel = (*r_ch_it).m_Channel;
}

/**
 *
 */
void
MegaCaptureReader::
Update()
{
  std::list< std::string > filelist;

  if( m_TimeBased )
    {
    filelist = GetAllFileNamesForGivenTCoordAndChannel( m_FileList,
      m_UpdateTimePoint, m_UpdateChannel );
    }
  else
    {
    filelist = GetAllFileNamesForGivenZCoordPointAndChannel( m_FileList,
      m_UpdateZSlice, m_UpdateChannel );
    }

  // prepare the final output
  vtkImageAppend* volumeBuilder = vtkImageAppend::New();
  volumeBuilder->SetAppendAxis( 2 ); //append along Z

  std::list< std::string >::iterator f_it    = filelist.begin();
  std::list< std::string >::iterator f_end = filelist.end();

  int counter = 0;

  while( f_it != f_end )
    {
    switch( m_FileType )
      {
      case GoFigure::JPEG:
        {
        AddToVolumeBuilder< vtkJPEGReader >( counter, (*f_it), volumeBuilder );
        break;
        }
      case GoFigure::BMP:
        {
        AddToVolumeBuilder< vtkBMPReader >( counter, (*f_it), volumeBuilder );
        break;
        }
      case GoFigure::PNG:
        {
        AddToVolumeBuilder< vtkPNGReader >( counter, (*f_it), volumeBuilder );
        break;
        }
      case GoFigure::TIFF:
        {
        AddToVolumeBuilder< vtkTIFFReader >( counter, (*f_it), volumeBuilder );
        break;
        }
      case GoFigure::MHA:
        {
        AddToVolumeBuilder< vtkMetaImageReader >( counter, (*f_it),
          volumeBuilder );
        break;
        }
      case GoFigure::LSM:
        {
        itkGenericExceptionMacro( << "stacks of 2D LSM are not supported at this time." );
        break;
        }
      default:
        {
        itkGenericExceptionMacro( << "unsupported type: " << m_FileType << "." );
        break;
        }
      }

      ++f_it;
      ++counter;
    }

    volumeBuilder->Update();
    vtkImageData* temp_output = volumeBuilder->GetOutput();
    std::cout <<temp_output->GetNumberOfScalarComponents() <<std::endl;

    m_OutputImage->ShallowCopy( temp_output );
    volumeBuilder->Delete();
}

/**
 *
 * \return
 */
vtkImageData*
MegaCaptureReader::
GetOutput()
{
  return m_OutputImage;
}

} //end of namespace