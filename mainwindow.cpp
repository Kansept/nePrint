#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QAxBase>
#include <QAxObject>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFontDialog>
#include <sqlite3.h>
#include <QVariant>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QPicture>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinterInfo>
#include <math.h>
#include <QMessageBox>
#include <QIcon>
#include <QtConcurrent>
#include <QMargins>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setting = new Setting();
    setting->loadSetting();

    createModelRecipient();
    createViewNotice();
    createViewEnvelope();
    createStatusBar();

    loadSetting();
    applySetting();

    dialogSetting = new DialogSetting();
    dialogRecepient = new DialogRecepient();
    dialogUpdate = new DialogUpdate();

    removeTempFile();

    // Файл
    connect ( ui->open, SIGNAL(triggered()), SLOT(open()) );
    connect ( ui->saveAs, SIGNAL(triggered()), SLOT(saveAs()) );
    connect ( ui->settingApp, SIGNAL(triggered()), dialogSetting, SLOT(show()) );
    connect ( dialogSetting, SIGNAL(settingSaved()), SLOT(applySetting()) );
    connect ( ui->saveAsPdf, SIGNAL(triggered()), SLOT(saveAsPdf()) );
    connect ( ui->printPreview, SIGNAL(triggered()), SLOT(printPreview()) );
    connect ( ui->quitApp, SIGNAL(triggered()), SLOT(close()) );
    connect ( ui->print, SIGNAL(triggered()), SLOT(print()) );
    // Получатели
    connect ( ui->tableView, SIGNAL(keyPressed(int,int)), SLOT(recepientHotKey(int, int)) );
    connect ( ui->loadRecipient, SIGNAL(triggered()), SLOT(loadExcelInThread()) );
    connect ( ui->widgetLoadRecipient, SIGNAL(clicked()), SLOT(loadExcelInThread()) );
    connect ( ui->addRecepient, SIGNAL(clicked()), SLOT(newRecepient()) );
    connect ( dialogRecepient, SIGNAL(sendRecepient(Recepient)), this, SLOT(addRecepient(Recepient)) );
    connect ( ui->tableView, SIGNAL(clicked(QModelIndex)), SLOT(changeRecipient(QModelIndex)) );
    connect ( ui->tableView, SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(changeRecipient(QModelIndex)) );
    connect ( ui->removeRecipient, SIGNAL(triggered()), SLOT(removeRecipient()) );
    connect ( ui->widgetRemoveRecipient, SIGNAL(clicked()), SLOT(removeRecipient()) );
    connect ( ui->clearRecipient, SIGNAL(triggered()), SLOT(clearRecipient())  );
    connect ( ui->widgetClearRecipient, SIGNAL(clicked()), SLOT(clearRecipient())  );
    connect ( ui->tableView, SIGNAL(doubleClicked(QModelIndex)), SLOT(editRecepient(QModelIndex)) );
    // Вид
    connect ( ui->zoomIn, SIGNAL(triggered()), SLOT(zoomIn()) );
    connect ( ui->zoomOut, SIGNAL(triggered()), SLOT(zoomOut()) );
    connect ( ui->modeDrag, SIGNAL(triggered()), SLOT(viewModeDrag()) );
    connect ( ui->modeSelect, SIGNAL(triggered()), SLOT(viewModeSelect()) );
    // Справка
    connect ( ui->about, SIGNAL(triggered()), SLOT(about()) );
    connect( ui->checkUpdate, SIGNAL(triggered()), dialogUpdate, SLOT(show()) );
    // Контекстное меню
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect ( ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(contextMenuRecepirnt(QPoint)) );
    ui->graphicsViewEnvelope->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( ui->graphicsViewEnvelope, SIGNAL(customContextMenuRequested(QPoint)), SLOT(contextMenuEnvelope(QPoint)) );
    ui->graphicsViewNoticeFront->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->graphicsViewNoticeBack->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( ui->graphicsViewNoticeFront, SIGNAL(customContextMenuRequested(QPoint)), SLOT(contextMenuNotice(QPoint)) );
    connect( ui->graphicsViewNoticeBack, SIGNAL(customContextMenuRequested(QPoint)), SLOT(contextMenuNotice(QPoint)) );
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::showEvent(QShowEvent *event)
{
    if ( event->isAccepted() )
        checkUpdate();
}

void MainWindow::removeTempFile()
{
    QFileInfoList listTempFiles;
    getListFiles( QCoreApplication::applicationDirPath(), listTempFiles );

    for ( int i = 0; i < listTempFiles.count(); i++ )
        QFile::remove( listTempFiles.at(i).absoluteFilePath() );
}


