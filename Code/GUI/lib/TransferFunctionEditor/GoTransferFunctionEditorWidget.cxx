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

  QPushButton *presetLUTPushButton = new QPushButton("Preset LUT", this);


  QHBoxLayout *lutLayout = new QHBoxLayout;
  QPushButton *saveLUTPushButton = new QPushButton("Save LUT", this);
  QPushButton *loadLUTPushButton = new QPushButton("Load LUT", this);
  lutLayout->addWidget(loadLUTPushButton);
  lutLayout->addWidget(saveLUTPushButton);

  QPushButton *okPushButton = new QPushButton("OK", this);
  QPushButton *resetPushButton = new QPushButton("Reset", this);

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(okPushButton);
  layout->addWidget(resetPushButton);

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

  connect(presetLUTPushButton, SIGNAL(pressed()), this, SLOT(presetLUT()));

  connect(okPushButton, SIGNAL(released()), this, SLOT(close()));
  connect(okPushButton, SIGNAL(released()), this, SLOT(savePoints()));

  connect(resetPushButton, SIGNAL(pressed()), this, SLOT(resetLUT()));
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
  }

  m_alpha_shade->setGradientStops(stops);

  emit gradientStopsChanged(stops);

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
  std::map< unsigned int, unsigned int>::const_iterator it0;
  std::map< unsigned int, unsigned int>::const_iterator it255;

  //red
  QPolygonF redPoints;
  it0 = iRGBA[0].begin();
  it255 = iRGBA[0].end();

  while(it0!=it255)
    {
    redPoints << QPointF((qreal)(it0->first)*(m_red_shade->width()-1)/255,
                         (m_red_shade->height()-1)*(1-(qreal)(it0->second)/255));
    ++it0;
    }

  m_red_shade->AddPoints(redPoints);

  // green
  QPolygonF greenPoints;
  it0 = iRGBA[1].begin();
  it255 = iRGBA[1].end();

  while(it0!=it255)
    {
    greenPoints << QPointF((qreal)(it0->first)*(m_green_shade->width()-1)/255,
                         (m_green_shade->height()-1)*(1-(qreal)(it0->second)/255));
    ++it0;
    }

  m_green_shade->AddPoints(greenPoints);

  // blue
  QPolygonF bluePoints;
  it0 = iRGBA[2].begin();
  it255 = iRGBA[2].end();

  while(it0!=it255)
    {
    bluePoints << QPointF((qreal)(it0->first)*(m_blue_shade->width()-1)/255,
                         (m_blue_shade->height()-1)*(1-(qreal)(it0->second)/255));
    ++it0;
    }

  m_blue_shade->AddPoints(bluePoints);

  // alpha
  QPolygonF alphaPoints;
  it0 = iRGBA[3].begin();
  it255 = iRGBA[3].end();

  while(it0!=it255)
    {
    alphaPoints << QPointF((qreal)(it0->first)*(m_alpha_shade->width()-1)/255,
                         (m_alpha_shade->height()-1)*(1-(qreal)(it0->second)/255));
    ++it0;
    }

  m_alpha_shade->AddPoints(alphaPoints);

  // update visu
  pointsUpdated();
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

  iHistogram->Delete();
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

  // update visu
  pointsUpdated();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionEditorWidget::
savePoints()
{
  std::vector< std::map< unsigned int, unsigned int> > pointsVector;
  pointsVector.resize(4);

  qreal width = m_red_shade->width() - 1;
  qreal height = m_red_shade->height() - 1;

  // RED ------------------------------
  QPolygonF redPoints = m_red_shade->points();

  int numberOfRedPoints = redPoints.size();

  if(numberOfRedPoints>255)
  {
    qDebug() << "Too many red points: " << numberOfRedPoints << " red points";
    return;
  }

  for(int i=0; i<numberOfRedPoints; ++i)
    {
    // to be inline..?
    unsigned int x = (redPoints.at(i).x())*255/width;
    unsigned int y = (1-(redPoints.at(i).y())/height)*255;
    pointsVector[0][x]  = y;
    }

  // GREEN ------------------------------

  QPolygonF greenPoints = m_green_shade->points();

  int numberOfGreenPoints = greenPoints.size();

  if(numberOfRedPoints>255)
  {
    qDebug() << "Too many green points: " << numberOfGreenPoints << " green points";
    return;
  }

  for(int i=0; i<numberOfGreenPoints; ++i)
    {
    unsigned int x = (greenPoints.at(i).x())*255/width;
    unsigned int y = (1-(greenPoints.at(i).y())/height)*255;
    pointsVector[1][x]  = y;
    }

  // BLUE ------------------------------

  QPolygonF bluePoints = m_blue_shade->points();

  int numberOfBluePoints = bluePoints.size();

  if(numberOfBluePoints>255)
  {
    qDebug() << "Too many blue points: " << numberOfBluePoints << " blue points";
    return;
  }

  for(int i=0; i<numberOfBluePoints; ++i)
    {
    unsigned int x = (bluePoints.at(i).x())*255/width;
    unsigned int y = (1-(bluePoints.at(i).y())/height)*255;
    pointsVector[2][x]  = y;
    }

  // BLUE ------------------------------

  QPolygonF alphaPoints = m_alpha_shade->points();

  int numberOfAlphaPoints = alphaPoints.size();

  if(numberOfAlphaPoints>255)
  {
    qDebug() << "Too many alpha points: " << numberOfAlphaPoints << " alpha points";
    return;
  }

  for(int i=0; i<numberOfAlphaPoints; ++i)
    {
    unsigned int x = (alphaPoints.at(i).x())*255/width;
    unsigned int y = (1-(alphaPoints.at(i).y())/height)*255;
    pointsVector[3][x]  = y;
    }

  emit updatePoints(m_Channel, pointsVector);
}
//-------------------------------------------------------------------------

