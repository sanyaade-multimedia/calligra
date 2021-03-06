/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2009      Inge Wallin    <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Own
#include "PlotAreaConfigWidget.h"
#include "ui_PlotAreaConfigWidget.h"

// Qt
#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QPair>
#include <QLatin1String>

// KF5
#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <kfontchooser.h>

// Calligra
#include <interfaces/KoChartModel.h>
#include <KoIcon.h>

// KChart
#include <KChartChart>
#include <KChartPosition>
#include <KChartCartesianAxis>
#include <KChartGridAttributes>
#include <KChartPieAttributes>
#include <KChartAbstractCartesianDiagram>
#include <KChartLegend>
#include <KChartDataValueAttributes>
#include <KChartTextAttributes>
#include <KChartMarkerAttributes>
#include <KChartMeasure>

// KoChart
#include "ChartProxyModel.h"
#include "PlotArea.h"
#include "Legend.h"
#include "DataSet.h"
#include "Axis.h"
#include "ui_ChartTableEditor.h"
#include "ui_PlotAreaConfigWidget.h"
#include "NewAxisDialog.h"
#include "AxisScalingDialog.h"
#include "FontEditorDialog.h"
#include "FormatErrorBarDialog.h"
#include "CellRegionDialog.h"
#include "TableEditorDialog.h"
#include "PieDataEditor.h"
#include "BubbleDataEditor.h"
#include "commands/ChartTypeCommand.h"
#include "CellRegionStringValidator.h"
#include "ChartTableModel.h"
#include "TableSource.h"
#include "AxesConfigWidget.h"
#include "DataSetConfigWidget.h"
#include "PieConfigWidget.h"
#include "BubbleConfigWidget.h"
#include "ConfigObjectBase.h"
#include "ChartDebug.h"

using namespace KoChart;

class PlotAreaConfigWidget::Private
{
public:
    Private(QWidget *parent);
    ~Private();

    // Basic properties of the chart.
    ChartType              type;
    ChartSubtype           subtype;
    bool                   threeDMode;

    Ui::PlotAreaConfigWidget  ui;
    bool                   isExternalDataSource;

    // Menus
    QMenu *dataSetBarChartMenu;
    QMenu *dataSetLineChartMenu;
    QMenu *dataSetAreaChartMenu;
    QMenu *dataSetRadarChartMenu;
    QMenu *dataSetStockChartMenu;

    // chart type selection actions
    QAction  *normalBarChartAction;
    QAction  *stackedBarChartAction;
    QAction  *percentBarChartAction;

    QAction  *normalLineChartAction;
    QAction  *stackedLineChartAction;
    QAction  *percentLineChartAction;

    QAction  *normalAreaChartAction;
    QAction  *stackedAreaChartAction;
    QAction  *percentAreaChartAction;

    QAction  *circleChartAction;
    QAction  *ringChartAction;
    QAction  *radarChartAction;
    QAction  *filledRadarChartAction;

    QAction  *scatterChartAction;
    QAction  *bubbleChartAction;

    QAction  *hlcStockChartAction;
    QAction  *ohlcStockChartAction;
    QAction  *candlestickStockChartAction;

    QAction  *surfaceChartAction;
    QAction  *ganttChartAction;

    // chart type selection actions for datasets
    QAction  *dataSetNormalBarChartAction;
    QAction  *dataSetStackedBarChartAction;
    QAction  *dataSetPercentBarChartAction;

    QAction  *dataSetNormalLineChartAction;
    QAction  *dataSetStackedLineChartAction;
    QAction  *dataSetPercentLineChartAction;

    QAction  *dataSetNormalAreaChartAction;
    QAction  *dataSetStackedAreaChartAction;
    QAction  *dataSetPercentAreaChartAction;

    QAction  *dataSetCircleChartAction;
    QAction  *dataSetRingChartAction;
    QAction  *dataSetRadarChartAction;
    QAction  *dataSetFilledRadarChartAction;
    QAction  *dataSetScatterChartAction;
    QAction  *dataSetBubbleChartAction;

