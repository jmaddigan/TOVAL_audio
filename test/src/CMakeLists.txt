add_executable(${TOVAL_EXE}
    "Tonal_Valley_test.cpp"
    "JsonParams.cpp")
#add_executable(${MODULE_TESTS} "Module_tests.cpp")

target_link_libraries(${TOVAL_EXE} ${TOVAL_LIB})
#target_link_libraries(${MODULE_TESTS} ${SOFTCLIP_LIB})  # Link all module libraries to the one module test executable

