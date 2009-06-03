#ifndef __vtkPolyDataMYSQLTextReader_h
#define __vtkPolyDataMYSQLTextReader_h

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

      for( vtkIdType i = 0; i < N-1; i++ )
        {
        cells->InsertNextCell( 2, i, i+1 );
        }
      oContour->SetCell( cells );
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
      oMesh->SetCell( cells );
      return oMesh;
      }
  private:
    vtkPolyDataMYSQLTextReader( const vtkPolyDataMYSQLTextReader& );
    void operator = ( const Self& );
};

#endif
