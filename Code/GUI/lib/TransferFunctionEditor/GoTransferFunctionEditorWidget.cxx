#include "GoTransferFunctionEditorWidget.h"

#include "GoTransferFunctionWidget.h"

#include "hoverpoints.h"

//vtk
#include "vtkLookupTable.h"
#include "vtkImageAccumulate.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

// temp
#include <iostream>

//-------------------------------------------------------------------------

GoTransferFunctionEditorWidget::GoTransferFunctionEditorWidget(QWidget *parent, QPolygonF iPoints)
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

  QPushButton *okPushButton = new QPushButton("OK", this);
  QPushButton *resetPushButton = new QPushButton("Reset", this);

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(okPushButton);
  layout->addWidget(resetPushButton);

  vbox->addWidget(m_red_shade);
  vbox->addWidget(m_green_shade);
  vbox->addWidget(m_blue_shade);
  vbox->addWidget(m_alpha_shade);
  vbox->addLayout(layout);

  connect(m_red_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
  connect(m_green_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
  connect(m_blue_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
  connect(m_alpha_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
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

      if (x / w > 1)
          return;

      stops << QGradientStop(x / w, color);
      //temp->
  }

  m_alpha_shade->setGradientStops(stops);

  emit gradientStopsChanged(stops);

  // update the LUT
  if(m_LUT)
    {
    // temp LUT
    vtkLookupTable* temp = vtkLookupTable::New();
    temp->SetTableRange (m_LUT->GetRange());
    m_alpha_shade->UpdateLookupTable(temp);
    m_LUT->DeepCopy(temp);
    m_LUT->Modified();
    temp->Delete();
    }

  // update the opcity TF

  // save points on quit only

  // send signal to update the visualization
  emit updateVisualization();
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
  std::map< unsigned int, unsigned int>::const_iterator it0 = iRGBA[0].find(0);
  std::map< unsigned int, unsigned int>::const_iterator it255 = iRGBA[0].find(255);

  m_red_shade->AddLockPoints(double(it0->second)/255, double(it255->second)/255);

  it0 = iRGBA[1].find(0);
  it255 = iRGBA[1].find(255);
  m_green_shade->AddLockPoints(double(it0->second)/255, double(it255->second)/255);

  it0 = iRGBA[2].find(0);
  it255 = iRGBA[2].find(255);
  m_blue_shade->AddLockPoints(double(it0->second)/255, double(it255->second)/255);

  it0 = iRGBA[3].find(0);
  it255 = iRGBA[3].find(255);
  m_alpha_shade->AddLockPoints(double(it0->second)/255, double(it255->second)/255);
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
AddHistogram(vtkImageAccumulate* iHistogram)
{
  int x_range = iHistogram->GetOutput()->GetNumberOfPoints();
  vtkDataArray* scalars =
      iHistogram->GetOutput()->GetPointData()->GetScalars();
  double* range;
  scalars->GetRange(range);

  qDebug() << "x range: "<< x_range;
  qDebug() << "range: " << range[0] << " to " << range[1];

  QVector<qreal> histo;

  for(int i=1; i<x_range; ++i)
    {
    double value;
    value = scalars->GetTuple1(i);
    qDebug() << "value: " << value;
    histo.push_back(value);
    }

  m_red_shade->SetHistogram(histo);
  m_green_shade->SetHistogram(histo);
  m_blue_shade->SetHistogram(histo);
  m_alpha_shade->SetHistogram(histo);
}
//-------------------------------------------------------------------------
