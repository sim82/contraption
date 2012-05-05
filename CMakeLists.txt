add_subdirectory(ivy_mike)
add_subdirectory(papara_nt)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -march=native")
find_package(Qt4 4.4.3 REQUIRED QtCore QtGui QtXml)
include(${QT_USE_FILE})


IF(WIN32)
include_directories( e:\\src\\boost_1_48_0)
file(GLOB IM_HEADERS *.h)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_SCL_SECURE_NO_WARNINGS") # as long as there is no support for std::array, these warnings are plain stupid!
ELSE()

ENDIF()

include_directories(${IVYMIKE_INCLUDE} papara_nt papara_nt/ublasJama-1.0.2.3 )

 # with SET() command you can change variables or define new ones
  # here we define SAMPLE_SRCS variable that contains a list of all .cpp files
  # note that we don't need \ at the end of line
SET( SAMPLE_SRCS
#        dialog.cpp
       main_widget.cpp
  )

  # another list, this time it includes all header files that should be treated with moc
  SET( SAMPLE_MOC_HDRS
#        dialog.h
       main_widget.h
  )

  # some .ui files
  SET( SAMPLE_UIS
#        dialog.ui
       main_widget.ui
  )



# this will run uic on .ui files:
QT4_WRAP_UI( SAMPLE_UI_HDRS ${SAMPLE_UIS} )

# and finally this will run moc:
QT4_WRAP_CPP( SAMPLE_MOC_SRCS ${SAMPLE_MOC_HDRS} )

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

# here we instruct CMake to build "sample" executable from all of the source files

add_executable( test main.cpp main_widget.cpp ${SAMPLE_MOC_SRCS} ${SAMPLE_UI_HDRS})


target_link_libraries(test ${QT_LIBRARIES} papara_core ivymike ublas_jama )
