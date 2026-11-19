#include "genericdatacollectionstableproxy.h"
#include "searchoptions.h"

Q_DECLARE_LOGGING_CATEGORY(GenericDataCollectionsTableProxyCategory)
Q_LOGGING_CATEGORY(GenericDataCollectionsTableProxyCategory,
                   "andy.genericdatacollectionstableproxycategory")

GenericDataCollectionsTableProxy::GenericDataCollectionsTableProxy(
    QObject *parent)
    : QSortFilterProxyModel(parent) {}

bool GenericDataCollectionsTableProxy::lessThan(
  const QModelIndex &left, const QModelIndex &right) const {
  QVariant leftData = sourceModel()->data(left);
  QVariant rightData = sourceModel()->data(right);

  if (leftData.metaType().id() == QMetaType::QString) {
    QString leftString = leftData.toString();
    QString rightString = rightData.toString();
    // return specialStringLessThan(leftString, rightString);
    return m_collator.stampIdLessThan(leftString, rightString);
    //return m_collator.compare(leftString, rightString) < 0;
  } else {
    return QSortFilterProxyModel::lessThan(left, right);
  }
}

bool GenericDataCollectionsTableProxy::oneMatch(
    const QModelIndex &startIndex, const SearchOptions &options,
    const QRegularExpression *regexp) const {
  QString s = data(startIndex).toString();
  if (options.isMatchAsString()) {
    if (options.isContains()) {
      return s.contains(options.getFindValue(), options.getCaseSensitivity());
    }
    if (options.isStartsWith()) {
      return s.startsWith(options.getFindValue(), options.getCaseSensitivity());
    }
    if (options.isEndsWith()) {
      return s.endsWith(options.getFindValue(), options.getCaseSensitivity());
    }
    if (options.isMatchEntireString()) {
      return s.compare(options.getFindValue(), options.getCaseSensitivity()) ==
             0;
    }
  } else if (regexp != nullptr) {
    if (options.isContains()) {
      return s.contains(*regexp);
    }
    // TODO: Do I care abotu match entire string, starts with or ends with?
    return regexp->match(s).hasMatch();
  }
  qDebug("regular expression is null in "
         "GenericDataCollectionsTableProxy::oneMatch");
  return false;
}

QModelIndexList GenericDataCollectionsTableProxy::searchOneColumn(
    const QModelIndex &startIndex, const SearchOptions &options) {
  qDebug(GenericDataCollectionsTableProxyCategory)
      << "searchOneColumn startIndex (row,col) : (" << startIndex.row() << ", "
      << startIndex.column() << ")";
  QString sFindText = options.getFindValue();

  if (options.isReplace()) {
    qDebug("Replace is not currently supported");
    return QModelIndexList();
  }

  // Although match supports wild-cards and regular expressions,
  // it seems to ONLY work for an entire match as opposed to contains.
  if (!options.isBackwards() && options.isMatchAsString()) {
    Qt::MatchFlags matchFlags = options.getMatchFlags();
    if (options.isAllColumns()) {
      // If searching everything, then just wrap!
      matchFlags |= Qt::MatchWrap;
    }
    return match(startIndex, Qt::DisplayRole, sFindText, 1, matchFlags);
  }
  QModelIndexList list;

  QRegularExpression regexp;
  if (options.isRegularExpression() || options.isWildCard()) {
    regexp = options.getRegularExpression();
    if (!regexp.isValid()) {
      qDebug("Failed to create a valid regular expression");
      return list;
    }
  }

  int startCol = startIndex.column();
  int startRow = startIndex.row();
  int numRows = rowCount();

  if (options.isBackwards()) {
    for (int row = startRow - 1; row >= 0; --row) {
      // Check one cell
      QModelIndex index = this->index(row, startCol, QModelIndex());
      if (oneMatch(index, options, &regexp)) {
        list.append(index);
        return list;
      }
    }
    for (int row = numRows - 1; row <= startRow; --row) {
      // Check one cell
      QModelIndex index = this->index(row, startCol, QModelIndex());
      if (oneMatch(index, options, &regexp)) {
        list.append(index);
        return list;
      }
    }
  } else {
    for (int row = startRow + 1; row < numRows; ++row) {
      // Check one cell
      QModelIndex index = this->index(row, startCol, QModelIndex());
      if (oneMatch(index, options, &regexp)) {
        list.append(index);
        return list;
      }
    }
    for (int row = 0; row <= startRow; ++row) {
      // Check one cell
      QModelIndex index = this->index(row, startCol, QModelIndex());
      if (oneMatch(index, options, &regexp)) {
        list.append(index);
        return list;
      }
    }
  }

  return list;
}

QModelIndexList
GenericDataCollectionsTableProxy::search(const QModelIndex &startIndex,
                                         const SearchOptions &options) {
  if (!options.isAllColumns()) {
    return searchOneColumn(startIndex, options);
  }
  // For each column, search.
  QList<int> columnList;
  int numCols = columnCount();
  int startCol = startIndex.column();
  // int startRow = startIndex.row();
  for (int i = startCol; i < numCols; ++i) {
    columnList << i;
  }

  for (int i = 0; i < startCol; ++i) {
    columnList << i;
  }

  QModelIndexList list;
  for (int i = 0; i < columnList.count(); ++i) {
    // qDebug(GenericDataCollectionsTableProxyCategory) << QString("Index %1
    // row: %2  col: %3").arg(i, startRow, columnList.at(i));

    QModelIndex index =
        sourceModel()->index(startIndex.row(), columnList.at(i), QModelIndex());
    // qDebug(GenericDataCollectionsTableProxyCategory) << QString("Search index
    // row: %1  col: %2").arg(index.row(), index.column());
    list.append(searchOneColumn(index, options));
    if (list.count() > 0 && !options.isReplaceAll()) {
      return list;
    }
  }
  // qDebug(GenericDataCollectionsTableProxyCategory) << QString("Cols: %1 Rows:
  // %2").arg(columnCount(), rowCount());
  return list;
}

QModelIndex GenericDataCollectionsTableProxy::getIndexByRowCol(int row,
                                                               int col) const {
  if (col < 0) {
    col = 0;
  }
  if (col >= columnCount()) {
    col = columnCount() - 1;
  }
  if (row < 0) {
    row = 0;
  }
  if (row >= rowCount()) {
    row = rowCount() - 1;
  }
  return (col >= 0 && row >= 0) ? index(row, col) : QModelIndex();
}