    QAction  *dataSetHLCStockChartAction;
    QAction  *dataSetOHLCStockChartAction;
    QAction  *dataSetCandlestickStockChartAction;

    QAction  *dataSetSurfaceChartAction;
    QAction  *dataSetGanttChartAction;

    // Table Editor (a.k.a. the data editor)
    TableEditorDialog    *tableEditorDialog;
    // Source containing all tables the chart uses (name/model pairs)
    TableSource          *tableSource;


    QList<DataSet*> dataSets;
    int selectedDataSet;
    int selectedDataSet_CellRegionDialog;


    // Dialogs
    CellRegionDialog  cellRegionDialog;

    CellRegionStringValidator *cellRegionStringValidator;
};


PlotAreaConfigWidget::Private::Private(QWidget *parent)
    : tableEditorDialog(0)
    , cellRegionDialog(parent)

{
    selectedDataSet = 0;
    tableSource = 0;

    type = KoChart::LastChartType;
    subtype = KoChart::NoChartSubtype;
    threeDMode = false;

    isExternalDataSource = false;
    cellRegionStringValidator = 0;

    dataSetBarChartMenu = 0;
    dataSetLineChartMenu = 0;
    dataSetAreaChartMenu = 0;
    dataSetRadarChartMenu = 0;
    dataSetStockChartMenu = 0;
    dataSetNormalBarChartAction = 0;
    dataSetStackedBarChartAction = 0;
    dataSetPercentBarChartAction = 0;
    dataSetNormalLineChartAction = 0;
    dataSetStackedLineChartAction = 0;
    dataSetPercentLineChartAction = 0;
    dataSetNormalAreaChartAction = 0;
    dataSetStackedAreaChartAction = 0;
    dataSetPercentAreaChartAction = 0;
    dataSetCircleChartAction = 0;
    dataSetRingChartAction = 0;
    dataSetScatterChartAction = 0;
    dataSetRadarChartAction = 0;
    dataSetFilledRadarChartAction = 0;
    dataSetHLCStockChartAction = 0;
    dataSetOHLCStockChartAction = 0;
    dataSetCandlestickStockChartAction = 0;
    dataSetBubbleChartAction = 0;
    dataSetSurfaceChartAction = 0;
    dataSetGanttChartAction = 0;
}

PlotAreaConfigWidget::Private::~Private()
{
}


// ================================================================
//                     class PlotAreaConfigWidget

