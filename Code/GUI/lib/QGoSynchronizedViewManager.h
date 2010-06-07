#ifndef QGOCOMPAREORCHESTRA_H
#define QGOCOMPAREORCHESTRA_H

#include <QObject>
#include "QGoSynchronizedView2D.h"
#include "QGoSynchronizedView3D.h"
class QGoSynchronizedView2DCallbacks;
class QGoSynchronizedView3DCallbacks;
class vtkImageData;



/*
* Class dealing with SynchronizedViews for correct synchronization and handeling
* provides a simple interface to create / delete / synchronize SynchronizedViews
*
* This class should be used with for any class using QGoSynchronizedView
* and QGoSynchronize.
*/


class QGoSynchronizedViewManager : public QObject
{
// QT macro
    Q_OBJECT


public:

    explicit QGoSynchronizedViewManager(QObject *parent = 0);
    ~QGoSynchronizedViewManager();


signals:

public slots:
    // create and add a SynchronizedView to QGoSynchronizedViewManager's parent Object/Widget
    QGoSynchronizedView2D* newSynchronizedView2D(QString iSynchronizedViewName, vtkImageData* iImage);
    // create and add a SynchronizedView 3D to QGoSynchronizedViewManager's parent Object/Widget
    QGoSynchronizedView3D* newSynchronizedView3D(QString iSynchronizedViewName, vtkImageData* iImage);


    // delete a SynchronizedView from QGoSynchronizedViewManager's parent Object/Widget
    // delete the SynchronizedView
    void deleteSynchronizedView2D(QGoSynchronizedView2D* ioSynchronizedView);
    void deleteSynchronizedView2D(const int& iIndex);
    void deleteSynchronizedView3D(QGoSynchronizedView3D* ioSynchronizedView);
    void deleteSynchronizedView3D(const int& iIndex);

    // take a SynchronizedView out of the Manager lists, without deleting it.

    void removeSynchronizedView2D(QGoSynchronizedView2D* ioSynchronizedView);
    void removeSynchronizedView3D(QGoSynchronizedView3D* ioSynchronizedView);

    // synchronize all open SynchronizedViews
    void synchronizeOpenSynchronizedViews();
    // remove the synchronization leaving the SynchronizedViews independent
    void unSynchronizeOpenSynchronizedViews();
    // true if the orchestra synchronizes copmarers
    bool isSynchronizing();

    //update the SynchronizedViews
    void Update();

    //show all the SynchronizedViews
    void show();

private:

    // vector to store the open SynchronizedViews
    std::vector<QGoSynchronizedView2D*>         m_openSynchronizedViews;
    std::vector<QGoSynchronizedView3D*>       m_openSynchronizedViews3D;

    QGoSynchronizedView2DCallbacks*                  m_Synchronizer;
    QGoSynchronizedView3DCallbacks*                m_Synchronizer3D;


};

#endif // QGOCOMPAREORCHESTRA_H
