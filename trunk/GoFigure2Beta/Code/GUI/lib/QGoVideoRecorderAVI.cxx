/*
 * QGoVideoRecorderAVI.cxx
 *
 *  Created on: Feb 1, 2010
 *      Author: nicolas
 */

#include "QGoVideoRecorderAVI.h"

#include "vtkAVIRenderWindowRecorder.h"

#include "vtkRenderWindow.h"

//-------------------------------------------------------------------------
///\todo move all doxygen documentation in the header file!

/**
 * \brief Constructor
 */

//-------------------------------------------------------------------------

QGoVideoRecorderAVI::
QGoVideoRecorderAVI( QWidget *iParent ) : QGoVideoRecorder( iParent )
{

  m_VideoRecorder = vtkSmartPointer< vtkAVIRenderWindowRecorder >::New();

  QObject::connect( m_InternalTimer, SIGNAL(timeout()),
    this, SLOT(timeout()) );

  QObject::connect( this->startVideo, SIGNAL(clicked()),
      this, SLOT(onStartVideoClicked()) );
  QObject::connect( this->startRecord, SIGNAL(clicked()),
        this, SLOT(onStartRecordClicked()) );
  onStartVideoClicked();
  QObject::connect( this->endRecord, SIGNAL(clicked()),
          this, SLOT(onEndRecordClicked()) );
}

//-------------------------------------------------------------------------

/**
 * \brief Destructor
 */

//-------------------------------------------------------------------------

QGoVideoRecorderAVI::
~QGoVideoRecorderAVI( )
{
}

//-------------------------------------------------------------------------

void
QGoVideoRecorderAVI::
onStartVideoClicked()
{
//something to record first view
  if( m_VideoName2.isNull() ||  m_VideoName2.isEmpty() || ( !m_RenderWindowSelected ) )
    {
    /// \todo use QMessageBox
    std::cerr <<"Please select a videoName and a good rendering window"<<std::endl;
    }
  else if( this->tabWidget->currentIndex () == 0 )
    {
    /// \todo Create one method here (for this scope)
    m_VideoRecorder->Setm_FrameRate( m_FrameRate2 );
    m_VideoRecorder->Setm_VideoQuality( m_VideoQuality2 );

    if( m_SliceFT == 0 )
      {
      /// \todo you can create one method to generate
      ///   1-the filename from m_SliceFT, min and max
      ///   2-take snapshot
      ///   3-emit the signal
      QString fileName = m_VideoName2;

      fileName.append( "-X-" );
      fileName.append( QString::number( m_XMinForVideo, 10 ) );
      fileName.append( "-" );
      fileName.append( QString::number( m_XMaxForVideo, 10 ) );
      fileName.append( ".avi" );

      m_VideoRecorder->SetFileName( fileName.toStdString() );

      m_VideoRecorder->StartCapture();

      for(unsigned int i = m_XMinForVideo; i < m_XMaxForVideo+1; i++)
        {
        //send signal to gofigure to change slice
        emit XSliceChanged(i);
        //capture screen
        m_VideoRecorder->TakeSnapshot();
        }

        m_VideoRecorder->EndCapture();

        emit XSliceChanged(m_XMinForVideo);
      }

    if( m_SliceFT == 1 )
      {
      QString fileName = m_VideoName2;

      fileName.insert( fileName.size(), QString("-Y-"));
      fileName.insert( fileName.size(), QString::number( m_YMinForVideo, 10 ) );
      fileName.insert( fileName.size(), QString("-"));
      fileName.insert( fileName.size(), QString::number( m_YMaxForVideo, 10 ) );
      fileName.insert( fileName.size(), QString(".avi"));

       m_VideoRecorder->SetFileName( fileName.toStdString() );

       m_VideoRecorder->StartCapture();

       for(unsigned int i = m_YMinForVideo; i < m_YMaxForVideo+1; i++)
         {
         //send signal to gofigure to change slice
         emit YSliceChanged(i);
         //capture screen
         m_VideoRecorder->TakeSnapshot();
         }
       m_VideoRecorder->EndCapture();

       emit YSliceChanged(m_YMinForVideo);
       }

    if( m_SliceFT == 2 )
      {
      QString fileName = m_VideoName2;

      fileName.insert( fileName.size(), QString("-Z-"));
      fileName.insert( fileName.size(), QString::number( m_ZMinForVideo, 10 ) );
      fileName.insert( fileName.size(), QString("-"));
      fileName.insert( fileName.size(), QString::number( m_ZMaxForVideo, 10 ) );
      fileName.insert( fileName.size(), QString(".avi"));

       m_VideoRecorder->SetFileName( fileName.toStdString() );

       m_VideoRecorder->StartCapture();

       for(unsigned int i = m_ZMinForVideo; i < m_ZMaxForVideo+1; i++)
         {
         //send signal to gofigure to change slice
         emit ZSliceChanged(i);
         //capture screen
         m_VideoRecorder->TakeSnapshot();
         }
       m_VideoRecorder->EndCapture();

       emit ZSliceChanged(m_ZMinForVideo);
       }
    }
  else
    {
    /// \todo create another method for this scope!
    QString fileName = m_VideoName2;

    fileName.insert( fileName.size(), QString("-T-"));
    fileName.insert( fileName.size(), QString::number( m_TMinForVideo, 10 ) );
    fileName.insert( fileName.size(), QString("-"));
    fileName.insert( fileName.size(), QString::number( m_TMaxForVideo, 10 ) );
    fileName.insert( fileName.size(), QString(".avi"));

    m_VideoRecorder->SetFileName( fileName.toStdString() );

    m_VideoRecorder->StartCapture();

    //problem with 3d view?

    for(unsigned int i = m_TMinForVideo; i < m_TMaxForVideo+1; i++)
      {
      //send signal to gofigure to change slice
      std::cout << "emit T"<<std::endl;
      emit TSliceChanged(i);
      //capture screenvtkAVIRenderWindowRecorder
      m_VideoRecorder->TakeSnapshot();
      }
    m_VideoRecorder->EndCapture();

    //emit TSliceChanged(m_InitialPosition);

    }

}

