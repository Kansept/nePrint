#ifndef MODELRECEPIENT
#define MODELRECEPIENT

#include <QSqlTableModel>
#include <QColor>

class ModelRecepient : public QSqlTableModel
{
    Q_OBJECT
public:
    ModelRecepient(QObject * parent = 0, QSqlDatabase db = QSqlDatabase())
        : QSqlTableModel(parent,db) {;}

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const
    {
        QVariant value = QSqlTableModel::data(index, role);
        // цвет текста
        if (role==Qt::TextColorRole) {
            QModelIndex controlIndex ;
            controlIndex = index.sibling(index.row(), 4);
            // Выключенный элемент
            if (controlIndex.data(Qt::DisplayRole).toString()==QObject::trUtf8("0"))
                return QColor(150,150,150);
        }
        if (role==Qt::BackgroundColorRole){
            QModelIndex controlIndex ;
            controlIndex= index.sibling(index.row(), 4);
            // Выключенный элемент
            if (controlIndex.data(Qt::DisplayRole).toString()==QObject::trUtf8("0"))
                return QColor(250,250,250);
        }


        return value;
    }
};

#endif // MODELRECEPIENT

