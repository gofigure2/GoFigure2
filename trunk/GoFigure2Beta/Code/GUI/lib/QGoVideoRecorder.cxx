#include "QGoVideoRecorder.h"

#include <QFileDialog>

#include "QGoImageView3D.h"

#include "vtkRenderWindow.h"
#include "vtkFFMPEGRenderWindowRecorder.h"

//---------------------------------------------------//
//                                                   //
//               CONSTRUCTOR/DESTRUCTOR              //
//                                                   //
//---------------------------------------------------//

/**
 * \brief Constructor
 */

QGoVideoRecorder::
QGoVideoRecorder(QWidget *iParent) : QDockWidget( iParent ), m_XMin( 0 ), m_XFixed( 0 ),
  m_XMax( 100 ), m_YMin(0), m_YFixed( 0 ), m_YMax( 100 ), m_ZMin( 0 ), m_ZFixed( 0 ), m_ZMax( 100 ),
  m_TMin( 0 ), m_TFixed( 0 ), m_TMax( 100 ), m_RecordX( 0 ), m_RecordY( 0 ),
  m_RecordZ( 0 ), m_RecordTX( 0 ), m_RecordTY( 0 ), m_RecordTZ( 0 ),
  m_WindowSelected(0), m_VideoName2( "" ),m_FrameRate2( 30 ),
  m_VideoQuality2 ( 1 ), m_FrameCounter( 0 ), m_RendererWindow(0),iRenderingWindowTEST(NULL),
  m_RenderWindowSelected( false )
{
	this->setupUi(this);

  m_VideoRecorder2 = vtkFFMPEGRenderWindowRecorder::New();

  m_InternalTimer = new QTimer( this );
  QObject::connect( m_InternalTimer, SIGNAL(timeout()),this, SLOT(timeout()) );

  //Initalize chrono with .00 (not 0)
  QString value = "0.00";
  this->videoLenght->display(value);
  this->endRecord->setEnabled(false);

  this->videoName->setReadOnly( true );
  this->videoName_2->setReadOnly( true );

  this->tabVideoWidget->setEnabled(false);



}

/**
 * \brief Destructor
 */
QGoVideoRecorder::
~QGoVideoRecorder( )
{
	delete m_InternalTimer;
}

//---------------------------------------------------//
//                                                   //
//                    METHODS                        //
//                                                   //
//---------------------------------------------------//

///////////////////////////////////////////////////////
//               CREATE VIDEO TAB
///////////////////////////////////////////////////////

void
QGoVideoRecorder::
SetXMinAndMax( int XMin, int XMax )
{
	this->xSpinMin->setMinimum( XMin );
	this->xSpinMax->setMinimum( XMin );
	this->xSpinFixed->setMinimum( XMin );

	this->xSpinMin->setMaximum( XMax );
	this->xSpinMax->setMaximum( XMax );
	this->xSpinFixed->setMaximum( XMax );


	this->xSpinMin->setValue(XMin);
	this->xSpinMax->setValue(XMax);
}
void
QGoVideoRecorder::
SetXSlice( int X )
{
	this->xSpinFixed->setValue( X );
}

void
QGoVideoRecorder::
SetYMinAndMax( int YMin, int YMax )
{
	this->ySpinMin->setMinimum( YMin );
	this->ySpinMax->setMinimum( YMin );
	this->ySpinFixed->setMinimum( YMin );

	this->ySpinMin->setMaximum( YMax );
	this->ySpinMax->setMaximum( YMax );
	this->ySpinFixed->setMaximum( YMax );


	this->ySpinMin->setValue(YMin);
	this->ySpinMax->setValue(YMax);
}
void
QGoVideoRecorder::
SetYSlice( int Y )
{
	this->ySpinFixed->setValue( Y );
}

void
QGoVideoRecorder::
SetZMinAndMax( int ZMin, int ZMax )
{
	this->zSpinMin->setMinimum( ZMin );
	this->zSpinMax->setMinimum( ZMin );
	this->zSpinFixed->setMinimum( ZMin );

	this->zSpinMin->setMaximum( ZMax );
	this->zSpinMax->setMaximum( ZMax );
	this->zSpinFixed->setMaximum( ZMax );


	this->zSpinMin->setValue(ZMin);
	this->zSpinMax->setValue(ZMax);
}
void
QGoVideoRecorder::
SetZSlice( int Z )
{
	this->zSpinFixed->setValue( Z );
}