void MainWindow::getListFiles( QString path, QFileInfoList &listTempFiles )
{
    QDir dir( path );
    QFileInfoList listFiles = dir.entryInfoList( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot );

    for ( int i = 0; i < listFiles.count(); i++ ) {
        if ( listFiles.at(i).isFile() ) {
            if ( listFiles.at(i).absoluteFilePath().split(".").last().toLower() == "tmp" )
                listTempFiles.append( listFiles.at(i) );
        } else if ( listFiles.at(i).isDir() ) {
            getListFiles( listFiles.at(i).absoluteFilePath(), listTempFiles );
        }
    }
}


void MainWindow::recepientHotKey(int key, int modifierKey)
{
    switch (key) {
    case Qt::Key_Space:
        changeStatusRecepient();
        break;
    case Qt::Key_Delete:
        removeRecipient();
        break;
    default:
        break;
    }
}


void MainWindow::createStatusBar()
{
    sb1 = new QLabel(statusBar());
    ui->statusBar->setSizeGripEnabled(false);
    ui->statusBar->addWidget(sb1, 1);
    sb1->setText("");
}


void MainWindow::contextMenuRecepirnt(QPoint pos)
{
    actionAddRecepient = new QAction(tr("Добавить получателя"), this);
    connect ( actionAddRecepient, SIGNAL(triggered()), SLOT(newRecepient()) );

    actionChangeStatusRecepient = new QAction(tr("Вкл./Выкл"), this);
    connect ( actionChangeStatusRecepient, SIGNAL(triggered()), SLOT(changeStatusRecepient()) );

    QMenu menuContext;
    menuContext.addAction(actionAddRecepient);
    menuContext.addAction(ui->loadRecipient);
    menuContext.addSeparator();
    menuContext.addAction(actionChangeStatusRecepient);
    menuContext.addSeparator();
    menuContext.addAction(ui->removeRecipient);
    menuContext.addAction(ui->clearRecipient);
    menuContext.exec( ui->tableView->mapToGlobal(pos) );
}


void MainWindow::contextMenuEnvelope(QPoint pos)
{
    actionSettingEnvelope = new QAction(tr("Настройки"), this);
    connect ( actionSettingEnvelope, SIGNAL(triggered()), dialogSetting, SLOT(showSettingEnvelope()) );

    actionSavePositionEnvelope = new QAction(tr("Сохранить позиции элементов"), this);
    connect ( actionSavePositionEnvelope, SIGNAL(triggered()), SLOT(savePositionEnvelope()) );

    QMenu menuContext;
    menuContext.addAction(ui->modeDrag);
    menuContext.addAction(ui->modeSelect);
    menuContext.addSeparator();
    menuContext.addAction(actionSettingEnvelope);
    menuContext.addAction(actionSavePositionEnvelope);
    menuContext.addSeparator();
    menuContext.addAction(ui->printPreview);
    menuContext.addAction(ui->print);
    menuContext.exec( ui->graphicsViewEnvelope->mapToGlobal(pos) );
}


void MainWindow::contextMenuNotice(QPoint pos)
{
    actionSettingNotice = new QAction(tr("Настройки"), this);
    connect ( actionSettingNotice, SIGNAL(triggered()), dialogSetting, SLOT(showSettingNotice()) );

    actionSavePositionNotice = new QAction(tr("Сохранить позиции элементов"), this);
    connect ( actionSavePositionNotice, SIGNAL(triggered()), SLOT(savePositionNotice()) );

    QMenu menuContext;
    menuContext.addAction(ui->modeDrag);
    menuContext.addAction(ui->modeSelect);
    menuContext.addSeparator();
    menuContext.addAction(actionSettingNotice);
    menuContext.addAction(actionSavePositionNotice);
    menuContext.addSeparator();
    menuContext.addAction(ui->printPreview);
    menuContext.addAction(ui->print);

    if ( ui->tabNoticeFront->isVisible() )
         menuContext.exec( ui->graphicsViewNoticeFront->mapToGlobal(pos) );
    else if ( ui->tabNoticeBack->isVisible() )
         menuContext.exec( ui->graphicsViewNoticeBack->mapToGlobal(pos) );
}


