#ifndef GENERICDATACOLLECTIONTABLEMODEL_H
#define GENERICDATACOLLECTIONTABLEMODEL_H

#include <QAbstractTableModel>

class GenericDataCollectionTableModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit GenericDataCollectionTableModel(QObject *parent = 0);
  
signals:
  
public slots:
  
};

#endif // GENERICDATACOLLECTIONTABLEMODEL_H
