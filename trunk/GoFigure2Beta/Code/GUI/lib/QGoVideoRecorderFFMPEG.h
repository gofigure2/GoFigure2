#ifndef __QGOVIDEORECORDERFFMPEG_H
#define __QGOVIDEORECORDERFFMPEG_H

#include <QtGui>

#include "QGoVideoRecorder.h"

class vtkFFMPEGRenderWindowRecorder;

#include "vtkSmartPointer.h"

#include <iostream>
#include <string>

class vtkRenderWindow;



class QGoVideoRecorderFFMPEG : public QGoVideoRecorder
{
    Q_OBJECT

    public:
        explicit QGoVideoRecorderFFMPEG( QWidget* parent = 0);
        ~QGoVideoRecorderFFMPEG();

    public slots:
        void SetRenderingWindow( vtkRenderWindow* );


    private:
        vtkSmartPointer< vtkFFMPEGRenderWindowRecorder > m_VideoRecorder;

    public slots:

        // in tab "create video"
            //Create video pushbutton
            void onStartVideoClicked();
            void onStartRecordClicked();
            void onEndRecordClicked();
            void timeout();

};

#endif /* __QGOVIDEORECORDERFFMPEG_H */
