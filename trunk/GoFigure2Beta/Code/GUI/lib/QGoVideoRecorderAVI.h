/*
 * QGoVideoRecorderAVI.h
 *
 *  Created on: Feb 1, 2010
 *      Author: nicolas
 */

#ifndef QGOVIDEORECORDERAVI_H_
#define QGOVIDEORECORDERAVI_H_


#include <QtGui>

#include "QGoVideoRecorder.h"

class vtkAVIRenderWindowRecorder;

#include "vtkSmartPointer.h"

#include <iostream>
#include <string>

class vtkRenderWindow;



class QGoVideoRecorderAVI : public QGoVideoRecorder
{
    Q_OBJECT

    public:
        explicit QGoVideoRecorderAVI( QWidget* parent = 0);
        ~QGoVideoRecorderAVI();

    public slots:
        void SetRenderingWindow( vtkRenderWindow* );


    private:
        vtkSmartPointer< vtkAVIRenderWindowRecorder > m_VideoRecorder;

    public slots:

        // in tab "create video"
            //Create video pushbutton
            void onStartVideoClicked();
            void onStartRecordClicked();
            void onEndRecordClicked();
            void timeout();

};

#endif /* QGOVIDEORECORDERAVI_H_ */