PlotAreaConfigWidget::PlotAreaConfigWidget()
    : d(new Private(this))
{
    setObjectName("Chart Type");
    d->ui.setupUi(this);

    setupWidgets();
    // Chart type button with its associated menu
    QMenu *chartTypeMenu = new QMenu(i18n("Chart Type"), this);
    chartTypeMenu->setIcon(ICON1(BarChartType));

    // Bar charts
    QMenu *barChartMenu = chartTypeMenu->addMenu(ICON1(BarChartType), i18n("Bar Chart"));
    d->normalBarChartAction  = barChartMenu->addAction(ICON2(BarChartType, NormalChartSubtype), i18n("Normal"));
    d->stackedBarChartAction = barChartMenu->addAction(ICON2(BarChartType, StackedChartSubtype), i18n("Stacked"));
    d->percentBarChartAction = barChartMenu->addAction(ICON2(BarChartType, PercentChartSubtype), i18n("Percent"));

    // Line charts
    QMenu *lineChartMenu = chartTypeMenu->addMenu(ICON1(LineChartType), i18n("Line Chart"));
    d->normalLineChartAction  = lineChartMenu->addAction(ICON2(LineChartType, NormalChartSubtype), i18n("Normal"));
    d->stackedLineChartAction = lineChartMenu->addAction(ICON2(LineChartType, StackedChartSubtype), i18n("Stacked"));
    d->percentLineChartAction = lineChartMenu->addAction(ICON2(LineChartType, PercentChartSubtype), i18n("Percent"));

    // Area charts
    QMenu *areaChartMenu = chartTypeMenu->addMenu(ICON1(AreaChartType), i18n("Area Chart"));
    d->normalAreaChartAction  = areaChartMenu->addAction(ICON2(AreaChartType, NormalChartSubtype), i18n("Normal"));
    d->stackedAreaChartAction = areaChartMenu->addAction(ICON2(AreaChartType, StackedChartSubtype), i18n("Stacked"));
    d->percentAreaChartAction = areaChartMenu->addAction(ICON2(AreaChartType, PercentChartSubtype), i18n("Percent"));

    chartTypeMenu->addSeparator();

    // Circular charts: pie and ring
    d->circleChartAction = chartTypeMenu->addAction(ICON1(CircleChartType), i18n("Pie Chart"));
    d->ringChartAction = chartTypeMenu->addAction(ICON1(RingChartType), i18n("Ring Chart"));

    chartTypeMenu->addSeparator();

    // Polar charts: radar
    QMenu *radarChartMenu = chartTypeMenu->addMenu(ICON1(RadarChartType), i18n("Polar Chart"));
    d->radarChartAction = radarChartMenu->addAction(ICON2(RadarChartType, NoChartSubtype), i18n("Normal"));
    d->filledRadarChartAction = radarChartMenu->addAction(ICON2(FilledRadarChartType, NoChartSubtype), i18n("Filled")); // Whay is Filled... not subtype?

    chartTypeMenu->addSeparator();

    // X/Y charts: scatter and bubble
    d->scatterChartAction = chartTypeMenu->addAction(ICON1(ScatterChartType), i18n("Scatter Chart"));
    d->bubbleChartAction = chartTypeMenu->addAction(ICON1(BubbleChartType), i18n("Bubble Chart"));

    chartTypeMenu->addSeparator();

    // Stock Charts
    QMenu *stockChartMenu = chartTypeMenu->addMenu(i18n("Stock Chart"));
    d->hlcStockChartAction  = stockChartMenu->addAction(i18n("HighLowClose"));
//     d->hlcStockChartAction->setEnabled(false);
    d->ohlcStockChartAction = stockChartMenu->addAction(i18n("OpenHighLowClose"));
//     d->ohlcStockChartAction->setEnabled(false);
    d->candlestickStockChartAction = stockChartMenu->addAction(i18n("Candlestick"));
//     d->candlestickStockChartAction->setEnabled(false);

    // Not supported
    d->surfaceChartAction = chartTypeMenu->addAction(i18n("Surface Chart"));
    d->surfaceChartAction->setEnabled(false);
    d->ganttChartAction = chartTypeMenu->addAction(i18n("Gantt Chart"));
    d->ganttChartAction->setEnabled(false);

    d->ui.chartTypeMenu->setMenu(chartTypeMenu);
    d->ui.chartTypeMenu->setIconSize(QSize(32, 32));

    connect(chartTypeMenu, SIGNAL(triggered(QAction*)),
            this,          SLOT(chartTypeSelected(QAction*)));


    connect(d->ui.threeDLook, SIGNAL(toggled(bool)),
            this,             SLOT(setThreeDMode(bool)));


    setupDialogs();
    createActions();

}

PlotAreaConfigWidget::~PlotAreaConfigWidget()
{
    deleteSubDialogs();
    delete d;
}

AxesConfigWidget *PlotAreaConfigWidget::cartesianAxesConfigWidget() const
{
    return d->ui.cartesianAxes;
}

DataSetConfigWidget *PlotAreaConfigWidget::cartesianDataSetConfigWidget() const
{
    return d->ui.cartesianDataSets;
}

PieConfigWidget *PlotAreaConfigWidget::pieConfigWidget() const
{
    return d->ui.pieConfigWidget;
}

BubbleConfigWidget *PlotAreaConfigWidget::bubbleConfigWidget() const
{
    return d->ui.bubbleConfigWidget;
}