void MainWindow::createViewNotice()
{
    // Извещения - холст
    QPen blackPen(Qt::white);
    blackPen.setWidth(1);
    QRectF rectCanvasNotice(0, 0, 526, 372);

    //  Извещение - лицевая сторона ===========================
    sceneNoticeFront = new QGraphicsScene(this);
    sceneNoticeFront->setSceneRect( rectCanvasNotice );
    // Подложка лицевой стороны извещения
    svgNoticeFront = new QGraphicsSvgItem( setting->pathTemplateNoticeFront );
    svgNoticeFront->setCachingEnabled(true);
    sceneNoticeFront->addItem(svgNoticeFront);
    svgNoticeFront->setZValue(1);

    // Холст
    rectangleFront = sceneNoticeFront->addRect( rectCanvasNotice, blackPen, Qt::white );
    rectangleFront->setZValue(-1);
    //
    sceneNoticeFront->setBackgroundBrush(Qt::lightGray);
    ui->graphicsViewNoticeFront->setDragMode(QGraphicsView::RubberBandDrag);
    ui->graphicsViewNoticeFront->setInteractive(true);
    ui->graphicsViewNoticeFront->setMouseTracking(true);
    ui->graphicsViewNoticeFront->setScene(sceneNoticeFront);

    //  Извещение - задняя сторона ===========================
    sceneNoticeBack = new QGraphicsScene(this);
    sceneNoticeBack->setSceneRect( rectCanvasNotice );
    // ФИО
    nameNoticeBack = sceneNoticeBack->addText("ФИО");
    nameNoticeBack->setPos(20, 20);
    configureTextItem(nameNoticeBack);
    // Адрес
    addressNoticeBack = sceneNoticeBack->addText("Адрес");
    addressNoticeBack->setPos(20, 50);
    configureTextItem(addressNoticeBack);
    // Лицевой счет
    numberNoticeBack = sceneNoticeBack->addText("Лицевой");
    numberNoticeBack->setPos(20, 70);
    configureTextItem(numberNoticeBack);
    // Подложка задней стороны извещения
    svgNoticeBack = new QGraphicsSvgItem( setting->pathTemplateNoticeBack );
    svgNoticeBack->setCachingEnabled(true);
    sceneNoticeBack->addItem(svgNoticeBack);
    svgNoticeBack->setZValue(1);
    // Холст
    rectangleBack = sceneNoticeBack->addRect( rectCanvasNotice, blackPen, Qt::white );
    rectangleBack->setZValue(-1);
    sceneNoticeBack->setBackgroundBrush(Qt::lightGray);
    ui->graphicsViewNoticeBack->setDragMode(QGraphicsView::RubberBandDrag);
    ui->graphicsViewNoticeBack->setInteractive(true);
    ui->graphicsViewNoticeBack->setMouseTracking(true);
    ui->graphicsViewNoticeBack->setScene(sceneNoticeBack);

    rectNoticeFront = sceneNoticeFront->addRect(
                sceneNoticeFront->sceneRect(), QPen(Qt::black, 2, Qt::DotLine), Qt::white );
    rectNoticeBack = sceneNoticeBack->addRect(
                sceneNoticeBack->sceneRect(), QPen(Qt::black, 2, Qt::DotLine), Qt::white );

    rectNoticeFront->hide();
    rectNoticeBack->hide();
}


void MainWindow::createViewEnvelope()
{
    QPen blackPen(Qt::white);
    blackPen.setWidth(1);

    QRectF rectCanvasEnvelope(0, 0, 780, 390);

    sceneEnvelope = new QGraphicsScene(this);
    sceneEnvelope->setSceneRect( rectCanvasEnvelope );
    // Холст
    rectEnvelope = sceneEnvelope->addRect( rectCanvasEnvelope, blackPen, Qt::white );
    rectEnvelope->setZValue(-1);
    sceneEnvelope->setBackgroundBrush(Qt::lightGray);
    ui->graphicsViewEnvelope->setDragMode(QGraphicsView::RubberBandDrag);
    ui->graphicsViewEnvelope->setInteractive(true);
    ui->graphicsViewEnvelope->setMouseTracking(true);
    ui->graphicsViewEnvelope->setScene(sceneEnvelope);
    // ФИО
    nameEnvelope = sceneEnvelope->addText("ФИО");
    nameEnvelope->setPos(20, 20);
    configureTextItem(nameEnvelope);
    // Адрес
    addressEnvelope = sceneEnvelope->addText("Адрес");
    addressEnvelope->setPos(20, 50);
    configureTextItem(addressEnvelope);
    // Подложка конверта
    svgEnvelope = new QGraphicsSvgItem( setting->pathTemplateEnvelope );
    svgEnvelope->setCachingEnabled(true);
    sceneEnvelope->addItem(svgEnvelope);
    svgEnvelope->setZValue(1);
}


void MainWindow::configureTextItem(QGraphicsTextItem *textItem)
{
    textItem->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
    textItem->setZValue(1);
    textItem->setFont( setting->fontNotice );
}


void MainWindow::createModelRecipient()
{
    DbRecepient::createDb();

    modelRecipient = new ModelRecepient( 0, QSqlDatabase::database("recepient") );
    modelRecipient->setEditStrategy( QSqlTableModel::OnManualSubmit );
    modelRecipient->setTable("recepient");
    modelRecipient->select();

    modelRecipient->setHeaderData(0, Qt::Horizontal, "Номер");
    modelRecipient->setHeaderData(1, Qt::Horizontal, "ФИО");
    modelRecipient->setHeaderData(2, Qt::Horizontal, "Адрес");
    modelRecipient->setHeaderData(3, Qt::Horizontal, "Лицевой");
    modelRecipient->setHeaderData(4, Qt::Horizontal, "Вкл.");

    ui->tableView->setModel(modelRecipient);
    ui->tableView->horizontalHeader()->moveSection(4,1);
    ui->tableView->hideColumn(0);
    ui->tableView->setItemDelegateForColumn(4, new DelegateStatus);

    ui->tableView->horizontalHeader()->resizeSection(0, 60);
    ui->tableView->horizontalHeader()->resizeSection(1, 150);
    ui->tableView->horizontalHeader()->resizeSection(2, 300);
    ui->tableView->horizontalHeader()->resizeSection(3, 60);
    ui->tableView->horizontalHeader()->resizeSection(4, 30);

    modelRecipient->sort(1, Qt::AscendingOrder);
}


