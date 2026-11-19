#ifndef GENERICDATACOLLECTIONSTABLEPROXY_H
#define GENERICDATACOLLECTIONSTABLEPROXY_H

#include "stampidcollator.h"

#include <QCollator>
#include <QLoggingCategory>
#include <QSortFilterProxyModel>

//**************************************************************************
/*! \class GenericDataCollectionsTableProxy
 * \brief Allow for sorting strings in "natural" order.
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2014 - 2020
 ***************************************************************************/

Q_DECLARE_LOGGING_CATEGORY(GenericDataCollectionsTableProxyCategory)

class SearchOptions;
class GenericDataCollectionsTableProxy : public QSortFilterProxyModel {
  Q_OBJECT
public:
  explicit GenericDataCollectionsTableProxy(QObject *parent = nullptr);

  /*! \brief Wrapper to allow searching all columns.
   *
   * If all columns are to be searched, it looks until it finds the first
   * instance.
   *
   *  \param [in] startIndex Where to start searching.
   *
   *  \param [in] options tell everything else about the search.
   */
  QModelIndexList search(const QModelIndex &startIndex,
                         const SearchOptions &options);

  /*! \brief Search one (or all) column(s), depending on the options.
   *
   * The standard search allows for the general searching, but it only supports
   * searching forward. This code will hopefully allow for "replace", and it
   * implements "search backwards".
   *
   *  \param [in] startIndex Where to start searching.
   *  \param [in] options tell everything else about the search.
   */
  QModelIndexList searchOneColumn(const QModelIndex &startIndex,
                                  const SearchOptions &options);

  bool oneMatch(const QModelIndex &startIndex, const SearchOptions &options,
                const QRegularExpression *regexp = nullptr) const;

  /*! \brief Wraps getting the index to validate that the row and col are in bounds.
   *
   * The standard search allows for the general searching, but it only supports
   * searching forward. This code will hopefully allow for "replace", and it
   * implements "search backwards".
   *
   *  \param [in] row The row of interest.
   *  \param [in] col The column of interest.
   *
   *  \returns an index that is valid even if row or col is out of bounds.
   */
  QModelIndex getIndexByRowCol(int row, int col) const;

signals:

public slots:

protected:
  // bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent)
  // const;
  bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

  /*! \brief One problem with the QCollator is that it fails  when the leading values are 1CVP because of the leading 1. */
  //QCollator m_collator;
  StampIdCollator m_collator;
};

#endif // GENERICDATACOLLECTIONSTABLEPROXY_H
