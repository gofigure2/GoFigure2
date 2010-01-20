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
  m_RecordZ( 0 ), m_RecordTX( 0 ), m_RecordTY( 0 ), m_RecordTZ( 0 ), m_VideoName( "" ),
  m_FrameRate( 30 ), m_VideoQuality ( 1 ), m_WindowSelected(0), m_VideoName2( "" ),
  m_FrameRate2( 30 ), m_VideoQuality2 ( 1 ), m_FrameCounter( 0 ), m_RendererWindow(0),iRenderingWindowTEST(NULL),
  m_RenderWindowSelected( false )
{
	this->setupUi(this);

  m_VideoRecorder = vtkFFMPEGRenderWindowRecorder::New();
  m_VideoRecorder2 = vtkFFMPEGRenderWindowRecorder::New();

  m_InternalTimer = new QTimer( this );
  QObject::connect( m_InternalTimer, SIGNAL(timeout()),this, SLOT(timeout()) );

  //Initalize chrono with .00 (not 0)
  QString value = "0.00";
  this->videoLenght->display(value);
  this->endRecord->setEnabled(false);
}

/**
 * \brief Destructor
 */
QGoVideoRecorder::
~QGoVideoRecorder( )
{

	m_VideoRecorder->Delete();
//	m_VideoRecorder2->Delete();
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

/**
 * \brief Set the minimum value for the X spin box
 * \param[in] iValue Value of the X spin box
 */
void
QGoVideoRecorder::
setXSpinMin(unsigned int iValue)
{
	this->xSpinMin->setMinimum(iValue);
	this->xSpinFixed->setMinimum(iValue);
	this->xSpinMax->setMinimum(iValue+1);
}

/**
 * \brief Set the maximum value for the X spin box
 * \param[in] iValue Value of the X spin box
 */
void
QGoVideoRecorder::
setXSpinMax(unsigned int iValue)
{
	this->xSpinMin->setMaximum(iValue-1);
	this->xSpinFixed->setMaximum(iValue);
	this->xSpinMax->setMaximum(iValue);
}

/**
 * \brief Set the minimum value for the Y spin box
 * \param[in] iValue Value of the Y spin box
 */
void
QGoVideoRecorder::
setYSpinMin(unsigned int iValue)
{
	this->ySpinMin->setMinimum(iValue);
	this->ySpinFixed->setMinimum(iValue);
	this->ySpinMax->setMinimum(iValue+1);
}

/**
 * \brief Set the maximum value for the Y spin box
 * \param[in] iValue Value of the Y spin box
 */
void
QGoVideoRecorder::
setYSpinMax(unsigned int iValue)
{
	this->ySpinMin->setMaximum(iValue-1);
	this->ySpinFixed->setMaximum(iValue);
	this->ySpinMax->setMaximum(iValue);
}

/**
 * \brief Set the minimum value for the Y spin box
 * \param[in] iValue Value of the Y spin box
 */
void
QGoVideoRecorder::
setZSpinMin(unsigned int iValue)
{
	this->zSpinMin->setMinimum(iValue);
	this->zSpinFixed->setMinimum(iValue);
	this->zSpinMax->setMinimum(iValue+1);
}

/**
 * \brief Set the maximum value for the Z spin box
 * \param[in] iValue Value of the Z spin box
 */
void
QGoVideoRecorder::
setZSpinMax(unsigned int iValue)
{
	this->zSpinMin->setMaximum(iValue-1);
	this->zSpinFixed->setMaximum(iValue);
	this->zSpinMax->setMaximum(iValue);
}

/**
* \brief Set the minimum value for the Y spin box
* \param[in] iValue Value of the Y spin box
*/
void
QGoVideoRecorder::
setTSpinMin(unsigned int iValue)
{
	this->tSpinMin->setMinimum(iValue);
	this->tSpinFixed->setMinimum(iValue);
	this->tSpinMax->setMinimum(iValue+1);
}

/**
* \brief Set the maximum value for the Z spin box
* \param[in] iValue Value of the Z spin box
*/
void
QGoVideoRecorder::
setTSpinMax(unsigned int iValue)
{
	this->tSpinMin->setMaximum(iValue-1);
	this->tSpinFixed->setMaximum(iValue);
	this->tSpinMax->setMaximum(iValue);
}

/**
* \brief Set the rendering window to record video
* \param[in] iRenderingWindow Observed rendering window
*/
/*void
QGoVideoRecorder::
SetRenderingWindow( vtkRenderWindow* iRenderingWindow )
{
	m_VideoRecorder->SetRenderingWindow(iRenderingWindow);
}*/

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
	m_VideoName = QFileDialog::getSaveFileName( this,
      tr( "Folder to Save Video" ), "fileName", 0 );

  this->videoName->setText( m_VideoName );
}

/**
 * \brief Function called when FrameRate changes
 */
void
QGoVideoRecorder::
on_frameRate_valueChanged( int value )
{
	m_FrameRate = value;
}

/**
 * \brief Function called when VideoQuality changes
 */
void
QGoVideoRecorder::
on_videoQuality_valueChanged( int value )
{
	m_VideoQuality = value;
}


/**
 * \brief Function called when "Create video" clicked
 */

