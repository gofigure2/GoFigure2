#include "QGoManualSegmentationDockWidget.h"

QGoManualSegmentationDockWidget::
QGoManualSegmentationDockWidget( QWidget* parent ) : QDockWidget( parent )
{
//   m_SettingsDialog = new QGoManualSegmentationSettingsDialog( this );
//   QObject::connect( m_SettingsDialog, SIGNAL( accepted() ),
//     this,
//     SLOT(/*here define the right slot that will update color, line width*/);

//   QObject::connect( this->SettingsBtn, SIGNAL( pressed() ),
//     m_SettingsDialog, SLOT( show() ) );
}

QGoManualSegmentationDockWidget::
~QGoManualSegmentationDockWidget()
{
}

void
QGoManualSegmentationDockWidget::
OnSetColorPressed()
{
//   m_ValidatedColor = QColorDialog::getColor( m_ValidatedColor, this,
//     tr( "Pick a color for validated contours" ) );
}

// QColor
// QGoManualSegmentationDockWidget::
// GenerateRandomColorForGivenId( const unsigned int& iId,
//   const QColor& iSelectedColor )
// {
//   int h, s, v;
//   iSelectedColor.getHsv( &h, &s, &v );
//
//   int min_hue_diff = 360;
//   int closest_hue = -1;
//   int hue;
//
//   for( int i = 0; i < 6; i++ )
//     {
//     hue = i * 60;
//     diff = vnl_math_abs( h - hue );
//     if( diff < min_hue_diff )
//       {
//       min_hue_diff = diff;
//       closest_hue = hue;
//       }
//     }
//
//   if( iId < 6 )
//     {
//     if( iId * 60 != hue )
//       {
//       oColor.setHsv( iId * 60, 255, 255 );
//       }
//     else
//       {
//
//       }
//     }
//
// }
