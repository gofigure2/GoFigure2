#ifndef __QGOVIDEORECORDERFFMPEG_H
#define __QGOVIDEORECORDERFFMPEG_H

#include <QtGui>

#include "QGoVideoRecorder.h"

#include "vtkFFMPEGRenderWindowRecorder.h"
#include "vtkRenderWindowMovieRecorder.h"

#include "vtkRenderWindow.h"
#include "vtkAlgorithm.h"
#include "vtkSmartPointer.h"

#include <iostream>
#include <string>


class QGoVideoRecorderFFMPEG : public QGoVideoRecorder
{
    Q_OBJECT

    public:
        explicit QGoVideoRecorderFFMPEG( QWidget* parent = 0);
        ~QGoVideoRecorderFFMPEG();

    public slots:
        void SetRenderingWindow( vtkRenderWindow* );


    private:
        vtkFFMPEGRenderWindowRecorder* m_VideoRecorder;
        void Acquisition( int, QString, unsigned int, unsigned int);

    public slots:

        // in tab "create video"
            //Create video pushbutton
            void onStartVideoClicked();
            void onStartRecordClicked();
            void onEndRecordClicked();
            void timeout();

};

#endif /* __QGOVIDEORECORDERFFMPEG_H */
