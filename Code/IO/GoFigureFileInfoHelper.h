#ifndef __FileInfo_h__
#define __FileInfo_h__

class GoFigureFileInfoHelper
{
public:
  std::string    Filename;
  unsigned int   TimePoint;
  unsigned int   ZDepth;
  unsigned int   Channel;
  unsigned int   CTile;
  unsigned int   RTile;
  unsigned int   YOffset;
  unsigned int   XOffset;

  bool operator<( const GoFigureFileInfoHelper& other ) const
    {
    if( this->Channel < other.Channel )
      {
      return true;
      }
    if( this->Channel == other.Channel )
      {
      if( this->TimePoint < other.TimePoint )
        {
        return true;
        }
      if( this->TimePoint == other.TimePoint )
        {
        if( this->ZDepth < other.ZDepth )
          {
          return true;
          }
        }
      }
    return false;
    };

  GoFigureFileInfoHelper()
    {
    Filename  = "";
    TimePoint = 0;
    ZDepth    = 0;
    Channel   = 0;
    CTile     = 0;
    RTile     = 0;
    YOffset   = 0;
    XOffset   = 0;
    };
};

typedef std::vector< GoFigureFileInfoHelper > FileListType;

#endif
