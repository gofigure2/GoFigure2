/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "QGoVideoRecorder.h"

#include <QFileDialog>
#include <QTimer>

#include "QGoImageView3D.h"

#include "vtkRenderWindow.h"
#include "vtkRenderWindowMovieRecorder.h"

#ifdef ENABLEFFMPEG
  #include "vtkFFMPEGRenderWindowRecorder.h"
#endif

#ifdef ENABLEAVI
  #include "vtkAVIRenderWindowRecorder.h"
#endif

//-------------------------------------------------------------------------
QGoVideoRecorder::
QGoVideoRecorder(QWidget *iParent) : QDockWidget(iParent),
  m_VideoName2(""), m_FrameRate2(10), m_VideoQuality2(2), m_SliceFT(0),
  m_WindowSelected(0), m_XMinForVideo(0), m_XMaxForVideo(0),
  m_YMinForVideo(0), m_YMaxForVideo(0), m_ZMinForVideo(0),
  m_ZMaxForVideo(0), m_TMinForVideo(0), m_TMaxForVideo(0),
  m_FrameCounter(0), m_RenderWindowSelected(false)
  {
  this->setupUi(this);

  QObject::connect(this->startVideo, SIGNAL(clicked()),
                   this, SLOT(onStartVideoClicked()));
  QObject::connect(this->startRecord, SIGNAL(clicked()),
                   this, SLOT(onStartRecordClicked()));
  QObject::connect(this->endRecord, SIGNAL(clicked()),
                   this, SLOT(onEndRecordClicked()));

  QIcon videoIcon(QPixmap(QString::fromUtf8(":/fig/video.png")));
  this->toggleViewAction()->setIcon(videoIcon);
  this->toggleViewAction()
  ->setStatusTip(tr("You have to be in full screen view to use the video recording"));
  this->warning_1->hide();
  this->warning_2->hide();

  // Tooltips to help the user
  QString ttoolTip = "Start recording the video";
  this->startVideo->setToolTip(ttoolTip);
  ttoolTip = "First slice of the video";
  this->tSpinMin_2->setToolTip(ttoolTip);
  ttoolTip = "Last slice of the video";
  this->tSpinMax_2->setToolTip(ttoolTip);
  ttoolTip = "Location and name of the video";
  this->createFile->setToolTip(ttoolTip);
  ttoolTip = "Create video along this axe";
  this->SliceFT->setToolTip(ttoolTip);
  ttoolTip = "Continue creating video from selected time points";
  this->pauseVideo->setToolTip(ttoolTip);
  ttoolTip = "End the video";
  this->endVideo->setToolTip(ttoolTip);
  ttoolTip = "Create video without manual interaction (usefull in 2D)";
  this->tabVideoMethod1->setToolTip(ttoolTip);
  ttoolTip = "Create video with manual interaction (usefull in 3D)";
  this->tabVideoMethod2->setToolTip(ttoolTip);
  ttoolTip = "Quality of the output video (high=2, medium=1, low=0)";
  this->videoQuality->setToolTip(ttoolTip);

  m_InternalTimer = new QTimer(this);

  QObject::connect(m_InternalTimer, SIGNAL(timeout()),
                   this, SLOT(timeout()));

  //Initalize chrono with .00 (not 0)
  QString value = "0.00";
  this->videoLenght->display(value);
  this->endRecord->setEnabled(false);

  this->tabVideoWidget->setEnabled(false);

#ifdef ENABLEFFMPEG
  m_FFMPEGWriter = vtkFFMPEGRenderWindowRecorder::New();
  this->SetMovieRecorder(m_FFMPEGWriter);
#endif /* ENABLEFFMPEG */

#ifdef ENABLEAVI
  m_AVIWriter = vtkAVIRenderWindowRecorder::New();
  this->SetMovieRecorder(m_AVIWriter);
#endif /* ENABLEAVI */

  QObject::connect(this, SIGNAL(FrameRateChanged(int)),
                   this, SLOT(SetSpecificParametersFrameRate(int)));
  QObject::connect(this, SIGNAL(QualityChanged(int)),
                   this, SLOT(SetSpecificParametersQuality(int)));

  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoVideoRecorder::
~QGoVideoRecorder()
  {
#ifdef ENABLEFFMPEG
  m_FFMPEGWriter->Delete();
#endif

#ifdef ENABLEAVI
  m_AVIWriter->Delete();
#endif
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetXMinAndMax(const int& XMin, const int& XMax)
{
  m_XMin = XMin;
  m_XMax = XMax;

  m_XMinForVideo = XMin;
  m_XMaxForVideo = XMax;

  //Initialization of the spinboxes
  this->tSpinMin_2->setMinimum(m_XMin);
  this->tSpinMin_2->setMaximum(m_XMax);
  this->tSpinMin_2->setValue(m_XMin);

  this->tSpinMax_2->setMinimum(m_XMin);
  this->tSpinMax_2->setMaximum(m_XMax);
  this->tSpinMax_2->setValue(m_XMax);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetYMinAndMax(const int& YMin, const int& YMax)
{
  m_YMin = YMin;
  m_YMax = YMax;

  m_YMinForVideo = YMin;
  m_YMaxForVideo = YMax;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetZMinAndMax(const int& ZMin, const int& ZMax)
{
  m_ZMin = ZMin;
  m_ZMax = ZMax;

  m_ZMinForVideo = ZMin;
  m_ZMaxForVideo = ZMax;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetTMinAndMax(const int& TMin, const int& TMax)
{
  m_TMin = TMin;
  m_TMax = TMax;

  m_TMinForVideo = TMin;
  m_TMaxForVideo = TMax;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetCurrentX(const int& X)
{
  m_CurrentX = X;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetCurrentY(const int& Y)
{
  m_CurrentY = Y;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetCurrentZ(const int& Z)
{
  m_CurrentZ = Z;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetCurrentT(const int& T)
{
  m_CurrentT = T;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
UpdateQSpinBoxFT(int value)
{
  switch (value)
    {
    case 0:
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

    case 1:
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

    default:
    case 3:
      {
      // Z Slice
      this->tSpinMin_2->setMinimum(m_TMin);
      this->tSpinMin_2->setMaximum(m_TMax);
      this->tSpinMin_2->setValue(m_TMin);

      this->tSpinMax_2->setMinimum(m_TMin);
      this->tSpinMax_2->setMaximum(m_TMax);
      this->tSpinMax_2->setValue(m_TMax);

      m_SliceFT = 3;
      break;
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_SliceFT_activated(int value)
{
  UpdateQSpinBoxFT(value);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_tSpinMin_2_valueChanged(int value)
{
  switch (m_SliceFT)
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

    case 2:
      {
      // Z Slice
      m_ZMinForVideo = value;
      break;
      }

    default:
    case 3:
      {
      // Z Slice
      m_TMinForVideo = value;
      break;
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_tSpinMax_2_valueChanged(int value)
{
  switch (m_SliceFT)
    {
    case 0:
      {
      // X Slice
      m_XMaxForVideo = value;
      break;
      }

    case 1:
      {
      // Y Slice
      m_YMaxForVideo = value;
      break;
      }

    case 2:
      {
      // Y Slice
      m_ZMaxForVideo = value;
      break;
      }

    default:
    case 3:
      {
      // Y Slice
      m_TMaxForVideo = value;
      break;
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_createFile_clicked()
{
  m_VideoName2 = QFileDialog::getSaveFileName(this,
                                              tr("Folder to Save Video"), "fileName", 0);

  this->createFile->setStyleSheet("background-color: rgb(0, 255, 0); color: rgb(0, 0, 0)");
  this->createFile_2->setStyleSheet("background-color: rgb(255, 165, 0); color: rgb(0, 0, 0)");

  QPalette plt;
  plt.setColor(QPalette::WindowText, Qt::black);
  this->label->setPalette(plt);
  this->label_2->setPalette(plt);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_frameRate_valueChanged(int value)
{
  if (m_FrameRate2 != static_cast<unsigned int>(value))
    {
    m_FrameRate2 = static_cast<unsigned int>(value);
    this->frameRate_2->setValue(value);
    emit FrameRateChanged(value);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_videoQuality_valueChanged(int value)
{
  if (m_VideoQuality2 != static_cast<unsigned int>(value))
    {
    m_VideoQuality2 = static_cast<unsigned int>(value);
    this->videoQuality_2->setValue(value);
    emit QualityChanged(value);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_createFile_2_clicked()
{
  m_VideoName2 = QFileDialog::getSaveFileName(this,
                                              tr("Folder to Save Video"), "fileName", 0);

  this->createFile_2->setStyleSheet("background-color: rgb(0, 255, 0); color: rgb(0, 0, 0)");
  this->createFile->setStyleSheet("background-color: rgb(255, 165, 0); color: rgb(0, 0, 0)");

  QPalette plt;
  plt.setColor(QPalette::WindowText, Qt::black);
  this->label->setPalette(plt);
  this->label_2->setPalette(plt);

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_frameRate_2_valueChanged(int value)
{
  if (m_FrameRate2 != static_cast<unsigned int>(value))
    {
    m_FrameRate2 = static_cast<unsigned int>(value);
    this->frameRate->setValue(value);
    emit FrameRateChanged(value);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_videoQuality_2_valueChanged(int value)
{
  if (m_VideoQuality2 != static_cast<unsigned int>(value))
    {
    m_VideoQuality2 = static_cast<unsigned int>(value);
    this->videoQuality->setValue(value);
    emit QualityChanged(value);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
onStartVideoClicked()
{
  //something to record first view
  if (m_VideoName2.isNull() ||  m_VideoName2.isEmpty() || (!m_RenderWindowSelected))
    {
    QPalette plt;
    plt.setColor(QPalette::WindowText, Qt::red);
    this->label->setPalette(plt);
    this->label_2->setPalette(plt);

    this->createFile->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(0, 0, 0)");
    this->createFile_2->setStyleSheet("background-color: rgb(255, 0, 0)");
    }
  else
    {
    QString fileName = m_VideoName2;
    QString fileName2 = m_VideoName2;

    if (!fileName.endsWith(".avi"))
      {
      fileName.insert(fileName.size(), QString(".avi"));
      fileName2.insert(fileName.size(), QString(".txt"));
      }
    else
      {
      fileName2.replace(QString(".avi"), QString(".txt"));
      }

    m_VideoRecorder->SetFileName(fileName.toStdString());
    m_VideoRecorder->StartCapture();

    this->startVideo->setEnabled(false);

    m_OutputVideoFile.open(fileName2.toStdString().c_str());
    m_OutputVideoFile << "FrameRate: ";
    m_OutputVideoFile << m_FrameRate2;
    m_OutputVideoFile << "\nVideoQuality: ";
    m_OutputVideoFile << m_VideoQuality2;
    m_OutputVideoFile << "\nX Y Z T ";

    AcquireWithPause(m_SliceFT);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
onStartRecordClicked()
{
  if ((m_VideoName2 == NULL) || (!m_RenderWindowSelected))
    {
    QPalette plt;
    plt.setColor(QPalette::WindowText, Qt::red);
    this->label->setPalette(plt);
    this->label_2->setPalette(plt);
    this->createFile->setStyleSheet("background-color: rgb(255, 0, 0)");
    this->createFile_2->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(0, 0, 0)");
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

    if (!fileName.endsWith(".avi"))
      {
      fileName.insert(fileName.size(), QString(".avi"));
      }

    m_VideoRecorder->SetFileName(fileName.toStdString());

    m_VideoRecorder->StartCapture();
    m_InternalTimer->start(1000 / m_FrameRate2);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
onEndRecordClicked()
{
  m_OutputVideoFile.close();

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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
timeout()
{
  m_VideoRecorder->TakeSnapshot();
  ++m_FrameCounter;

  // for a better visualisation, always show 2 decimal
  double doubleCounter;
  doubleCounter = (double) m_FrameCounter / (double) m_FrameRate2;

  int     test = 100 * doubleCounter;
  QString value = QString::number(test, 10);

  if (test >= 10)
    {
    value.insert(value.size() - 2, QString("."));
    }
  else
    {
    value.insert(value.size() - 1, QString(".0"));
    }

  if (test < 100)
    {
    value.insert(value.size() - 3, QString("0"));
    }
  this->videoLenght->display(value);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void
QGoVideoRecorder::
SetRenderingWindow(vtkRenderWindow* iRenderingWindow)
{
  m_RenderWindowSelected = iRenderingWindow;
  this->tabVideoWidget->setEnabled(m_RenderWindowSelected);

  if (m_RenderWindowSelected)
    {
    m_VideoRecorder->SetRenderingWindow(iRenderingWindow);
    }

  //Tell the user to go in full screen mode
  /* if( iRenderingWindow )
       {
       this->warning_1->hide();
       this->warning_2->hide();
       }
     else
       {
       this->warning_1->show();
       this->warning_2->show();
       }*/

  /**
 * \todo Resize image with the first one if we want to change views during record
 */
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
SetMovieRecorder(vtkRenderWindowMovieRecorder* Test)
{
  m_VideoRecorder = Test;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
AcquireWithPause(int value)
{
  unsigned int iMin;
  unsigned int iMax;

  switch (value)
    {
    case 0:
      {
      // X Slice
      iMin = m_XMinForVideo;
      iMax = m_XMaxForVideo;
      break;
      }
    case 1:
      {
      // Y Slice
      iMin = m_YMinForVideo;
      iMax = m_YMaxForVideo;
      break;
      }

    case 2:
      {
      // Z Slice
      iMin = m_ZMinForVideo;
      iMax = m_ZMaxForVideo;
      break;
      }

    default:
    case 3:
      {
      // T Slice
      iMin = m_TMinForVideo;
      iMax = m_TMaxForVideo;
      break;
      }
    }

  for (unsigned int i = iMin; i < iMax + 1; i++)
    {
    //send signal to change slice
    emitChangeSliceSignal(value, i);

    //send signal to know our position in the 3D volume
    emit GetSliceView();
    m_OutputVideoFile << "\n";
    m_OutputVideoFile << m_CurrentX;
    m_OutputVideoFile << " ";
    m_OutputVideoFile << m_CurrentY;
    m_OutputVideoFile << " ";
    m_OutputVideoFile << m_CurrentZ;
    m_OutputVideoFile << " ";
    m_OutputVideoFile << m_CurrentT;

    //capture screen
    m_VideoRecorder->TakeSnapshot();
    }
  this->pauseVideo->setEnabled(true);
  this->endVideo->setEnabled(true);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_pauseVideo_clicked()
{
  AcquireWithPause(m_SliceFT);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
on_endVideo_clicked()
{
  m_VideoRecorder->EndCapture();

  this->endVideo->setEnabled(false);
  this->pauseVideo->setEnabled(false);
  this->startVideo->setEnabled(true);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVideoRecorder::
emitChangeSliceSignal(const int& iSlice, const int& iSlide)
{
  switch (iSlice)
    {
    case 0:
      {
      // X Slice
      emit XSliceChanged(iSlide);
      break;
      }
    case 1:
      {
      // Y Slice
      emit YSliceChanged(iSlide);
      break;
      }
    case 2:
      {
      // Z Slice
      emit ZSliceChanged(iSlide);
      break;
      }

    default:
    case 3:
      {
      // Z Slice
      emit TSliceChanged(iSlide);
      break;
      }
    }
}

void
QGoVideoRecorder::
SetSpecificParametersFrameRate(int iValue)
{
  m_VideoRecorder->SetFrameRate(iValue);
  m_VideoRecorder->SetSpecificParameters();
}

void
QGoVideoRecorder::
SetSpecificParametersQuality(int iValue)
{
  m_VideoRecorder->SetVideoQuality(iValue);
  m_VideoRecorder->SetSpecificParameters();
}