void PlotAreaConfigWidget::deleteSubDialogs(ChartType type)
{
    switch (type) {
        case BarChartType:
        case LineChartType:
        case AreaChartType:
        case RingChartType:
        case RadarChartType:
        case FilledRadarChartType:
        case ScatterChartType:
        case SurfaceChartType:
        case StockChartType:
            delete d->tableEditorDialog;
            d->tableEditorDialog = 0;
            break;
        case CircleChartType:
            delete findChildren<PieDataEditor*>().value(0);
            break;
        case BubbleChartType:
            delete findChildren<BubbleDataEditor*>().value(0);
            break;
        default:
            delete d->tableEditorDialog;
            d->tableEditorDialog = 0;
            delete findChildren<PieDataEditor*>().value(0);
            delete findChildren<BubbleDataEditor*>().value(0);
            break;
    }
}

void PlotAreaConfigWidget::open(KoShape* shape)
{
    d->tableSource = 0;
    ConfigWidgetBase::open(shape);
    if (!chart) {
        return;
    }
    for (ConfigObjectBase *w : findChildren<ConfigObjectBase*>()) {
        w->open(chart);
    }

    d->tableSource = chart->tableSource();

// NOTE: There's no single source table anymore, a Calligra Sheets workbook allows multiple to be used with a chart.
//    KoChart::ChartModel *spreadSheetModel = qobject_cast<KoChart::ChartModel*>(chart->internalModel());
// NOTE: This is obsolete, ChartShape::usesInternalModelOnly() is now used instead.
//    ChartTableModel *tableModel = qobject_cast<ChartTableModel*>(chart->model());
//    d->isExternalDataSource = (spreadSheetModel != 0 && tableModel == 0);


    // Update the legend title
    //d->ui.legendTitle->setText(chart->legend()->title());

    // Fill the data table
    if (!chart->usesInternalModelOnly()) {
 // FIXME: CellRegion itself together with a TableSource should now be used
 // to validate  the correctness of a table range address.
#if 0
        d->cellRegionStringValidator = new CellRegionStringValidator(spreadSheetModel);
        d->cellRegionDialog.labelDataRegion->setValidator(d->cellRegionStringValidator);
        d->cellRegionDialog.xDataRegion->setValidator(d->cellRegionStringValidator);
        d->cellRegionDialog.yDataRegion->setValidator(d->cellRegionStringValidator);
        d->cellRegionDialog.categoryDataRegion->setValidator(d->cellRegionStringValidator);
#endif

        debugChartUiPlotArea<<"external data source";
        // If the data source is external, the editData button opens a
        // dialog to edit the data ranges instead of the data itself.
        d->ui.editData->setText(i18n("Data Ranges..."));
        connect(d->ui.editData, SIGNAL(clicked(bool)),
                this, SLOT(slotShowCellRegionDialog()));
        connect(d->cellRegionDialog.xDataRegion, SIGNAL(editingFinished()),
                this, SLOT(ui_dataSetXDataRegionChanged()));
        connect(d->cellRegionDialog.yDataRegion, SIGNAL(editingFinished()),
                this, SLOT(ui_dataSetYDataRegionChanged()));
        connect(d->cellRegionDialog.labelDataRegion, SIGNAL(editingFinished()),
                this, SLOT(ui_dataSetLabelDataRegionChanged()));
        //connect(d->cellRegionDialog.customDataRegion, SIGNAL(textEdited(QString)),
        //        this, SLOT(ui_dataSetCustomDataRegionChanged(QString)));
        connect(d->cellRegionDialog.categoryDataRegion, SIGNAL(editingFinished()),
                this, SLOT(ui_dataSetCategoryDataRegionChanged()));
        connect(d->cellRegionDialog.dataSets, SIGNAL(currentIndexChanged(int)),
                this, SLOT(ui_dataSetSelectionChanged_CellRegionDialog(int)));
    }
    else {
        // This part is run when the data source is not external,
        // i.e. the data is handled by the chart shape itself.
        connect(d->ui.editData, SIGNAL(clicked(bool)),
                this,           SLOT(slotShowTableEditor()));
    }

    updateData();
}