void MainWindow::open()
{
    QString pathFile = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "",
                                                    tr("QPrint (*.qpr);;") );
    if ( !pathFile.isEmpty() ) {
        DbRecepient::saveToFile( QSqlDatabase::database("recepient"), pathFile, false );
        modelRecipient->submitAll();
    }
}


void MainWindow::saveAs()
{
    QString pathFile = QFileDialog::getSaveFileName(this, tr("Сохранить файл"), "",
                                                    tr("QPrint (*.qpr);;") );
    if ( !pathFile.isEmpty() )
        DbRecepient::saveToFile( QSqlDatabase::database("recepient"), pathFile, true );
}


void MainWindow::loadExcelInThread()
{   
//    if ( QThread::idealThreadCount() >= 2 )
//        QtConcurrent::run( this, &MainWindow::loadExcel );
//    else
        loadExcel();
}


void MainWindow::loadExcel()
{
    QStringList strlRow;
    strlRow.clear();

    QString pathFile = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "",
                                                    tr("All support files (*.xls *.xlsx);;") );

    if (pathFile.isEmpty())
        return;

    sb1->setText("Файл открыт");
    QAxObject *excel = new QAxObject("Excel.Application", this);

    if ( excel->isNull() ) {
        sb1->setText("MS Excel не найден!");
        return;
    }

    excel->dynamicCall( "SetVisible(bool)", false );

    QAxObject *workbooks = excel->querySubObject("WorkBooks");
    workbooks->dynamicCall( "Open (const QString&)", pathFile );
    QAxObject *workbook = excel->querySubObject("ActiveWorkBook");

    if (workbook) {
        QAxObject *sheets = workbook->querySubObject( "Sheets" );
        QAxObject *sheet = sheets->querySubObject("Item(1)");
        sheet->dynamicCall( "Select()" );

        QString textCell = "";
        sb1->setText("Загрузка данных");
        int currentRow(4);
        bool endTable = false;
        while ( !endTable ) {
            for ( int currentColumn = 0; currentColumn < 7; currentColumn++ ) {
                QAxObject *cell = sheet->querySubObject( "cells(int, int)", currentRow+1, currentColumn+1 );
                cell->dynamicCall( "Select()" );
                textCell = (cell)? cell->property("Value").toString() : "";
                if ( currentColumn == 1 and textCell == "" ) {
                    endTable = true;
                } else {
                    strlRow.append(textCell.simplified());
                }
            }
            if( strlRow.at(2) != "" ) {
                Recepient recepient;
                recepient.setName( strlRow.at(2) );
                recepient.setAddress(  strlRow.at(1) );
                recepient.setNumber( strlRow.at(6) );

                DbRecepient::insert( recepient );
            }
            strlRow.clear();
            currentRow++;
            sb1->setText("Загрузка строки - " + QString::number(currentRow-4) );
        }
    }

    excel->dynamicCall("Quit()");
    delete excel;

    modelRecipient->submitAll();
    showStatusBarCountRecepient();
}


void MainWindow::newRecepient()
{
    Recepient recepient;
    dialogRecepient->show();
    dialogRecepient->add(recepient);
}


void MainWindow::addRecepient(Recepient recepient)
{
    if ( recepient.getId() == -1 )
        DbRecepient::insert(recepient);
    else
        DbRecepient::update(recepient);

    modelRecipient->submitAll();
}


void MainWindow::changeStatusRecepient()
{\
    QModelIndex index;
    foreach (index, ui->tableView->selectionModel()->selectedRows())
    {
        QSqlRecord record = modelRecipient->record( index.row() );
        record.setValue("status", !record.value("status").toBool());
        modelRecipient->setRecord(index.row(), record);
    }
    modelRecipient->submitAll();
}


void MainWindow::editRecepient(QModelIndex index)
{
    QSqlRecord record = modelRecipient->record(index.row());

    Recepient recepient;
    recepient.setId( record.value("id").toInt() );
    recepient.setName( record.value("name").toString() );
    recepient.setAddress( record.value("address").toString() );
    recepient.setNumber( record.value("number").toString() );

    dialogRecepient->show();
    dialogRecepient->add(recepient);
}


