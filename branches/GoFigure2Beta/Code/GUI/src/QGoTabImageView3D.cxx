#include "QGoTabImageView3D.h"

QGoTabImageView3D::QGoTabImageView3D( QWidget* parent )
{
//   m_ImageView = new QGoImageView3D( this );
}

QGoTabImageView3D::~QGoTabImageView3D( )
{
}

GoFigure::TabDimensionType QGoTabImageView3D::GetTabDimensionType( ) const
{
  return GoFigure::THREE_D;
}