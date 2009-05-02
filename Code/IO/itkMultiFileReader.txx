#ifndef __itkMultiFileReader_txx_
#define __itkMultiFileReader_txx_

#include "vtkImageExtractComponents.h"
#include "vtkLSMReader.h"

namespace itk
{

//-----------------------------------------------------------------------------
void MultiFileReader::SetTimePoint( const int& UserTimePoint )
{ 
  if( UserTimePoint <= m_NumberOfTimePoints )
    {
    this->m_UpdateTimePoint = UserTimePoint;
    }
  else
    {
    this->m_UpdateTimePoint = m_NumberOfTimePoints;
    }
  this->Modified();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void MultiFileReader::SetChannel( const int& UserChannel )
  {
  this->m_UpdateChannel = UserChannel;
  this->Modified();
  }
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void MultiFileReader::UpdateChannel() 
  {
  if( this->m_UpdateChannel > m_NumberOfChannels )
    {
    this->m_UpdateChannel = m_NumberOfChannels;
    }
  this->Modified();
  }
//-----------------------------------------------------------------------------

  
//-----------------------------------------------------------------------------
void MultiFileReader::SetInput( FileListType* UserFileList )
  { 
  if( UserFileList->size() == 0 ) return;

  this->m_FileList = UserFileList;

  FileListType::iterator endIt;
  FileListType::iterator It = m_FileList->begin();
  unsigned int CurrentTimePoint = (*It).TimePoint;
  this->m_NumberOfTimePoints = 1;
  while( It != m_FileList->end())
    {
    if( (*It).TimePoint > CurrentTimePoint )
      {
      CurrentTimePoint = (*It).TimePoint;
      this->m_NumberOfTimePoints++;
      }
    It++;
    }
  this->Modified();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void MultiFileReader::PrintSelf( std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Images Dimensionality: " 
     << m_Dimensionality << std::endl;
  os << indent << "Images Value Type: "     
     << m_DataScalarType << std::endl;
  os << indent << "Number Of Channels: "    
     << m_NumberOfChannels << std::endl;
  os << indent << "Number of Time Points: " 
     << m_NumberOfTimePoints << std::endl;
  os << indent << "Update Time Point: "     
     << m_UpdateTimePoint << std::endl;
  os << indent << "Update Channel: " 
     << m_UpdateChannel << std::endl;
  os << indent << "Are Images multichannel? " 
     << m_AreImagesMultiChannel << std::endl;
}
//-----------------------------------------------------------------------------

   
//-----------------------------------------------------------------------------
void MultiFileReader::Update( void )
{
  this->ComputeUpdateFileList();
 
  FileListType::iterator startIt;
  FileListType::iterator endIt;
  FileListType::iterator It;

  if( this->m_Dimensionality == 2 )
    {
    // prepare the final output
    vtkImageAppend* volumeBuilder = vtkImageAppend::New();
    volumeBuilder->SetAppendAxis( 2 ); //append along Z
    
    // read the files and feed the volume builder
    // NOTE ALEX: sould convert to a vtkStringArray and use SetFileNames
    // note the S at  the end of the method name
    endIt = m_UpdateFileList.end();
    It    = m_UpdateFileList.begin();
    int counter = 0;
    while( It != endIt )
      {
      switch( this->m_FileType )
        {
        case JPEG:
          {
          vtkJPEGReader* reader = vtkJPEGReader::New();
          reader->SetFileName( (*It).Filename.c_str() );
	      reader->SetFileDimensionality( this->m_Dimensionality );
          reader->Update();
	      volumeBuilder->SetInput( counter, reader->GetOutput( ) );
	      break;
          }
	case BMP:
          { 
          itkExceptionMacro( << "BMP is not supported at this time." );
          break;
          }
        case PNG:
          {
          itkExceptionMacro( << "PNG is not supported at this time." );
          break;
          }
        case TIFF:
          {
          itkExceptionMacro( << "TIFF is not supported at this time." );
          break;
          }
	case MHA:
          {
          itkExceptionMacro( << "MHA is not supported at this time." );
          break;
          }
        case LSM:
          {
          itkExceptionMacro( << "LSM is not supported at this time." );
          break;
          }
        default:
          itkExceptionMacro( << "unsupported type: " << this->m_FileType << "." );
	      break;
        }
      It++;
      counter++;
      }

    volumeBuilder->Update();
    this->m_NumberOfChannels = volumeBuilder->GetOutput()->GetNumberOfScalarComponents();
    this->UpdateChannel();

    if( this->m_AreImagesMultiChannel )
      {
      m_OutputImage = volumeBuilder->GetOutput();
      }
    else
      {
      vtkImageExtractComponents* extractComp = vtkImageExtractComponents::New();
      extractComp->SetComponents( this->m_UpdateChannel );
      extractComp->SetInput( volumeBuilder->GetOutput() );
      extractComp->Update( );
      m_OutputImage = extractComp->GetOutput();
      }

    } // end of dimensionality == 2
    
  if( this->m_Dimensionality == 3 )
    {
    switch( this->m_FileType )
      {
      case JPEG: // fallthrough
      case BMP:  // falltrhough
      case PNG:  // fallthrough
        itkExceptionMacro( << "JPEG/BMP/PNG cannot be of dimensionality 3." );
        break;
      case TIFF:
        itkExceptionMacro( << "TIFF is not supported at this time." );
        break;
      case MHA:
        itkExceptionMacro( << "MHA is not supported at this time." );
        break; 
      case LSM:
        {
        It = m_UpdateFileList.begin();
        vtkLSMReader* reader = vtkLSMReader::New();
        reader->SetFileName( (*It).Filename.c_str() );
        if( this->m_AreImagesMultiChannel )
          { 
          // compose channels
          }
        else
         {
         reader->SetUpdateChannel( m_UpdateChannel );
         reader->Update();
         this->m_NumberOfChannels = reader->GetOutput()->GetNumberOfScalarComponents();
         this->UpdateChannel();
         m_OutputImage = reader->GetOutput();
         }
        break;
        }
      default:
        itkExceptionMacro( << "unsupported type: " << this->m_FileType << "." );
        break;
      }
    }  
}
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------- 
void MultiFileReader::ComputeUpdateFileList()
{
  if( m_FileList->size() == 0 )
    {
    itkExceptionMacro( << "The File List is empty." );
    return;
    }
    
  if( this->m_Dimensionality < 4 )    
  {
    // extract files of interest
    FileListType::iterator startIt;
    FileListType::iterator endIt;
    FileListType::iterator It = m_FileList->begin();
  
    itkDebugMacro( "Number of Files imported: " << m_FileList->size() );
    itkDebugMacro( "First File Time Point: " << (*It).TimePoint );
    itkDebugMacro( "Requested  Time Point: " << m_UpdateTimePoint ); 
    // get the first file
    while( It != m_FileList->end() 
           && (int)((*It).TimePoint) < this->m_UpdateTimePoint )
      {
      It++;
      }
    startIt = It;
  
    itkDebugMacro( "First File: " << (*It).Filename.c_str() );

    // get the last file
    unsigned int counter = 0;
    if( this->m_Dimensionality == 3 )  // only one file
      {
      It++;
      }
    else // find all the 2D files needed for the 3D volume
      {
      while( It != m_FileList->end() 
             && (int)((*It).TimePoint) == this->m_UpdateTimePoint )
        {
        It++;
        counter++;
        }
      }
    endIt = It;

    itkDebugMacro( "Last File: " << (*It).Filename.c_str() );
    itkDebugMacro( "Found " << counter << " Files in between." );
 
    // build up the file list;
    m_UpdateFileList.clear();
    It = startIt;
    while( It != endIt )
      {
      m_UpdateFileList.push_back( (*It) );
      It++;
      }
  
    itkDebugMacro( "Number Of files to Read: " << m_UpdateFileList.size() );

    }
  else
    {
    // TBD (ex LSM 4D)
    }
}
//-----------------------------------------------------------------------------
  
  
//-----------------------------------------------------------------------------
void MultiFileReader::SetFileType( const FILETYPE UserFileType )
{
  this->m_FileType = UserFileType;
  this->Modified();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void MultiFileReader::SetDimensionality( int UserDimensionality )
{
  this->m_Dimensionality = UserDimensionality;
  this->Modified();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void MultiFileReader::SetMultiChannelImages( int value )
{
  if( value ) 
    m_AreImagesMultiChannel = true;
  else
    m_AreImagesMultiChannel = false;
  this->Modified();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
MultiFileReader::MultiFileReader( )
{
  m_OutputImage = (vtkImageData*)(0);
  m_Dimensionality   = 0;
  m_DataScalarType   = -1;
  m_NumberOfChannels = -1;
  m_NumberOfTimePoints = -1;
  m_UpdateTimePoint  = -1;
  m_UpdateChannel    = -1;
  m_AreImagesMultiChannel = false;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
MultiFileReader::~MultiFileReader()
{
  if( m_OutputImage )
    {
    m_OutputImage->Delete();
    m_OutputImage = (vtkImageData*)(0);
    }
}
//-----------------------------------------------------------------------------
    
}

#endif
