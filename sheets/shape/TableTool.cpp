/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

// Local
#include "TableTool.h"
#include "SheetsEditor.h"

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QSpinBox>
#include <QToolBar>
#include <QPushButton>
#include <QUrl>
#include <QMimeDatabase>
#include <QFileDialog>

#include <kcombobox.h>
#include <KLocalizedString>
#include <kpagedialog.h>

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoIcon.h>

#include "SheetsDebug.h"
#include "AutoFillStrategy.h"
#include "Cell.h"
#include "calligra_sheets_limits.h"
#include "Map.h"
#include "MergeStrategy.h"
#include "Selection.h"
#include "SelectionStrategy.h"
#include "Sheet.h"

#include "commands/DataManipulators.h"

#include "TableShape.h"

using namespace Calligra::Sheets;

class TableTool::Private
{
public:
    Selection* selection;
    TableShape* tableShape;

    KComboBox* sheetComboBox;
};


TableTool::TableTool(KoCanvasBase* canvas)
        : CellToolBase(canvas)
        , d(new Private)
{
    setObjectName(QLatin1String("TableTool"));

    d->selection = new Selection(canvas);
    d->tableShape = 0;

    QAction* importAction = new QAction(koIcon("document-import"), i18n("Import OpenDocument Spreadsheet File"), this);
    importAction->setIconText(i18n("Import"));
    addAction("import", importAction);
    connect(importAction, SIGNAL(triggered()), this, SLOT(importDocument()));

    QAction* exportAction = new QAction(koIcon("document-export"), i18n("Export OpenDocument Spreadsheet File"), this);
    exportAction->setIconText(i18n("Export"));
    addAction("export", exportAction);
    connect(exportAction, SIGNAL(triggered()), this, SLOT(exportDocument()));
}

TableTool::~TableTool()
{
    delete d->selection;
    delete d;
}

void TableTool::importDocument()
{
    const QString filterString =
        QMimeDatabase().mimeTypeForName("application/vnd.oasis.opendocument.spreadsheet").filterString();
    // TODO: i18n for title
    QString file = QFileDialog::getOpenFileName(0, "Import", QString(), filterString);
    if (file.isEmpty())
        return;
#if 0 // FIXME Stefan: Port!
    d->tableShape->doc()->setModified(false);
    if (! d->tableShape->doc()->importDocument(file))
        return;
#endif
    updateSheetsList();
    if (Sheet* sheet = d->tableShape->sheet()) {
        QRect area = sheet->usedArea();
        if (area.width() > d->tableShape->columns())
            d->tableShape->setColumns(area.width());
        if (area.height() > d->tableShape->rows())
            d->tableShape->setRows(area.height());
    }
}

void TableTool::exportDocument()
{
    const QString filterString =
        QMimeDatabase().mimeTypeForName("application/vnd.oasis.opendocument.spreadsheet").filterString();
    // TODO: i18n for title
    QString file = QFileDialog::getSaveFileName(0, "Export", QString(), filterString);
    if (file.isEmpty())
        return;
#if 0 // FIXME Stefan: Port!
    d->tableShape->doc()->exportDocument(file);
#endif
}

void TableTool::repaintDecorations()
{
    if (!d->tableShape) return;
    // TODO Stefan: restrict to the changed area
    canvas()->updateCanvas(d->tableShape->boundingRect());
}

Selection* TableTool::selection()
{
    return d->selection;
}

void TableTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    foreach(KoShape* shape, shapes) {
        d->tableShape = dynamic_cast<TableShape*>(shape);
        if (d->tableShape)
            break;
    }
    if (!d->tableShape) {
        warnSheets << "No table shape found in selection.";
        emit done();
        return;
    }
    d->selection->setActiveSheet(d->tableShape->sheet());
    d->selection->setOriginSheet(d->tableShape->sheet());
    useCursor(Qt::ArrowCursor);
    d->tableShape->update();

    CellToolBase::activate(toolActivation, shapes);
}

void TableTool::deactivate()
{
    CellToolBase::deactivate();
    d->tableShape = 0;
}

QPointF TableTool::offset() const
{
    return d->tableShape->position();
}

QSizeF TableTool::size() const
{
    return d->tableShape->size();
}

QPointF TableTool::canvasOffset() const
{
    return QPointF();
}

int TableTool::maxCol() const
{
    return d->tableShape->columns();
}

int TableTool::maxRow() const
{
    return d->tableShape->rows();
}

SheetView* TableTool::sheetView(const Sheet* sheet) const
{
    Q_UNUSED(sheet);
    return d->tableShape->sheetView();
}

