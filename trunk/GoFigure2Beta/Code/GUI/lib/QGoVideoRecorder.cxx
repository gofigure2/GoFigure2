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
	m_VideoRecorder2->Delete();
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
  m_XMin = XMin;
  m_XFixed = XMin;
  m_XMax = XMax;

  m_XMinForVideo = XMin;
  m_XMaxForVideo = XMax;

  //Initialization of the spinbox
  this->tSpinMin_2->setMinimum(m_XMin);
  this->tSpinMin_2->setMaximum(m_XMax);
  this->tSpinMin_2->setValue(m_XMin);

  this->tSpinMax_2->setMinimum(m_XMin);
  this->tSpinMax_2->setMaximum(m_XMax);
  this->tSpinMax_2->setValue(m_XMax);
}
void
QGoVideoRecorder::
SetXSlice( int X )
{
  m_XFixed = X;
}

void
QGoVideoRecorder::
SetYMinAndMax( int YMin, int YMax )
{
  m_YMin = YMin;
  m_YFixed = YMin;
  m_YMax = YMax;

  m_YMinForVideo = YMin;
  m_YMaxForVideo = YMax;
}
void
QGoVideoRecorder::
SetYSlice( int Y )
{
  m_YFixed = Y;
}

void
QGoVideoRecorder::
SetZMinAndMax( int ZMin, int ZMax )
{
  m_ZMin = ZMin;
  m_ZFixed = ZMin;
  m_ZMax = ZMax;

  m_ZMinForVideo = ZMin;
  m_ZMaxForVideo = ZMax;
}
void
QGoVideoRecorder::
SetZSlice( int Z )
{
  m_ZFixed = Z;
}

void
QGoVideoRecorder::
SetTMinAndMax( int TMin, int TMax )
{
  m_TMin = TMin;
  m_TFixed = TMin;
  m_TMax = TMax;

  m_TMinForVideo = TMin;
  m_TMaxForVideo = TMax;
}
void
QGoVideoRecorder::
SetTSlice( int T )
{
  m_TFixed = T;
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
 * \brief Get and print the location to store MegaCapture file
 */
void
QGoVideoRecorder::
on_SliceFT_activated( int value)
{
  UpdateQSpinBoxFT( value );
}

void
QGoVideoRecorder::
UpdateQSpinBoxFT( int value )
{
  switch ( value ) {
    case 0 :
    // X Slice
    this->tSpinMin_2->setMinimum(m_XMin);
    this->tSpinMin_2->setMaximum(m_XMax);
    this->tSpinMin_2->setValue(m_XMin);

    this->tSpinMax_2->setMinimum(m_XMin);
    this->tSpinMax_2->setMaximum(m_XMax);
    this->tSpinMax_2->setValue(m_XMax);

    m_SliceFT = 0;
    break;

    case 1 :
    // Y Slice
    this->tSpinMin_2->setMinimum(m_YMin);
    this->tSpinMin_2->setMaximum(m_YMax);
    this->tSpinMin_2->setValue(m_YMin);

    this->tSpinMax_2->setMinimum(m_YMin);
    this->tSpinMax_2->setMaximum(m_YMax);
    this->tSpinMax_2->setValue(m_YMax);

    m_SliceFT = 1;
    break;

    default :
    // Z Slice
    this->tSpinMin_2->setMinimum(m_ZMin);
    this->tSpinMin_2->setMaximum(m_ZMax);
    this->tSpinMin_2->setValue(m_ZMin);

    this->tSpinMax_2->setMinimum(m_ZMin);
    this->tSpinMax_2->setMaximum(m_ZMax);
    this->tSpinMax_2->setValue(m_ZMax);

    m_SliceFT = 2;
	}
}


void
QGoVideoRecorder::
on_tSpinMin_2_valueChanged( int value )
{
  switch ( m_SliceFT )
    {
    case 0 :
    // X Slice
    m_XMinForVideo = value;
    break;

    case 1 :
    // Y Slice
    m_YMinForVideo = value;
    break;

    default :
    // Z Slice
    m_ZMinForVideo = value;
    }
}

void
QGoVideoRecorder::
on_tSpinMax_2_valueChanged( int value )
{
  switch ( m_SliceFT )
    {
    case 0 :
    // X Slice
    m_XMaxForVideo = value;
    break;

	    case 1 :
    // Y Slice
    m_YMaxForVideo = value;
    break;

    default :
    // Z Slice
    m_ZMaxForVideo = value;
    }
}

/**
 * \brief Get and print the location to store MegaCapture file
 */
void
QGoVideoRecorder::
on_tSpinFixed_valueChanged( int value)
{
  m_TForVideo = value;
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
    //check selected tab
    std::cout << "index: " << this->tabWidget->currentIndex () << std::endl;

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

    if( m_SliceFT == 0 )
  	  {
      QString fileName = m_VideoName2;

      fileName.insert( fileName.size(), QString("-X-"));
      fileName.insert( fileName.size(), QString::number( m_XMin, 10 ) );
      fileName.insert( fileName.size(), QString("-"));
      fileName.insert( fileName.size(), QString::number( m_XMax, 10 ) );
      fileName.insert( fileName.size(), QString(".avi"));
      m_VideoRecorder2->SetFileName( fileName.toStdString() );

      m_VideoRecorder2->StartCapture();

      for(unsigned int i = m_XMinForVideo; i < m_XMaxForVideo+1; i++)
        {
        //send signal to gofigure to change slice
        emit XSliceChanged(i);
        //capture screen
        m_VideoRecorder2->TakeSnapshot();
        }

        m_VideoRecorder2->EndCapture();
      }

    if( m_SliceFT == 1 )
      {
      QString fileName = m_VideoName2;

      fileName.insert( fileName.size(), QString("-Y-"));
      fileName.insert( fileName.size(), QString::number( m_YMin, 10 ) );
      fileName.insert( fileName.size(), QString("-"));
      fileName.insert( fileName.size(), QString::number( m_YMax, 10 ) );
      fileName.insert( fileName.size(), QString(".avi"));

       m_VideoRecorder2->SetFileName( fileName.toStdString() );

       m_VideoRecorder2->StartCapture();

       for(unsigned int i = m_YMinForVideo; i < m_YMaxForVideo+1; i++)
         {
         //send signal to gofigure to change slice
         emit YSliceChanged(i);
         //capture screen
         m_VideoRecorder2->TakeSnapshot();
         }
       m_VideoRecorder2->EndCapture();
       }

    if( m_SliceFT == 2 )
      {
      QString fileName = m_VideoName2;

      fileName.insert( fileName.size(), QString("-Z-"));
      fileName.insert( fileName.size(), QString::number( m_ZMin, 10 ) );
      fileName.insert( fileName.size(), QString("-"));
      fileName.insert( fileName.size(), QString::number( m_ZMax, 10 ) );
      fileName.insert( fileName.size(), QString(".avi"));

       m_VideoRecorder2->SetFileName( fileName.toStdString() );

       m_VideoRecorder2->StartCapture();

       for(unsigned int i = m_ZMinForVideo; i < m_ZMaxForVideo+1; i++)
         {
         //send signal to gofigure to change slice
         emit ZSliceChanged(i);
         //capture screen
         m_VideoRecorder2->TakeSnapshot();
         }
       m_VideoRecorder2->EndCapture();
       }
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
