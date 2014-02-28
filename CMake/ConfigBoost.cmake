find_package( Boost COMPONENTS program_options REQUIRED )

if( Boost_FOUND )
  include_directories( BEFORE ${Boost_INCLUDE_DIRS} )
else( Boost_FOUND )
  message( SEND_ERROR "BOOST NOT FOUND, CMAKE WILL STOP NOW")
endif( Boost_FOUND )