void TableTool::changeColumns(int num)
{
    d->tableShape->setColumns(num);
    d->tableShape->update();
}

void TableTool::changeRows(int num)
{
    d->tableShape->setRows(num);
    d->tableShape->update();
}

void TableTool::updateSheetsList()
{
    d->sheetComboBox->blockSignals(true);
    d->sheetComboBox->clear();
    Map *map = d->tableShape->map();
    foreach(Sheet* sheet, map->sheetList()) {
        if (sheet->isHidden())
            continue;
        d->sheetComboBox->addItem(sheet->sheetName());
        //d->sheetComboBox->setCurrentIndex( d->sheetComboBox->count()-1 );
    }
    d->sheetComboBox->blockSignals(false);
}

void TableTool::sheetActivated(const QString& sheetName)
{
    if (d->tableShape)
        d->tableShape->setSheet(sheetName);
}

void TableTool::sheetsBtnClicked()
{
    QPointer<KPageDialog> dialog = new KPageDialog();
    dialog->setWindowTitle(i18n("Sheets"));
    dialog->setStandardButtons(QDialogButtonBox::Ok);
    dialog->setFaceType(KPageDialog::Plain);
    SheetsEditor* editor = new SheetsEditor(d->tableShape);
    dialog->layout()->addWidget(editor);
    dialog->exec();
    updateSheetsList();
    delete dialog;
}

QList<QPointer<QWidget> > TableTool::createOptionWidgets()
{
    QWidget* optionWidget = new QWidget();
    optionWidget->setObjectName(QLatin1String("TableTool/Table Options"));

    QVBoxLayout* l = new QVBoxLayout(optionWidget);
    l->setMargin(0);
    optionWidget->setLayout(l);

    QGridLayout* layout = new QGridLayout();
    l->addLayout(layout);

    QLabel* label = 0;
    QSpinBox* spinBox = 0;

    QHBoxLayout* sheetlayout = new QHBoxLayout();
    sheetlayout->setMargin(0);
    sheetlayout->setSpacing(3);
    layout->addLayout(sheetlayout, 0, 1);
    d->sheetComboBox = new KComboBox(optionWidget);
    sheetlayout->addWidget(d->sheetComboBox, 1);
    Map *map = d->tableShape->map();
    foreach(Sheet* s, map->sheetList()) {
        d->sheetComboBox->addItem(s->sheetName());
        //d->sheetComboBox->setCurrentIndex( d->sheetComboBox->count()-1 );
    }
    connect(d->sheetComboBox, SIGNAL(activated(QString)), this, SLOT(sheetActivated(QString)));

    QPushButton *sheetbtn = new QPushButton(koIcon("table"), QString(), optionWidget);
    sheetbtn->setFixedHeight(d->sheetComboBox->sizeHint().height());
    connect(sheetbtn, SIGNAL(clicked()), this, SLOT(sheetsBtnClicked()));
    sheetlayout->addWidget(sheetbtn);
    label = new QLabel(i18n("Sheet:"), optionWidget);
    label->setBuddy(d->sheetComboBox);
    label->setToolTip(i18n("Selected Sheet"));
    layout->addWidget(label, 0, 0);

    spinBox = new QSpinBox(optionWidget);
    spinBox->setRange(1, KS_colMax);
    spinBox->setValue(d->tableShape->columns());
    layout->addWidget(spinBox, 2, 1);
    connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(changeColumns(int)));

    label = new QLabel(i18n("Columns:"), optionWidget);
    label->setBuddy(spinBox);
    label->setToolTip(i18n("Number of columns"));
    layout->addWidget(label, 2, 0);

    spinBox = new QSpinBox(optionWidget);
    spinBox->setRange(1, KS_rowMax);
    spinBox->setValue(d->tableShape->rows());
    layout->addWidget(spinBox, 3, 1);
    connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(changeRows(int)));

    label = new QLabel(i18n("Rows:"), optionWidget);
    label->setBuddy(spinBox);
    label->setToolTip(i18n("Number of rows"));
    layout->addWidget(label, 3, 0);

//layout->setColumnStretch( 1, 1 );
    layout->setRowStretch(4, 1);

    QToolBar* tb = new QToolBar(optionWidget);
    l->addWidget(tb);
    tb->setMovable(false);
    tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tb->addAction(action("import"));
    tb->addAction(action("export"));

    QList<QPointer<QWidget> > ow = CellToolBase::createOptionWidgets();
    optionWidget->setWindowTitle(i18n("Table Options"));
    ow.append(optionWidget);
    return ow;
}