void
QGoVideoRecorder::
SetTMinAndMax( int TMin, int TMax )
{
	this->tSpinMin->setMinimum( TMin );
	this->tSpinMax->setMinimum( TMin );
	this->tSpinFixed->setMinimum( TMin );

	this->tSpinMin->setMaximum( TMax );
	this->tSpinMax->setMaximum( TMax );
	this->tSpinFixed->setMaximum( TMax );


	this->tSpinMin->setValue(TMin);
	this->tSpinMax->setValue(TMax);
}
void
QGoVideoRecorder::
SetTSlice( int T )
{
	this->tSpinFixed->setValue( T );
}


//---------------------------------------------------//
//                                                   //
//                    SIGNALS                        //
//                                                   //
//---------------------------------------------------//

///////////////////////////////////////////////////////
//               CREATE VIDEO TAB
///////////////////////////////////////////////////////

/**
 * \brief Function called when checkbox state changes
 */
void
QGoVideoRecorder::
on_xSliceCheckBox_stateChanged( int state )
{
	m_RecordX = state;
}

/**
 * \brief Function called when checkbox state changes
 */
void
QGoVideoRecorder::
on_ySliceCheckBox_stateChanged( int state )
{
	m_RecordY = state;
}

/**
 * \brief Function called when checkbox state changes
 */
void
QGoVideoRecorder::
on_zSliceCheckBox_stateChanged( int state )
{
	m_RecordZ = state;
}

/**
 * \brief Function called when checkbox state changes
 */
void
QGoVideoRecorder::
on_xtSliceCheckBox_stateChanged( int state )
{
	m_RecordTX = state;
}

/**
 * \brief Function called when checkbox state changes
 */
void
QGoVideoRecorder::
on_ytSliceCheckBox_stateChanged( int state )
{
	m_RecordTY = state;
}

/**
 * \brief Function called when checkbox state changes
 */
void
QGoVideoRecorder::
on_ztSliceCheckBox_stateChanged( int state )
{
	m_RecordTZ = state;
}

/**
 * \brief Function called when first slice changes
 */
void
QGoVideoRecorder::
on_xSpinMin_valueChanged( int value )
{
	m_XMin = value;
}

/**
 * \brief Function called when first slice changes
 */
void
QGoVideoRecorder::
on_ySpinMin_valueChanged( int value )
{
	m_YMin = value;
}
/**
 * \brief Function called when first slice changes
 */
void
QGoVideoRecorder::
on_zSpinMin_valueChanged( int value )
{
	m_ZMin = value;
}

/**
 * \brief Function called when first slice changes
 */
void
QGoVideoRecorder::
on_tSpinMin_valueChanged( int value )
{
	m_TMin = value;
}

/**
 * \brief Function called when last slice changes
 */
void
QGoVideoRecorder::
on_xSpinMax_valueChanged( int value )
{
	m_XMax = value;
}

/**
 * \brief Function called when last slice changes
 */
void
QGoVideoRecorder::
on_ySpinMax_valueChanged( int value )
{
	m_YMax = value;
}
/**
 * \brief Function called when last slice changes
 */
void
QGoVideoRecorder::
on_zSpinMax_valueChanged( int value )
{
	m_ZMax = value;
}

/**
 * \brief Function called when last slice changes
 */
void
QGoVideoRecorder::
on_tSpinMax_valueChanged( int value )
{
	m_TMax = value;
}

/**
 * \brief Function called when slice changes
 */
void
QGoVideoRecorder::
on_xSpinFixed_valueChanged( int value )
{
	m_XFixed = value;
}

/**
 * \brief Function called when slice changes
 */
void
QGoVideoRecorder::
on_ySpinFixed_valueChanged( int value )
{
	m_YFixed = value;
}

/**
 * \brief Function called when slice changes
 */
void
QGoVideoRecorder::
on_zSpinFixed_valueChanged( int value )
{
	m_ZFixed = value;
}

/**
 * \brief Function called when slice changes
 */
void
QGoVideoRecorder::
on_tSpinFixed_valueChanged( int value )
{
	m_TFixed = value;
}

/**
 * \brief Get and print the location to store MegaCapture file
 */
