#ifndef __vtkPolyDataMYSQLTextWriter_h
#define __vtkPolyDataMYSQLTextWriter_h

#include <string>
#include <sstream>

#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkIdList.h"

class vtkPolyDataMYSQLTextWriter : public vtkObject
{
public:
  static vtkPolyDataMYSQLTextWriter* New();
  vtkTypeRevisionMacro( vtkPolyDataMYSQLTextWriter, vtkObject );

  std::string GetMySQLText( vtkPolyData* iPolyData )
    {
    m_PolyData = iPolyData;

    if( IsPlanarContour() )
      {
      return ContourProcessing();
      }
    else
      {
      return MeshProcessing();
      }
    }

protected:
  vtkPolyDataMYSQLTextWriter() : m_PolyData(0) {}
  ~vtkPolyDataMYSQLTextWriter() {}

  vtkPolyData* m_PolyData;

  bool IsPlanarContour()
    {
    double bounds[6];
    m_PolyData->GetBounds( bounds );

    for( int dim = 0; dim < 3; dim++ )
      {
      if( bounds[2*dim] == bounds[2*dim+1] )
        {
        return true;
        }
      }
    return false;
    }

  std::string ContourProcessing()
    {
    vtkIdType N = m_PolyData->GetNumberOfPoints();

    double pt[3];
    std::stringstream oMyString;
    oMyString <<N <<" ";

    for( vtkIdType i = 0; i < N; i++ )
      {
      m_PolyData->GetPoint( i, pt );
      oMyString <<pt[0] <<" " <<pt[1] <<" " <<pt[2] <<" ";
      }
    return oMyString.str();
    }

  std::string MeshProcessing()
    {
    vtkIdType N = m_PolyData->GetNumberOfPoints();
    double pt[3];

    std::stringstream oMyString;
    oMyString <<N <<" ";

    for( vtkIdType i = 0; i < N; i++ )
      {
      m_PolyData->GetPoint( i, pt );
      oMyString <<pt[0] <<" " <<pt[1] <<" " <<pt[2] <<" ";
      }

    vtkIdList* cell_points = vtkIdList::New();

    vtkIdType NbOfPointsInCell;
    N = m_PolyData->GetNumberOfCells();
    oMyString <<N <<" ";

    for( vtkIdType i = 0; i < N; i++ )
      {
      m_PolyData->GetCellPoints( i, cell_points );
      NbOfPointsInCell = cell_points->GetNumberOfIds();
      oMyString <<NbOfPointsInCell <<" ";

      for( vtkIdType k = 0; k < NbOfPointsInCell; k++ )
        {
        oMyString <<cell_points->GetId( k ) <<" ";
        }
      }
    return oMyString.str();
    }

    private:
      vtkPolyDataMYSQLTextWriter( const vtkPolyDataMYSQLTextWriter& );
      void operator = ( const vtkPolyDataMYSQLTextWriter& ) ;
};
#endif