void PlotAreaConfigWidget::setupWidgets()
{
    QList<ChartType> types;
    types << BarChartType << LineChartType << AreaChartType;
    // TODO: temporary, these should have different widgets
    types << RingChartType << RadarChartType << FilledRadarChartType << ScatterChartType;
    cartesianAxesConfigWidget()->setChartTypes(types);

    cartesianDataSetConfigWidget()->setChartTypes(types);

    pieConfigWidget()->setChartTypes(QList<ChartType>()<<CircleChartType);

    bubbleConfigWidget()->setChartTypes(QList<ChartType>()<<BubbleChartType);
}

QAction *PlotAreaConfigWidget::createAction()
{
    return 0;
}

void PlotAreaConfigWidget::chartTypeSelected(QAction *action)
{
    ChartType     type = LastChartType;
    ChartSubtype  subtype = NoChartSubtype;

    // Bar charts
    if (action == d->normalBarChartAction) {
        type    = BarChartType;
        subtype = NormalChartSubtype;
    } else if (action == d->stackedBarChartAction) {
        type    = BarChartType;
        subtype = StackedChartSubtype;
    } else if (action == d->percentBarChartAction) {
        type    = BarChartType;
        subtype = PercentChartSubtype;
    }

    // Line charts
    else if (action == d->normalLineChartAction) {
        type    = LineChartType;
        subtype = NormalChartSubtype;
    } else if (action == d->stackedLineChartAction) {
        type    = LineChartType;
        subtype = StackedChartSubtype;
    } else if (action == d->percentLineChartAction) {
        type    = LineChartType;
        subtype = PercentChartSubtype;
    }

    // Area charts
    else if (action == d->normalAreaChartAction) {
        type    = AreaChartType;
        subtype = NormalChartSubtype;
    } else if (action == d->stackedAreaChartAction) {
        type    = AreaChartType;
        subtype = StackedChartSubtype;
    } else if (action == d->percentAreaChartAction) {
        type    = AreaChartType;
        subtype = PercentChartSubtype;
    }

    // also known as polar chart.
    else if (action == d->radarChartAction) {
        type    = RadarChartType;
        subtype = NoChartSubtype;
    }
    else if (action == d->filledRadarChartAction) {
        type    = FilledRadarChartType;
        subtype = NoChartSubtype;
    }

    // Also known as pie chart
    else if (action == d->circleChartAction) {
        type    = CircleChartType;
        subtype = NoChartSubtype;
    }
    else if (action == d->ringChartAction) {
        type    = RingChartType;
        subtype = NoChartSubtype;
    }

    else if (action == d->scatterChartAction) {
        type    = ScatterChartType;
        subtype = NoChartSubtype;
    }

    // Stock charts
    else if (action == d->hlcStockChartAction) {
        type    = StockChartType;
        subtype = HighLowCloseChartSubtype;
    }

    else if (action == d->ohlcStockChartAction) {
        type    = StockChartType;
        subtype = OpenHighLowCloseChartSubtype;
    }

    else if (action == d->candlestickStockChartAction) {
        type    = StockChartType;
        subtype = CandlestickChartSubtype;
    }

    else if (action == d->bubbleChartAction) {
        type    = BubbleChartType;
        subtype = NoChartSubtype;
    }

    else if (action == d->surfaceChartAction) {
        type    = SurfaceChartType;
        subtype = NoChartSubtype;
    }

    else if (action == d->ganttChartAction) {
        type    = GanttChartType;
        subtype = NoChartSubtype;
    }

    emit chartTypeChanged(type, subtype);
    updateData();
}

void PlotAreaConfigWidget::setThreeDMode(bool threeD)
{
    d->threeDMode = threeD;
    emit threeDModeToggled(threeD);

    updateData();
}

/**
 * Only some chart types support a 3D mode in KD Chart.
 */
static bool supportsThreeD(ChartType type)
{
    switch (type) {
        case BarChartType:
        case LineChartType:
        case AreaChartType:
        case CircleChartType:
        case BubbleChartType:
            return true;
        default:
            break;
    }
    return false;
}


