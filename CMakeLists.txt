cmake_minimum_required(VERSION 3.0.0)
project(A_stock_info_project)


include(./pre_common.cmake)
copy_include_to_build()

##############################
### libraries        #########
##############################

set(SOURCE_FILES src/stock_info.cpp        
)
set(LIBRARY_FILES boost_serialization
                   curl
        
)

#add_library(pisode_fs SHARED ${SOURCE_FILES})
#set_property(TARGET pisode_fs PROPERTY VERSION "0.0.1")
#target_link_libraries(pisode_fs 
#                        ${LIBRARY_FILES})


###########################
### test             ######
###########################
## test source  files
set(TEST_SOURCE_FILES test/test_get_up_stock_info.cpp ${SOURCE_FILES})
add_executable(up_stock_info ${TEST_SOURCE_FILES})
target_link_libraries(up_stock_info ${LIBRARY_FILES}) 




#########################
###### install ##########
#########################
