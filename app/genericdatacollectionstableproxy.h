#ifndef GENERICDATACOLLECTIONSTABLEPROXY_H
#define GENERICDATACOLLECTIONSTABLEPROXY_H

#include <QSortFilterProxyModel>
#include <QCollator>
#include <QLoggingCategory>

//**************************************************************************
/*! \class GenericDataCollectionsTableProxy
 * \brief Allow for sorting strings in "natural" order.
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2014 - 2020
 ***************************************************************************/

Q_DECLARE_LOGGING_CATEGORY(GenericDataCollectionsTableProxyCategory)

class SearchOptions;
class GenericDataCollectionsTableProxy : public QSortFilterProxyModel
{
  Q_OBJECT
public:
  explicit GenericDataCollectionsTableProxy(QObject *parent = nullptr);

  /*! \brief Set case sensitivity on the collator and then on the base class.
   *
   *  Defaults to case sensitive.
   *
   *  \param [in] cs Desired case sensitivity.
   */
  void 	setSortCaseSensitivity(Qt::CaseSensitivity cs);

  /*! \brief If on is set to true, punctuation characters and symbols are ignored when determining sort order.
   *
   * Default is locale dependent.
   *
   *  \param [in] on Desired state of ignore punctuation.
   */
  void 	setIgnorePunctuation(bool on);

  /*! \brief Set the collator locale.
   *
   *  \param [in] locale
   */
  void 	setLocale(const QLocale & locale);

  /*! \brief Enables numeric sorting mode (use natural order) when on is set to true.
   *
   * Enables proper sorting of numeric digits, so that 100 sorts after 99.
   * By default this mode is off.
   * Note: On Windows, this functionality makes use of the ICU library.
   * If Qt was compiled without ICU support, it falls back to code using native Windows API,
   * which only works from Windows 7 onwards. On older versions of Windows,
   * it will not work and a warning will be emitted at runtime.
   *
   *  \param [in]
   */
  void 	setNumericMode(bool on);

  /*! \brief Wrapper to allow searching all columns.
   *
   * If all columns are to be searched, it looks until it finds the first instance.
   *
   *  \param [in] startIndex Where to start searching.
   *
   *  \param [in] options tell everything else about the search.
   */
  QModelIndexList search(const QModelIndex& startIndex, const SearchOptions& options);

  /*! \brief Search one (or all) column(s), depending on the options.
   *
   * The standard search allows for the general searching, but it only supports searching forward.
   * This code will hopefully allow for "replace",
   * and it implements "search backwards".
   *
   *  \param [in] startIndex Where to start searching.
   *
   *  \param [in] options tell everything else about the search.
   */
  QModelIndexList searchOneColumn(const QModelIndex& startIndex, const SearchOptions& options);

  bool oneMatch(const QModelIndex& startIndex, const SearchOptions& options, const QRegularExpression* regexp = nullptr) const;

  QModelIndex getIndexByRowCol(int row, int col) const;


signals:

public slots:

protected:
  //bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
  bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

  /*! \brief Recognizes integers inside of strings so that they are sorted properly.
   *
   * A number in a string sorts incorrectly. It is even worse if the strings are similar to:
   * RW2 and RW10. This sort routine recognizes numbers and then sorts them as integers.
   * This will still fail on floating point numbers. If we have "1.23" and "1.101", it will
   * compare 1 to 1 and then 23 to 101. This will incorrectly claim that 1.23 < 1.101.
   * This is not a use case for me, so I don't care.
   *
   *  \param [in] left
   *  \param [in] right
   *  \return Equivalent meta type if known, or QMetaType::Void otherwise.
   */
  bool specialStringLessThan(const QString &left, const QString &right) const;

  QCollator m_collator;

};

#endif // GENERICDATACOLLECTIONSTABLEPROXY_H
