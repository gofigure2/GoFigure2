#include "LSMToMegaCapture.h"

#include <fstream>
#include <sstream>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "vtkExtractVOI.h"
#include "vtkImageData.h"
#include "vtkPNGWriter.h"

LSMToMegaCapture::LSMToMegaCapture( ) : m_Plaque( 0 ), m_Column( 0 ), m_Row( 0 ),
  m_XTile( 0 ), m_YTile( 0 ), m_ZTile( 0 )
{
}

LSMToMegaCapture::~LSMToMegaCapture()
{
  if( !m_LSMReaders.empty() )
    {
    for( unsigned int i = 0; i < m_LSMReaders.size(); i++ )
      {
      m_LSMReaders[i]->Delete();
      }
    }
}

void
LSMToMegaCapture::
SetFileName( const std::string& iFileName )
{
  m_FileName = iFileName;

  if( !m_LSMReaders.empty() )
    {
    for( unsigned int i = 0; i < m_LSMReaders.size(); i++ )
      {
      m_LSMReaders[i]->Delete();
      }
    }
  m_LSMReaders.push_back( vtkLSMReader::New() );
  m_LSMReaders.front()->SetFileName( iFileName.c_str() );
  m_LSMReaders.front()->Update();
}

void
LSMToMegaCapture::
Export()
{
  m_NumberOfChannels = m_LSMReaders[0]->GetNumberOfChannels();
  m_NumberOfTimePoints = m_LSMReaders[0]->GetNumberOfTimePoints();

  std::cout <<m_LSMReaders[0]->GetDescription() <<std::endl;

  double spacing[3];
  m_LSMReaders[0]->GetVoxelSizes( spacing );

  int dim[5];
  m_LSMReaders[0]->GetDimensions( dim );

  std::ofstream file( "test.txt" );
  file <<"MegaCapture" <<std::endl;
  file <<"<ImageSessionData>" <<std::endl;
  file <<"Version 3.0" <<std::endl;
  file <<"ExperimentTitle " <<std::endl;
  file <<"ExperimentDescription ";
  if( m_LSMReaders[0]->GetDescription() )
    {
    file <<m_LSMReaders[0]->GetDescription();
    }
  file <<std::endl;
  file <<"TimeInterval " <<m_LSMReaders[0]->GetTimeInterval() <<std::endl;
  file <<"Objective " <<m_LSMReaders[0]->GetObjective() <<std::endl;
  file <<"VoxelSizeX " <<spacing[0] * 1000000 <<std::endl;
  file <<"VoxelSizeY " <<spacing[1] * 1000000 <<std::endl;
  file <<"VoxelSizeZ " <<spacing[2] * 1000000 <<std::endl;
  file <<"DimensionX " <<dim[0] <<std::endl;
  file <<"DimensionY " <<dim[1] <<std::endl;
  file <<"DimensionPL " <<m_Plaque <<std::endl;
  file <<"DimensionCO " <<m_Column <<std::endl;
  file <<"DimensionRO " <<m_Row <<std::endl;
  file <<"DimensionZT " <<m_ZTile <<std::endl;
  file <<"DimensionYT " <<m_YTile <<std::endl;
  file <<"DimensionXT " <<m_XTile <<std::endl;
  file <<"DimensionTM " <<m_NumberOfTimePoints <<std::endl;
  file <<"DimensionZS " <<dim[2] <<std::endl;
  file <<"DimensionCH " <<m_NumberOfChannels <<std::endl;

  for( unsigned int i = 0; i < m_NumberOfChannels; i++ )
    {
    int r = m_LSMReaders[0]->GetChannelColorComponent( i, 0 );
    int g = m_LSMReaders[0]->GetChannelColorComponent( i, 1 );
    int b = m_LSMReaders[0]->GetChannelColorComponent( i, 2 );

    file <<"ChannelColor" <<i <<" " << r * 256 * 256 + g * 256 + b <<std::endl;
    }

  file <<"ChannelDepth 8" <<std::endl;
  file <<"FileType PNG" <<std::endl;
  file <<"</ImageSessionData>" <<std::endl;

  if( m_NumberOfChannels > 1 )
    {
    for( unsigned int i = 1; i < m_NumberOfChannels; i++ )
      {
      m_LSMReaders.push_back( vtkLSMReader::New() );
      m_LSMReaders[i]->SetFileName( m_FileName.c_str() );
      m_LSMReaders[i]->SetUpdateChannel( i );
      }
    }

  int extent[6];

  char timeStr[ 100 ] = "";
  struct stat buf;

  if (!stat(m_FileName.c_str(), &buf))
    {
    strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S", localtime( &buf.st_mtime));
    }

  for( unsigned int i = 0; i < m_NumberOfTimePoints; i++ )
    {
    for( int k = 0; k < m_NumberOfChannels; k++ )
      {
      m_LSMReaders[k]->SetUpdateTimePoint( i );
      }

    for( int j = 0; j < m_NumberOfChannels; j++ )
      {
      m_LSMReaders[j]->Update();
      vtkImageData* image3d = m_LSMReaders[j]->GetOutput();
      image3d->GetExtent( extent );

      for( int k = 0; k < dim[2]; k++ )
        {
        std::stringstream filename;
        filename <<"image-PL00-CO00-RO00-ZT00-YT00-XT00-TM";
        filename << setfill('0') << setw(2) <<i <<"-ch";
        filename << setfill('0') << setw(4) <<j <<"-zs";
        filename << setfill('0') << setw(4) <<k <<".png";

        file <<"<Image>"<<std::endl;
        file <<"Filename " <<filename.str() <<std::endl;
        file <<"DateTime " <<timeStr <<std::endl;
        file <<"StageX 1000" <<std::endl;
        file <<"StageY -1000" <<std::endl;
        file <<"Pinhole 44.216" <<std::endl;
        file <<"</Image>"<<std::endl;

        vtkExtractVOI* extract = vtkExtractVOI::New();
        extract->SetSampleRate( 1, 1, 1 );
        extract->SetInput( image3d );

        extract->SetVOI( extent[0], extent[1], extent[2], extent[3], k, k );
        extract->Update();

        vtkImageData* image2d = extract->GetOutput();

        vtkPNGWriter* writer2d = vtkPNGWriter::New();
        writer2d->SetInput( image2d );
        writer2d->SetFileName( filename.str().c_str() );
        writer2d->Write();

        writer2d->Delete();
        extract->Delete();
        }
      }
    }
  file.close();


}

