#include "linkbackfilterdelegate.h"
#include <QLineEdit>
#include <QComboBox>
#include <typeinfo>
#include <QCheckBox>
#include <QDoubleSpinBox>

LinkBackFilterDelegate::LinkBackFilterDelegate(QObject *parent) :
  QStyledItemDelegate(parent)
{
}
// TODO: Look here: http://qt-project.org/forums/viewthread/17880

#include <QFile>
#include <QTextStream>
#include <QCheckBox>
#include <QDateEdit>

QWidget *LinkBackFilterDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option,
                                              const QModelIndex &index) const
{
  if (!index.isValid())
  {
    return nullptr;
  }
  QVariant qvar = index.model()->data(index, Qt::EditRole);
  if (qvar.type() == QVariant::StringList)
  {
    return new QComboBox(parent);
  }
  else if (qvar.type() == QVariant::Bool)
  {
    QCheckBox* checkBox = new QCheckBox(parent);
    checkBox->setTristate(false);
    return checkBox;
  }
  else if (qvar.type() == QVariant::Date && !getDateFormatString().isEmpty())
  {
    QDateEdit* editor = new QDateEdit(parent);
    editor->setDisplayFormat(getDateFormatString());
    // Annoying that I need this!
    editor->setAutoFillBackground(true);
    return editor;
  }

  //widget = new QLineEdit(parent);
  // Returns an expanding line editor.
  QWidget* widget = QStyledItemDelegate::createEditor(parent, option, index);

  // If I do not set this to true, then the "view" data shows through the text while editing.
  // Also, the expanding editor grows over existing controls, and you can see those through
  // the edit display as well (distracting).
  widget->setAutoFillBackground(true);
  return widget;
}

QString LinkBackFilterDelegate::displayText(const QVariant & value, const QLocale & locale ) const
{
  if (value.type() == QVariant::Date && !getDateFormatString().isEmpty())
  {
    //qDebug(qPrintable(QString("%1 %2 %3").arg(value.toDate().toString()).arg(value.toDate().toString("")).arg(value.toDate().toString("MM/dd/yyy"))));
    return value.toDate().toString(getDateFormatString());
  }
  return QStyledItemDelegate::displayText(value, locale);
}

void LinkBackFilterDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  if (!index.isValid())
  {
    return;
  }
  QVariant qvar = index.model()->data(index, Qt::EditRole);
  if (qvar.type() == QVariant::StringList)
  {
    QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);
    QStringList qsl = qvar.toStringList();
    if (qsl.size() == 1)
    {
      comboBox->addItems(qsl);
      comboBox->setCurrentIndex(0);
    }
    else if (qsl.size() > 1)
    {
      QString desiredItemText = qsl.at(0);
      qsl.removeAt(0);
      comboBox->addItems(qsl);
      for (int i=0; i<comboBox->count(); ++i)
      {
        if (QString::compare(comboBox->itemText(i), desiredItemText) == 0)
        {
          qDebug(qPrintable(QString("Found item text %1 at %2").arg(desiredItemText).arg(i)));
          comboBox->setCurrentIndex(i);
          return;
        }
      }
      qDebug(qPrintable(QString("Did not find item text %1").arg(desiredItemText)));
      comboBox->setCurrentIndex(0);
    }
  }
//  else if (qvar.type() == QVariant::Bool)
//  {
//    QCheckBox* checkBox = dynamic_cast<QCheckBox*>(editor);
//    checkBox->setChecked(qvar.toBool());
//  }
  else if (QMetaType::Bool == (QMetaType::Type) qvar.type())
  {
    QCheckBox* checkBox = dynamic_cast<QCheckBox*>(editor);
    checkBox->setChecked(qvar.toBool());
  }
  else
  {
    QStyledItemDelegate::setEditorData(editor, index);

    /**
    if (QMetaType::Double == (QMetaType::Type) qvar.type()) {
      QDoubleSpinBox edit = nullptr;
    }
    qDebug(qPrintable(QString("Edit type is: %1").arg(qvar.typeName())));
    QString s = qvar.toString();
    qDebug(qPrintable(QString("s = (%1)").arg(s)));
    qDebug("1");
    QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
    if (editor != nullptr && lineEdit == nullptr) {
      qDebug(typeid(*editor).name());
    } else {
      lineEdit->setText(s);
    }
    **/
  }
}

void LinkBackFilterDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                          const QModelIndex &index) const
{
  if (!index.isValid())
  {
    return;
  }
  QVariant currentModelValue = index.model()->data(index, Qt::DisplayRole);
  if (dynamic_cast<QComboBox*>(editor) != nullptr)
  {
    QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);
    QString s = comboBox->currentText();
    if (QString::compare(s, currentModelValue.toString(), Qt::CaseSensitive) != 0)
    {
      model->setData(index, s, Qt::EditRole);
    }
  }
  else if (dynamic_cast<QLineEdit*>(editor) != nullptr)
  {
    QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
    QString s = lineEdit->displayText();
    if (QString::compare(s, currentModelValue.toString(), Qt::CaseSensitive) != 0)
    {
      //qDebug(qPrintable(QString("Setting data (%1) current model (%2) index %3").arg(s, currentModelValue.toString(), QString::number(index.column()))));
      model->setData(index, s, Qt::EditRole);
    }
  }
  else if (dynamic_cast<QCheckBox*>(editor) != nullptr)
  {
    QCheckBox* checkBox = dynamic_cast<QCheckBox*>(editor);
    if (currentModelValue.toBool() != checkBox->isChecked())
    {
      model->setData(index, checkBox->isChecked(), Qt::EditRole);
    }
    return;
  }
  else
  {
    QStyledItemDelegate::setModelData(editor, model, index);
  }
}
