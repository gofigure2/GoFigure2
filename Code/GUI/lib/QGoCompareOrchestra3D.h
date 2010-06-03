#ifndef QGOCOMPAREORCHESTRA_H
#define QGOCOMPAREORCHESTRA_H

#include <QObject>




class vtkImageData;
#include "QGoComparer.h"
class QGoComparerSync;

/*
* Class dealing with comparers for correct synchronization and handeling
* provides a simple interface to create / delete / synchronize comparers
*
* This class should be used with for any class using QGoComparer
* and QGoSynchronize.
*/


class QGoCompareOrchestra : public QObject
{
Q_OBJECT
public:
    explicit QGoCompareOrchestra(QObject *parent = 0);
~QGoCompareOrchestra();

signals:

public slots:


    // create and add a comparer to QGoCompareOrchestra's parent Object/Widget
    QGoComparer* newComparer(QString iComparerName, vtkImageData* iImage);

    // delete a comparer from QGoCompareOrchestra's parent Object/Widget
    //deleteComparer(QString ComparerName);
    void deleteComparer(QGoComparer* ioComparer);
    void deleteComparer(const int& iIndex);
    // synchronize all open comparers
    void synchronizeOpenComparers();
    // remove the synchronization leaving the comparers independent
    void unSynchronizeOpenComparers();

    //update the comparers
    void Update();

    //show all the comparers
    void show();

private:

// vector to store the open comparers
std::vector<QGoComparer*>         m_OpenComparers;
QGoComparerSync*                   m_Synchronizer;



};

#endif // QGOCOMPAREORCHESTRA_H
