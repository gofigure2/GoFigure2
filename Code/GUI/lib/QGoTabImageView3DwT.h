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
#ifndef __QGoTabImageView3DwT_h
#define __QGoTabImageView3DwT_h

#include "QGoTabElementBase.h"

#include "GoFigureFileInfoMultiIndexContainerHelper.h"
#include "itkMegaCaptureReader.h"
#include "ContourMeshContainer.h"
#include "QGoPrintDatabase.h"

#include "GoFigureMeshAttributes.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <string>

#include "vtkSmartPointer.h"

// base segmentation dock widget
class QGoContourSegmentationBaseDockWidget;
class QGoMeshSegmentationBaseDockWidget;

class QGoImageView3D;
class QGoNavigationDockWidget;
class QGoPrintDatabase;

#if defined ENABLEFFMPEG || defined ENABLEAVI
class QGoVideoRecorder;
#endif /* ENABLEFFMPEG */

class vtkLSMReader;
class vtkImageData;

class vtkMySQLDatabase;
class vtkProperty;
// class vtkQuadricLODActor;
class vtkActor;
class vtkViewImage2D;
class vtkProp3D;

class QGoSeedsSegmentation;

#include "QGoGUILibConfigure.h"

/**
\class QGoTabImageView3DwT
\brief
\example GUI/lib/qgotabimageview3dwt.cxx
*/
class QGOGUILIB_EXPORT QGoTabImageView3DwT:public QGoTabElementBase
{
  Q_OBJECT
public:
  /**
   * \brief Default Constructor
   * \param parent
   */
  explicit QGoTabImageView3DwT(QWidget *parent = 0);

  /**
   * \brief Destructor
   */
  virtual ~QGoTabImageView3DwT();

  typedef QGoTabElementBase::QGoDockWidgetStatusPair QGoDockWidgetStatusPair;
  typedef QGoPrintDatabase::NamesDescrContainerType  NamesDescrContainerType;
  typedef QGoPrintDatabase::IDWithColorData          IDWithColorData;

  /**
   * \brief
   * \return
   */
  GoFigure::TabDimensionType GetTabDimensionType() const;

  /**
   *
   * \param[in] iReader
   * \param[in] iTimePoint
   */
  void SetLSMReader(vtkLSMReader *iReader, const int & iTimePoint);

  /**
   *
   * \param[in] iContainer  MegaCapture file container
   * \param[in] iFileType   file type (PNG, JPEG...)
   * \param[in] iHeader     path to MegaCapture header
   * \param[in] iTimePoint  Time point
   */
  void SetMegaCaptureFile(
    const GoFigureFileInfoHelperMultiIndexContainer & iContainer,
    const GoFigure::FileType & iFileType,
    const std::string & iHeader,
    const unsigned int & iTimePoint);

  /**
   * \brief
   */
  virtual void Update();

  /**
   * \brief
   * \param parent
   */
  void setupUi(QWidget *parent);

  /**
   * \brief
   * \param parent
   */
  void retranslateUi(QWidget *parent);

  /**
   *
   */
  virtual void WriteSettings() {}
  /**
   *
   */
  virtual void ReadSettings() {}

  ContourMeshContainer * GetContourContainer()
  {
    return m_ContourContainer;
  }

  ContourMeshContainer * GetMeshContainer()
  {
    return m_MeshContainer;
  }

  template< class TIndex >
  void AddTraceFromNodesManager(
    typename ContourMeshContainer::MultiIndexContainer::index< TIndex >::type::iterator iIt,
    const std::string & iTrace)
  {
    // If we want to add a contour
    if ( iTrace.compare("contour") == 0 )
      {
      AddContourFromNodes< TIndex >(iIt);
      }
    // If we want to add a mesh
    if ( iTrace.compare("mesh") == 0 )
      {
      AddMeshFromNodes< TIndex >(iIt);
      }
  }

  template< class TIndex >
  void AddMeshFromNodes(
    typename ContourMeshContainer::MultiIndexContainer::index< TIndex >::type::iterator iIt)
  {
    VisualizeMesh< TIndex >(iIt);
  }

