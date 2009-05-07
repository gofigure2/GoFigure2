#include "QGoImageInfo.h"
#include <iostream>

QGoImageInfo::QGoImageInfo( QWidget* parent ) :QWidget( parent )
{
}

QGoImageInfo::~QGoImageInfo()
{
}

void QGoImageInfo::setupUi( QWidget *Form )
{
if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->resize(288, 323);
        scrollArea = new QScrollArea(Form);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(0, -10, 287, 331));
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 281, 325));
        gridLayout = new QGridLayout(scrollAreaWidgetContents);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        TImageStaticlabel = new QLabel(scrollAreaWidgetContents);
        TImageStaticlabel->setObjectName(QString::fromUtf8("TImageStaticlabel"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TImageStaticlabel->sizePolicy().hasHeightForWidth());
        TImageStaticlabel->setSizePolicy(sizePolicy);

        gridLayout->addWidget(TImageStaticlabel, 0, 0, 1, 1);

        DimensionStaticLabel = new QLabel(scrollAreaWidgetContents);
        DimensionStaticLabel->setObjectName(QString::fromUtf8("DimensionStaticLabel"));
        sizePolicy.setHeightForWidth(DimensionStaticLabel->sizePolicy().hasHeightForWidth());
        DimensionStaticLabel->setSizePolicy(sizePolicy);

        gridLayout->addWidget(DimensionStaticLabel, 1, 0, 1, 1);

        DimensionLabel = new QLabel(scrollAreaWidgetContents);
        DimensionLabel->setObjectName(QString::fromUtf8("DimensionLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(DimensionLabel->sizePolicy().hasHeightForWidth());
        DimensionLabel->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(DimensionLabel, 1, 1, 1, 1);

        SizeStaticLabel = new QLabel(scrollAreaWidgetContents);
        SizeStaticLabel->setObjectName(QString::fromUtf8("SizeStaticLabel"));
        sizePolicy.setHeightForWidth(SizeStaticLabel->sizePolicy().hasHeightForWidth());
        SizeStaticLabel->setSizePolicy(sizePolicy);

        gridLayout->addWidget(SizeStaticLabel, 3, 0, 1, 1);

        SizeLabel = new QLabel(scrollAreaWidgetContents);
        SizeLabel->setObjectName(QString::fromUtf8("SizeLabel"));
        sizePolicy1.setHeightForWidth(SizeLabel->sizePolicy().hasHeightForWidth());
        SizeLabel->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(SizeLabel, 3, 1, 1, 1);

        SpacingLabel = new QLabel(scrollAreaWidgetContents);
        SpacingLabel->setObjectName(QString::fromUtf8("SpacingLabel"));
        sizePolicy1.setHeightForWidth(SpacingLabel->sizePolicy().hasHeightForWidth());
        SpacingLabel->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(SpacingLabel, 5, 1, 1, 1);

        MemoryLabel = new QLabel(scrollAreaWidgetContents);
        MemoryLabel->setObjectName(QString::fromUtf8("MemoryLabel"));
        sizePolicy1.setHeightForWidth(MemoryLabel->sizePolicy().hasHeightForWidth());
        MemoryLabel->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(MemoryLabel, 6, 1, 1, 1);

        PixelPosLabel = new QLabel(scrollAreaWidgetContents);
        PixelPosLabel->setObjectName(QString::fromUtf8("PixelPosLabel"));
        sizePolicy1.setHeightForWidth(PixelPosLabel->sizePolicy().hasHeightForWidth());
        PixelPosLabel->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(PixelPosLabel, 8, 1, 1, 1);

        WorldPositionLabel = new QLabel(scrollAreaWidgetContents);
        WorldPositionLabel->setObjectName(QString::fromUtf8("WorldPositionLabel"));

        gridLayout->addWidget(WorldPositionLabel, 9, 1, 1, 1);

        TimeLabel = new QLabel(scrollAreaWidgetContents);
        TimeLabel->setObjectName(QString::fromUtf8("TimeLabel"));
        sizePolicy1.setHeightForWidth(TimeLabel->sizePolicy().hasHeightForWidth());
        TimeLabel->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(TimeLabel, 11, 1, 1, 1);

        SpacingStaticLabel = new QLabel(scrollAreaWidgetContents);
        SpacingStaticLabel->setObjectName(QString::fromUtf8("SpacingStaticLabel"));
        sizePolicy.setHeightForWidth(SpacingStaticLabel->sizePolicy().hasHeightForWidth());
        SpacingStaticLabel->setSizePolicy(sizePolicy);

        gridLayout->addWidget(SpacingStaticLabel, 5, 0, 1, 1);

        MemoryStaticLabel = new QLabel(scrollAreaWidgetContents);
        MemoryStaticLabel->setObjectName(QString::fromUtf8("MemoryStaticLabel"));
        sizePolicy.setHeightForWidth(MemoryStaticLabel->sizePolicy().hasHeightForWidth());
        MemoryStaticLabel->setSizePolicy(sizePolicy);

        gridLayout->addWidget(MemoryStaticLabel, 6, 0, 1, 1);

        PositionLabel = new QLabel(scrollAreaWidgetContents);
        PositionLabel->setObjectName(QString::fromUtf8("PositionLabel"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(PositionLabel->sizePolicy().hasHeightForWidth());
        PositionLabel->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(PositionLabel, 7, 0, 1, 1);

        PixelPosStaticLabel = new QLabel(scrollAreaWidgetContents);
        PixelPosStaticLabel->setObjectName(QString::fromUtf8("PixelPosStaticLabel"));
        sizePolicy2.setHeightForWidth(PixelPosStaticLabel->sizePolicy().hasHeightForWidth());
        PixelPosStaticLabel->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(PixelPosStaticLabel, 8, 0, 1, 1);

        WorldPosStaticLabel = new QLabel(scrollAreaWidgetContents);
        WorldPosStaticLabel->setObjectName(QString::fromUtf8("WorldPosStaticLabel"));
        sizePolicy2.setHeightForWidth(WorldPosStaticLabel->sizePolicy().hasHeightForWidth());
        WorldPosStaticLabel->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(WorldPosStaticLabel, 9, 0, 1, 1);

        ChannelsStaticLabel = new QLabel(scrollAreaWidgetContents);
        ChannelsStaticLabel->setObjectName(QString::fromUtf8("ChannelsStaticLabel"));
        sizePolicy2.setHeightForWidth(ChannelsStaticLabel->sizePolicy().hasHeightForWidth());
        ChannelsStaticLabel->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(ChannelsStaticLabel, 2, 0, 1, 1);

        TTimeStaticLabel = new QLabel(scrollAreaWidgetContents);
        TTimeStaticLabel->setObjectName(QString::fromUtf8("TTimeStaticLabel"));
        sizePolicy2.setHeightForWidth(TTimeStaticLabel->sizePolicy().hasHeightForWidth());
        TTimeStaticLabel->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(TTimeStaticLabel, 10, 0, 1, 1);

        TValueStaticLabel = new QLabel(scrollAreaWidgetContents);
        TValueStaticLabel->setObjectName(QString::fromUtf8("TValueStaticLabel"));
        sizePolicy2.setHeightForWidth(TValueStaticLabel->sizePolicy().hasHeightForWidth());
        TValueStaticLabel->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(TValueStaticLabel, 13, 0, 1, 1);

        ValueStaticLabel = new QLabel(scrollAreaWidgetContents);
        ValueStaticLabel->setObjectName(QString::fromUtf8("ValueStaticLabel"));
        sizePolicy2.setHeightForWidth(ValueStaticLabel->sizePolicy().hasHeightForWidth());
        ValueStaticLabel->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(ValueStaticLabel, 14, 0, 1, 1);

        ValueLabel = new QLabel(scrollAreaWidgetContents);
        ValueLabel->setObjectName(QString::fromUtf8("ValueLabel"));
        sizePolicy1.setHeightForWidth(ValueLabel->sizePolicy().hasHeightForWidth());
        ValueLabel->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(ValueLabel, 14, 1, 1, 1);

        ChannelsLabel = new QLabel(scrollAreaWidgetContents);
        ChannelsLabel->setObjectName(QString::fromUtf8("ChannelsLabel"));
        sizePolicy1.setHeightForWidth(ChannelsLabel->sizePolicy().hasHeightForWidth());
        ChannelsLabel->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(ChannelsLabel, 2, 1, 1, 1);

        TimeSpacingStaticLabel = new QLabel(scrollAreaWidgetContents);
        TimeSpacingStaticLabel->setObjectName(QString::fromUtf8("TimeSpacingStaticLabel"));
        sizePolicy.setHeightForWidth(TimeSpacingStaticLabel->sizePolicy().hasHeightForWidth());
        TimeSpacingStaticLabel->setSizePolicy(sizePolicy);

        gridLayout->addWidget(TimeSpacingStaticLabel, 12, 0, 1, 1);

        TimeSpacingLabel = new QLabel(scrollAreaWidgetContents);
        TimeSpacingLabel->setObjectName(QString::fromUtf8("TimeSpacingLabel"));
        sizePolicy1.setHeightForWidth(TimeSpacingLabel->sizePolicy().hasHeightForWidth());
        TimeSpacingLabel->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(TimeSpacingLabel, 12, 1, 1, 1);

        TimeStaticLabel = new QLabel(scrollAreaWidgetContents);
        TimeStaticLabel->setObjectName(QString::fromUtf8("TimeStaticLabel"));
        sizePolicy2.setHeightForWidth(TimeStaticLabel->sizePolicy().hasHeightForWidth());
        TimeStaticLabel->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(TimeStaticLabel, 11, 0, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void QGoImageInfo::retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
        TImageStaticlabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#0000ff;\">Image :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        DimensionStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">  Dimension :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        DimensionLabel->setText(QString());
        SizeStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">  Size :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        SizeLabel->setText(QString());
        SpacingLabel->setText(QString());
        MemoryLabel->setText(QString());
        PixelPosLabel->setText(QString());
        WorldPositionLabel->setText(QString());
        TimeLabel->setText(QString());
        SpacingStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">  Spacing :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        MemoryStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">  Memory :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        PositionLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#0000ff;\">Position :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        PixelPosStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">  Position (pixel) :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        WorldPosStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">  Position (um) :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        ChannelsStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">  # Channels : </span></p></body></html>", 0, QApplication::UnicodeUTF8));
        TTimeStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#0000ff;\">Time :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        TValueStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#0000ff;\">Value :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        ValueStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">  Pixel value :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        ValueLabel->setText(QString());
        ChannelsLabel->setText(QString());
        TimeSpacingStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">  Time Spacing :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        TimeSpacingLabel->setText(QString());
        TimeStaticLabel->setText(QApplication::translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">  Time Point :</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Form);
    } // retranslateUi


void QGoImageInfo::setDimension( const unsigned int& dim )
{
  m_Dimension = dim;
  this->DimensionLabel->setText( QString( "%1" ).arg( m_Dimension ) );

  bool space_time = ( dim >= 3 );
  this->TTimeStaticLabel->setVisible( space_time );
  this->TimeStaticLabel->setVisible( space_time );
  this->TimeLabel->setVisible( space_time );
  this->TimeSpacingStaticLabel->setVisible( space_time );
  this->TimeSpacingLabel->setVisible( space_time );

  if( space_time )
  {
    m_Size.resize( 3 );
    m_Spacing.resize( 3 );
    m_PPos.resize( 3 );
    m_WPos.resize( 3 );
  }
  else
  {
    m_Size.resize( dim );
    m_Spacing.resize( dim );
    m_PPos.resize( dim );
    m_WPos.resize( dim );
  }
}

void QGoImageInfo::setNumberOfChannel( const unsigned int& channel )
{
  m_Channel = channel;
  this->ChannelsLabel->setText( QString( "%1" ).arg( m_Channel ) );

  m_Value.resize( channel );
}

void QGoImageInfo::setSize( const std::vector< unsigned int >& iSize )
{
  if( !m_Size.empty() )
  {
    if( iSize.size() == m_Size.size() )
    {
      m_Size = iSize;
      this->SizeLabel->setText( ConvertToQString( m_Size ) );
    }
    else
    {
      std::cout <<"iSize.size() != m_Size.size()" <<std::endl;
    }
  }
  else
  {
    std::cout <<"Please setDimension first!" <<std::endl;
  }
}

void QGoImageInfo::setSpacing( const std::vector< float >& iSpacing )
{
  if( !m_Spacing.empty() )
  {
    if( iSpacing.size() == m_Spacing.size() )
    {
      m_Spacing = iSpacing;
      this->SpacingLabel->setText( ConvertToQString( m_Spacing ) );
    }
    else
    {
      std::cout <<"iSpacing.size() != m_Spacing.size()" <<std::endl;
    }
  }
  else
  {
    std::cout <<"Please setDimension first!" <<std::endl;
  }
}

void QGoImageInfo::setPixelPosition( const std::vector< unsigned int >& iPos )
{
  if( !m_PPos.empty() )
  {
    if( iPos.size() == m_PPos.size() )
    {
      m_PPos = iPos;
      this->PositionLabel->setText( ConvertToQString( m_PPos ) );
    }
    else
    {
      std::cout <<"iPos.size() != m_PPos.size()" <<std::endl;
    }
  }
  else
  {
    std::cout <<"Please setDimension first!" <<std::endl;
  }
}

void QGoImageInfo::setWorldPosision( const std::vector< float >& iPos )
{
  if( !m_WPos.empty() )
  {
    if( iPos.size() == m_WPos.size() )
    {
      m_WPos = iPos;
      this->WorldPositionLabel->setText( ConvertToQString( m_WPos ) );
    }
    else
    {
      std::cout <<"iPos.size() != m_WPos.size()" <<std::endl;
    }
  }
  else
  {
    std::cout <<"Please setDimension first!" <<std::endl;
  }
}

void QGoImageInfo::setTimePoint( const float& iTime )
{
  m_TimePoint = iTime;
  this->TimeLabel->setText( QString( "%1" ).arg( m_TimePoint ) );
}

void QGoImageInfo::setTimeSpacing( const float& iSpacing )
{
  m_TimeSpacing = iSpacing;
  this->TimeSpacingLabel->setText( QString( "%1" ).arg( m_TimePoint ) );
}

void QGoImageInfo::setValue( const std::vector< float >& iVal )
{
  QString val;

  if( !m_Value.empty() )
  {
    if( iVal.size() == m_Value.size() )
    {
      m_Value = iVal;
      this->ValueLabel->setText( ConvertToQString( m_Value ) );
    }
    else
    {
      std::cout <<"iVal.size() != m_Value.size()" <<std::endl;
    }
  }
  else
  {
    std::cout <<"Please setDimension first!" <<std::endl;
  }
}
