#ifndef __QGOVIDEORECORDER_H
#define __QGOVIDEORECORDER_H

#include <QtGui>
#include "ui_NewVideoRecorderDockWidget.h"

#include <iostream>
#include <string>

//#include "vtkSmartPointer.h"

class vtkRenderWindow;
class vtkRenderWindowMovieRecorder;


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
