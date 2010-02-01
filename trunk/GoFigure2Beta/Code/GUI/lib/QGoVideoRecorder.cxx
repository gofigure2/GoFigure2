#include "QGoVideoRecorder.h"

#include <QFileDialog>

#include "QGoImageView3D.h"

#include "vtkRenderWindow.h"
#include "vtkFFMPEGRenderWindowRecorder.h"


//-------------------------------------------------------------------------
///\todo move all doxygen documentation in the header file!

/**
 * \brief Constructor
 */

//-------------------------------------------------------------------------

QGoVideoRecorder::
QGoVideoRecorder( QWidget *iParent ) : QDockWidget( iParent )
/*, m_XMin( 0 ),
  m_YMax( 100 ), m_ZMin( 0 ), m_ZMax( 100 ), m_TMin( 0 ), m_TMax( 100 ),
  m_WindowSelected( 0 ), m_FrameRate2( 30 ),
  m_VideoQuality2 ( 1 ), m_FrameCounter( 0 ), m_RenderWindowSelected( false )*/
{
  this->setupUi( this );

  m_InternalTimer = new QTimer( this );

  //Initalize chrono with .00 (not 0)
  QString value = "0.00";
  this->videoLenght->display( value );
  this->endRecord->setEnabled( false );

  this->tabVideoWidget->setEnabled( false );
}

//-------------------------------------------------------------------------

/**
 * \brief Destructor
 */

//-------------------------------------------------------------------------

QGoVideoRecorder::
~QGoVideoRecorder( )
{
}

//-------------------------------------------------------------------------

/**
 * \brief Set value XMin and XMax for a given image (useful for spin boxes)
 * update content of spin box (X default Slice)
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
SetXMinAndMax( int XMin, int XMax )
{
  m_XMin = XMin;
  m_XMax = XMax;

  m_XMinForVideo = XMin;
  m_XMaxForVideo = XMax;

  //Initialization of the spinboxes
  this->tSpinMin_2->setMinimum( m_XMin );
  this->tSpinMin_2->setMaximum( m_XMax );
  this->tSpinMin_2->setValue( m_XMin );

  this->tSpinMax_2->setMinimum( m_XMin );
  this->tSpinMax_2->setMaximum( m_XMax );
  this->tSpinMax_2->setValue( m_XMax );
}


//-------------------------------------------------------------------------

/**
 * \brief Set value YMin and YMax for a given image (useful for spin boxes)
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
SetYMinAndMax( int YMin, int YMax )
{
  m_YMin = YMin;
  m_YMax = YMax;

  m_YMinForVideo = YMin;
  m_YMaxForVideo = YMax;
}


//-------------------------------------------------------------------------

/**
 * \brief Set value ZMin and ZMax for a given image (useful for spin boxes)
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
SetZMinAndMax( int ZMin, int ZMax )
{
  m_ZMin = ZMin;
  m_ZMax = ZMax;

  m_ZMinForVideo = ZMin;
  m_ZMaxForVideo = ZMax;
}

//-------------------------------------------------------------------------

/**
 * \brief Set value TMin and TMax for a given image (useful for spin boxes)
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
SetTMinAndMax( int TMin, int TMax )
{
  m_TMin = TMin;
  m_TMax = TMax;

  m_TMinForVideo = TMin;
  m_TMaxForVideo = TMax;

  this->tSpinMin->setMinimum( m_TMin );
  this->tSpinMin->setMaximum( m_TMax );
  this->tSpinMin->setValue( m_TMin );

  this->tSpinMax->setMinimum( m_TMin );
  this->tSpinMax->setMaximum( m_TMax );
  this->tSpinMax->setValue( m_TMax );
}

//-------------------------------------------------------------------------

/**
 * \brief Update content of spin box depending of moving slice (X,Y,Z) ( T Fixed )
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
UpdateQSpinBoxFT( int value )
{
  switch ( value )
    {
    case 0 :
      {
      // X Slice
      this->tSpinMin_2->setMinimum(m_XMin);
      this->tSpinMin_2->setMaximum(m_XMax);
      this->tSpinMin_2->setValue(m_XMin);

      this->tSpinMax_2->setMinimum(m_XMin);
      this->tSpinMax_2->setMaximum(m_XMax);
      this->tSpinMax_2->setValue(m_XMax);

      m_SliceFT = 0;
      break;
      }

    case 1 :
      {
      // Y Slice
      this->tSpinMin_2->setMinimum(m_YMin);
      this->tSpinMin_2->setMaximum(m_YMax);
      this->tSpinMin_2->setValue(m_YMin);

      this->tSpinMax_2->setMinimum(m_YMin);
      this->tSpinMax_2->setMaximum(m_YMax);
      this->tSpinMax_2->setValue(m_YMax);

      m_SliceFT = 1;
      break;
      }

    default:
    case 2:
      {
      // Z Slice
      this->tSpinMin_2->setMinimum(m_ZMin);
      this->tSpinMin_2->setMaximum(m_ZMax);
      this->tSpinMin_2->setValue(m_ZMin);

      this->tSpinMax_2->setMinimum(m_ZMin);
      this->tSpinMax_2->setMaximum(m_ZMax);
      this->tSpinMax_2->setValue(m_ZMax);

      m_SliceFT = 2;
      break;
      }
	}
}

//-------------------------------------------------------------------------

/**
 * \brief Update content of spin box depending of moving slice (X,Y,Z)
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
on_SliceFT_activated( int value)
{
  UpdateQSpinBoxFT( value );
}

//-------------------------------------------------------------------------

/**
 * \brief Update value for the video length ( T Fixed )
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
on_tSpinMin_2_valueChanged( int value )
{
  switch ( m_SliceFT )
    {
    case 0:
      {
      // X Slice
      m_XMinForVideo = value;
      break;
      }

    case 1:
      {
      // Y Slice
      m_YMinForVideo = value;
      break;
      }

    default:
    case 2:
      {
      // Z Slice
      m_ZMinForVideo = value;
      break;
      }
    }
}

//-------------------------------------------------------------------------

/**
 * \brief Update value for the video length ( T Fixed )
 */

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_tSpinMax_2_valueChanged( int value )
{
  switch ( m_SliceFT )
    {
    case 0:
      {
      // X Slice
      m_XMaxForVideo = value;
      break;
      }

	    case 1 :
    // Y Slice
    m_YMaxForVideo = value;
    break;

    default :
    // Z Slice
    m_ZMaxForVideo = value;
    }
}

