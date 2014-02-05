/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "CellViewPage.h"

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QMetaObject>
#include <QMetaMethod>
#include <QString>
#include <QSortFilterProxyModel>
#include <QPrintDialog>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QColorDialog>
#include <QImageReader>

#include "viewPages/CellViewPageToolBar.h"

<<<<<<< HEAD:src/view/pages/CellViewPage.cpp
#include "core/io/GeneXMLExporter.h"
#include "core/io/GeneTXTExporter.h"
=======
#include "io/GeneXMLExporter.h"
#include "io/GeneTXTExporter.h"

>>>>>>> 39bfdff81cd1108639383d1d72dd0dc33bfb4925:src/viewPages/CellViewPage.cpp
#include "utils/Utils.h"
#include "dialogs/SelectionDialog.h"

<<<<<<< HEAD:src/view/pages/CellViewPage.cpp
#include "view/openGL/CellGLView.h"
#include "view/openGL/ImageTextureGL.h"
#include "view/openGL/GridRendererGL.h"
#include <QSurfaceFormat>
=======
#include "dialogs/SelectionDialog.h"
#include "viewOpenGL/GraphicsViewGL.h"
#include "viewOpenGL/GraphicsSceneGL.h"
#include "viewOpenGL/ImageItemGL.h"
#include "viewOpenGL/GenePlotterGL.h"
#include "viewOpenGL/HeatMapLegendGL.h"
>>>>>>> 39bfdff81cd1108639383d1d72dd0dc33bfb4925:src/viewPages/CellViewPage.cpp

#include "model/GeneSelectionItemModel.h"
#include "model/GeneFeatureItemModel.h"

#include "ui_cellview.h"

CellViewPage::CellViewPage(QWidget *parent)
    : Page(parent)
{
    onInit();
}

CellViewPage::~CellViewPage()
{
    //finalizeGL();

    if (colorDialog_grid) {
        delete colorDialog_grid;
    }
    colorDialog_grid = 0;

    if (colorDialog_genes) {
        delete colorDialog_genes;
    }
    colorDialog_genes = 0;

    if (toolBar) {
        delete toolBar;
    }
    toolBar = 0;

    // grid gene_plotter and image are removed automatically

    delete ui;
}

void CellViewPage::onInit()
{
    DEBUG_FUNC_NAME
    
    //create UI objects
    ui = new Ui::CellView;
    ui->setupUi(this);
    
    ui->clearSelection->setIcon(QIcon(QStringLiteral(":/images/clear2.png")));
    ui->saveSelection->setIcon(QIcon(QStringLiteral(":/images/file_export.png")));
    ui->exportSelection->setIcon(QIcon(QStringLiteral(":/images/export.png")));

    //gene search displays a clear button
    ui->lineEdit->setClearButtonEnabled(true);

    //mouse tracking could cause problems on OSX
#if defined Q_OS_MAC
    ui->lineEdit->setMouseTracking(true);
    ui->lineEdit->setFocusPolicy(Qt::ClickFocus);
    ui->genes_tableview->setFocusPolicy(Qt::NoFocus);
    ui->genes_tableview->setMouseTracking(false);
    ui->selectAllGenes->setFocusPolicy(Qt::NoFocus);
    ui->selectAllGenes->setMouseTracking(false);
#endif

    // color dialogs
    colorDialog_grid = new QColorDialog(Globals::color_grid);
    colorDialog_grid->setOption(QColorDialog::DontUseNativeDialog, true); //OSX native color dialog gives problems
    colorDialog_genes = new QColorDialog(Globals::color_gene);
    colorDialog_genes->setOption(QColorDialog::DontUseNativeDialog, true); //OSX native color dialog gives problems

    // selection dialog
    selectionDialog = new SelectionDialog(this);

    //create tool bar and add it
    createToolBar();

    //create connections
    createConnections();

    // init OpenGL graphical objects
    initGLView();
}

