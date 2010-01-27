#ifndef __QGOVIDEORECORDER_H
#define __QGOVIDEORECORDER_H

#include <QtGui>
#include "ui_NewVideoRecorderDockWidget.h"

#include <QProgressDialog>

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

        void SetXMinAndMax( int XMin, int XMax );
        void SetXSlice( int X );

        void SetYMinAndMax( int YMin, int YMax );
        void SetYSlice( int Y );

        void SetZMinAndMax( int ZMin, int ZMax );
        void SetZSlice( int Z );

        void SetTMinAndMax( int TMin, int TMax );
        void SetTSlice( int T );

    private:

        unsigned int m_XMin;
        unsigned int m_XFixed;
        unsigned int m_XMax;
        unsigned int m_YMin;
        unsigned int m_YFixed;
        unsigned int m_YMax;
        unsigned int m_ZMin;
        unsigned int m_ZFixed;
        unsigned int m_ZMax;
        unsigned int m_TMin;
        unsigned int m_TFixed;
        unsigned int m_TMax;

        unsigned int m_RecordX;
        unsigned int m_RecordY;
        unsigned int m_RecordZ;
        unsigned int m_RecordTX;
        unsigned int m_RecordTY;
        unsigned int m_RecordTZ;

        unsigned int m_XMinForVideo;
        unsigned int m_XMaxForVideo;
        unsigned int m_YMinForVideo;
        unsigned int m_YMaxForVideo;
        unsigned int m_ZMinForVideo;
        unsigned int m_ZMaxForVideo;
        unsigned int m_TMinForVideo;
        unsigned int m_TForVideo;
        unsigned int m_TMaxForVideo;

        QProgressDialog* m_ProgressDialog;

    // in tab "record video"
        unsigned int m_WindowSelected;

        QString m_VideoName2;

        unsigned int m_FrameRate2;
        unsigned int m_VideoQuality2;

        vtkFFMPEGRenderWindowRecorder *m_VideoRecorder2;

        QTimer *m_InternalTimer;
        unsigned int m_FrameCounter;

        int m_RendererWindow;
        vtkRenderWindow* iRenderingWindowTEST;

        bool m_RenderWindowSelected;

        void UpdateQSpinBoxFT( int );

        int m_SliceFT;

    public slots:
      void SetRendererWindow( int iSlice );
      void SetRenderingWindow2( vtkRenderWindow* iRenderingWindow );

    signals:
      void XSliceChanged( int Slice );
      void YSliceChanged( int Slice );
      void ZSliceChanged( int Slice );
      void TSliceChanged( int Slice );

      void FullScreenViewXY();
      void FullScreenViewYZ();
      void FullScreenViewXZ();


    private slots:

    // in tab "create video"
        //Create video pushbutton
        void on_startVideo_clicked();
        
        //Choose a slice T Fixed
        void on_tSpinFixed_valueChanged(int value);
        void on_tSpinMin_2_valueChanged(int value);
        void on_tSpinMax_2_valueChanged(int value);


        //Video parameters
        void on_createFile_clicked();
        void on_frameRate_valueChanged(int value);
        void on_videoQuality_valueChanged(int value);

        //Video parameters
        void on_createFile_2_clicked();
        void on_frameRate_2_valueChanged(int value);
        void on_videoQuality_2_valueChanged(int value);

        void on_startRecord_clicked();
        void on_endRecord_clicked();

        void timeout();


        void on_SliceFT_activated(int value);

};

#endif
