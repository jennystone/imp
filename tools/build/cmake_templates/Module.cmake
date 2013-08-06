# Autogenerated file, run tools/build/setup_cmake.py to regenerate

project(IMP.%(name)s)

%(tests)s

imp_get_process_exit_code("Setting up module %(name)s" status ${CMAKE_BINARY_DIR}
                          COMMAND ${CMAKE_SOURCE_DIR}/tools/build/setup_module.py
                          --name=%(name)s
                          --datapath=${IMP_DATAPATH}
                          --defines=${IMP_%(NAME)s_CONFIG}:%(defines)s
                           --source=${CMAKE_SOURCE_DIR})
if(${status} EQUAL 0)
  imp_execute_process("setup_swig_wrappers %(name)s" ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/tools/build/setup_swig_wrappers.py
    --module=%(name)s
    --datapath=${IMP_DATAPATH}
    --source=${CMAKE_SOURCE_DIR})

  # for warning control
  add_definitions(-DIMP%(NAME)s_COMPILATION)

  if(IMP_DOXYGEN_FOUND)
    # documentation
    file(GLOB headers ${CMAKE_BINARY_DIR}/include/IMP/%(name)s/*.h)
    file(GLOB docs ${CMAKE_SOURCE_DIR}/modules/%(name)s/doc/*.dox
      ${CMAKE_SOURCE_DIR}/modules/%(name)s/doc/*.md)
    file(GLOB examples ${CMAKE_BINARY_DIR}/doc/examples/%(name)s/*.py
      ${CMAKE_BINARY_DIR}/doc/examples/%(name)s/*.cpp)

    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/doxygen/%(name)s/tags ${CMAKE_BINARY_DIR}/doc/html/%(name)s/index.html
      COMMAND mkdir -p ${CMAKE_BINARY_DIR}/doc/html
      COMMAND ln -s -f ../../include
      COMMAND ln -s -f ../../doc/examples
      COMMAND ln -s -f ../../lib
      COMMAND ${IMP_DOXYGEN_EXECUTABLE} ../../doxygen/%(name)s/Doxyfile 2>&1 /dev/null
      COMMAND ${CMAKE_SOURCE_DIR}/tools/build/doxygen_patch_tags.py --module=%(name)s --file=../../doxygen/%(name)s/tags
      COMMAND ${CMAKE_SOURCE_DIR}/tools/build/doxygen_show_warnings.py --warn=../../doxygen/%(name)s/warnings.txt
      DEPENDS %(tags)s ${headers} ${docs} ${examples} ${CMAKE_SOURCE_DIR}/modules/%(name)s/README.md IMP.%(name)s-python ${IMP_DOXYGEN_FETCH}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doxygen/%(name)s/
      COMMENT "Running doxygen on %(name)s")

    add_custom_target("IMP.%(name)s-doc" ALL DEPENDS ${CMAKE_BINARY_DIR}/doxygen/%(name)s/tags)
    set_property(TARGET "IMP.%(name)s-doc" PROPERTY FOLDER "IMP.%(name)s")

    set(IMP_%(NAME)s_DOC "IMP.%(name)s-doc" CACHE INTERNAL "" FORCE)

    list(APPEND IMP_DOC_DEPENDS "${IMP_%(NAME)s_DOC}")
    list(REMOVE_DUPLICATES IMP_DOC_DEPENDS)
    set(IMP_DOC_DEPENDS ${IMP_DOC_DEPENDS} CACHE INTERNAL "" FORCE)
  else()
    set(IMP_%(NAME)s_DOC "" CACHE INTERNAL "" FORCE)
  endif(IMP_DOXYGEN_FOUND)

  list(APPEND imp_%(name)s_libs %(modules)s)
  list(APPEND imp_%(name)s_libs %(dependencies)s)
  list(REMOVE_DUPLICATES imp_%(name)s_libs)
  %(subdirs)s
  set(IMP_%(NAME)s "IMP.%(name)s" CACHE INTERNAL "" FORCE)
else()

  # make sure it is empty
  set(IMP_%(NAME)s_DOC "" CACHE INTERNAL "" FORCE)
  set(IMP_%(NAME)s_PYTHON "" CACHE INTERNAL "" FORCE)
  set(IMP_%(NAME)s "" CACHE INTERNAL "" FORCE)

  if(${status} EQUAL 1)
    message("Module IMP.%(name)s disabled")
  else()
    message(FATAL_ERROR "setup_module returned ${status}")
  endif()
  set(IMP_%(NAME)s_LIBRARY CACHE INTERNAL "" FORCE)
endif()