  //-------------------------------------------------------------------------
  template< class TIndex >
  void
  AddContourFromNodes(
    typename ContourMeshContainer::MultiIndexContainer::index< TIndex >::type::iterator iIt)
  {
    vtkPolyData *nodes = iIt->Nodes;

    if ( nodes->GetNumberOfPoints() > 2 )
      {
      int dir = ContourMeshContainer::ComputeDirectionFromContour(nodes);

      if ( dir != -1 )
        {
        m_ImageView->EnableContourWidget(true);
        m_ImageView->InitializeContourWidgetNodes(dir, nodes);
        vtkPolyData *contour = m_ImageView->GetContourRepresentationAsPolydata(dir);

        VisualizeContour< TIndex >(iIt, contour);

        m_ImageView->InitializeContourWidgetNodes(dir, NULL);
        m_ImageView->EnableContourWidget(false);
        }
      }
  }

  int GetSliceViewXY() const;

  int GetSliceViewXZ() const;

  int GetSliceViewYZ() const;

  int GetTimePoint() const;

  QGoTraceManualEditingWidget *     GetTraceManualEditingWidget();

  QGoPrintDatabase *m_DataBaseTables;

  GoFigureMeshAttributes ComputeMeshAttributes(vtkPolyData *iMesh);

signals:
  void TimePointChanged(int TimePoint);

  void SliceViewXYChanged(int Slice);

  void SliceViewXZChanged(int Slice);

  void SliceViewYZChanged(int Slice);

  void FullScreenViewChanged(int FullScreen);

  void UpdateBookmarkOpenActions(std::vector< QAction * > );

  void ContourRepresentationPropertiesChanged();

  void StartMeshSegmentation(vtkPoints *iPoints);

  void StartContourSegmentation(vtkPoints *iPoints);

public slots:
  void SetTimePoint(const int &);

  void TakeSnapshot();

  void SetSliceView();

  void GoToDefaultMenu(bool iEnable = false);

#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  void SetRendererWindow(int);

#endif /* ENABLEVIDEORECORD */

  QString SnapshotViewXY( const GoFigure::FileType & iType,
                          const QString & iBaseName = QString("snapshot") );

  QString SnapshotView2( const GoFigure::FileType & iType,
                         const QString & iBaseName = QString("snapshot") );

  QString SnapshotView3( const GoFigure::FileType & iType,
                         const QString & iBaseName = QString("snapshot") );

  QString SnapshotViewXYZ( const GoFigure::FileType & iType,
                           const QString & iBaseName = QString("snapshot") );

  void SetSliceViewXY(const int &);

  void SetSliceViewXZ(const int &);

  void SetSliceViewYZ(const int &);

  void SetFullScreenView(const int & iS);

  void Quadview();

  void FullScreenViewXY();

  void FullScreenViewXZ();

  void FullScreenViewYZ();

  void FullScreenViewXYZ();

  void ChangeLookupTable();

  void ShowScalarBar(const bool &);

  void ChangeBackgroundColor();

  void ShowAllChannels(bool iChecked);

  void ShowOneChannel(int iChannel);

  void ValidateContour();

  int SaveAndVisuContour(vtkPolyData *iView = NULL);

  //void SaveAndVisuContoursList(std::vector<vtkPolyData* >* iContours);

  /** \brief Save a mesh in the database and render the mesh
   * at the given time point.
  \todo to be renamed */
  void  SaveAndVisuMesh(vtkPolyData *iView, unsigned int iTCoord);

  /** \brief Save a mesh in the database and render the mesh.
   * at the current time point
  \todo to be renamed */
  void  SaveAndVisuMesh(vtkPolyData *iView);

  void ReEditContour(const unsigned int & iId);

  void HighlightXY();

  void HighlightXZ();

  void HighlightYZ();

  void HighlightXYZ();

  void Change3DPerspectiveToAxial();

  void Change3DPerspectiveToCoronal();

  void Change3DPerspectiveToSagittal();

  void CreateMeshFromSelectedContours(std::list< unsigned int > ListContourIDs, int iMeshID);

  void VisibilityXYZ();