void MainWindow::changeRecipient(QModelIndex index)
{
    QSqlRecord record = modelRecipient->record(index.row());

    nameNoticeBack->setPlainText( record.value("name").toString() );
    addressNoticeBack->setPlainText( record.value("address").toString() );
    numberNoticeBack->setPlainText( record.value("number").toString() );

    nameEnvelope->setPlainText( record.value("name").toString() );
    addressEnvelope->setPlainText( record.value("address").toString() );
}


void MainWindow::removeRecipient()
{
    if(ui->tableView->selectionModel()->currentIndex().row() == -1)
        return;

    QModelIndex index;
    foreach(index, ui->tableView->selectionModel()->selectedRows())
        modelRecipient->removeRow(index.row());

    modelRecipient->submitAll();

    showStatusBarCountRecepient();
}


void MainWindow::clearRecipient()
{
    for ( int i = 0; i < modelRecipient->rowCount(); i++ )
        modelRecipient->removeRow(i);

    modelRecipient->submitAll();
}


void MainWindow::showStatusBarCountRecepient()
{
    while (modelRecipient->canFetchMore())
        modelRecipient->fetchMore();
    sb1->setText("Загружено " + QString::number(modelRecipient->rowCount()) + " получателей");
}


void MainWindow::print()
{
    QPrinter printer;
    if (QPrintDialog(&printer).exec() == QDialog::Accepted) {
        if ( ui->tabEnvelope->isVisible() ) {
            configurePrinterEnvelope(&printer);
            printEnvelope(&printer);
        } else {
            configurePrinterNotice(&printer);
            printNotice(&printer);
        }
    }
}


void MainWindow::configurePrinterEnvelope(QPrinter *printer)
{
    printer->setPageSize(QPrinter::DLE);
    printer->setOrientation(QPrinter::Landscape);
    printer->setResolution(300);
}


void MainWindow::configurePrinterNotice(QPrinter *printer)
{
    printer->setPageSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Landscape);
    printer->setResolution(300);
}


void MainWindow::printNotice(QPrinter *printer)
{
    if ( setting->showBoundary ) {
       rectNoticeFront->show();
       rectNoticeBack->show();
    } else {
        rectNoticeFront->hide();
        rectNoticeBack->hide();
    }

    nameNoticeBack->setFlag(QGraphicsItem::ItemIsSelectable, false);
    addressNoticeBack->setFlag(QGraphicsItem::ItemIsSelectable, false);

    while (modelRecipient->canFetchMore())
        modelRecipient->fetchMore();

    if ( setting->duplexPrintNotice ) {
        printNoticeDuplex(printer);
    } else {
        if ( ui->tabNoticeFront->isVisible() ) {
            printNoticeFront(printer);
        } else if ( ui->tabNoticeBack->isVisible() ) {
            printNoticeBack(printer);
        }
    }

    rectNoticeFront->hide();
    rectNoticeBack->hide();
    nameNoticeBack->setFlag(QGraphicsItem::ItemIsSelectable, true);
    addressNoticeBack->setFlag(QGraphicsItem::ItemIsSelectable, true);
}


void MainWindow::printNoticeFront(QPrinter *printer)
{
    QPainter painter;
    painter.begin(printer);
    painter.setBackground(QBrush(Qt::white));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);

    bool firstPage (true);
    QVector<Recepient> recepients ( getRecepient() );

    for ( int i = 0; i < countPageNotice(recepients.count()); i++ ) {
        if ( !firstPage ) {
            printer->newPage();
            printer->setPageMargins( 0, 0, 0, 0, QPrinter::Millimeter );
        }
        for ( int j = 0; j < 4; j++ ) {
            if ( (i*4+j) < recepients.count() ) {
                setRecepient( recepients.at(i*4+j) );
                sceneNoticeFront->render(&painter, positionPrintNotice(j, NoticeFront),
                                        sceneNoticeFront->sceneRect(), Qt::KeepAspectRatio);
            }
            firstPage = false;
        }
    }
    painter.end();
}


void MainWindow::printNoticeBack(QPrinter *printer)
{
    QPainter painter;
    painter.begin(printer);
    painter.setBackground(QBrush(Qt::white));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);

    bool firstPage (true);
    QVector<Recepient> recepients ( getRecepient() );

    for ( int i = 0; i < countPageNotice(recepients.count()); i++ ) {
        if ( !firstPage ) {
            printer->newPage();
            printer->setPageMargins( 0, 0, 0, 0, QPrinter::Millimeter );
        }
        for ( int j = 0; j < 4; j++ ) {
            if ( (i*4+j) < recepients.count() ) {
                setRecepient( recepients.at(i*4+j) );
                sceneNoticeBack->render(&painter, positionPrintNotice(j, NoticeBack),
                                        sceneNoticeBack->sceneRect(), Qt::KeepAspectRatio);
            }
            firstPage = false;
        }
    }
    painter.end();
}


