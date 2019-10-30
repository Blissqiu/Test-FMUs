set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(EXAMPLE_SOURCES
  include/fmi3Functions.h
  include/fmi3FunctionTypes.h
  include/fmi3PlatformTypes.h
  examples/util.h
)

set(MODEL_SOURCES
  VanDerPol/sources/fmi3Functions.c
  VanDerPol/sources/model.c
  VanDerPol/sources/slave.c
)

# cs_clocked
add_executable(cs_clocked
  ${EXAMPLE_SOURCES}
  PositionControl/config.h
  PositionControl/model.c
  src/fmi3Functions.c
  src/slave.c
  examples/cs_clocked.c
)
set_target_properties(cs_clocked PROPERTIES FOLDER examples)
target_include_directories(cs_clocked PRIVATE include PositionControl)
target_compile_definitions(cs_clocked PRIVATE FMI_VERSION=3)
if(UNIX AND NOT APPLE)
  target_link_libraries(cs_clocked m)
endif()

# cs_early_return
add_executable(cs_early_return
  ${EXAMPLE_SOURCES}
  BouncingBall/config.h
  BouncingBall/model.c
  src/fmi3Functions.c
  src/slave.c
  examples/cs_early_return.c
)
set_target_properties(cs_early_return PROPERTIES FOLDER examples)
target_include_directories(cs_early_return PRIVATE include BouncingBall)
target_compile_definitions(cs_early_return PRIVATE FMI_VERSION=3)
if(UNIX AND NOT APPLE)
  target_link_libraries(cs_early_return m)
endif()

# co_simulation
add_library(slave1 STATIC ${EXAMPLE_SOURCES} src/fmi3Functions.c VanDerPol/model.c src/slave.c)
set_target_properties(slave1 PROPERTIES FOLDER examples)
target_compile_definitions(slave1 PRIVATE FMI_VERSION=3 FMI3_FUNCTION_PREFIX=s1_ _CRT_SECURE_NO_WARNINGS)
target_include_directories(slave1 PRIVATE include VanDerPol)

add_library(slave2 STATIC ${EXAMPLE_SOURCES} src/fmi3Functions.c VanDerPol/model.c src/slave.c)
set_target_properties(slave2 PROPERTIES FOLDER examples)
target_compile_definitions(slave2 PRIVATE FMI_VERSION=3 FMI3_FUNCTION_PREFIX=s2_ _CRT_SECURE_NO_WARNINGS)
target_include_directories(slave2 PRIVATE include VanDerPol)

add_executable(co_simulation ${EXAMPLE_SOURCES} examples/co_simulation.c)
set_target_properties(co_simulation PROPERTIES FOLDER examples)
target_include_directories(co_simulation PRIVATE include)
target_link_libraries(co_simulation slave1 slave2)

# jacobian
add_executable(jacobian ${EXAMPLE_SOURCES} src/fmi3Functions.c VanDerPol/model.c src/slave.c examples/jacobian.c)
set_target_properties (jacobian PROPERTIES FOLDER examples)
target_include_directories(jacobian PRIVATE include VanDerPol)
target_compile_definitions(jacobian PRIVATE FMI_VERSION=3 DISABLE_PREFIX _CRT_SECURE_NO_WARNINGS)

# model exchange
add_library(model STATIC ${EXAMPLE_SOURCES} src/fmi3Functions.c VanDerPol/model.c src/slave.c)
set_target_properties(model PROPERTIES FOLDER examples)
target_compile_definitions(model PRIVATE FMI_VERSION=3 FMI3_FUNCTION_PREFIX=M_ _CRT_SECURE_NO_WARNINGS)
target_include_directories(model PRIVATE include VanDerPol)

add_executable (model_exchange ${EXAMPLE_SOURCES} src/fmi3Functions.c VanDerPol/model.c src/slave.c examples/model_exchange.c)
set_target_properties(model_exchange PROPERTIES FOLDER examples)
target_include_directories(model_exchange PRIVATE include VanDerPol)
target_link_libraries(model_exchange model)
target_compile_definitions(model_exchange PRIVATE FMI_VERSION=3 DISABLE_PREFIX _CRT_SECURE_NO_WARNINGS)