  void AddContourForMeshToContours(vtkPolyData *);

protected:
  QHBoxLayout *                                  m_HBoxLayout;
  QSplitter *                                    m_VSplitter;
  QGoImageView3D *                               m_ImageView;
  std::vector< vtkSmartPointer< vtkLSMReader > > m_LSMReader;
  std::vector< vtkImageData * >                  m_InternalImages;
  vtkImageData *                                 m_Image;

  vtkProperty *m_HighlightedContoursProperty;
  vtkProperty *m_HighlightedMeshesProperty;

  itk::MegaCaptureReader::Pointer           m_MegaCaptureReader;
  GoFigureFileInfoHelperMultiIndexContainer m_FileList;
  GoFigure::FileType                        m_FileType;
  QColor                                    m_BackgroundColor;
  QAction *                                 m_BackgroundColorAction;
  QAction *                                 m_TakeSnapshotAction;

  int m_PCoord;
  int m_RCoord;
  int m_CCoord;
  int m_XTileCoord;
  int m_YTileCoord;
  int m_ZTileCoord;
  int m_TCoord;

  unsigned int m_ContourId;

  QGoNavigationDockWidget *m_NavigationDockWidget;

  // base segmentation dockwidget for contours
  QGoContourSegmentationBaseDockWidget *m_ContourSegmentationDockWidget;

  // base segmentation dockwidget for meshes
  QGoMeshSegmentationBaseDockWidget *m_MeshSegmentationDockWidget;

  vtkPoints *m_Seeds;

  /// \todo remove m_FFMPEGWriter and m_AVIWriter from this class
  #if defined ENABLEFFMPEG || defined ENABLEAVI
  QGoVideoRecorder *m_VideoRecorderWidget;
  #endif /* ENABLEFFMPEG || ENABLEAVI */

  ContourMeshContainer *m_ContourContainer;
  ContourMeshContainer *m_MeshContainer;
  ContourMeshContainer *m_TrackContainer;

  void SaveContour(vtkPolyData *contour, vtkPolyData *contour_nodes);

  std::vector< vtkActor * > VisualizeContour(vtkPolyData *contour);

  std::vector< vtkActor * > VisualizeMesh(vtkPolyData *iMesh);

  //int VisualizeContour(const int& iContourID,
  //    const unsigned int& iTCoord, vtkPolyData* contour,
  //    vtkPolyData* contour_nodes, const double iRGBA[4]);

  template< class TIndex >
  void VisualizeContour(
    typename ContourMeshContainer::MultiIndexContainer::index< TIndex >::type::iterator iIt,
    vtkPolyData *iContour)
  {
    if ( ( iContour->GetNumberOfPoints() > 2 ) && ( m_TCoord >= 0 ) )
      {
      m_ContourId = iIt->TraceID;
      const double *RGBA = iIt->rgba;

      bool visibility =
        ( static_cast< unsigned int >( m_TCoord ) == iIt->TCoord );

      vtkProperty *contour_property = vtkProperty::New();
      contour_property->SetColor(RGBA[0], RGBA[1], RGBA[2]);
      contour_property->SetOpacity(RGBA[3]);

      vtkPolyData *contour_copy = vtkPolyData::New();
      contour_copy->DeepCopy(iContour);

      std::vector< vtkActor * > contour_actor =
        this->AddContour(contour_copy, contour_property);

      contour_copy->Delete();
      contour_property->Delete();

      // fill the container
      m_ContourContainer->UpdateVisualizationForGivenElement< TIndex >(
        iIt,
        contour_actor,
        false,       //highlighted
        visibility); //visible

      // to increase accurately
      ++m_ContourId;
      }
  }

  template< class TIndex >
  void
  VisualizeMesh(
    typename ContourMeshContainer::MultiIndexContainer::index< TIndex >::type::iterator iIt)
  {
    const double *iRgba = iIt->rgba;
    vtkPolyData * iMesh = iIt->Nodes;

    if ( iMesh )
      {
      bool visibility =
        ( static_cast< unsigned int >( m_TCoord ) == iIt->TCoord );

      vtkProperty *mesh_property = vtkProperty::New();
      mesh_property->SetColor(iRgba[0], iRgba[1], iRgba[2]);
      mesh_property->SetOpacity(iRgba[3]);

      /// TODO fix bug, shouldn't be required
      std::vector< vtkActor * > mesh_actor = this->AddContour(iMesh, mesh_property);
      mesh_property->Delete();

      m_MeshContainer->UpdateVisualizationForGivenElement< TIndex >(iIt,
                                                                    mesh_actor,
                                                                    false,
                                                                    visibility);
      }
  }