void MainWindow::printNoticeDuplex(QPrinter *printer)
{
    QPainter painter;
    painter.begin(printer);
    painter.setBackground(QBrush(Qt::white));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform,
                           true);

    bool firstPage (true);
    QVector<Recepient> recepients ( DbRecepient::getEnabledRecepient() );

    for ( int i = 0; i < countPageNotice( recepients.count() ); i++ ) {
        if ( !firstPage )
            printer->newPage();

        for ( int j = 0; j < 4; j++ ) {
            if ( (i*4+j) < recepients.count() ) {
                setRecepient( recepients.at(i*4+j) );
                sceneNoticeFront->render(
                    &painter, positionPrintNotice(j, NoticeFront),
                    sceneNoticeFront->sceneRect(), Qt::KeepAspectRatio
                );
            }
        }

        firstPage = false;

        printer->newPage();
        for ( int j = 0; j < 4; j++ ) {
            if ( (i*4+j) < recepients.count() ) {
                setRecepient( recepients.at(i*4+j) );
                sceneNoticeBack->render(
                    &painter, positionPrintNotice(j, NoticeBack),
                    sceneNoticeBack->sceneRect(), Qt::KeepAspectRatio
                );
            }
        }
    }
    painter.end();
}


void MainWindow::printEnvelope(QPrinter *printer)
{
    nameEnvelope->setFlag(QGraphicsItem::ItemIsSelectable, false);
    addressEnvelope->setFlag(QGraphicsItem::ItemIsSelectable, false);

    while (modelRecipient->canFetchMore())
        modelRecipient->fetchMore();

    QPainter painter;
    painter.begin(printer);
        painter.setBackground(QBrush(Qt::white));
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform,
                               true);
        bool firstPage (true);
        QVector<Recepient> recepients ( getRecepient() );

        for ( int i = 0; i < recepients.count(); ++i ) {
            if ( !firstPage )
                printer->newPage();
            setRecepient( recepients.at(i) );
            sceneEnvelope->render(&painter, printer->pageRect(), sceneEnvelope->sceneRect(), Qt::KeepAspectRatio);
            firstPage = false;
        }

    painter.end();
    nameEnvelope->setFlag(QGraphicsItem::ItemIsSelectable, true);
    addressEnvelope->setFlag(QGraphicsItem::ItemIsSelectable, true);
}


QVector<Recepient> MainWindow::getRecepient()
{
    QVector<Recepient> recepients;

    for ( int i = 0; i < modelRecipient->rowCount(); ++i ) {
        ui->tableView->selectRow(i);
        Recepient recepient;
        QSqlRecord record = modelRecipient->record( ui->tableView->currentIndex().row() );
        if ( record.value("status").toInt() == 1 ) {
            recepient.setName( record.value("name").toString() );
            recepient.setAddress( record.value("address").toString() );
            recepient.setNumber( record.value("number").toString() );
            recepients.append(recepient);
        }
    }

    return recepients;
}


void MainWindow::setRecepient(Recepient recepient)
{
    nameNoticeBack->setPlainText( recepient.getName() );
    addressNoticeBack->setPlainText( recepient.getAddress() );
    numberNoticeBack->setPlainText( recepient.getNumber() );

    nameEnvelope->setPlainText( recepient.getName() );
    addressEnvelope->setPlainText( recepient.getAddress() );
}


void MainWindow::printPreview()
{
    if ( ui->tabNotice->isVisible() )
        printPreviewNotice();
    else if ( ui->tabEnvelope->isVisible() )
        printPreviewEnvelope();
}


void MainWindow::printPreviewNotice()
{
    QPrinter printer;
    configurePrinterNotice(&printer);
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowTitle("Предварительный просмотр");
    preview.setWindowFlags(Qt::Window);
    connect ( &preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printNotice(QPrinter*)) );
    preview.exec();
}


void MainWindow::printPreviewEnvelope()
{
    QPrinter printer;
    configurePrinterEnvelope(&printer);
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowTitle("Предварительный просмотр");
    preview.setWindowFlags(Qt::Window);
    connect ( &preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printEnvelope(QPrinter*)) );
    preview.exec();
}


int MainWindow::countPageNotice(int page)
{
    float remainder;
    int countPage;

    remainder = ( page/4.0 - int(page/4.0) );

    if ( remainder < 0.1 ) {
        countPage = int(page/4.0);
    } else {
        countPage = int(page/4.0) + 1;
    }

    return countPage;
}


