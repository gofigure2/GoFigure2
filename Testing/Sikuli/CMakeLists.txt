add_sikuli_test( googletest test.sikuli gofigure2_images )

add_sikuli_test( DropMySQLDatabase DropMySQLdatabase.sikuli)

# set up database
add_sikuli_test( setUpDatabase setUpDatabase.sikuli )

# you need a sikuli script to import an imaging session
add_sikuli_test( ImportAnImagingSession ImportAnImagingSession.sikuli )

add_sikuli_test( Draw3DMesh Draw3DMesh.sikuli )

add_sikuli_test( Export3DMesh Export3DMesh.sikuli )

#drop MySQL database
add_sikuli_test( DropMySQLDatabase DropMySQLdatabase.sikuli)




#@GOFIGURE2_BINARY_DIR@/bin/gofigure &

# set up database
add_sikuli_test( setUpDatabase setUpDatabase.sikuli )


# you need a sikuli script to import an imaging session
add_sikuli_test( ImportAnImagingSession ImportAnImagingSession.sikuli )

add_sikuli_test( ImportTracks ImportTracks.sikuli )

add_sikuli_test( MeshSegmentation MeshSegmentation.sikuli )

add_sikuli_test( ContourSegmentation ContourSegmentation.sikuli )

add_sikuli_test( TraceAdd TraceAdd.sikuli )

add_sikuli_test( TraceDelete TraceDelete.sikuli )

add_sikuli_test( Bookmark Bookmark.sikuli )

# Start the test
#for var in MeshSegmentation ContourSegmentation TraceAdd TraceDelete Bookmark 
#do
#     echo "$var"
#     @SIKULI_EXECUTABLE@ -t ../$var.sikuli > @GOFIGURE2_BINARY_DIR@/Testing/$var.log;
#     echo "Done!"
#done