  std::vector< int > GetBoundingBox(vtkPolyData *contour);

  void CreateContour(vtkPolyData *contour_nodes, vtkPolyData *iView);

  /**
   * \param[in] iMeshID
   * \param[in] iDir
   * \param[in] iHighlighted
   * \param[in] iR red component in [0,1]
   * \param[in] iG green component in [0,1]
   * \param[in] iB blue component in [0,1]
   * \param[in] iA alpha component in [0,1]
   * \param[in] iSaveInDataBase save in data base if true
   * \todo Alpha component is not used at all, it is assumed to be opaque
   */
  void SaveMesh(vtkPolyData *iView);

  void GetBackgroundColorFromImageViewer();

  void SetBackgroundColorToImageViewer();

  void CreateAllViewActions();

  void CreateToolsActions();

  void CreateBookmarkActions();

  void CreateModeActions();

  void CreateVisuDockWidget();

  // segmentation dockwidgets
  void CreateContourSegmentationDockWidget();

  void CreateMeshSegmentationDockWidget();

  void CreateDataBaseTablesConnection();

#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  void CreateVideoRecorderWidget();

#endif /* ENABLEVIDEORECORD */

  int *GetImageCoordinatesFromWorldCoordinates(double pos[3]);

//   std::vector< vtkQuadricLODActor* >
  std::vector< vtkActor * > AddContour(vtkPolyData *dataset,
                                       vtkProperty *property = NULL);

  void SetTimePointWithLSMReaders();

  void SetTimePointWithMegaCapture();

  void SetTimePointWithMegaCaptureExperimental(int channel);

  bool m_TraceWidgetRequiered;
protected slots:
  void AddBookmark();

  void GetTheRelatedToDBActions();

  void GetTheOpenBookmarksActions();

  void OpenExistingBookmark();

  void ShowTraceDockWidgetForContour(bool ManualSegVisible = true);

  void ShowTraceDockWidgetForMesh(bool MeshVisible = true);

  void UpdateSeeds();

  void GoToLocation(int iX, int iY, int iZ, int iT);

  void CloseTabRequest(bool iTable);

  void RequieresTraceWidget(bool iTable);

  /**
   * \brief Mouse interaction style allows contours segmentation, according to
   * the selected type of segmentation
   */
  void ManualInteractorBehavior(bool);

  /**
   * \brief Mouse interaction style allows meshes segmentation, according to
   * the selected type of segmentation
   */
  void SeedInteractorBehavior(bool);

  /**
   * \brief Mouse interaction style set as default
   */
  virtual void DefaultInteractorBehavior(bool);

  /**
   * \brief Mouse interaction style allows user to zoom in/out volume with all
   * buttons
   */
  virtual void ZoomInteractorBehavior(bool);

  /**
   * \brief Mouse interaction style allows user to pan volume with all buttons
   */
  virtual void PanInteractorBehavior(bool);

  /**
   * \brief Mouse interaction style allows user to pick contours
   */
  // void ContourPickingInteractorBehavior(bool);

  /**
   * \brief Mouse interaction style allows user to pick contours
   */
  void ActorPickingInteractorBehavior(bool);

  void DistanceWidgetInteractorBehavior(bool);

  void AngleWidgetInteractorBehavior(bool);

  void Box3DPicking(bool);

  void PlaneWidgetInteractorBehavior(bool);

  void ImportContours();

  void ImportMeshes();

  /**
  \brief give the adress for the contours and meshes container to the QGoPrintDatabase,
  once the database variables have been set for the QGoPrintDatabase
  */
  void SetTheContainersForDB();

  void ChannelTimeMode( int );

  void ChannelTimeModeBool( bool );

  void handleChannelTimeMode();

private:

  bool m_ChannelClassicMode;
  int  m_ChannelOfInterest;

  Q_DISABLE_COPY(QGoTabImageView3DwT);
};

#endif