void
QGoVideoRecorder::
on_createFile_clicked()
{
	m_VideoName2 = QFileDialog::getSaveFileName( this,
	      tr( "Folder to Save Video" ), "fileName", 0 );

	this->videoName->setText( m_VideoName2 );
}

/**
 * \brief Function called when FrameRate changes
 */
void
QGoVideoRecorder::
on_frameRate_valueChanged( int value )
{
	m_FrameRate2= value;
	this->frameRate_2->setValue(value);
    // update video length

}

/**
 * \brief Function called when VideoQuality changes
 */
void
QGoVideoRecorder::
on_videoQuality_valueChanged( int value )
{
	m_VideoQuality2= value;
	this->videoQuality_2->setValue(value);
}


/**
 * \brief Function called when "Create video" clicked
 */

void
QGoVideoRecorder::
on_startVideo_clicked()
{
//something to record first view
  if( ( m_VideoName2 == NULL ) || ( !m_RenderWindowSelected ) || ( m_RendererWindow == 0 ) )
  {
    std::cout<<"Please select a videoName and a good rendering window"<<std::endl;
  }
  else
  {
  // update values in thread
    m_VideoRecorder2->SetRenderingWindow(iRenderingWindowTEST);
    m_VideoRecorder2->Setm_FrameRate( m_FrameRate2 );
    m_VideoRecorder2->Setm_VideoQuality( m_VideoQuality2 );

    if(m_RecordX>0)
  	  {
      QString fileName = m_VideoName2;

      fileName.insert( fileName.size(), QString("-X-"));
      fileName.insert( fileName.size(), QString::number( m_XMin, 10 ) );
      fileName.insert( fileName.size(), QString("-"));
      fileName.insert( fileName.size(), QString::number( m_XMax, 10 ) );
      fileName.insert( fileName.size(), QString(".avi"));

      m_VideoRecorder2->SetFileName( fileName.toStdString() );

      m_VideoRecorder2->StartCapture();

      for(unsigned int i = m_XMin; i < m_XMax+1; i++)
        {
        //send signal to gofigure to change slice
        emit XSliceChanged(i);
        //capture screen
        m_VideoRecorder2->TakeSnapshot();
        i++;
        }

        m_VideoRecorder2->EndCapture();
      }

    if(m_RecordY>0)
      {
      QString fileName = m_VideoName2;

      fileName.insert( fileName.size(), QString("-Y-"));
      fileName.insert( fileName.size(), QString::number( m_YMin, 10 ) );
      fileName.insert( fileName.size(), QString("-"));
      fileName.insert( fileName.size(), QString::number( m_YMax, 10 ) );
      fileName.insert( fileName.size(), QString(".avi"));

       m_VideoRecorder2->SetFileName( fileName.toStdString() );

       m_VideoRecorder2->StartCapture();

       for(unsigned int i = m_YMin; i < m_YMax+1; i++)
         {
         //send signal to gofigure to change slice
         emit YSliceChanged(i);
         //capture screen
         m_VideoRecorder2->TakeSnapshot();
         i++;
         }
       m_VideoRecorder2->EndCapture();
       }

    if(m_RecordZ>0)
      {
      QString fileName = m_VideoName2;

      fileName.insert( fileName.size(), QString("-Z-"));
      fileName.insert( fileName.size(), QString::number( m_ZMin, 10 ) );
      fileName.insert( fileName.size(), QString("-"));
      fileName.insert( fileName.size(), QString::number( m_ZMax, 10 ) );
      fileName.insert( fileName.size(), QString(".avi"));

       m_VideoRecorder2->SetFileName( fileName.toStdString() );

       m_VideoRecorder2->StartCapture();

       for(unsigned int i = m_ZMin; i < m_ZMax+1; i++)
         {
         //send signal to gofigure to change slice
         emit ZSliceChanged(i);
         //capture screen
         m_VideoRecorder2->TakeSnapshot();
         i++;
         }
       m_VideoRecorder2->EndCapture();
       }

    //ADD T
  }


}

///////////////////////////////////////////////////////
//               RECORD VIDEO TAB
///////////////////////////////////////////////////////


/**
 * \brief Get and print the location to store MegaCapture file
 */
