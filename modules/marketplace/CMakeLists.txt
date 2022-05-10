ivw_module(Marketplace)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets Network REQUIRED)
if(QT_VERSION_MAJOR EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS Core5Compat)
endif()



find_package(QuaZip)

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

if(QT_VERSION_MAJOR EQUAL 6)
    qt_wrap_cpp(MOCED_FILES ${MOC_FILES})
else()
    qt5_wrap_cpp(MOCED_FILES ${MOC_FILES})
endif()
source_group("MOC Files" FILES ${MOCED_FILES})

set(TEST_FILES
    tests/unittests/marketplace-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${MOCED_FILES} ${HEADER_FILES} ${SHADER_FILES})

add_subdirectory(ext/quazip)
ivw_register_license_file(NAME "QuaZip" VERSION ${QuaZip_VERSION} MODULE Marketplace
    URL https://stachenov.github.io/quazip/
    TYPE "GNU LGP v2.1"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/ext/quazip/COPYING
)
ivw_default_install_targets(QuaZip)
ivw_make_package(QuaZip QuaZip)

ivw_qt_add_to_install(inviwo-module-marketplace 
    Qt${QT_VERSION_MAJOR}Core 
    Qt${QT_VERSION_MAJOR}Gui 
    Qt${QT_VERSION_MAJOR}Widgets 
    Qt${QT_VERSION_MAJOR}Network)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(inviwo-module-marketplace PRIVATE 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets 
    Qt6::Network  
    Qt6::Core5Compat
    QuaZip::QuaZip)
else()
    target_link_libraries(inviwo-module-marketplace PRIVATE 
        Qt5::Core 
        Qt5::Gui 
        Qt5::Widgets 
        Qt5::Network  QuaZip::QuaZip)
endif()

ivw_deploy_qt(inviwo-module-marketplace)

ivw_make_package(InviwoMarketplaceModule inviwo-module-marketplace)
ivw_default_install_targets(inviwo-module-marketplace)

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
