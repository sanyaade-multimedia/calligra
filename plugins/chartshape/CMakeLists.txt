add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_chart\")

# To hide all the warnings from embedded 3rd party software like kdchart
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-switch)
endif ()


if(BUILD_TESTING)
    add_subdirectory( tests )
endif()


include_directories( ${CMAKE_SOURCE_DIR}/plugins/chartshape
                     ${CMAKE_SOURCE_DIR}/plugins/chartshape/dialogs
                     ${CMAKE_SOURCE_DIR}/interfaces
                     ${KDCHART_INCLUDES}
                     ${KOMAIN_INCLUDES}
                     ${TEXTLAYOUT_INCLUDES}
)

########### Chart shape Plugin library ###############

set (chartshape_LIB_SRCS
    ChartDebug.cpp
    kochart_global.cpp
    KChartConvertions.cpp
    Axis.cpp
    DataSet.cpp
    CellRegion.cpp
    CellRegionStringValidator.cpp
    ChartTableModel.cpp
    Legend.cpp
    TextLabelDummy.cpp
    PlotArea.cpp
    TableSource.cpp
    ChartProxyModel.cpp
    KChartModel.cpp
    Surface.cpp
    ChartDocument.cpp
    ChartPart.cpp
    ChartShape.cpp
    ChartTool.cpp
    ChartToolFactory.cpp
    ConfigObjectBase.cpp
    TitlesConfigWidget.cpp
    LegendConfigWidget.cpp
    PlotAreaConfigWidget.cpp
    AxesConfigWidget.cpp
    DataSetConfigWidget.cpp
    PieConfigWidget.cpp
    BubbleConfigWidget.cpp
    ChartTableView.cpp
    ScreenConversions.cpp
    ChartLayout.cpp
    SingleModelHelper.cpp
    OdfLoadingHelper.cpp
    OdfHelper.cpp
    dialogs/TableEditorDialog.cpp
    dialogs/NewAxisDialog.cpp
    dialogs/AxisScalingDialog.cpp
    dialogs/CellRegionDialog.cpp
    dialogs/FontEditorDialog.cpp
    dialogs/FormatErrorBarDialog.cpp
    dialogs/PieDataEditor.cpp
    dialogs/BubbleDataEditor.cpp
    dialogs/DataSetTableModel.cpp
    commands/ChartTypeCommand.cpp
    commands/LegendCommand.cpp
    commands/AxisCommand.cpp
    commands/DatasetCommand.cpp
    commands/ChartTextShapeCommand.cpp
    commands/AddRemoveAxisCommand.cpp
    commands/GapCommand.cpp
)

ki18n_wrap_ui(chartshape_LIB_SRCS
    TitlesConfigWidget.ui
    LegendConfigWidget.ui
    PlotAreaConfigWidget.ui
    AxesConfigWidget.ui
    DataSetConfigWidget.ui
    PieConfigWidget.ui
    BubbleConfigWidget.ui
    dialogs/ChartTableEditor.ui
    dialogs/NewAxisDialog.ui
    dialogs/AxisScalingDialog.ui
    dialogs/CellRegionDialog.ui
    dialogs/FormatErrorBarDialog.ui
    dialogs/PieDataEditor.ui
    dialogs/BubbleDataEditor.ui
)

add_library(chartshapecore STATIC ${chartshape_LIB_SRCS})
target_link_libraries(chartshapecore  komain kotextlayout KChart)
if(NOT MSVC AND NOT (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
    set_target_properties(chartshapecore PROPERTIES COMPILE_FLAGS "-fPIC")
endif()


add_library(calligra_shape_chart MODULE ChartShapeFactory.cpp )
calligra_shape_desktop_to_json(calligra_shape_chart calligra_shape_chart.desktop)

target_link_libraries(calligra_shape_chart chartshapecore KChart flake KF5::IconThemes)
install(TARGETS calligra_shape_chart DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/shapes)