void CellViewPage::onEnter()
{
    DEBUG_FUNC_NAME

    // initialize gui elements
    //initGLView();

    // reset and start gene plotter
    //gene_plotter_gl->initGL();
    //gene_plotter_gl->reset();
    //gene_plotter_gl->updateChipSize();
    //gene_plotter_gl->updateGeneData();
    //gene_plotter_gl->updateTransformation();
    
    //DataProxy *dataProxy = DataProxy::getInstance();
    //DataProxy::HitCountPtr hitCount = dataProxy->getHitCount(dataProxy->getSelectedDataset());
    //Q_ASSERT_X(hitCount, "Cell View", "HitCountPtr is NULL");

    // update hitcount gene plotter
    //gene_plotter_gl->setHitCountLimits(hitCount->min(), hitCount->max(), hitCount->sum());

    // update hitcount heat map
    //m_heatmap->setHitCountLimits(hitCount->min(), hitCount->max(), hitCount->sum());

    // reset main variabless
    resetActionStates();

    // create GL connections (important to do this after reseting action states)
    initGLConnections();

    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::DatasetPtr dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());
    Q_ASSERT(dataset != 0);
    DataProxy::ChipPtr currentChip = dataProxy->getChip(dataset->chipId());
    Q_ASSERT(currentChip != 0);

    QRectF m_rect = QRectF(
                QPointF(currentChip->x1(), currentChip->y1()),
                QPointF(currentChip->x2(), currentChip->y2())
                );
    QRectF m_border = QRectF(
                QPointF(currentChip->x1Border(), currentChip->y1Border()),
                QPointF(currentChip->x2Border(), currentChip->y2Border())
                );

    m_grid->clearData();
    m_grid->setDimensions(m_border, m_rect);
    m_grid->generateData();

    // load cell tissue
    slotLoadCellFigure();
}

void CellViewPage::onExit()
{
    DEBUG_FUNC_NAME

    ui->lineEdit->clearFocus();
    ui->genes_tableview->clearFocus();
    ui->selectAllGenes->clearFocus();
    ui->selections_tableview->clearFocus();
    ui->clearSelection->clearFocus();
    ui->saveSelection->clearFocus();

    finishGLConnections();
}


void CellViewPage::createConnections()
{
    DEBUG_FUNC_NAME

    // go back signal
    connect(toolBar->actionNavigate_goBack, SIGNAL(triggered(bool)), this, SIGNAL(moveToPreviousPage()));

    // gene model signals
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->genes_tableview->model());
    GeneFeatureItemModel *geneModel = qobject_cast<GeneFeatureItemModel*>(proxyModel->sourceModel());
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterFixedString(QString)));
    connect(ui->selectAllGenes, SIGNAL(clicked(bool)), geneModel, SLOT(selectAllGenesPressed(bool)));
    connect(colorDialog_genes, SIGNAL(colorSelected(QColor)), geneModel, SLOT(setColorGenes(const QColor&)));

    // cell tissue
    connect(toolBar->actionShow_cellTissueBlue, SIGNAL(triggered(bool)), this, SLOT(slotLoadCellFigure()));
    connect(toolBar->actionShow_cellTissueRed, SIGNAL(triggered(bool)), this, SLOT(slotLoadCellFigure()));

    // graphic view signals
    //connect(toolBar->actionZoom_zoomIn, SIGNAL(triggered(bool)), ui->view, SLOT(zoomIn()));
    //connect(toolBar->actionZoom_zoomOut, SIGNAL(triggered(bool)), ui->view, SLOT(zoomOut()));

    // print canvas
    connect(toolBar->actionSave_save,  SIGNAL(triggered(bool)), this, SLOT(slotSaveImage()));
    connect(toolBar->actionSave_print, SIGNAL(triggered(bool)), this, SLOT(slotPrintImage()));

    // export
    connect(ui->exportSelection, SIGNAL(clicked(bool)), this, SLOT(slotExportSelection()));

    // selection mode
    connect(toolBar->actionSelection_toggleSelectionMode, SIGNAL(triggered(bool)), this, SLOT(slotActivateSelection(bool)));
    connect(toolBar->actionSelection_showSelectionDialog, SIGNAL(triggered(bool)), this, SLOT(slotSelectByRegExp()));

    //color selectors
    connect(toolBar->actionColor_selectColorGenes, SIGNAL(triggered(bool)), this, SLOT(slotLoadColor()));
    connect(toolBar->actionColor_selectColorGrid,  SIGNAL(triggered(bool)), this, SLOT(slotLoadColor()));
}

