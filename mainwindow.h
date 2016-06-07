#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScreen>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QSqlTableModel>
#include <QGraphicsSvgItem>
#include <QPrinter>
#include <QAction>
#include <QLabel>
#include <QHeaderView>
#include <QFileInfoList>
#include <QStyledItemDelegate>

#include "textitem.h"
#include "dialogsetting.h"
#include "dialogrecepient.h"
#include "dialogupdate.h"
#include "recepient.h"
#include "dbrecepient.h"
#include "modelrecepient.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum NoticeSide { NoticeFront, NoticeBack };
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void removeTempFile();
private slots:
    void contextMenuRecepirnt(QPoint pos);
    void contextMenuNotice(QPoint pos);
    void contextMenuEnvelope(QPoint pos);

    void open();
    void saveAs();
    void createModelRecipient();
    void loadExcel();
    void loadExcelInThread();
    void newRecepient();
    void addRecepient(Recepient recepient);
    void editRecepient(QModelIndex index);
    void changeRecipient(const QModelIndex index);
    void changeStatusRecepient();
    void removeRecipient();
    void clearRecipient();
    void showStatusBarCountRecepient();

    void viewModeDrag();
    void viewModeSelect();
    void zoomIn();
        void zoomInNoticeFront();
        void zoomInNoticeBack();
        void zoomInEnvelope();
    void zoomOut();
        void zoomOutNoticeBack();
        void zoomOutNoticeFront();
        void zoomOutEnvelope();

    void loadSetting();
    void applySetting();
    void savePositionEnvelope();
    void savePositionNotice();
    void enableDuplexPrintNotice(bool b);

    void print();
        void configurePrinterNotice(QPrinter *printer);
        void configurePrinterEnvelope(QPrinter *printer);
        void printNotice(QPrinter *printer);
            void printNoticeFront(QPrinter *printer);
            void printNoticeBack(QPrinter *printer);
            void printNoticeDuplex(QPrinter *printer);
        void printEnvelope(QPrinter *printer);
    void printPreview();
        void printPreviewNotice();
        void printPreviewEnvelope();

        int countPageNotice(const int page);
        QRectF positionPrintNotice(const int pos, int side);

    void saveAsPdf();
        void savePdfNotice();
        void savePdfEnvelope();

    void about();
    void checkUpdate();

    void recepientHotKey(int key, int modifierKey);
    void setRecepient(Recepient recepient);
    void getListFiles( QString path, QFileInfoList &listTempFiles );

private:
    void createViewNotice();
    void createViewEnvelope();
    void configureTextItem(QGraphicsTextItem *textItem);
    void createStatusBar();

    Ui::MainWindow *ui;

    Setting *setting;
    DialogSetting *dialogSetting;
    DialogRecepient *dialogRecepient;
    DialogUpdate *dialogUpdate;
    ModelRecepient *modelRecipient;
    QHeaderView *headerTable;

    QLabel *sb1;
    QGraphicsRectItem *rectNoticeFront;
    QGraphicsRectItem *rectNoticeBack;
    QGraphicsScene *sceneEnvelope;
    QGraphicsRectItem *rectEnvelope;
    QGraphicsTextItem *nameEnvelope;
    QGraphicsTextItem *addressEnvelope;
    QGraphicsSvgItem *svgEnvelope;
    QAction *actionSavePositionEnvelope;
    QAction *actionSettingEnvelope;

    QAction *actionAddRecepient;
    QAction* actionChangeStatusRecepient;

    QGraphicsScene *sceneNoticeFront;
    QGraphicsRectItem *rectangleFront;
    QGraphicsSvgItem *svgNoticeFront;
    QGraphicsScene *sceneNoticeBack;
    QGraphicsRectItem *rectangleBack;
    QGraphicsTextItem *nameNoticeBack;
    QGraphicsTextItem *addressNoticeBack;
    QGraphicsTextItem *numberNoticeBack;
    QGraphicsSvgItem *svgNoticeBack;
    QAction *actionSettingNotice;
    QAction *actionSavePositionNotice;

    QVector<Recepient> getRecepient();
protected:
    void showEvent(QShowEvent *event);
};

// ---------------- ДЕЛЕГАТ ДЛЯ ЗАДАНИЙ -----------------
class DelegateStatus:public QStyledItemDelegate{
    Q_OBJECT
public:
    DelegateStatus(QWidget *parent=0):QStyledItemDelegate(parent){}
    QString displayText(const QVariant &value, const QLocale &locale) const{

        QString text = value.toString();

        if ( text == "1" )
            text = "да";
        else
            text = "нет";

        return QStyledItemDelegate::displayText(text, locale);
    }
};

#endif // MAINWINDOW_H
