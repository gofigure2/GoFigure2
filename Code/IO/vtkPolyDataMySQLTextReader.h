#ifndef __vtkPolyDataMYSQLTextReader_h
#define __vtkPolyDataMYSQLTextReader_h

#include "vtkObject.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"

#include <string>
#include <sstream>

class vtkPolyDataMYSQLTextReader : public vtkObject
{
  public:
    static vtkPolyDataMYSQLTextReader* New();

    vtkPolyData* GetPolyData( std::string iString )
      {
      if( IsContour )
        return GetContour();
      else
        return GetMesh();
      }
  protected:
    vtkPolyDataMYSQLTextReader();
    ~vtkPolyDataMYSQLTextReader();

    std::string m_Text;
    bool IsContour;

    vtkPolyData* GetContour( )
      {
      vtkPolyData* oContour = vtkPolyData::New();
      vtkPoints* points = vtkPoints::New();

      std::stringstream str( m_Text );

      vtkIdType N;
      str >>N;
      points->SetNumberOfPoints( N );

      double pt[3];

      for( vtkIdType i = 0; i < N; i++ )
        {
        str >>pt[0] >>pt[1] >>pt[2];
        points->SetPoint( i, pt );
        }
      oContour->SetPoints( points );

      vtkCellArray* cells = vtkCellArray::New();
      vtkIdList* id_list = vtkIdList::New();
      id_list->SetNumberOfIds(2);
      for( vtkIdType i = 0; i < N-1; i++ )
        {
        id_list->SetId( 0, i );
        id_list->SetId( 1, i+1 );
        cells->InsertNextCell( id_list );
        }
      oContour->SetPolys( cells );
      return oContour;
      }

    vtkPolyData* GetMesh( )
      {
      vtkPolyData* oMesh = vtkPolyData::New();
      vtkPoints* points = vtkPoints::New();

      std::stringstream str( m_Text );

      vtkIdType N;
      str >>N;
      points->SetNumberOfPoints( N );

      double pt[3];

      for( vtkIdType i = 0; i < N; i++ )
        {
        str >>pt[0] >>pt[1] >>pt[2];
        points->SetPoint( i, pt );
        }
      oMesh->SetPoints( points );

      vtkCellArray* cells = vtkCellArray::New();
      str >>N;

      vtkIdList* cell_points = vtkIdList::New();
      vtkIdType NbOfPointsInCell;
      vtkIdType id;

      for( vtkIdType i = 0; i < N; i++ )
        {
        str >>NbOfPointsInCell;
        cell_points->Reset();
        for( vtkIdType k = 0; k < NbOfPointsInCell; k++ )
          {
          str >>id;
          cell_points->InsertNextId( id );
          }
        cells->InsertNextCell( cell_points );
        }
      oMesh->SetPolys( cells );
      return oMesh;
      }
  private:
    vtkPolyDataMYSQLTextReader( const vtkPolyDataMYSQLTextReader& );
    void operator = ( const vtkPolyDataMYSQLTextReader& );
};

#endif