void CellViewPage::resetActionStates()
{
    //reset selection mode
    slotActivateSelection(false);

    // load gene model (need to be done first)
    QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->genes_tableview->model());
    GeneFeatureItemModel* geneModel = qobject_cast<GeneFeatureItemModel*>(proxyModel->sourceModel());
    GeneSelectionItemModel* selectionModel = qobject_cast<GeneSelectionItemModel*>(ui->selections_tableview->model());
    // reset gene model data
    geneModel->loadGenes();
    // reset gene colors
    geneModel->setColorGenes(Globals::color_gene);
    // set all genes selected to false
    geneModel->selectAllGenesPressed(false);
    
    // reset gene selection model data
    selectionModel->reset();

    // reset color dialogs
    colorDialog_genes->setCurrentColor(Globals::color_gene);
    colorDialog_grid->setCurrentColor(Globals::color_grid);

    // reset gene list selection status
    ui->selectAllGenes->setChecked(false);

    // reset cell image to show
    //cell_tissue->setVisible(true);

    // reset gene grid to not show
    //gene_plotter_gl->setGridVisible(false);

    // reset tool bar actions
    toolBar->resetActions();

    DataProxy *dataProxy = DataProxy::getInstance();
    // restrict interface
    DataProxy::UserPtr current_user = dataProxy->getUser();
    Q_ASSERT_X(current_user, "Cell View", "Current User is NULL");
    toolBar->actionGroup_cellTissue->setVisible((current_user->role() == Globals::ROLE_CM));
}

void CellViewPage::createToolBar()
{
    toolBar = new CellViewPageToolBar();
    // add tool bar to the layout
    ui->pageLayout->insertWidget(0, toolBar);
}

void CellViewPage::initGLView()
{
    DEBUG_FUNC_NAME

    // creates graphic canvas scene and view

    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::NoProfile);
    format.setAlphaBufferSize(8);
    format.setRedBufferSize(8);
    format.setBlueBufferSize(8);
    format.setGreenBufferSize(8);
    format.setDepthBufferSize(-1);
    format.setStereo(false);
    format.setVersion(2,1);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setOption(QSurfaceFormat::DeprecatedFunctions);
    format.setRenderableType(QSurfaceFormat::OpenGL);

    view = new CellGLView(format);
    view->setOption(QGLView::FOVZoom, true); //enable orthographical zoom
    view->setOption(QGLView::CameraNavigation, false); // no panning

    QWidget *container = QWidget::createWindowContainer(view);
    container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->mainLayout->addWidget(container);

    //TODO modify layout so container takes 1/3 of the canvas

    // image texture graphical object
    m_image = new ImageTextureGL(this);
    view->setBackGroundTexture(m_image);

    // grid graphical object
    m_grid = new GridRendererGL(this);
    view->addRenderingNode(m_grid);

    // gene plotter component
    //gene_plotter_gl = new GenePlotterGL(this);
    //gene_plotter_gl->setZValue(1);
    //scene->addItem(gene_plotter_gl);

    // heatmap component
    //if (m_heatmap == 0) {
    //    m_heatmap = new HeatMapLegendGL();
    //    m_heatmap->setTransform(QTransform::fromTranslate(-10.0, 10.0));
    //    m_heatmap->setAnchor(ViewItemGL::NorthEast);
    //    m_heatmap->setVisible(false);
    //    ui->view->addViewItem(m_heatmap); //NOTE relinquish ownership
    //}
    //m_heatmap->setVisible(false);
}


