#ifndef __QGoVisualizationDockWidget_h
#define __QGoVisualizationDockWidget_h

#include <QDockWidget>
#include "ui_VisualizationDockWidget.h"

#include "qtcolorcombobox.h"

class QHBoxLayout;
class QVBoxLayout;

class QGoVisualizationDockWidget :
  public QDockWidget,
  private Ui::VisualizationDockWidget
{
  Q_OBJECT

  public:
    explicit QGoVisualizationDockWidget( QWidget* parent = 0,
                                const unsigned int& iDim = 4 );
    ~QGoVisualizationDockWidget();

    void SetNumberOfChannels( const unsigned int& iN );
    void SetChannel( const unsigned int& i, const QString& iText = QString() );

    void SetXMinimumAndMaximum( const int& iMin, const int& iMax );
    void SetYMinimumAndMaximum( const int& iMin, const int& iMax );
    void SetZMinimumAndMaximum( const int& iMin, const int& iMax );
    void SetTMinimumAndMaximum( const int& iMin, const int& iMax );
    int GetCurrentChannel() const;
    int GetCurrentCollectionID ();
    void SetEnableTraceCollectionColorBoxes(bool Enable);
    std::string GetCurrentColor();
    
    QtColorComboBox* ColorTraceComboBox;
    QtColorComboBox* ColorIDCollectionComboBox;
    QLabel* TraceName;
    QLabel* CollectionName;

  public slots:
    void SetXSlice( int iSlice );
    void SetYSlice( int iSlice );
    void SetZSlice( int iSlice );
    void SetTSlice( int iSlice );
    void SetCollectionID(
      std::list<std::pair<std::string,QColor> > iListExistingID);

  signals:
    void ShowAllChannelsChanged( bool iChanged );
    void ShowOneChannelChanged( int Channel );

    void XSliceChanged( int Slice );
    void YSliceChanged( int Slice );
    void ZSliceChanged( int Slice );
    void TSliceChanged( int Slice );

  protected:
    unsigned int m_Dimension;
    std::list<std::pair<std::string,std::vector<int> > > m_DataColors;
    void SetColorTraceComboBox();
    void SetColorIDCollectionComboBox();
    QHBoxLayout* HLayoutForColor;
    QVBoxLayout* VLayoutForCollection;
};
#endif