void PlotAreaConfigWidget::updateData()
{
    if (!chart) {
        return;
    }
    deleteSubDialogs(chart->chartType());
    switch (chart->chartType()) {
        case CircleChartType:
            d->ui.stackedWidget->setCurrentIndex(1);
            break;
        case BubbleChartType:
            d->ui.stackedWidget->setCurrentIndex(2);
            break;
        default:
            d->ui.stackedWidget->setCurrentIndex(0);
            break;
    }
    blockSignals(true);
    debugChartUiPlotArea<<"chart:"<<chart->chartType()<<','<<chart->chartSubType()<<"current:"<<d->type<<','<<d->subtype;
    if (d->type != chart->chartType() || d->subtype != chart->chartSubType()) {
        // Set the chart type icon in the chart type button.
        const QLatin1String iconName = chartTypeIconName(chart->chartType(), chart->chartSubType());
        debugChartUiPlotArea<<iconName;
        if (iconName.size() > 0) {
            d->ui.chartTypeMenu->setIcon(QIcon::fromTheme(iconName));
        }
        d->type = chart->chartType();
        d->subtype = chart->chartSubType();
    }

    // If the "3D" checkbox is checked, then adapt the chart to that.
    bool enableThreeDOption = supportsThreeD(d->type);
    d->threeDMode = enableThreeDOption && chart->isThreeD();
    chart->setThreeD(d->threeDMode);
    d->ui.threeDLook->setChecked(d->threeDMode);
    d->ui.threeDLook->setEnabled(enableThreeDOption);

    d->cellRegionDialog.hide();
    d->dataSets = chart->plotArea()->dataSets();
    d->cellRegionDialog.dataSets->clear();
    int i = 1;
    foreach (DataSet *dataSet, d->dataSets) {
        QString title = dataSet->labelData().toString();
        if (title.isEmpty())
            title = i18n("Data Set %1", i++);
        d->cellRegionDialog.dataSets->addItem(title);
    }
    blockSignals(false);
    debugChartUiPlotArea<<"datasets blocked:"<<d->cellRegionDialog.dataSets->signalsBlocked();
    ui_dataSetSelectionChanged_CellRegionDialog(0);

    for (ConfigObjectBase *w : findChildren<ConfigObjectBase*>()) {
        w->updateData(d->type, d->subtype);
    }
}


void PlotAreaConfigWidget::slotShowTableEditor()
{
    debugChartUiPlotArea;
    switch (chart->chartType()) {
        case CircleChartType: {
            PieDataEditor *dlg = findChildren<PieDataEditor*>().value(0);
            if (!dlg) {
                dlg = new PieDataEditor(this);
                dlg->setModel(chart->internalModel());
                connect(dlg, SIGNAL(finished()), dlg, SLOT(hide()));
            }
            dlg->show();
            dlg->raise();
            return;
        }
        case BubbleChartType: {
            BubbleDataEditor *dlg = findChildren<BubbleDataEditor*>().value(0);
            if (!dlg) {
                dlg = new BubbleDataEditor(chart, this);
                connect(dlg, SIGNAL(finished()), dlg, SLOT(hide()));
                connect(dlg, &BubbleDataEditor::xDataChanged, this, &PlotAreaConfigWidget::dataSetXDataRegionChanged);
                connect(dlg, &BubbleDataEditor::yDataChanged, this, &PlotAreaConfigWidget::dataSetYDataRegionChanged);
                connect(dlg, &BubbleDataEditor::bubbleDataChanged, this, &PlotAreaConfigWidget::dataSetCustomDataRegionChanged);
            }
            dlg->show();
            dlg->raise();
            return;
        }
        default:
            if (!d->tableEditorDialog) {
                d->tableEditorDialog = new TableEditorDialog;
                d->tableEditorDialog->setProxyModel(chart->proxyModel());
                d->tableEditorDialog->setModel(chart->internalModel());
            }
            d->tableEditorDialog->show();
            d->tableEditorDialog->raise();
            return;
    }
}


void PlotAreaConfigWidget::slotShowCellRegionDialog()
{
    // Update regions of selected dataset
    int selectedDataSet = d->cellRegionDialog.dataSets->currentIndex();
    ui_dataSetSelectionChanged_CellRegionDialog(selectedDataSet);

    d->cellRegionDialog.show();
}

