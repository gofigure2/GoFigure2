CONFIGURE_FILE(${MEGAVTK2_SOURCE_DIR}/MegaVTK2Configure.h.in
               ${MEGAVTK2_BINARY_DIR}/MegaVTK2Configure.h @ONLY IMMEDIATE)

SUBDIRS( vtkRenderingAddOn )
SUBDIRS( vtkItk )
SUBDIRS( qt )