void CellViewPage::initGLConnections()
{
    /*
    // gene model signals
    QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->genes_tableview->model());
    GeneFeatureItemModel* geneModel = qobject_cast<GeneFeatureItemModel*>(proxyModel->sourceModel());
    GeneSelectionItemModel* selectionModel = qobject_cast<GeneSelectionItemModel*>(ui->selections_tableview->model());

    //connect gene list model to gene plotter
    connect(geneModel, SIGNAL(signalSelectionChanged(DataProxy::GenePtr)), gene_plotter_gl,
            SLOT(updateGeneSelection(DataProxy::GenePtr)));
    connect(geneModel, SIGNAL(signalColorChanged(DataProxy::GenePtr)), gene_plotter_gl,
            SLOT(updateGeneColor(DataProxy::GenePtr)));

    //connect gene plotter to gene selection model
    connect(gene_plotter_gl, SIGNAL(featuresSelected(DataProxy::FeatureListPtr)), selectionModel,
            SLOT(loadGenes(DataProxy::FeatureListPtr)), Qt::UniqueConnection);

    connect(ui->clearSelection, SIGNAL(clicked(bool)), gene_plotter_gl, SLOT(clearSelectionArea()) );

    //threshold slider signal
    connect(toolBar, SIGNAL(thresholdLowerValueChanged(int)), gene_plotter_gl, SLOT(setGeneLowerLimit(int)));
    connect(toolBar, SIGNAL(thresholdUpperValueChanged(int)), gene_plotter_gl, SLOT(setGeneUpperLimit(int)));
    
    //gene attributes signals
    connect(toolBar, SIGNAL(intensityValueChanged(qreal)), gene_plotter_gl, SLOT(setGeneIntensity(qreal)));
    connect(toolBar, SIGNAL(sizeValueChanged(qreal)), gene_plotter_gl, SLOT(setGeneSize(qreal)));
    connect(toolBar, SIGNAL(shapeIndexChanged(Globals::Shape)), gene_plotter_gl, SLOT(setGeneShape(Globals::Shape)));

    //show/not genes signal
    connect(toolBar->actionShow_showGenes, SIGNAL(triggered(bool)), gene_plotter_gl, SLOT(setGeneVisible(bool))) ;

    //visual mode signal
    connect(toolBar->actionGroup_toggleVisualMode, SIGNAL(triggered(QAction*)), this,
            SLOT(slotSetGeneVisualMode(QAction*)));

    //threshold mode signal
    connect(toolBar->actionGroup_toggleThresholdMode, SIGNAL(triggered(QAction*)), this,
            SLOT(slotSetGeneThresholdMode(QAction*)));

    // grid signals
    connect(colorDialog_grid, SIGNAL(colorSelected(const QColor&)), gene_plotter_gl, SLOT(setGridColor(const QColor&)));
    connect(toolBar->actionShow_showGrid, SIGNAL(triggered(bool)), gene_plotter_gl, SLOT(setGridVisible(bool)));

    // cell tissue canvas
    connect(toolBar->actionShow_showCellTissue, SIGNAL(triggered(bool)), cell_tissue, SLOT(visible(bool)));

    // connect setvisible signals
    connect(toolBar->actionShow_toggleHeatMap, SIGNAL(toggled(bool)), m_heatmap, SLOT(setVisible(bool)));

    // connect threshold slider to the heatmap
    connect(toolBar, SIGNAL(thresholdLowerValueChanged(int)), m_heatmap, SLOT(setLowerLimit(int)));
    connect(toolBar, SIGNAL(thresholdUpperValueChanged(int)), m_heatmap, SLOT(setUpperLimit(int)));
   */
}

