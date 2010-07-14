/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#ifndef __QGoImageView3D_h
#define __QGoImageView3D_h

#include "QGoImageView.h"
#include "SnapshotHelper.h"

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QSplitter>
#include <QSlider>

#include <map>

#include "vtkInteractorStyleImage2D.h"
#include "QGoGUILibConfigure.h"

#include "vtkCommand.h"
#include "vtkSeedRepresentation.h"

#include "vtkPoints.h"

class QVTKWidget;
class vtkEventQtSlotConnect;
class QSplitterChild;

class vtkProp3D;
class vtkProperty;
class vtkImageData;
class vtkViewImage2D;
class vtkViewImage3D;
class vtkViewImage2DCollection;

class vtkSeedWidget;
class vtkConstrainedPointHandleRepresentation;
class vtkImageActorPointPlacer;

/**
\class QGoImageView3D
\brief class for the visualization of 3D Image represented by one vtkImageData*.
\example GUI/lib/qgoimageview3d.cxx
*/
class QGOGUILIB_EXPORT QGoImageView3D : public QGoImageView
{
  Q_OBJECT
public:
  /** \brief Constructor by default */
  explicit QGoImageView3D( QWidget* parent = 0 );

  /** \brief Destructor. */
  virtual ~QGoImageView3D();

  std::vector< vtkSeedWidget* >                            SeedWidget;
  std::vector< vtkConstrainedPointHandleRepresentation* >  Handle;
  std::vector< vtkSeedRepresentation* >                    SeedRep;

  /** \brief Set the image to be displaid. */
  virtual void SetImage( vtkImageData* iImage );

  /** \brief Get the displaid image.*/
  virtual vtkImageData* GetImage();

  virtual void Update();

  vtkViewImage3D* GetImageViewer3D();

  vtkImageActor* GetImageActor( const int& );
  QVTKInteractor* GetInteractor( const int& );

  virtual void RemoveActor( const int& iId, vtkActor* iActor );
  virtual void AddActor( const int& iId, vtkActor* iActor );

  std::vector< vtkActor* > AddMesh( const int& iId,
       vtkPolyData* dataset,
       vtkProperty* property = NULL );

//   std::vector< vtkQuadricLODActor* >
  std::vector< vtkActor* > AddContour( const int& iId,
      vtkPolyData* dataset,
      vtkProperty* property = NULL );

  virtual void setupUi( QWidget* parent );
  virtual void retranslateUi(QWidget *parent);

  int GetFullScreenView( ) const;

  void SaveStateSplitters();

  int GetSliceViewXY() const;
  int GetSliceViewXZ() const;
  int GetSliceViewYZ() const;

  virtual void ChangeActorProperty( vtkProp3D* iActor, vtkProperty* iProperty );
  virtual void ChangeActorProperty( int iDir, vtkProp3D* iActor, vtkProperty* iProperty );

  std::list< vtkProp3D* > GetListOfPickedActors();
  std::list< vtkProp3D* > GetListOfUnPickedActors();
  std::list< vtkProp3D* > GetListOfModifiedActors3D();

  void ShowAnnotations();
  void ShowSplinePlane();
  void ShowCube3D();
  void UpdateRenderWindows();

  /**
   * \brief Define the cursor interactor style to the normal behaviour
   */
  void SetDefaultInteractionStyle( vtkViewImage2D&);
  /**
   * \brief Use the default interactor style
   */
  void DefaultMode();
  /**
   * \brief Use the zoom interactor style
   */
  void ZoomMode();
  /**
   * \brief Use the pan interactor style
   */
  void PanMode();
  /**
   * \brief Use the one click interactor style
   */
  void OneClickMode();
  /**
   * \brief Enable the One Click mode (usefull since it is in a vtk widget)
   */
  void EnableOneClickMode();
  /**
   * \brief Disable the One Click mode (usefull since it is in a vtk widget)
   */
  void DisableOneClickMode();
  /**
   * \brief Use the contour picking interactor style
   */
  void ContourPickingMode();
  /**
   * \brief Enable the contour picking interactor style
   */
  void EnableContourPickingMode();
  /**
   * \brief Disable the contour picking interactor style
   */
  void DisableContourPickingMode();
  /**
   * \brief Use the mesh picking interactor style
   */
  void MeshPickingMode();
  /**
   * \brief Disable the mesh picking interactor style
   */
  void DisableMeshPickingMode();
  /**
   * \brief Creates a box in 3d view to allow multiple meshes selection
   */
  void SetBox3DPicking( bool );


