ivw_module(Marketplace)

find_package(Qt6Core QUIET REQUIRED)
find_package(Qt6Gui QUIET REQUIRED)
find_package(Qt6Widgets QUIET REQUIRED)
find_package(Qt6Network QUIET REQUIRED)
# find_package(QuaZip)

set(HEADER_FILES
    include/inviwo/marketplace/downloadmanager.h
    include/inviwo/marketplace/marketbinmodulewidgetqt.h
    include/inviwo/marketplace/marketmanager.h
    include/inviwo/marketplace/marketplacemodule.h
    include/inviwo/marketplace/marketplacemoduledefine.h
    include/inviwo/marketplace/marketplacesettings.h
    include/inviwo/marketplace/marketplacewidgetqt.h
    include/inviwo/marketplace/marketsrcmodulewidgetqt.h
    include/inviwo/marketplace/processors/marketplace.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/downloadmanager.cpp
    src/marketbinmodulewidgetqt.cpp
    src/marketmanager.cpp
    src/marketplacemodule.cpp
    src/marketplacesettings.cpp
    src/marketplacewidgetqt.cpp
    src/marketsrcmodulewidgetqt.cpp
    src/processors/marketplace.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(SHADER_FILES
	# Add shaders
)
ivw_group("Shader Files" ${SHADER_FILES})

set(MOC_FILES
    include/inviwo/marketplace/downloadmanager.h
)
qt5_wrap_cpp(MOCED_FILES ${MOC_FILES})
source_group("MOC Files" FILES ${MOCED_FILES})

set(TEST_FILES
    tests/unittests/marketplace-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${MOCED_FILES} ${HEADER_FILES} ${SHADER_FILES})
ivw_qt_add_to_install(inviwo-module-marketplace Qt6Core Qt6Gui Qt6Widgets Qt6Network)
target_link_libraries(inviwo-module-marketplace PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network QuaZip::QuaZip)

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
