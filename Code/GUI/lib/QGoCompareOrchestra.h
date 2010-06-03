#ifndef QGOCOMPAREORCHESTRA_H
#define QGOCOMPAREORCHESTRA_H

#include <QObject>
#include "QGoComparer2D.h"
#include "QGoComparer3D.h"
class QGoComparer2DSync;
class QGoComparer3DSync;
class vtkImageData;



/*
* Class dealing with comparers for correct synchronization and handeling
* provides a simple interface to create / delete / synchronize comparers
*
* This class should be used with for any class using QGoComparer
* and QGoSynchronize.
*/


class QGoCompareOrchestra : public QObject
{
// QT macro
    Q_OBJECT


public:

    explicit QGoCompareOrchestra(QObject *parent = 0);
    ~QGoCompareOrchestra();


signals:

public slots:
    // create and add a comparer to QGoCompareOrchestra's parent Object/Widget
    QGoComparer2D* newComparer2D(QString iComparerName, vtkImageData* iImage);
    // create and add a comparer 3D to QGoCompareOrchestra's parent Object/Widget
    QGoComparer3D* newComparer3D(QString iComparerName, vtkImageData* iImage);


    // delete a comparer from QGoCompareOrchestra's parent Object/Widget
    // delete the comparer
    void deleteComparer2D(QGoComparer2D* ioComparer);
    void deleteComparer2D(const int& iIndex);
    void deleteComparer3D(QGoComparer3D* ioComparer);
    void deleteComparer3D(const int& iIndex);

    // take a comparer out of the Orchestra lists, without deleting it.

    void removeComparer2D(QGoComparer2D* ioComparer);
    void removeComparer3D(QGoComparer3D* ioComparer);

    // synchronize all open comparers
    void synchronizeOpenComparers();
    // remove the synchronization leaving the comparers independent
    void unSynchronizeOpenComparers();
    // true if the orchestra synchronizes copmarers
    bool isSynchronizing();

    //update the comparers
    void Update();

    //show all the comparers
    void show();

private:

    // vector to store the open comparers
    std::vector<QGoComparer2D*>         m_openComparers;
    std::vector<QGoComparer3D*>       m_openComparers3D;

    QGoComparer2DSync*                  m_Synchronizer;
    QGoComparer3DSync*                m_Synchronizer3D;


};

#endif // QGOCOMPAREORCHESTRA_H