void
QGoVideoRecorderAVI::
onStartRecordClicked()
{

  if( ( m_VideoName2 == NULL ) || ( !m_RenderWindowSelected ))
  {
  /// \todo QMessageBox
    std::cout<<"Please select a videoName and a good rendering window"<<std::endl;
  }
  else
  {
  this->createFile_2->setEnabled(false);
  this->frameRate_2->setEnabled(false);
  this->videoQuality_2->setEnabled(false);
  this->startRecord->setEnabled(false);
  this->tabVideoMethod1->setEnabled(false);
  this->endRecord->setEnabled(true);

  QString fileName = m_VideoName2;
  fileName.insert( fileName.size(), QString(".avi"));

  m_VideoRecorder->SetFileName( fileName.toStdString() );
  m_VideoRecorder->Setm_FrameRate( m_FrameRate2 );
  m_VideoRecorder->Setm_VideoQuality( m_VideoQuality2 );
  m_VideoRecorder->StartCapture();
  m_InternalTimer->start( 1000/m_FrameRate2 );
  }
}

void
QGoVideoRecorderAVI::
onEndRecordClicked()
{
  m_VideoRecorder->EndCapture();
  m_InternalTimer->stop();
  m_FrameCounter = 0;

  this->createFile_2->setEnabled(true);
  this->frameRate_2->setEnabled(true);
  this->videoQuality_2->setEnabled(true);
  this->startRecord->setEnabled(true);
  this->tabVideoMethod1->setEnabled(true);
  this->endRecord->setEnabled(false);
}
void
QGoVideoRecorderAVI::
timeout()
{
  m_VideoRecorder->TakeSnapshot();

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

/**
 * \brief Function called when "Create video" clicked in Create Video Tab
 */

//--
void
QGoVideoRecorderAVI::
SetRenderingWindow( vtkRenderWindow* iRenderingWindow )
{
  m_RenderWindowSelected = iRenderingWindow;
  this->tabVideoWidget->setEnabled( m_RenderWindowSelected );

  if( m_RenderWindowSelected )
    {
    m_VideoRecorder->SetRenderingWindow(iRenderingWindow);
    }

  // TODO Resize image with the first one if we want to change views during record
}
