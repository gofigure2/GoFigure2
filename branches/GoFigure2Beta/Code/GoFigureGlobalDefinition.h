#ifndef __GoFigureGlobalDefinition_h
#define __GoFigureGlobalDefinition_h

namespace GoFigure
{
  enum FileType
    {
      BMP = 0,
      JPEG,
      PNG,
      TIFF,
      MHA,
      LSM,
      EPS
    };

  enum TabDimensionType
    {
    TWO_D = 2,
    THREE_D,
    THREE_D_WITH_T,
    FOUR_D
    };
}

#endif
