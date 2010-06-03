#ifndef QGOCOMPARER2D_H
#define QGOCOMPARER2D_H

#include "itkImageToVTKImageFilter.h"
#include "itkSmartPointer.h"
#include "itkImage.h"


#include "QGoComparer.h"

#include "SnapshotHelper.h"
#include "vtkSmartPointer.h"
//#include "ui_QGoComparer.h"
class vtkCamera;
class vtkImageData;
class vtkEventQtSlotConnect;
class QGoImageView2D;
class QGoCompareOrchestra;


class QGoComparer2D : public QGoComparer {
    Q_OBJECT
public:
    QGoComparer2D(QString iComparerName, QWidget *parent = 0);

    ~QGoComparer2D();

    void PrintOs(ostream &os);

    // returns the type of comparer (2 for 2D, 3 for 3D)
    int GetComparerType();

    /** Set image displayed by the comparer
    */
    virtual void SetImage(vtkImageData* iImage);


    /** Set ITK image displayed by the comparer
    */
    template <typename ITKInputImageType>
    void SetITKImage(typename ITKInputImageType::Pointer iImage)
    {

    typedef typename ITKInputImageType::Pointer ITKimagePointerType;
    // we create the appropriate connector
    typedef typename itk::ImageToVTKImageFilter<ITKInputImageType>
      itkvtkConnectorType;

    typedef typename itkvtkConnectorType::Pointer itkvtkConnectorPointerType;


    itkvtkConnectorPointerType connector = itkvtkConnectorType::New();

    connector->SetInput(iImage);

    connector->Update();

    //connector->DisconnectPipeline();
    //ITKimagePointerType ITKImage = connector->GetOutput();
    //ITKImage->DisconnectPipeline();
    //vtkSmartPointer<vtkImageData> tempvtkImage = vtkSmartPointer<vtkImageData>::New();
    //
    //connector->GetOutput();// = vtkSmartPointer<vtkImageData>::New();
    //tempvtkImage->ShallowCopy(connector->GetOutput());
    // tempvtkImage.Take(connector->GetOutput());
    // we store the transformed image
    //vtkSmartPointer<vtkImageData> tempvtkImage = connector->GetOutput();



    // we set the current image
    SetImage(connector->GetOutput());
    Update();

    }



    /** Set the address of the current orchestra
    */
    void SetCurrentOrchestra(QGoCompareOrchestra* iCurrentOrchestra);

    /** Returns the imageview managed by this comparer
    */
    QGoImageView2D* GetImageView();

public slots:

    /** Save a screenshot of the viewer's content
    */
    QString SnapshotViewXY( const GoFigure::FileType& iType,
    const QString& iBaseName = tr( "Snapshot" ) );


private:

  // create the viewer contained in the widget
  void createViewer();


};

#endif // QGOCOMPARER2D_H
