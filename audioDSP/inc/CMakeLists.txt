target_include_directories(${TOVAL_LIB} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/modules"   # Include modules directory
    "${CMAKE_CURRENT_SOURCE_DIR}/TOVALEffect"
    "${CMAKE_CURRENT_SOURCE_DIR}/primatives"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils"  
)

#target_include_directories(${SOFTCLIP_LIB} PUBLIC 
#    "${CMAKE_CURRENT_SOURCE_DIR}/modules"   # Include modules directory
#    "${CMAKE_CURRENT_SOURCE_DIR}/TOVALeffect"
#    "${CMAKE_CURRENT_SOURCE_DIR}/primatives"
#    "${CMAKE_CURRENT_SOURCE_DIR}/utils"  
#)
