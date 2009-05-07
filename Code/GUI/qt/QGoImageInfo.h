#ifndef __QGoImageInfo_h
#define __QGoImageInfo_h

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>
#include <QtGui/QWidget>

class QGoImageInfo : public QWidget
{
  Q_OBJECT
public:
  explicit QGoImageInfo( QWidget* parent );
  virtual ~QGoImageInfo( );

public slots:
  void setDimension( const unsigned int& );
  void setNumberOfChannel( const unsigned int& );
  void setSize( const std::vector< unsigned int >& );
  void setSpacing( const std::vector< float >& );
  void setPixelPosition( const std::vector< unsigned int >& );
  void setWorldPosision( const std::vector< float >& );
  void setTimePoint( const float& );
  void setTimeSpacing( const float& );
  void setValue( const std::vector< float >& );


protected:
QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout;
    QLabel *TImageStaticlabel;
    QLabel *DimensionStaticLabel;
    QLabel *DimensionLabel;
    QLabel *SizeStaticLabel;
    QLabel *SizeLabel;
    QLabel *SpacingLabel;
    QLabel *MemoryLabel;
    QLabel *PixelPosLabel;
    QLabel *WorldPositionLabel;
    QLabel *TimeLabel;
    QLabel *SpacingStaticLabel;
    QLabel *MemoryStaticLabel;
    QLabel *PositionLabel;
    QLabel *PixelPosStaticLabel;
    QLabel *WorldPosStaticLabel;
    QLabel *ChannelsStaticLabel;
    QLabel *TTimeStaticLabel;
    QLabel *TValueStaticLabel;
    QLabel *ValueStaticLabel;
    QLabel *ValueLabel;
    QLabel *ChannelsLabel;
    QLabel *TimeSpacingStaticLabel;
    QLabel *TimeSpacingLabel;
    QLabel *TimeStaticLabel;

  unsigned int m_Dimension;
  unsigned int m_Channel;
  std::vector< unsigned int > m_Size;
  std::vector< float >  m_Spacing;
  std::vector< unsigned int > m_PPos;
  std::vector< float > m_WPos;
  std::vector< float > m_Value;
  float m_TimeSpacing;
  float m_TimePoint;

  template< class TContainer >
  QString ConvertToQString( const TContainer& iVector )
  {
    if( iVector.empty() )
      return QString();
    else
    {
      size_t size = iVector.size();

      if( size == 1 )
      {
        return QString( "%1" ).arg( *iVector.begin() );
      }
      else
      {
        QString v( "[ ");
        unsigned int i = 0;
        for( typename TContainer::const_iterator it = iVector.begin();
             it != iVector.end();
             ++it, ++i )
        {
          if( i == ( size - 1 ) )
            v.append( QString( "%1 ]" ).arg( *it ) );
          else
            v.append( QString( "%1 ; " ).arg( *it ) );
        }
        return v;
      }
    }
  }

  void setupUi( QWidget* Form );
  void retranslateUi(QWidget* Form);
};

#endif