void
QGoVideoRecorder::
on_startVideo_clicked()
{
  // Print parameters for testings

	std::cout<<"m_XMin: "<< m_XMin <<std::endl;
	std::cout<<"m_XFixed: "<< m_XFixed <<std::endl;
	std::cout<<"m_XMax: "<< m_XMax <<std::endl;
	std::cout<<"m_YMin: "<< m_YMin <<std::endl;
	std::cout<<"m_YFixed: "<< m_YFixed <<std::endl;
	std::cout<<"m_YMax: "<< m_YMax <<std::endl;
	std::cout<<"m_ZMin: "<< m_ZMin <<std::endl;
	std::cout<<"m_ZFixed: "<< m_ZFixed <<std::endl;
	std::cout<<"m_ZMax: "<< m_ZMax <<std::endl;
	std::cout<<"m_TMin: "<< m_TMin <<std::endl;
	std::cout<<"m_TFixed: "<< m_TFixed <<std::endl;
	std::cout<<"m_TMax: "<< m_TMax <<std::endl;
	std::cout<<"m_RecordX: "<< m_RecordX <<std::endl;
	std::cout<<"m_RecordY: "<< m_RecordY <<std::endl;
	std::cout<<"m_RecordZ: "<< m_RecordZ <<std::endl;
	std::cout<<"m_RecordTX: "<< m_RecordTX <<std::endl;
	std::cout<<"m_RecordTY: "<< m_RecordTY <<std::endl;
	std::cout<<"m_RecordTZ: "<< m_RecordTZ <<std::endl;
	std::cout<<"m_FrameRate: "<< m_FrameRate <<std::endl;
	std::cout<<"m_VideoQuality: "<< m_VideoQuality <<std::endl;

	//Set good rendering window from gofigure
	//vtkRenderWindow* renderingWindow;
    //...
	//m_VideoRecorder->SetRenderingWindow(renderingWindow);

	//Create a QTDialogProgress

  //unsigned int m_SizeProgressBar;
	//...

	// for X Slices
  if(m_RecordX>0)
  	{
    QString fileName = m_VideoName;

    fileName.insert( fileName.size(), QString("-X-"));
    fileName.insert( fileName.size(), QString::number( m_XMin, 10 ) );
    fileName.insert( fileName.size(), QString("-"));
    fileName.insert( fileName.size(), QString::number( m_XMax, 10 ) );
    fileName.insert( fileName.size(), QString(".avi"));

    m_VideoRecorder->SetFileName( fileName.toStdString() );
    std::cout<<"FileName X: "<< fileName.toStdString() << std::endl;

	  m_VideoRecorder->StartCapture();
    //...
	  for(unsigned int i = m_XMin; i < m_XMax+1; i++)
	  	{
	    //send signal to gofigure

	    //...

	    //capture screen
	    m_VideoRecorder->TakeSnapshot();
	  	}
    //...
	  m_VideoRecorder->EndCapture();
  	}

	// for Y Slices
  if(m_RecordY>0)
  	{
    QString fileName = m_VideoName;

    fileName.insert( fileName.size(), QString("-Y-"));
    fileName.insert( fileName.size(), QString::number( m_YMin, 10 ) );
    fileName.insert( fileName.size(), QString("-"));
    fileName.insert( fileName.size(), QString::number( m_YMax, 10 ) );
    fileName.insert( fileName.size(), QString(".avi"));

    m_VideoRecorder->SetFileName( fileName.toStdString() );
    std::cout<<"FileName Y: "<< fileName.toStdString() << std::endl;

	  m_VideoRecorder->StartCapture();
    //...
	  for(unsigned int i = m_YMin; i < m_YMax+1; i++)
	  	{
	    //send signal to gofigure

	    //...

	    //capture screen
	    m_VideoRecorder->TakeSnapshot();
	  	}
    //...
	  m_VideoRecorder->EndCapture();
  	}

	// for Z Slices
  if(m_RecordZ>0)
  	{
    QString fileName = m_VideoName;

    fileName.insert( fileName.size(), QString("-z-"));
    fileName.insert( fileName.size(), QString::number( m_ZMin, 10 ) );
    fileName.insert( fileName.size(), QString("-"));
    fileName.insert( fileName.size(), QString::number( m_ZMax, 10 ) );
    fileName.insert( fileName.size(), QString(".avi"));

    m_VideoRecorder->SetFileName( fileName.toStdString() );
    std::cout<<"FileName Z: "<< fileName.toStdString() << std::endl;

	  m_VideoRecorder->StartCapture();
    //...
	  for(unsigned int i = m_ZMin; i < m_ZMax+1; i++)
	  	{
	    //send signal to gofigure

	    //...

	    //capture screen
	    m_VideoRecorder->TakeSnapshot();
	  	}
    //...
	  m_VideoRecorder->EndCapture();
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
}

/**
 * \brief Function called when VideoQuality changes
 */
void
QGoVideoRecorder::
on_videoQuality_2_valueChanged( int value )
{
	m_VideoQuality2 = value;
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

  std::cout<< "value: "<< value.toStdString() << std::endl;
  this->videoLenght->display(value);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetRendererWindow( int iSlice )
{
  m_RendererWindow = iSlice;
}
//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetRenderingWindow2( vtkRenderWindow* iRenderingWindow )
{
	iRenderingWindowTEST = iRenderingWindow;
	if(iRenderingWindowTEST == NULL)
	{
	std::cout << " RENDERING WINDOW NULL " << std::endl;
	}
	else
	{
	m_RenderWindowSelected = true;
	std::cout << " RENDERING WINDOW NON NULL " << std::endl;
	}
}

//----------------------------------------------------------------------------//
/* emit to be done
void
QGoVideoRecorder::
XSliceChanged( int Slice )
{

}
void
QGoVideoRecorder::
YSliceChanged( int Slice )
{

}
void
QGoVideoRecorder::
ZSliceChanged( int Slice )
{

}
void
QGoVideoRecorder::
TSliceChanged( int Slice )
{

}
void
QGoVideoRecorder::
SetXSlice( int iSlice )
{

}
void
QGoVideoRecorder::
SetYSlice( int iSlice )
{

}
void
QGoVideoRecorder::
SetZSlice( int iSlice )
{

}
void
SetTSlice( int iSlice )
{

}
*/