void
QGoVideoRecorder::
on_createFile_2_clicked()
{
	m_VideoName2 = QFileDialog::getSaveFileName( this,
	      tr( "Folder to Save Video" ), "fileName", 0 );

	this->videoName_2->setText( m_VideoName2 );
}

void
QGoVideoRecorder::
on_frameRate_2_valueChanged( int value )
{
	m_FrameRate2 = value;
	this->frameRate->setValue(value);
}

/**
 * \brief Function called when VideoQuality changes
 */
void
QGoVideoRecorder::
on_videoQuality_2_valueChanged( int value )
{
	m_VideoQuality2 = value;
	this->videoQuality->setValue(value);
}

/**
 * \brief Function called when "start recording" clicked
 */
void
QGoVideoRecorder::
on_startRecord_clicked()
{

  // Print parameters for testings
	std::cout<<"m_VideoName2: "<< m_VideoName2.toStdString() <<std::endl;
	std::cout<<"m_FrameRate2: "<< m_FrameRate2 <<std::endl;
	std::cout<<"m_VideoQuality2: "<< m_VideoQuality2 <<std::endl;

  if( ( m_VideoName2 == NULL ) || ( !m_RenderWindowSelected ) || ( m_RendererWindow == 0 ) )
  {
  // TODO: display pop up window
    std::cout<<"Please select a videoName and a good rendering window"<<std::endl;
  }
  else
  {
  this->videoName_2->setEnabled(false);
  this->createFile_2->setEnabled(false);
  this->frameRate_2->setEnabled(false);
  this->videoQuality_2->setEnabled(false);
  this->startRecord->setEnabled(false);
  this->tabVideoMethod1->setEnabled(false);
  this->endRecord->setEnabled(true);

  m_VideoRecorder2->SetRenderingWindow(iRenderingWindowTEST);

  QString fileName = m_VideoName2;

  fileName.insert( fileName.size(), QString(".avi"));

  m_VideoRecorder2->SetFileName( fileName.toStdString() );
  m_VideoRecorder2->Setm_FrameRate( m_FrameRate2 );
  m_VideoRecorder2->Setm_VideoQuality( m_VideoQuality2 );
  std::cout<<"FileName : "<< fileName.toStdString() << std::endl;
  m_VideoRecorder2->StartCapture();
  m_InternalTimer->start( 1000/m_FrameRate2 );
  }
}

/**
 * \brief Function called when "end recording" clicked
 */
void
QGoVideoRecorder::
on_endRecord_clicked()
{
  m_VideoRecorder2->EndCapture();
  m_InternalTimer->stop();
  m_FrameCounter = 0;

  	this->videoName_2->setEnabled(true);
  	this->createFile_2->setEnabled(true);
  	this->frameRate_2->setEnabled(true);
  	this->videoQuality_2->setEnabled(true);
  	this->startRecord->setEnabled(true);
  	this->tabVideoMethod1->setEnabled(true);
  	this->endRecord->setEnabled(false);
}

void
QGoVideoRecorder::
timeout()
{
  m_VideoRecorder2->TakeSnapshot();

  ++m_FrameCounter;

  // for a better visualisation, always show 2 decimal
  double doubleCounter;
  doubleCounter = (double)m_FrameCounter/(double)m_FrameRate2;

  int test = 100*doubleCounter;
  QString value = QString::number( test, 10 );

  if ( test >= 10 )
  	{
    value.insert( value.size()-2, QString("."));
  	}
  else
  	{
    value.insert( value.size()-1, QString(".0"));
  	}

  if(test<100)
  	{
    value.insert( value.size()-3, QString("0"));
  	}
  this->videoLenght->display(value);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetRendererWindow( int iSlice )
{
  m_RendererWindow = iSlice;
  if ( m_RendererWindow == 0 )
	  {
      this->tabVideoWidget->setEnabled(false);
	  }
  else
	  {
      this->tabVideoWidget->setEnabled(true);
	  }
}
//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetRenderingWindow2( vtkRenderWindow* iRenderingWindow )
{
	iRenderingWindowTEST = iRenderingWindow;
	if(iRenderingWindowTEST != NULL)
	{
	m_RenderWindowSelected = true;
	m_VideoRecorder2->SetRenderingWindow(iRenderingWindowTEST);
	}
}

//----------------------------------------------------------------------------//
