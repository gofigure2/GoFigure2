#ifndef __MegaCaptureHeaderReader_h
#define __MegaCaptureHeaderReader_h

#include <string>
#include <vector>

class MegaCaptureHeaderReader
{
  public:
    explicit MegaCaptureHeaderReader( const std::string& iFileName = "" );
    ~MegaCaptureHeaderReader();

    void SetFileName( const std::string& iFileName );
    void Read();

    double      m_TimeInterval;
    double      m_VoxelSizeX;
    double      m_VoxelSizeY;
    double      m_VoxelSizeZ;
    unsigned int m_DimensionX;
    unsigned int m_DimensionY;
    unsigned int m_NumberOfChannels;
    unsigned int m_ChannelDepth;
    std::vector< std::vector< int > > m_ChannelColor;
    std::string m_CreationDate;

  protected:
    std::string m_FileName;
    std::vector< int > ConvertUnsignedLongColorToRGBIntColor(
      const unsigned long& iColor );


};
#endif
