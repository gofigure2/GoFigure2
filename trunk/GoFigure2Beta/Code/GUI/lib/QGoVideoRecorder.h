#ifndef __QGOVIDEORECORDER_H
#define __QGOVIDEORECORDER_H

#include <QtGui>
#include "ui_NewVideoRecorderDockWidget.h"

#include <iostream>
#include <string>

class vtkRenderWindow;
class vtkFFMPEGRenderWindowRecorder;

class QGoVideoRecorder : public QDockWidget, private Ui::NewDockWidgetVideoRecorder
{
    Q_OBJECT

    public:
        explicit QGoVideoRecorder( QWidget* parent = 0);
        ~QGoVideoRecorder();

        void SetXMinAndMax( int , int );
        void SetYMinAndMax( int , int );
        void SetZMinAndMax( int , int );
        void SetTMinAndMax( int , int );

    private:

        unsigned int m_XMin;
        unsigned int m_XMax;
        unsigned int m_YMin;
        unsigned int m_YMax;
        unsigned int m_ZMin;
        unsigned int m_ZMax;
        unsigned int m_TMin;
        unsigned int m_TMax;

        unsigned int m_XMinForVideo;
        unsigned int m_XMaxForVideo;
        unsigned int m_YMinForVideo;
        unsigned int m_YMaxForVideo;
        unsigned int m_ZMinForVideo;
        unsigned int m_ZMaxForVideo;
        unsigned int m_TMinForVideo;
        unsigned int m_TMaxForVideo;

        // in tab "record video"
        unsigned int m_WindowSelected;

        QString m_VideoName2;

        unsigned int m_FrameRate2;
        unsigned int m_VideoQuality2;

        vtkFFMPEGRenderWindowRecorder *m_VideoRecorder;

        QTimer *m_InternalTimer;
        unsigned int m_FrameCounter;

        bool m_RenderWindowSelected;

        void UpdateQSpinBoxFT( int );
        void UpdateQSpinBoxF( int );

        int m_SliceFT;

    public slots:
      void SetRenderingWindow( vtkRenderWindow* );

    signals:
      void XSliceChanged( int );
      void YSliceChanged( int );
      void ZSliceChanged( int );
      void TSliceChanged( int );

    private slots:

    // in tab "create video"
        //Create video pushbutton
        void on_startVideo_clicked();
        
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

        void on_startRecord_clicked();
        void on_endRecord_clicked();

        void timeout();


        void on_SliceFT_activated( int );

        void on_tSpinMin_valueChanged( int );
        void on_tSpinMax_valueChanged( int );


};

#endif
