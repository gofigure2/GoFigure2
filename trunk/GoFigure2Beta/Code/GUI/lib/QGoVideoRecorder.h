#ifndef __QGOVIDEORECORDER_H
#define __QGOVIDEORECORDER_H

#include <QtGui>
#include "ui_NewVideoRecorderDockWidget.h"

#include <iostream>
#include <string>

//#include "vtkSmartPointer.h"

class vtkRenderWindow;
class vtkRenderWindowMovieRecorder;

/** \todo What about making it a template class
* template< class TRenderWindowRecorder > class QGoVideoRecorder, like this you
* can instantiate easily for FFMPEG or OGGTHEORA without changing anything.
* However, you still need to create a similar class to vtkFFMPEGRenderWindowRecorder
* which used OGGTHEORA. Note that since there is an additional parameter when using
* oggtheora it amy be better to inherit instead (but up to you).
*/

class QGoVideoRecorder : public QDockWidget, public Ui::NewDockWidgetVideoRecorder
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

    private:

        /// \todo comment these variables (briefly)
        unsigned int m_XMin;
        unsigned int m_XMax;
        unsigned int m_YMin;
        unsigned int m_YMax;
        unsigned int m_ZMin;
        unsigned int m_ZMax;
        unsigned int m_TMin;
        unsigned int m_TMax;

        void UpdateQSpinBoxFT( int );
        void UpdateQSpinBoxF( int );

        vtkRenderWindowMovieRecorder* m_VideoRecorder;


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


    public slots:
        void SetRenderingWindow( vtkRenderWindow* );


    private:
        void Acquisition( int, QString, unsigned int, unsigned int);

    public slots:

    // in tab "create video"
    //Create video pushbutton
    void onStartVideoClicked();
    void onStartRecordClicked();
    void onEndRecordClicked();
    void timeout();

};

#endif