void CellViewPage::finishGLConnections()
{
    /*
    // gene model signals
    QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->genes_tableview->model());
    GeneFeatureItemModel* geneModel = qobject_cast<GeneFeatureItemModel*>(proxyModel->sourceModel());
    GeneSelectionItemModel* selectionModel = qobject_cast<GeneSelectionItemModel*>(ui->selections_tableview->model());

    //connect gene list model to gene plotter
    disconnect(geneModel, SIGNAL(signalSelectionChanged(DataProxy::GenePtr)), gene_plotter_gl,
               SLOT(updateGeneSelection(DataProxy::GenePtr)));
    disconnect(geneModel, SIGNAL(signalColorChanged(DataProxy::GenePtr)), gene_plotter_gl,
               SLOT(updateGeneColor(DataProxy::GenePtr)));

    //connect gene plotter to gene selection model
    disconnect(gene_plotter_gl, SIGNAL(featuresSelected(DataProxy::FeatureListPtr)), selectionModel,
            SLOT(loadGenes(DataProxy::FeatureListPtr)));
    disconnect(ui->clearSelection, SIGNAL(clicked(bool)), gene_plotter_gl, SLOT(clearSelectionArea()));

    //threshold slider signal
    disconnect(toolBar, SIGNAL(thresholdLowerValueChanged(int)), gene_plotter_gl, SLOT(setGeneLowerLimit(int)));
    disconnect(toolBar, SIGNAL(thresholdUpperValueChanged(int)), gene_plotter_gl, SLOT(setGeneUpperLimit(int)));

    //gene attributes signals
    disconnect(toolBar, SIGNAL(intensityValueChanged(qreal)), gene_plotter_gl, SLOT(setGeneIntensity(qreal)));
    disconnect(toolBar, SIGNAL(sizeValueChanged(qreal)), gene_plotter_gl, SLOT(setGeneSize(qreal)));
    disconnect(toolBar, SIGNAL(shapeIndexChanged(Globals::Shape)), gene_plotter_gl, SLOT(setGeneShape(Globals::Shape)));

    //show/not genes signal
    disconnect(toolBar->actionShow_showGenes, SIGNAL(triggered(bool)), gene_plotter_gl, SLOT(setGeneVisible(bool))) ;

    //visual mode signal
    disconnect(toolBar->actionGroup_toggleVisualMode, SIGNAL(triggered(QAction*)), this, SLOT(slotSetGeneVisualMode(QAction*)));

    //threshold mode signal
    disconnect(toolBar->actionGroup_toggleThresholdMode, SIGNAL(triggered(QAction*)), this,
               SLOT(slotSetGeneThresholdMode(QAction*)));

    // grid signals
    disconnect(colorDialog_grid, SIGNAL(colorSelected(const QColor&)), gene_plotter_gl, SLOT(setGridColor(const QColor&)));
    disconnect(toolBar->actionShow_showGrid, SIGNAL(triggered(bool)), gene_plotter_gl, SLOT(setGridVisible(bool)));

    // cell tissue canvas
    disconnect(toolBar->actionShow_showCellTissue, SIGNAL(triggered(bool)), cell_tissue, SLOT(visible(bool)));

    // connect setvisible signals
    disconnect(toolBar->actionShow_toggleHeatMap, SIGNAL(toggled(bool)), m_heatmap, SLOT(setVisible(bool)));

    // connect threshold slider to the heatmap
    disconnect(toolBar, SIGNAL(thresholdLowerValueChanged(int)), m_heatmap, SLOT(setLowerLimit(int)));
    disconnect(toolBar, SIGNAL(thresholdUpperValueChanged(int)), m_heatmap, SLOT(setUpperLimit(int)));

    */
}

void CellViewPage::slotLoadCellFigure()
{
    DEBUG_FUNC_NAME

    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::UserPtr current_user = dataProxy->getUser();
    DataProxy::DatasetPtr dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());
    Q_ASSERT(!current_user.isNull() && !dataset.isNull());

    const bool forceRedFigure = (QObject::sender() == toolBar->actionShow_cellTissueRed);
    const bool forceBlueFigure = (QObject::sender() == toolBar->actionShow_cellTissueBlue);
    const bool defaultRedFigure =
        (current_user->role() == Globals::ROLE_CM)
        && !(dataset->figureStatus() & Dataset::Aligned);
    const bool loadRedFigure = (defaultRedFigure || forceRedFigure) && !forceBlueFigure;

    QString figureid = (loadRedFigure) ? dataset->figureRed() : dataset->figureBlue();
    QIODevice *device = dataProxy->getFigure(figureid);

    //read image (TODO check file is present)
    QImageReader reader(device);
    const QImage image = reader.read();

    //deallocate device
    device->deleteLater();

    // add image to the texture image holder
    m_image->createTexture(image);

    //update checkboxes
    toolBar->actionShow_cellTissueBlue->setChecked(!loadRedFigure);
    toolBar->actionShow_cellTissueRed->setChecked(loadRedFigure);
}

void CellViewPage::slotPrintImage()
{
    QPrinter printer;
    printer.setOrientation(QPrinter::Landscape);
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    if (dialog->exec() != QDialog::Accepted) {
        return;
    }

    QPainter painter(&printer);
    QRect rect = painter.viewport();
    QImage image = view->grabPixmapGL();

    QSize size = image.size();
    size.scale(rect.size(), Qt::KeepAspectRatio);

    painter.setViewport(rect);
    painter.setWindow(image.rect());
    painter.drawImage(0, 0, image);
}

