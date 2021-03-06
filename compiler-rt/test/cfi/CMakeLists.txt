configure_lit_site_cfg(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
  )

set(CFI_TEST_DEPS ${SANITIZER_COMMON_LIT_TEST_DEPS})
if(NOT COMPILER_RT_STANDALONE_BUILD)
  list(APPEND CFI_TEST_DEPS
    opt
    ubsan
  )
  if(COMPILER_RT_HAS_CFI)
    list(APPEND CFI_TEST_DEPS cfi)
  endif()
  if(LLVM_ENABLE_PIC AND LLVM_BINUTILS_INCDIR)
    list(APPEND CFI_TEST_DEPS
      LLVMgold
    )
  endif()
  if(APPLE)
    list(APPEND CFI_TEST_DEPS
      LTO
    )
  endif()
  if(WIN32 AND COMPILER_RT_HAS_LLD_SOURCES)
    list(APPEND CFI_TEST_DEPS
      lld
    )
  endif()
endif()

add_lit_testsuite(check-cfi "Running the cfi regression tests"
  ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${CFI_TEST_DEPS})

add_lit_target(check-cfi-and-supported "Running the cfi regression tests"
  ${CMAKE_CURRENT_BINARY_DIR}
  PARAMS check_supported=1
  DEPENDS ${CFI_TEST_DEPS})

set_target_properties(check-cfi PROPERTIES FOLDER "Tests")
