
file(GLOB_RECURSE TOVAL_LIB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/TOVALEffect/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/modules/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/primatives/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp"
)

add_library(${TOVAL_LIB} STATIC ${TOVAL_LIB_SOURCES})

# Include directories for headers
target_include_directories(${TOVAL_LIB} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/TOVALEffect"
    "${CMAKE_CURRENT_SOURCE_DIR}/modules"
    "${CMAKE_CURRENT_SOURCE_DIR}/primatives"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils"
)

# Handle DELIVERY option
if(DELIVERY)
    message(STATUS "DELIVERY option enabled: copying libraries to delivery folder")

    # Define delivery directories based on the toolchain name
    set(DELIVERY_DIR_LIB "${CMAKE_SOURCE_DIR}/delivery/${TOOLCHAIN_NAME}/lib")
    set(DELIVERY_DIR_INC "${CMAKE_SOURCE_DIR}/delivery/${TOOLCHAIN_NAME}/inc")

    # Ensure the directories exist
    file(MAKE_DIRECTORY ${DELIVERY_DIR_LIB})
    file(MAKE_DIRECTORY ${DELIVERY_DIR_INC})

    # Install the TOVALeffect library to the delivery lib folder
    install(TARGETS ${TOVAL_LIB}
            ARCHIVE DESTINATION ${DELIVERY_DIR_LIB}
            LIBRARY DESTINATION ${DELIVERY_DIR_LIB}
            RUNTIME DESTINATION ${DELIVERY_DIR_LIB})

    # Install the headers directly into the delivery inc folder
    install(FILES
        ${CMAKE_SOURCE_DIR}/audioDSP/inc/TOVALEffect/TOVAL_Effect.h  # Add header files explicitly
        DESTINATION ${DELIVERY_DIR_INC}                        # Copy directly to the inc folder
    )
endif()

# Ensure CMake knows how to install targets
include(GNUInstallDirs)
