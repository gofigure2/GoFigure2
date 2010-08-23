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
#include "ContourMeshStructureHelper.h"
#include "QGoPrintDatabase.h"

#include "GoFigureMeshAttributes.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <string>

#include "vtkSmartPointer.h"

class QGoImageView3D;
class QGoNavigationDockWidget;
class QGoManualSegmentationDockWidget;
class QGoPrintDatabase;
class QGoOneClickSegmentationDockWidget;
class QGoManualSegmentationSettingsDialog;

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
class QGOGUILIB_EXPORT QGoTabImageView3DwT : public QGoTabElementBase
  {
  Q_OBJECT
public:
  /**
   * \brief Default Constructor
   * \param parent
   */
  QGoTabImageView3DwT(QWidget * parent = 0);

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
  void SetLSMReader(vtkLSMReader* iReader, const int& iTimePoint);

  /**
   *
   * \param[in] iContainer  MegaCapture file container
   * \param[in] iFileType   file type (PNG, JPEG...)
   * \param[in] iHeader     path to MegaCapture header
   * \param[in] iTimePoint  Time point
   */
  void SetMegaCaptureFile(
    const GoFigureFileInfoHelperMultiIndexContainer& iContainer,
    const GoFigure::FileType& iFileType,
    const std::string& iHeader,
    const unsigned int& iTimePoint);

  /**
   * \brief
   */
  virtual void Update();

  /**
   * \brief
   * \param parent
   */
  void setupUi(QWidget* parent);
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

  /**
   * \brief Add the current trace in the database and updates the visualization
   * useful when we load a dataset from the databse
   * \param[in] iContourID Trace ID
   * \param[in] iNodes Data to be stored in the DB
   * \param[in] iRgba
   * \param[in] iTCoord Current time point
   * \param[in] iTrace Name of the traces to be loaded (contour or mesh)
   */
  void AddTraceFromNodesManager(const unsigned int& iContourID,
                                vtkPolyData* iNodes,
                                double iRgba[4],
                                const unsigned int& iTCoord,
                                std::string iTrace);
  /**
   *
   * \param[in] iNodes Nodes to be used by
   * \param[in] iRgba[]
   * \param[in] iHighlighted
   */
  void AddContourFromNodes(const unsigned int& iContourID, vtkPolyData* iNodes,
                           const double iRgba[4], const unsigned int& iTCoord);

  void AddMeshFromNodes(const unsigned int& iMeshID, vtkPolyData* iNodes,
                        const double iRgba[4], const unsigned int& iTCoord);

  int GetSliceViewXY() const;
  int GetSliceViewXZ() const;
  int GetSliceViewYZ() const;
  int GetTimePoint() const;
  QGoManualSegmentationDockWidget* GetContourSegmentationWidget();
  QGoTraceManualEditingWidget*     GetTraceManualEditingWidget();
  QGoPrintDatabase* m_DataBaseTables;

  GoFigureMeshAttributes ComputeMeshAttributes(vtkPolyData* iMesh);

signals:
  void TimePointChanged(int TimePoint);
  void SliceViewXYChanged(int Slice);
  void SliceViewXZChanged(int Slice);
  void SliceViewYZChanged(int Slice);
  void FullScreenViewChanged(int FullScreen);
  void UpdateBookmarkOpenActions(std::vector<QAction*>);
  void ContourRepresentationPropertiesChanged();
  void StartMeshSegmentation(vtkPoints* iPoints);
  void StartContourSegmentation(vtkPoints* iPoints);

public slots:
  void SetTimePoint(const int&);
  void TakeSnapshot();
  void SetSliceView();

  void GenerateContourRepresentationProperties();
  void GoToDefaultMenu();

#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
  void SetRendererWindow(int);
#endif /* ENABLEVIDEORECORD */

  QString SnapshotViewXY(const GoFigure::FileType& iType,
                         const QString& iBaseName = QString("snapshot"));
  QString SnapshotView2(const GoFigure::FileType& iType,
                        const QString& iBaseName = QString("snapshot"));
  QString SnapshotView3(const GoFigure::FileType& iType,
                        const QString& iBaseName = QString("snapshot"));
  QString SnapshotViewXYZ(const GoFigure::FileType& iType,
                          const QString& iBaseName = QString("snapshot"));

  void SetSliceViewXY(const int&);
  void SetSliceViewXZ(const int&);
  void SetSliceViewYZ(const int&);

  void SetFullScreenView(const int& iS);
  void Quadview();
  void FullScreenViewXY();
  void FullScreenViewXZ();
  void FullScreenViewYZ();
  void FullScreenViewXYZ();

  void ChangeLookupTable();
  void ShowScalarBar(const bool&);
  void ChangeBackgroundColor();

  void ShowAllChannels(bool iChecked);
  void ShowOneChannel(int iChannel);

  /**
   *
   */
  void LoadAllTracesForCurrentTimePointManager();
  /**
   *
   */
  void LoadAllTracesForGivenTimePoint(const unsigned int& iT,
                                      ContourMeshStructureMultiIndexContainer& iContainer);
  /**
   *
   */
  void RemoveAllTracesForPresentTimePointManager();
  /**
   *
   */
  void RemoveAllTracesForGivenTimePoint(const unsigned int& iT,
                                        ContourMeshStructureMultiIndexContainer& iContainer);
  void RemoveAllTracesForGivenTimePoint(const unsigned int& iT,
                                        const std::string& iTraceName);

  void ValidateContour();

  int SaveAndVisuContour(vtkPolyData* iView);

  void SaveAndVisuContoursList(std::vector<vtkPolyData* >* iContours);

  /** \brief Save a mesh in the database and render the mesh.
  \todo to be renamed */
  void  SaveAndVisuMesh(vtkPolyData* iView);

  void ReinitializeContour();
  void ReEditContour(const unsigned int& iId);

  void HighlightXY();
  void HighlightXZ();
  void HighlightYZ();
  void HighlightXYZ();

  void HighlightContoursXY();
  void HighlightContoursXZ();
  void HighlightContoursYZ();

  /**
   * \brief Calls HighLightTracesFromTable( ... ) with the good
   *  container and trace name
   */
  void HighLightTracesFromTableManager();

  /**
   * \brief Highlights a trace in the visualization
   * \param[in] iContainer Container which contains traces to be highlighted
   * \param[in] iCurrentTrace Name of the current trace useful to initialize
   * the container iterator
   */
  void HighLightTracesFromTable(
    ContourMeshStructureMultiIndexContainer& iContainer,
    std::string iCurrentTrace);

  /**
   * \brief Show a trace in the visualization
   * \param[in] iContainer Container which contains traces to be highlighted
   * \param[in] iCurrentTrace Name of the current trace useful to initialize
   * the container iterator
   */
  void ShowTracesFromTable(ContourMeshStructureMultiIndexContainer& iContainer,
                           ContourMeshStructureMultiIndexContainer* iTbContainer );

  void             SelectContoursInTable();
  void             ListSelectMeshesInTable();
  std::list<int> SelectTraceInTable(
    ContourMeshStructureMultiIndexContainer& iContainer,
    std::list<vtkProp3D*>                  iActorList);

  /**
   * \brief Calls DeleteTracesFromTable( ... ) with the good
   *  container and list
   * \param[in] iList Contains the selected traces in tablewidget
   */
  void DeleteTracesFromTableManager(const std::list<int>& iList);

  /**
   * \brief Deletes the selected traces from the table and visu
   * \param[in] iList Contains the selected traces in tablewidget
   * \param[in] iContainer Useful to delete in the visu
   */
  void DeleteTracesFromTable(ContourMeshStructureMultiIndexContainer& iContainer,
                             const std::list<int>& iList);

  /**
   * \brief Changes the apparence of a contour in the contourWidget
   */
  void ChangeContourRepresentationProperty();

  void Change3DPerspectiveToAxial();
  void Change3DPerspectiveToCoronal();
  void Change3DPerspectiveToSagittal();

  /*
   * \brief Change the visibility of the selected meshes
   */
  void ChangeSelectedMeshesVisibility();

  void ModifyTracesVisibilityFromTableManager();

  void ModifyTracesVisibilityFromTable(
    ContourMeshStructureMultiIndexContainer& iContainer,
    ContourMeshStructureMultiIndexContainer* iTbContainer );

  void CreateMeshFromSelectedContours(std::list<int> ListContourIDs,int iMeshID);

  void HighlightMeshXYZ();

  void ApplyMeshFilterPressed();
  void ApplyContourFilterPressed();

protected:
  QHBoxLayout*                                m_HBoxLayout;
  QSplitter*                                  m_VSplitter;
  QGoImageView3D*                             m_ImageView;
  std::vector<vtkSmartPointer<vtkLSMReader> > m_LSMReader;
  std::vector<vtkImageData*>                  m_InternalImages;
  vtkImageData*                               m_Image;

  double m_LinesWidth;
  QColor m_LinesColor;
  QColor m_NodesColor;
  QColor m_ActiveNodesColor;

  itk::MegaCaptureReader::Pointer           m_MegaCaptureReader;
  GoFigureFileInfoHelperMultiIndexContainer m_FileList;
  GoFigure::FileType                        m_FileType;
  QColor                                    m_BackgroundColor;
  QAction*                                  m_BackgroundColorAction;
  QAction*                                  m_TakeSnapshotAction;

  int m_PCoord;
  int m_RCoord;
  int m_CCoord;
  int m_XTileCoord;
  int m_YTileCoord;
  int m_ZTileCoord;
  int m_TCoord;

  unsigned int                              m_ContourId;
  bool                                      m_ReEditContourMode;

  /// \todo rename as QGoNavigationDockWidget
  QGoNavigationDockWidget*           m_NavigationDockWidget;
  QGoManualSegmentationDockWidget*   m_ContourSegmentationDockWidget;
  QGoOneClickSegmentationDockWidget* m_MeshSegmentationDockWidget;
  QGoSeedsSegmentation*              m_SeedsSegmentation;

  /// \todo remove m_FFMPEGWriter and m_AVIWriter from this class
  #if defined ENABLEFFMPEG || defined ENABLEAVI
  QGoVideoRecorder* m_VideoRecorderWidget;
  #endif /* ENABLEFFMPEG || ENABLEAVI */

  ContourMeshStructureMultiIndexContainer m_ContourContainer;
  ContourMeshStructureMultiIndexContainer m_MeshContainer;

  // ID + color map, real save+real visu
  IDWithColorData SaveContour(vtkPolyData* contour, vtkPolyData* contour_nodes);
  IDWithColorData UpdateContour(vtkPolyData* contour, vtkPolyData* contour_nodes);

  int VisualizeContour(const int& iContourID,
      const unsigned int& iTCoord, vtkPolyData* contour,
      vtkPolyData* contour_nodes, const double iRGBA[4]);

  int* GetBoundingBox(vtkPolyData* contour);

  void CreateContour( vtkPolyData* contour_nodes,vtkPolyData* iView);

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
  IDWithColorData SaveMesh(vtkPolyData* iView, const int& iMeshID,
                           double iRGBA[4], bool NewMesh );

  void VisualizeMesh(vtkPolyData* iView, const int& iMeshID,
      const unsigned int& iTCoord, const double iRGBA[4]);

  void GetBackgroundColorFromImageViewer();
  void SetBackgroundColorToImageViewer();
  void CreateAllViewActions();
  void CreateToolsActions();
  void CreateBookmarkActions();
  void CreateModeActions();
  void CreateVisuDockWidget();
  //void CreateSettingAndDialogSegmentationWidgets();
  void CreateContourSegmentationdockWidget();
  void CreateMeshSegmentationDockWidget();
  void CreateDataBaseTablesConnection();

  template< typename TActor >
  void HighLightActorsInContainer(
    ContourMeshStructureMultiIndexContainer& iContainer, vtkActor* iActor);

  template< typename TActor >
  void HighLightContours();

  template< typename TActor >
  void HighLightMeshes();

#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
  void CreateVideoRecorderWidget();
#endif /* ENABLEVIDEORECORD */

  int* GetImageCoordinatesFromWorldCoordinates(double pos[3]);

//   std::vector< vtkQuadricLODActor* >
  std::vector<vtkActor*> AddContour(vtkPolyData* dataset,
                                    vtkProperty* property = NULL);

  void SetTimePointWithLSMReaders(const int& iTimePoint);
  void SetTimePointWithMegaCapture(const int& iTimePoint);

  void
  ChangeColorOfSelectedTraces(
      ContourMeshStructureMultiIndexContainer& ioContainer,
      ContourMeshStructureMultiIndexContainer* iTbContainer,
      QColor iSelectedColor );

  //void GetTraceColor(double* rgba);

protected slots:
  void AddBookmark();
  void GetTheRelatedToDBActions();
  void GetTheOpenBookmarksActions();
  void OpenExistingBookmark();
  void ShowTraceDockWidgetForContour(bool ManualSegVisible);
  void ShowTraceDockWidgetForMesh(bool MeshVisible);
  void ChangeColorOfSelectedTracesManager(QColor iSelectedColor);

  void GoToLocation(int iX, int iY, int iZ, int iT);

  /**
   * \brief Mouse interaction style allows contours segmentation, according to
   * the selected type of segmentation
   */
  void ContourInteractorBehavior(bool);
  /**
   * \brief Mouse interaction style allows meshes segmentation, according to
   * the selected type of segmentation
   */
  void MeshInteractorBehavior(bool);
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
 /* void ContourPickingInteractorBehavior(bool);
*/

  /**
   * \brief Mouse interaction style allows user to pick contours
   */
  void ActorPickingInteractorBehavior(bool);

  void DistanceWidgetInteractorBehavior(bool);
  void AngleWidgetInteractorBehavior(bool);
  void Box3DPicking(bool);

  void ImportContours();
  void ImportMeshes();

private:
  Q_DISABLE_COPY(QGoTabImageView3DwT);
  };

#include "QGoTabImageView3DwT.txx"
#endif
