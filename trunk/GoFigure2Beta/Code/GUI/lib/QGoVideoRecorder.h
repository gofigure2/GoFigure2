/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

#ifndef __QGOVIDEORECORDER_H
#define __QGOVIDEORECORDER_H

#include <QtGui>
#include "ui_NewVideoRecorderDockWidget.h"

#include <iostream>
#include <string>

//#include "vtkSmartPointer.h"

class vtkRenderWindow;
class vtkRenderWindowMovieRecorder;


class QGoVideoRecorder : public QDockWidget, private Ui::NewDockWidgetVideoRecorder
{
    Q_OBJECT

    public:
        explicit QGoVideoRecorder( QWidget* parent = 0);
        ~QGoVideoRecorder();

        /// \todo use const int& instead
        void SetXMinAndMax( int , int );
        void SetYMinAndMax( int , int );
        void SetZMinAndMax( int , int );
        void SetTMinAndMax( int , int );

        QString m_VideoName2;

        unsigned int m_FrameRate2;
        unsigned int m_VideoQuality2;
        int m_SliceFT;
        // in tab "record video"
        unsigned int m_WindowSelected;

        unsigned int m_XMinForVideo;
        unsigned int m_XMaxForVideo;
        unsigned int m_YMinForVideo;
        unsigned int m_YMaxForVideo;
        unsigned int m_ZMinForVideo;
        unsigned int m_ZMaxForVideo;
        unsigned int m_TMinForVideo;
        unsigned int m_TMaxForVideo;

        QTimer *m_InternalTimer;
        unsigned int m_FrameCounter;

        bool m_RenderWindowSelected;

        void SetMovieRecorder( vtkRenderWindowMovieRecorder* );

    public slots:

    signals:
      void XSliceChanged( int );
      void YSliceChanged( int );
      void ZSliceChanged( int );
      void TSliceChanged( int );
      void QualityChanged( int );
      void FrameRateChanged( int );

    private:

        // Video recorder
        vtkRenderWindowMovieRecorder* m_VideoRecorder;

        // Min/Max value of X/Y/ZSlice to set up spin box
        unsigned int m_XMin;
        unsigned int m_XMax;
        unsigned int m_YMin;
        unsigned int m_YMax;
        unsigned int m_ZMin;
        unsigned int m_ZMax;
        unsigned int m_TMin;
        unsigned int m_TMax;

        /**
         * \brief Function to update spinbox extent according to selected slice
        **/
        void UpdateQSpinBoxFT( int );

        void Acquire( int );


    private slots:

    // in tab "create video"
    //Choose a slice T Fixed
    void on_tSpinMin_2_valueChanged( int );
    void on_tSpinMax_2_valueChanged( int );
    //Video parameters
    void on_createFile_clicked();
    void on_frameRate_valueChanged( int );
    void on_videoQuality_valueChanged( int );
    //Video parameters
    void on_createFile_2_clicked();
    void on_frameRate_2_valueChanged( int );
    void on_videoQuality_2_valueChanged( int );
    void on_SliceFT_activated( int );
    void on_tSpinMin_valueChanged( int );
    void on_tSpinMax_valueChanged( int );
    /**
     * \brief Function called when "Create video" clicked in Create
    **/
    void onStartVideoClicked();
    /**
     * \brief Function called when "Create video" clicked in Record
    **/
    void onStartRecordClicked();
    /**
      * \brief Function called when "Create video" clicked in Record
     **/
    void onEndRecordClicked();
    /**
      * \brief Function called when "Create video" clicked in Record
    **/
    void timeout();


    public slots:
        void SetRenderingWindow( vtkRenderWindow* );


};

#endif