void PlotAreaConfigWidget::setupDialogs()
{
}

void PlotAreaConfigWidget::createActions()
{
}



void PlotAreaConfigWidget::ui_dataSetXDataRegionChanged()
{
    // Check for valid index
    if (d->selectedDataSet_CellRegionDialog < 0)
        return;

    const QString regionString = d->cellRegionDialog.xDataRegion->text();
    const CellRegion region(d->tableSource, regionString);

    DataSet *dataSet = d->dataSets[d->selectedDataSet_CellRegionDialog];

    emit dataSetXDataRegionChanged(dataSet, region);
}

void PlotAreaConfigWidget::ui_dataSetYDataRegionChanged()
{
    // Check for valid index
    if (d->selectedDataSet_CellRegionDialog < 0)
        return;

    const QString regionString = d->cellRegionDialog.yDataRegion->text();
    const CellRegion region(d->tableSource, regionString);

    DataSet *dataSet = d->dataSets[d->selectedDataSet_CellRegionDialog];

    emit dataSetYDataRegionChanged(dataSet, region);
}

void PlotAreaConfigWidget::ui_dataSetCustomDataRegionChanged()
{
    // Only makes sense when bubble charts are implemented
    // TODO: ui_dataSetCustomDataRegionChanged
    return;

  /*
    // Check for valid index
    if (d->selectedDataSet_CellRegionDialog < 0)
        return;

    const QString region = d->cellRegionDialog.customDataRegion->text();

    DataSet *dataSet = d->dataSets[d->selectedDataSet_CellRegionDialog];

    emit dataSetCustomDataRegionChanged(dataSet, region);
    */
}

void PlotAreaConfigWidget::ui_dataSetCategoryDataRegionChanged()
{
    // Check for valid index
    if (d->selectedDataSet_CellRegionDialog < 0)
        return;

    const QString regionString = d->cellRegionDialog.categoryDataRegion->text();
    const CellRegion region(d->tableSource, regionString);

    DataSet *dataSet = d->dataSets[d->selectedDataSet_CellRegionDialog];

    emit dataSetCategoryDataRegionChanged(dataSet, region);
}

void PlotAreaConfigWidget::ui_dataSetLabelDataRegionChanged()
{
    // Check for valid index
    if (d->selectedDataSet_CellRegionDialog < 0)
        return;

    const QString regionString = d->cellRegionDialog.labelDataRegion->text();
    const CellRegion region(d->tableSource, regionString);

    DataSet *dataSet = d->dataSets[d->selectedDataSet_CellRegionDialog];

    emit dataSetLabelDataRegionChanged(dataSet, region);

    // label may have changed
    QString title = dataSet->labelData().toString();
    if (title.isEmpty()) {
        title = i18n("Data Set %1", d->selectedDataSet_CellRegionDialog);
    }
    d->cellRegionDialog.dataSets->setItemText(d->selectedDataSet_CellRegionDialog, title);
}


void PlotAreaConfigWidget::ui_dataSetSelectionChanged_CellRegionDialog(int index)
{
    // Check for valid index
    debugChartUiPlotArea<<index<<d->dataSets;
    if (index < 0 || index >= d->dataSets.size())
        return;

    DataSet *dataSet = d->dataSets[index];
    const int dimensions = dataSet->dimension();

    d->cellRegionDialog.labelDataRegion->setText(dataSet->labelDataRegion().toString());
    debugChartUiPlotArea<<"dim"<<dimensions;
    if (dimensions > 1) {
        d->cellRegionDialog.xDataRegion->setEnabled(true);
        d->cellRegionDialog.xDataRegion->setText(dataSet->xDataRegion().toString());
    } else {
        d->cellRegionDialog.xDataRegion->setEnabled(false);
    }
    d->cellRegionDialog.yDataRegion->setText(dataSet->yDataRegion().toString());
    d->cellRegionDialog.categoryDataRegion->setText(dataSet->categoryDataRegion().toString());

    d->selectedDataSet_CellRegionDialog = index;
}

