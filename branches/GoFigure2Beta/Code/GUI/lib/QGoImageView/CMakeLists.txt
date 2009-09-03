FILE( GLOB QGoImageViewLIB_SRC *.cxx )
FILE( GLOB QGoImageViewLIB_HDRS *.h )

QT4_WRAP_CPP( QGoImageViewLIB_MOC ${QGoImageViewLIB_HDRS} )

ADD_LIBRARY( QGoImageView
  ${QGoImageViewLIB_SRC}
  ${QGoImageViewLIB_MOC}
)

TARGET_LINK_LIBRARIES( QGoImageView
  QGoImageView
  ${QT_LIBRARIES}
  QVTK
  vtkRenderingAddOn2
)
