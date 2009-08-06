/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "itkMultiFileReader.h"

#include "vtkImageAppendComponents.h"
#include "vtkImageExtractComponents.h"

#include "vtkLSMReader.h"
#include "vtkMetaImageReader.h"
#include "vtkBMPReader.h"
#include "vtkTIFFReader.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"

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
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::SetZDepth( const int& iZ )
{
  if( iZ <= m_NumberOfZSlices )
    {
    this->m_UpdateZSlice = iZ;
    }
  else
    {
    this->m_UpdateZSlice = m_NumberOfZSlices;
    }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::SetChannel( const int& UserChannel )
{
  this->m_UpdateChannel = UserChannel;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void MultiFileReader::UpdateChannel()
{
  if( this->m_UpdateChannel > m_NumberOfChannels )
    {
    this->m_UpdateChannel = m_NumberOfChannels;
    }
  if( this->m_UpdateChannel < 0 )
    {
    this->m_UpdateChannel = 0;
    }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::SetInput( FileListType* UserFileList )
{
  if( UserFileList->empty() )
    {
    return;
    }

  this->m_FileList = UserFileList;

  FileListType::iterator endIt;
  FileListType::iterator It = m_FileList->begin();

  if( this->m_TimeBased )
    {
    unsigned int CurrentTimePoint = (*It).m_TimePoint;
    this->m_NumberOfTimePoints = 1;
    while( It != m_FileList->end())
      {
      if( (*It).m_TimePoint > CurrentTimePoint )
        {
        CurrentTimePoint = (*It).m_TimePoint;
        this->m_NumberOfTimePoints++;
        }
      It++;
      }
    }
  else
    {
    unsigned int CurrentZSlice = (*It).m_ZDepth;
    this->m_NumberOfZSlices = 1;
    while( It != m_FileList->end() )
      {
      if( (*It).m_ZDepth > CurrentZSlice )
        {
        CurrentZSlice = (*It).m_ZDepth;
        this->m_NumberOfZSlices++;
        }
      It++;
      }
    }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::PrintSelf( std::ostream& os, Indent indent) const
{
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
  if( this->m_OutputImage )
    {
    this->m_OutputImage->Delete();
    }

  if( this->IsProgressBarSet )
    {
    this->m_ProgressBar->show();
    this->m_ProgressBar->setValue( 1 );
    }

  this->ComputeUpdateFileList();
  if( m_UpdateFileList.empty() )
    {
    std::cout <<"Problem: m_UpdateFileList is empty :-/ (after ComputeUpdateFileList)" <<std::endl;
    return;
    }
  if( this->IsProgressBarSet )
    {
    this->m_ProgressBar->setValue( 5 );
    }

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
          reader->SetFileName( (*It).m_Filename.c_str() );
          reader->SetFileDimensionality( this->m_Dimensionality );
          reader->Update();

          volumeBuilder->SetInput( counter, reader->GetOutput( ) );
          reader->Delete();
          break;
          }
        case BMP:
          {
          vtkBMPReader* reader = vtkBMPReader::New();
          reader->SetFileName( (*It).m_Filename.c_str() );
          reader->SetFileDimensionality( this->m_Dimensionality );
          reader->Update();

          volumeBuilder->SetInput( counter, reader->GetOutput( ) );
          reader->Delete();
          break;
          }
        case PNG:
          {
          vtkPNGReader* reader = vtkPNGReader::New();
          reader->SetFileName( (*It).m_Filename.c_str() );
          reader->SetFileDimensionality( this->m_Dimensionality );
          reader->Update();

          volumeBuilder->SetInput( counter, reader->GetOutput( ) );
          reader->Delete();
          break;
          }
        case TIFF:
          {
          vtkTIFFReader* reader = vtkTIFFReader::New();
          reader->SetFileName( (*It).m_Filename.c_str() );
          reader->SetFileDimensionality( this->m_Dimensionality );
          reader->Update();

          volumeBuilder->SetInput( counter, reader->GetOutput( ) );
          reader->Delete();
          break;
          }
        case MHA:
          {
          vtkMetaImageReader* reader = vtkMetaImageReader::New();
          reader->SetFileName( (*It).m_Filename.c_str() );
          reader->SetFileDimensionality( this->m_Dimensionality );
          reader->Update();

          volumeBuilder->SetInput( counter, reader->GetOutput( ) );
          reader->Delete();
          break;
          }
        case LSM:
          {
          itkGenericExceptionMacro( << "stacks of 2D LSM are not supported at this time." );
          break;
          }
        default:
          {
          itkGenericExceptionMacro( << "unsupported type: " << this->m_FileType << "." );
          break;
          }
        }

      if( this->IsProgressBarSet )
        {
        this->m_ProgressBar->setValue(
          float( counter * 80 ) / float( m_UpdateFileList.size() ) );
        }
      It++;
      counter++;
      }

    volumeBuilder->Update();
    this->m_NumberOfChannels = volumeBuilder->GetOutput()->GetNumberOfScalarComponents();
    this->UpdateChannel();

    if( this->m_AreImagesMultiChannel )
      {
      m_OutputImage = vtkImageData::New();
      m_OutputImage->ShallowCopy( volumeBuilder->GetOutput() );
        volumeBuilder->Delete();
      }
    else
      {
      vtkImageExtractComponents* extractComp = vtkImageExtractComponents::New();
      extractComp->SetComponents( this->m_UpdateChannel );
      extractComp->SetInput( volumeBuilder->GetOutput() );
      extractComp->Update( );
      volumeBuilder->Delete();
      m_OutputImage = vtkImageData::New();
      m_OutputImage->ShallowCopy( extractComp->GetOutput() );
      extractComp->Delete();
      }

    } // end of dimensionality == 2

  if( this->m_Dimensionality == 3 )
    {
    switch( this->m_FileType )
      {
      case JPEG: // fallthrough
      case BMP:  // falltrhough
      case PNG:  // fallthrough
        itkGenericExceptionMacro( << "JPEG/BMP/PNG cannot be of dimensionality 3." );
        break;
      case TIFF:
        {
        vtkTIFFReader* reader = vtkTIFFReader::New();
        reader->SetFileName( (*It).m_Filename.c_str() );
        reader->SetFileDimensionality( this->m_Dimensionality );
        reader->Update();
        m_OutputImage = vtkImageData::New();
        m_OutputImage->ShallowCopy( reader->GetOutput() );
        reader->Delete();
        break;
        }
      case MHA:
        {
        vtkMetaImageReader* reader = vtkMetaImageReader::New();
        reader->SetFileName( (*It).m_Filename.c_str() );
        reader->SetFileDimensionality( this->m_Dimensionality );
        reader->Update();
        m_OutputImage = vtkImageData::New();
        m_OutputImage->ShallowCopy( reader->GetOutput() );
        reader->Delete();
        break;
        }
      case LSM:
        {
        It = m_UpdateFileList.begin();
        if( this->m_AreImagesMultiChannel )
          {
          vtkImageData* myImage_ch1 = vtkImageData::New();
          vtkLSMReader* reader = vtkLSMReader::New();
          reader->SetFileName( (*It).m_Filename.c_str() );
          reader->Update();
          int NumberOfChannels = reader->GetNumberOfChannels();
          myImage_ch1->ShallowCopy( reader->GetOutput() );
          reader->Delete();

          if( this->IsProgressBarSet )
            {
            this->m_ProgressBar->setValue( 20 );
            }

          if( ( NumberOfChannels == 1 ) )
            {
            m_OutputImage = myImage_ch1;
            return;
            }

          vtkImageData* myImage_ch2 = vtkImageData::New();
          vtkLSMReader* reader2=vtkLSMReader::New();
          reader2->SetFileName( (*It).m_Filename.c_str() );
          reader2->SetUpdateChannel( 1 );
          reader2->Update();
          myImage_ch2->ShallowCopy( reader2->GetOutput() );
          reader2->Delete();

          if( this->IsProgressBarSet )
            {
            this->m_ProgressBar->setValue( 40 );
            }

          vtkImageData* myImage2 = vtkImageData::New();
          vtkImageAppendComponents* appendFilter1 = vtkImageAppendComponents::New();
          appendFilter1->AddInput( myImage_ch1 );
          appendFilter1->AddInput( myImage_ch2 );
          appendFilter1->Update();
          myImage2->ShallowCopy( appendFilter1->GetOutput() );
          appendFilter1->Delete();
          myImage_ch2->Delete();

          if( this->IsProgressBarSet )
            {
            this->m_ProgressBar->setValue( 60 );
            }

          vtkImageData* myImage_ch3 = vtkImageData::New();
          if( NumberOfChannels == 2 )
            {
            myImage_ch3->ShallowCopy( myImage_ch1 );
            }
          else
            {
            vtkLSMReader* reader3 = vtkLSMReader::New();
            reader3->SetFileName( (*It).m_Filename.c_str() );
            reader3->SetUpdateChannel( 2 );
            reader3->Update();
            myImage_ch3->ShallowCopy( reader3->GetOutput() );
            reader3->Delete();
            }
          myImage_ch1->Delete();

          if( this->IsProgressBarSet )
            {
            this->m_ProgressBar->setValue( 80 );
            }

          vtkImageData* myImage3 = vtkImageData::New();
          vtkImageAppendComponents* appendFilter2 = vtkImageAppendComponents::New();
          appendFilter2->AddInput( myImage2    );
          appendFilter2->AddInput( myImage_ch3 );
          appendFilter2->Update();
          myImage3->ShallowCopy( appendFilter2->GetOutput() );
          appendFilter2->Delete();
          myImage2->Delete();
          myImage_ch3->Delete();

          if( this->IsProgressBarSet )
            {
            this->m_ProgressBar->setValue( 100 );
            }

          m_OutputImage = myImage3;
          this->m_NumberOfChannels = m_OutputImage->GetNumberOfScalarComponents();
          this->UpdateChannel();
          }
        else
          {
          vtkLSMReader* reader = vtkLSMReader::New();
          reader->SetFileName( (*It).m_Filename.c_str() );
          reader->SetUpdateChannel( m_UpdateChannel );
          reader->Update();
          this->m_NumberOfChannels = reader->GetOutput()->GetNumberOfScalarComponents();
          this->UpdateChannel();
          m_OutputImage = vtkImageData::New();
          m_OutputImage->ShallowCopy( reader->GetOutput() );
          reader->Delete();
          }
        break;
        }
      default:
        itkGenericExceptionMacro( << "unsupported type: " << this->m_FileType << "." );
        break;
      }
    }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::ComputeUpdateFileList()
{
  if( m_FileList->empty() )
    {
    itkGenericExceptionMacro( << "The File List is empty." );
    return;
    }

  if( this->m_Dimensionality < 4 )
    {
    // extract files of interest
    FileListType::iterator startIt;
    FileListType::iterator endIt;
    FileListType::iterator It = m_FileList->begin();

    // get the first file
    if( m_TimeBased )
      {
      while( It != m_FileList->end()
            && (int)((*It).m_TimePoint) < this->m_UpdateTimePoint )
        {
        It++;
        }
      }
    else
      {
      while( It != m_FileList->end()
            && (int)((*It).m_ZDepth) < this->m_UpdateZSlice )
        {
        It++;
        }
      }
    startIt = It;

    // get the last file
    unsigned int counter = 0;
    if( this->m_Dimensionality == 3 )  // only one file
      {
      It++;
      }
    else // find all the 2D files needed for the 3D volume
      {
      if( m_TimeBased )
        {
        while( It != m_FileList->end()
               && (int)((*It).m_TimePoint) == this->m_UpdateTimePoint )
          {
          It++;
          counter++;
          }
        }
      else
        {
        while( It != m_FileList->end()
               && (int)((*It).m_ZDepth) == this->m_UpdateZSlice )
          {
          It++;
          counter++;
          }
        }
      }
    endIt = It;

    // build up the file list;
    m_UpdateFileList.clear();
    It = startIt;
    while( It != endIt )
      {
      m_UpdateFileList.push_back( (*It) );
      It++;
      }
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
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::SetDimensionality( int UserDimensionality )
{
  this->m_Dimensionality = UserDimensionality;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::SetVolumePerTimePoint( const bool& iBool )
{
  m_TimeBased = iBool;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::SetMultiChannelImages( int value )
{
  if( value )
    {
    m_AreImagesMultiChannel = true;
    }
  else
    {
    m_AreImagesMultiChannel = false;
    }
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
  m_NumberOfZSlices = -1;
  m_UpdateZSlice    = -1;
  m_UpdateChannel    = -1;
  m_AreImagesMultiChannel = false;
  m_TimeBased = true;
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