//-------------------------------------------------------------------------

/**
 * \brief Get and print the location to store MegaCapture file
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
on_createFile_clicked()
{
	m_VideoName2 = QFileDialog::getSaveFileName( this,
	      tr( "Folder to Save Video" ), "fileName", 0 );
}

//-------------------------------------------------------------------------

/**
 * \brief Function called when FrameRate changes
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
on_frameRate_valueChanged( int value )
{
	m_FrameRate2= value;
	this->frameRate_2->setValue(value);
    // update video length

}

//-------------------------------------------------------------------------

/**
 * \brief Function called when VideoQuality changes
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
on_videoQuality_valueChanged( int value )
{
	m_VideoQuality2= value;
	this->videoQuality_2->setValue(value);
}

//-------------------------------------------------------------------------

/**
 * \brief Function called when "Create video" clicked in Create Video Tab
 */


//-------------------------------------------------------------------------

/**
 * \brief Update value for the video length ( T Fixed )
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
on_tSpinMin_valueChanged( int value )
{
  m_TMinForVideo = value;
}

//-------------------------------------------------------------------------

/**
 * \brief Update value for the video length ( T moving )
 */

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_tSpinMax_valueChanged( int value )
{
  m_TMaxForVideo = value;
}

//-------------------------------------------------------------------------

/**
 * \brief Function called to choose name/path of output file
 */

//-------------------------------------------------------------------------


void
QGoVideoRecorder::
on_createFile_2_clicked()
{
  m_VideoName2 = QFileDialog::getSaveFileName( this,
    tr( "Folder to Save Video" ), "fileName", 0 );
}

//-------------------------------------------------------------------------

/**
 * \brief Update value of frame rate
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
on_frameRate_2_valueChanged( int value )
{
  m_FrameRate2 = value;
  this->frameRate->setValue(value);
}

//-------------------------------------------------------------------------

/**
 * \brief Update value of video quality
 */

//-------------------------------------------------------------------------

void
QGoVideoRecorder::
on_videoQuality_2_valueChanged( int value )
{
  m_VideoQuality2 = value;
  this->videoQuality->setValue(value);
}

//-------------------------------------------------------------------------

/**
 * \brief Function called when "Create video" clicked in Record Video Tab
 */

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

/**
 * \brief Function called when "End" clicked in Record Video Tab
 */


/**
 * \brief Update the value of the timer whilr recording
 */


//-------------------------------------------------------------------------

/**
 * \brief Function called when "Create video" clicked in Create Video Tab
 */