QRectF MainWindow::positionPrintNotice(int pos, int side)
{
    QRectF rectTarget;
    QRectF rectSourse( sceneNoticeBack->sceneRect() );

    rectSourse.setWidth( rectSourse.width()*3 );
    rectSourse.setHeight( rectSourse.height()*3 );

    QPrinter printer;
    printer.setPageSize(QPrinter::A4);
    printer.setResolution(300);
    printer.setOrientation(QPrinter::Landscape);

    float marginWidth(0);
    float marginHeight(0);

    if ( setting->autoMargin ) {
        marginWidth  = ( (printer.pageRect().width() - (rectSourse.width()*2))/2 );
        marginHeight = ( (printer.pageRect().height() - (rectSourse.height()*2))/2 );
    } else {
        if ( side == NoticeFront ) {
            marginWidth  = setting->marginFrontLeft;
            marginHeight = setting->marginFrontTop;
        } else if ( side == NoticeBack ) {
            marginWidth  = setting->marginBackLeft;
            marginHeight = setting->marginBackTop;
        }
    }

    switch (pos) {
    case 0:       
        rectTarget = QRectF(marginWidth + rectSourse.width(), marginHeight,
                            rectSourse.width(), rectSourse.height());
        break;
    case 1:
        rectTarget = QRectF(marginWidth + rectSourse.width(), marginHeight + rectSourse.height(),
                            rectSourse.width(), rectSourse.height());
        break;
    case 2:
        rectTarget = QRectF(marginWidth, marginHeight,
                            rectSourse.width(), rectSourse.height());
        break;
    case 3:
        rectTarget = QRectF(marginWidth, marginHeight + rectSourse.height(),
                            rectSourse.width(), rectSourse.height());
        break;
    default:
        break;
    }
    return rectTarget;
}


void MainWindow::enableDuplexPrintNotice(bool b)
{
    setting->duplexPrintNotice = b;
}


void MainWindow::saveAsPdf()
{
    if ( ui->tabNotice->isVisible() )
        savePdfNotice();
    else if ( ui->tabEnvelope->isVisible() )
        savePdfEnvelope();
}


void MainWindow::savePdfNotice()
{
    QString fileName("Notice");
    fileName = QFileDialog::getSaveFileName(this, tr("Экспорт в PDF"), fileName, tr("PDF files (*.pdf)"),
                                            NULL, QFileDialog::DontConfirmOverwrite );

    if ( !fileName.isEmpty() ) {
        QPrinter printer;
        configurePrinterNotice(&printer);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        printNotice(&printer);
    }
}


void MainWindow::savePdfEnvelope()
{
    QString fileName("Envelope");
    fileName = QFileDialog::getSaveFileName(this, tr("Экспорт в PDF"), fileName, tr("PDF files (*.pdf)"),
                                            NULL, QFileDialog::DontUseSheet );

    if ( !fileName.isEmpty() ) {
        QPrinter printer;
        configurePrinterEnvelope(&printer);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        printEnvelope(&printer);
    }
}


void MainWindow::zoomIn() \
{
    if ( ui->tabWidgetType->currentIndex() == 0) {
        if( ui->tabWidgetNotice->currentIndex() == 0 ) {
            zoomInNoticeFront();
        } else {
            zoomInNoticeBack();
        }
    } else {
        zoomInEnvelope() ;
    }
}


void MainWindow::zoomInEnvelope() \
{
    QMatrix matrix (ui->graphicsViewEnvelope->matrix());
    matrix.scale(1.1, 1.1);
    ui->graphicsViewEnvelope->setMatrix( matrix );
}


void MainWindow::zoomInNoticeFront() \
{
    QMatrix matrix (ui->graphicsViewNoticeFront->matrix());
    matrix.scale(1.1, 1.1);
    ui->graphicsViewNoticeFront->setMatrix( matrix );
}


void MainWindow::zoomInNoticeBack() \
{
    QMatrix matrix (ui->graphicsViewNoticeBack->matrix());
    matrix.scale(1.1, 1.1);
    ui->graphicsViewNoticeBack->setMatrix( matrix );
}


void MainWindow::zoomOut() \
{
    if ( ui->tabWidgetType->currentIndex() == 0 ) {
        if( ui->tabWidgetNotice->currentIndex() == 0 ) {
            zoomOutNoticeFront();
        } else {
            zoomOutNoticeBack();
        }
    } else {
        zoomOutEnvelope() ;
    }
}


void MainWindow::zoomOutEnvelope() \
{
    QMatrix matrix (ui->graphicsViewEnvelope->matrix());
    matrix.scale(0.9, 0.9);
    ui->graphicsViewEnvelope->setMatrix(matrix);
}


void MainWindow::zoomOutNoticeFront() \
{
    QMatrix matrix (ui->graphicsViewNoticeFront->matrix());
    matrix.scale(0.9, 0.9);
    ui->graphicsViewNoticeFront->setMatrix(matrix);
}


void MainWindow::zoomOutNoticeBack() \
{
    QMatrix matrix (ui->graphicsViewNoticeBack->matrix());
    matrix.scale(0.9, 0.9);
    ui->graphicsViewNoticeBack->setMatrix( matrix );
}