  /**
   * \brief Get all the seeds positions.
   * The seeds have been defined using the one click mode.
   */
  vtkPoints* GetAllSeeds();
  /**
   * \brief Clear all the seeds positions after using it.
   */
  void       ClearAllSeeds();

  /**
   * \brief Get a list of all the picked contours
   */
  std::list< vtkProp3D* > GetListOfPickedContours();

  /**
   * \brief Get a list of all the unpicked contours
   */
  std::list< vtkProp3D* > GetListOfUnPickedContours();

  vtkProp* GetPickedActor();

  void ResetWindowLevel();

signals:
  void SliceViewXYChanged( int Slice );
  void SliceViewXZChanged( int Slice );
  void SliceViewYZChanged( int Slice );

  void FullScreenViewChanged( int View );
  void ContoursSelectionChanged();
  void MeshesSelectionChanged();
  void ListMeshesSelectionChanged();


public slots:
  QString SnapshotViewXY( const GoFigure::FileType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotView2( const GoFigure::FileType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotView3( const GoFigure::FileType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotViewXYZ( const GoFigure::FileType& iType,
    const QString& iBaseName = QString( "snapshot" ) );

  void SetSliceViewXY( const int& );
  void SetSliceViewXZ( const int& );
  void SetSliceViewYZ( const int& );

  void SetFullScreenView( const int& iS );

  void SetCamera( int );

  void UpdateScalarBarIn3DWiew();

//   void HighLightContours();

  /**
   *
   * @param
   */
  virtual void SetLookupTable( vtkLookupTable* );

  virtual void ShowScalarBar( const bool& );



protected:
  QSplitter*          VSplitter;
  QSplitterChild*     HtSplitter;
  QSplitterChild*     HbSplitter;

  QWidget*      LayOutWidget1;
  QHBoxLayout*  LayOut1;
  QSlider*      SliderXY;
  QVTKWidget*   QvtkWidget_XY;

  QWidget*      LayOutWidget2;
  QHBoxLayout*  LayOut2;
  QSlider*      SliderXZ;
  QVTKWidget*   QvtkWidget_XZ;

  QWidget*      LayOutWidget3;
  QHBoxLayout*  LayOut3;
  QSlider*      SliderYZ;
  QVTKWidget*   QvtkWidget_YZ;

  QWidget*      LayOutWidget4;
  QHBoxLayout*  LayOut4;
  QVTKWidget*   QvtkWidget_XYZ;

  vtkViewImage3D* m_View3D;

  vtkEventQtSlotConnect*    VtkEventQtConnector;
  int                       IsFullScreen;
  bool                      m_FirstRender;
  bool                      m_Initialized;

  vtkProperty* m_HighlightedContourProperty;

  virtual void resizeEvent( QResizeEvent* event );

  void SetupVTKtoQtConnections();
//   std::map< vtkProp3D*, vtkProperty* > m_ActorsPropertyMap;

  void Quadview();
  void FullScreenViewXY();
  void FullScreenViewXZ();
  void FullScreenViewYZ();
  void FullScreenViewXYZ();

  void InitializeSeedWidgetInteraction();

  bool m_ShowAnnotations;
  bool m_ShowSplinePlane;
  bool m_ShowCube;

protected slots:
  void MoveSliderXY();
  void MoveSliderXZ();
  void MoveSliderYZ();

};

#endif