void CellViewPage::slotSaveImage()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::homePath(),
                       QString("%1;;%2").
                       arg(tr("JPEG Image Files (*.jpg *.jpeg)")).
                       arg(tr("PNG Image Files (*.png)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    // append default extension
    QRegExp regex("^.*\\.(jpg|jpeg|png)$", Qt::CaseInsensitive);
    if (!regex.exactMatch(filename)) {
        filename.append(".jpg");
    }

    const int quality = 100; //quality format (100 max, 0 min, -1 default)
    const QString format = filename.split(".", QString::SkipEmptyParts).at(1); //get the file extension
    QImage image = view->grabPixmapGL();

    if (!image.save(filename, format.toStdString().c_str(), quality)) {
        QMessageBox::warning(this, tr("Save Image"), tr("Error saving image."));
    }
}

void CellViewPage::slotExportSelection()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Export File"), QDir::homePath(),
                       QString("%1;;%2").
                       arg(tr("Text Files (*.txt)")).
                       arg(tr("XML Files (*.xml)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }
    // append default extension
    QRegExp regex("^.*\\.(txt|xml)$", Qt::CaseInsensitive);
    if (!regex.exactMatch(filename)) {
        filename.append(".txt");
    }

    // get selected features and extend with data
    //DataProxy::FeatureListPtr featureList = gene_plotter_gl->getSelectedFeatures();
    DataProxy::FeatureListPtr featureList; //JUST TEMP HACK

    // create export context
    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::HitCountPtr hitCount = dataProxy->getHitCount(dataProxy->getSelectedDataset());

    QObject context;
    context.setProperty("hitCountMin", QVariant(hitCount->min()));
    context.setProperty("hitCountMax", QVariant(hitCount->max()));
    context.setProperty("hitCountSum", QVariant(hitCount->sum()));

    QFile textFile(filename);
    QFileInfo info(textFile);

    if (textFile.open(QFile::WriteOnly | QFile::Truncate)) {
        QObject memoryGuard;
        //TODO move intantiation to factory
        GeneExporter *exporter =
            (QString::compare(info.suffix(), "XML", Qt::CaseInsensitive) == 0) ?
            dynamic_cast<GeneExporter *>(new GeneXMLExporter(&memoryGuard)) :
            dynamic_cast<GeneExporter *>(new GeneTXTExporter(GeneTXTExporter::SimpleFull,
                                        GeneTXTExporter::TabDelimited, &memoryGuard));

        exporter->exportItem(&textFile, featureList, context);
    }
    textFile.close();
}

void CellViewPage::slotActivateSelection(bool status)
{
    //ui->view->setDragMode(status ? QGraphicsView::RubberBandDrag : QGraphicsView::ScrollHandDrag);
    m_selection_mode = status;
}

void CellViewPage::slotSetGeneVisualMode(QAction *action)
{
    QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        //Globals::VisualMode mode = static_cast<Globals::VisualMode>(variant.toInt());
        //gene_plotter_gl->setGeneVisualMode(mode);
    } else {
        qDebug() << "[CellViewPage] Undefined gene visual mode!";
    }
}

void CellViewPage::slotSetGeneThresholdMode(QAction *action)
{
    QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        //Globals::ThresholdMode mode = static_cast<Globals::ThresholdMode>(variant.toInt());
        //gene_plotter_gl->setGeneThresholdMode(mode);
    } else {
        qDebug() << "[CellViewPage] Undefined gene visual mode!";
    }
}

void CellViewPage::slotLoadColor()
{
    if (QObject::sender() == toolBar->actionColor_selectColorGenes) {
        qDebug() << "[Cell View] : open gene color dialog";
        colorDialog_genes->open();
    } else if (QObject::sender() == toolBar->actionColor_selectColorGrid) {
        qDebug() << "[Cell View] : open grid color dialog";
        colorDialog_grid->open();
    }
}

void CellViewPage::slotSelectByRegExp()
{
    //const DataProxy::GeneList& geneList = SelectionDialog::selectGenes(this);
    //gene_plotter_gl->selectGenes(geneList);
}