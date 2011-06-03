#include "GoTransferFunctionEditorWidget.h"

// Gofigure
#include "GoTransferFunctionWidget.h"
#include "QGoLUTDialog.h"

//qt
#include "hoverpoints.h"

//vtk
#include "vtkLookupTable.h"
#include "vtkImageAccumulate.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPiecewiseFunction.h"

// temp
#include <iostream>

//-------------------------------------------------------------------------

GoTransferFunctionEditorWidget::GoTransferFunctionEditorWidget(QWidget *parent,
                                                               QString iChannel)
    : QWidget(parent )
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setSpacing(1);
  vbox->setMargin(1);

  m_red_shade = new GoTransferFunctionWidget(
        GoTransferFunctionWidget::RedShade, this);
  m_green_shade = new GoTransferFunctionWidget(
        GoTransferFunctionWidget::GreenShade, this);
  m_blue_shade = new GoTransferFunctionWidget(
        GoTransferFunctionWidget::BlueShade, this);
  m_alpha_shade = new GoTransferFunctionWidget(
        GoTransferFunctionWidget::ARGBShade, this);

  m_LUT = NULL;

  m_Channel = iChannel;

  QPushButton *presetLUTPushButton = new QPushButton("Preset LUT", this);
  presetLUTPushButton->setEnabled(false);


  QHBoxLayout *lutLayout = new QHBoxLayout;
  QPushButton *saveLUTPushButton = new QPushButton("Save LUT", this);
  QPushButton *loadLUTPushButton = new QPushButton("Load LUT", this);
  lutLayout->addWidget(loadLUTPushButton);
  lutLayout->addWidget(saveLUTPushButton);

  QPushButton *okPushButton = new QPushButton("OK", this);
  QPushButton *resetLUTPushButton = new QPushButton("Reset", this);

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(okPushButton);
  layout->addWidget(resetLUTPushButton);

  QHBoxLayout *nameLayout = new QHBoxLayout;
  QLabel* channelName = new QLabel("Channel:");
  QLabel* channelNameText = new QLabel(iChannel);
  nameLayout->addWidget(channelName);
  nameLayout->addWidget(channelNameText);

  vbox->addLayout(nameLayout);
  vbox->addWidget(m_red_shade);
  vbox->addWidget(m_green_shade);
  vbox->addWidget(m_blue_shade);
  vbox->addWidget(m_alpha_shade);
  vbox->addLayout(lutLayout);
  vbox->addWidget(presetLUTPushButton);
  vbox->addLayout(layout);

  connect(m_red_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
  connect(m_green_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
  connect(m_blue_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
  connect(m_alpha_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
  connect(m_alpha_shade, SIGNAL(colorsChanged()), this, SLOT(updateOpacityTF()));

  connect(okPushButton, SIGNAL(released()), this, SLOT(close()));
  connect(okPushButton, SIGNAL(released()), this, SLOT(savePoints()));

  connect(resetLUTPushButton, SIGNAL(pressed()), this, SLOT(resetLUT()));
  connect(saveLUTPushButton, SIGNAL(pressed()), this, SLOT(saveLUT()));
  connect(loadLUTPushButton, SIGNAL(pressed()), this, SLOT(readLUT()));
  // useless...?
  connect(presetLUTPushButton, SIGNAL(pressed()), this, SLOT(presetLUT()));


  // enable event filter
  this->installEventFilter(this);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
inline static bool x_less_than(const QPointF &p1, const QPointF &p2)
{
  return p1.x() < p2.x();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoTransferFunctionEditorWidget::pointsUpdated()
{
  changeAlphaGradients();

  // update the LUT
  if(m_LUT)
    {
    m_alpha_shade->UpdateLookupTable(m_LUT);
    // send signal to update the visualization
    emit updateVisualization();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
static void set_shade_points(const QPolygonF &points, GoTransferFunctionWidget *shade)
{
  shade->hoverPoints()->setPoints(points);
  shade->hoverPoints()->setPointLock(0, HoverPoints::LockToLeft);
  shade->hoverPoints()->setPointLock(points.size() - 1, HoverPoints::LockToRight);
  shade->update();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoTransferFunctionEditorWidget::setGradientStops(const QGradientStops &stops)
{
  QPolygonF pts_red, pts_green, pts_blue, pts_alpha;

  qreal h_red = m_red_shade->height();
  qreal h_green = m_green_shade->height();
  qreal h_blue = m_blue_shade->height();
  qreal h_alpha = m_alpha_shade->height();

  for (int i=0; i<stops.size(); ++i) {
      qreal pos = stops.at(i).first;
      QRgb color = stops.at(i).second.rgba();
      pts_red << QPointF(pos * m_red_shade->width(), h_red - qRed(color) * h_red / 255);
      pts_green << QPointF(pos * m_green_shade->width(), h_green - qGreen(color) * h_green / 255);
      pts_blue << QPointF(pos * m_blue_shade->width(), h_blue - qBlue(color) * h_blue / 255);
      pts_alpha << QPointF(pos * m_alpha_shade->width(), h_alpha - qAlpha(color) * h_alpha / 255);
  }

  set_shade_points(pts_red, m_red_shade);
  set_shade_points(pts_green, m_green_shade);
  set_shade_points(pts_blue, m_blue_shade);
  set_shade_points(pts_alpha, m_alpha_shade);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddPoints( const std::vector< std::map< unsigned int, unsigned int> >& iRGBA)
{


  //red
  QPolygonF redPoints;
  computePointsFromMap(iRGBA[0], redPoints);
  m_red_shade->AddPoints(redPoints);

  // green
  QPolygonF greenPoints;
  computePointsFromMap(iRGBA[1], greenPoints);
  m_green_shade->AddPoints(greenPoints);

  // blue
  QPolygonF bluePoints;
  computePointsFromMap(iRGBA[2], bluePoints);
  m_blue_shade->AddPoints(bluePoints);

  // alpha
  QPolygonF alphaPoints;
  computePointsFromMap(iRGBA[3], alphaPoints);
  m_alpha_shade->AddPoints(alphaPoints);

  // update histogram and alpha gradient
  pointsUpdated();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
computePointsFromMap(const std::map< unsigned int, unsigned int>& iMap, QPolygonF& iPoints)
{
  // all shades have same width and height
  qreal width = m_red_shade->width();
  qreal height = m_red_shade->height();

  std::map< unsigned int, unsigned int>::const_iterator it0;
  std::map< unsigned int, unsigned int>::const_iterator it255;
  it0 = iMap.begin();
  it255 = iMap.end();

  // check x and y
  while(it0!=it255)
    {
    iPoints << QPointF((qreal)(it0->first)*width/255,
                         height*(1-(qreal)(it0->second)/255));
    ++it0;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddLookupTable(vtkLookupTable* iLUT)
{
  m_LUT = iLUT;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddOpacityTransferFunction(vtkPiecewiseFunction* iOpacity)
{
  m_OpacityTF = iOpacity;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddHistogram(vtkImageAccumulate* iHistogram)
{
  int x_range = iHistogram->GetOutput()->GetNumberOfPoints();
  vtkDataArray* scalars = iHistogram->GetOutput()->GetPointData()->GetScalars();
  double* range = iHistogram->GetOutput()->GetScalarRange();

  QVector<qreal> histo;

  // do we want the background values in the histogram...?
  for(int i=0; i<x_range; ++i)
    {
    double value;
    value = scalars->GetTuple1(i);
    histo.push_back(log(value)/log(range[1]));
    }

  m_red_shade->SetHistogram(histo);
  m_green_shade->SetHistogram(histo);
  m_blue_shade->SetHistogram(histo);
  m_alpha_shade->SetHistogram(histo);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddColor(const std::vector<double>& iColor)
{
  m_Color.setRedF(iColor[0]/255);
  m_Color.setGreenF(iColor[1]/255);
  m_Color.setBlueF(iColor[2]/255);
  m_Color.setAlphaF(iColor[3]/255);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
AddName(QString iChannel)
{
  m_Channel = iChannel;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
presetLUT()
{
  vtkLookupTable *lut = QGoLUTDialog::GetLookupTable( this,
                                                      tr("Choose one look-up table") );
  if ( lut )
    {
      if(m_LUT)
        {
          m_LUT->DeepCopy(lut);
          //m_red_shade->setEnabled(false);
          m_red_shade->setDisabled(true);
          //m_green_shade->setEnabled(false);
          //m_blue_shade->setEnabled(false);
          // update alpha color..

        //m_->UpdateLookupTable(m_LUT);
        // send signal to update the visualization
        emit updateVisualization();
        }
    // free memory since it is not freed in the QGoLUTDialog
    lut->Delete();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
resetLUT()
{
  m_red_shade->Reset(m_Color.redF());
  m_green_shade->Reset(m_Color.greenF());
  m_blue_shade->Reset(m_Color.blueF());
  m_alpha_shade->Reset(m_Color.alphaF());

  // update gradient and transfer function
  pointsUpdated();

  //update opacity TF
  updateOpacityTF();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
savePoints()
{
  std::vector< std::map< unsigned int, unsigned int> > pointsVector;
  pointsVector.resize(4);

  // RED ------------------------------
  QPolygonF redPoints = m_red_shade->points();
  computeMapFromPoints(pointsVector[0], redPoints);

  // GREEN ------------------------------
  QPolygonF greenPoints = m_green_shade->points();
  computeMapFromPoints(pointsVector[1], greenPoints);

  // BLUE ------------------------------
  QPolygonF bluePoints = m_blue_shade->points();
  computeMapFromPoints(pointsVector[2], bluePoints);

  // BLUE ------------------------------
  QPolygonF alphaPoints = m_alpha_shade->points();
  computeMapFromPoints(pointsVector[3], alphaPoints);

  emit updatePoints(m_Channel, pointsVector);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
computeMapFromPoints(std::map< unsigned int, unsigned int>& iMap, const QPolygonF& iPoints)
{
  // all shades have same width and height
  qreal width = m_red_shade->width();
  qreal height = m_red_shade->height();
  int numberOfPoints = iPoints.size();

  if(numberOfPoints>255)
  {
    qDebug() << "Too many points: " << numberOfPoints << " points";
    return;
  }

  for(int i=0; i<numberOfPoints; ++i)
    {
    unsigned int x = (iPoints.at(i).x())*255/width;
    unsigned int y = (1-(iPoints.at(i).y())/height)*255;
    iMap[x]  = y;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
GoTransferFunctionEditorWidget::
eventFilter(QObject *object, QEvent *event)
{
      switch (event->type()) {

      case QEvent::Resize:
        {
        changeAlphaGradients();
        return true;
        }
      default:
        break;
      }
      return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
changeAlphaGradients()
{
  qreal w = m_alpha_shade->width();

  QGradientStops stops;

  QPolygonF points;

  points += m_red_shade->points();
  points += m_green_shade->points();
  points += m_blue_shade->points();
  points += m_alpha_shade->points();

  qSort(points.begin(), points.end(), x_less_than);

  for (int i=0; i<points.size(); ++i) {
      qreal x = int(points.at(i).x());
      if (i < points.size() - 1 && x == points.at(i+1).x())
          continue;
      QColor color((0x00ff0000 & m_red_shade->colorAt(int(x))) >> 16,
                   (0x0000ff00 & m_green_shade->colorAt(int(x))) >> 8,
                   (0x000000ff & m_blue_shade->colorAt(int(x))),
                   (0xff000000 & m_alpha_shade->colorAt(int(x))) >> 24);

      stops << QGradientStop(x / w, color);
  }

  m_alpha_shade->setGradientStops(stops);

  emit gradientStopsChanged(stops);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
saveLUT()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"));
  //qDebug() << "filename: " << fileName;
  QFile file(fileName);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
      //if (!)
         // return 1;

  QTextStream out(&file);

  // format text file - xml style
  // RED ------------------------------
  out <<"<RED> ";
  WriteLUTComponent(m_red_shade, out);
  out <<"</RED>\n";
  // GREEN ------------------------------
  out <<"<GREEN> ";
  WriteLUTComponent(m_green_shade, out);
  out <<"</GREEN>\n";
  // BLUE ------------------------------
  out <<"<BLUE> ";
  WriteLUTComponent(m_blue_shade, out);
  out <<"</BLUE>\n";
  // ALPHA ------------------------------
  out <<"<ALPHA> ";
  WriteLUTComponent(m_alpha_shade, out);
  out <<"</ALPHA>\n";

  file.close();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
WriteLUTComponent(GoTransferFunctionWidget* iTFWidget, QTextStream& iStream)
{
  qreal width = iTFWidget->width();
  qreal height = iTFWidget->height();

  int numberOfPoints = iTFWidget->points().size();

  for(int i=0; i<numberOfPoints; ++i)
    {
    unsigned int x = (iTFWidget->points().at(i).x())*255/width;
    unsigned int y = (1-(iTFWidget->points().at(i).y())/height)*255;
    iStream << x  << " " << y << " ";
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
readLUT()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Save File"));
  //qDebug() << "filename: " << fileName;
  QFile file(fileName);
  file.open(QIODevice::ReadOnly | QIODevice::Text);

  QTextStream in(&file);
  QString firstBalise;
  in >> firstBalise;

  ReadLUTComponent(m_red_shade, in, "</RED>");
  ReadLUTComponent(m_green_shade, in, "</GREEN>");
  ReadLUTComponent(m_blue_shade, in, "</BLUE>");
  ReadLUTComponent(m_alpha_shade, in, "</ALPHA>");

  file.close();

  // update histogram and alpha gradient
  pointsUpdated();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
ReadLUTComponent(GoTransferFunctionWidget* iTFWidget, QTextStream& iStream, const QString& iBalise)
{
  qreal width = m_red_shade->width();
  qreal height = m_red_shade->height();

  QString x,y;

  QPolygonF points;
  while (1)
    {
    iStream >> x;
    iStream >> y;
    if(x!=iBalise)
      {
      points << QPointF((x.toDouble())*width/255,
                             height*(1-(y.toDouble()/255)));
      }
    else
      {
      iTFWidget->AddPoints(points);
      break;
      }

    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
updateOpacityTF()
{
  // all shades have same width and height
  qreal width = m_red_shade->width();
  qreal height = m_red_shade->height();
  int numberOfPoints = m_alpha_shade->points().size();

  m_OpacityTF->Initialize();

  for(int i=0; i<numberOfPoints; ++i)
    {
    // x 0 to 255
    double x = (m_alpha_shade->points().at(i).x())*255/width;
    // y 0 to 1
    double y = (1-(m_alpha_shade->points().at(i).y())/height);
    m_OpacityTF->AddPoint(x, y);
    }
  m_OpacityTF->Modified();
}