/// --------------------------------------------- Меню -> Вид --------------------------------------------- ///
void MainWindow::viewModeSelect()
{
    if(ui->modeSelect->isChecked() ) {
        ui->modeDrag->setChecked(false);

        ui->graphicsViewNoticeFront->setDragMode(QGraphicsView::RubberBandDrag);
        ui->graphicsViewNoticeBack->setDragMode(QGraphicsView::RubberBandDrag);
        ui->graphicsViewEnvelope->setDragMode(QGraphicsView::RubberBandDrag);

        ui->graphicsViewNoticeFront->setInteractive(true);
        ui->graphicsViewNoticeBack->setInteractive(true);
        ui->graphicsViewEnvelope->setInteractive(true);
    }
    ui->modeSelect->setChecked(true);
}


void MainWindow::viewModeDrag()
{
    if(ui->modeDrag->isChecked()) {
        ui->modeSelect->setChecked(false);

        ui->graphicsViewNoticeFront->setDragMode(QGraphicsView::ScrollHandDrag);
        ui->graphicsViewNoticeBack->setDragMode(QGraphicsView::ScrollHandDrag);
        ui->graphicsViewEnvelope->setDragMode(QGraphicsView::ScrollHandDrag);

        ui->graphicsViewNoticeFront->setInteractive(false);
        ui->graphicsViewNoticeBack->setInteractive(false);
        ui->graphicsViewEnvelope->setInteractive(false);
    }
    ui->modeDrag->setChecked(true);
}


void MainWindow::loadSetting()
{
    nameNoticeBack->setPos( setting->positionNameNoticeBack );
    addressNoticeBack->setPos( setting->positionAddressNoticeBack );
    numberNoticeBack->setPos( setting->positionNumberNoticeBack );
    nameNoticeBack->setFont( setting->fontNotice );
    addressNoticeBack->setFont( setting->fontNotice );
    numberNoticeBack->setFont( setting->fontNotice );

    nameEnvelope->setPos( setting->positionNameEnvelope );
    addressEnvelope->setPos( setting->positionAddressEnvelope );
    nameEnvelope->setFont( setting->fontEnvelope );
    addressEnvelope->setFont( setting->fontEnvelope );
}


void MainWindow::applySetting()
{
    setting->loadSetting();

    delete svgEnvelope;
    svgEnvelope = new QGraphicsSvgItem( setting->pathTemplateEnvelope );
    svgEnvelope->setCachingEnabled(true);
    sceneEnvelope->addItem(svgEnvelope);
    svgEnvelope->setZValue(1);

    delete svgNoticeFront;
    svgNoticeFront = new QGraphicsSvgItem( setting->pathTemplateNoticeFront );
    svgNoticeFront->setCachingEnabled(true);
    sceneNoticeFront->addItem(svgNoticeFront);
    svgNoticeFront->setZValue(1);

    delete svgNoticeBack;
    svgNoticeBack = new QGraphicsSvgItem( setting->pathTemplateNoticeBack );
    svgNoticeBack->setCachingEnabled(true);
    sceneNoticeBack->addItem(svgNoticeBack);
    svgNoticeBack->setZValue(1);

    enableDuplexPrintNotice( setting->duplexPrintNotice  );

    nameEnvelope->setFont( setting->fontEnvelope );
    addressEnvelope->setFont( setting->fontEnvelope );

    nameNoticeBack->setFont( setting->fontNotice );
    addressNoticeBack->setFont( setting->fontNotice );
    numberNoticeBack->setFont( setting->fontNotice );
}


void MainWindow::savePositionEnvelope()
{
    setting->setPositionEnvelope( nameEnvelope->pos(), addressEnvelope->pos() );
}


void MainWindow::savePositionNotice()
{
    setting->setPositionNotice( nameNoticeBack->pos(), addressNoticeBack->pos(), numberNoticeBack->pos() );
}


void MainWindow::checkUpdate()
{
    if ( dialogUpdate->checkNewVersion() ) {
            QTimer::singleShot(1000, dialogUpdate, SLOT(show()));
       // dialogUpdate->show();
    }
}


void MainWindow::about()
{
    QString strAbout;
    strAbout = " <!DOCTYPE HTML> \
                <html><head><meta name=qrichtext content=1 /> \
                <style type=text/css> \
                html, body { font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; \
                font-style:normal; margin:0px; padding:0px;}  \
                p { white-space: pre-wrap; margin:0px; -qt-block-indent:0; text-indent:0px; }\
                </style> \
                </head> \
                <body> \
                <h3>Печать на конвертах</h3> \
                <p>Версия: 0.72</p> \
                <p>Автор: Владимир Kansept</p> \
                <p>email:<a href=\"mailto:kansept@yandex.ru\">kansept@yandex.ru</a></p><br/> \
                </body></html>";

    QMessageBox *about = new QMessageBox();
    about->setWindowTitle("О программе");
    about->setWindowIcon(QIcon(":/res/help_about.png"));
    about->setInformativeText(strAbout);
    about->exec();
    delete about;
}
