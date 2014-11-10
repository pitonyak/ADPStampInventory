#include "genericdatacollectionstableproxy.h"

GenericDataCollectionsTableProxy::GenericDataCollectionsTableProxy(QObject *parent) :
  QSortFilterProxyModel(parent)
{
}

void GenericDataCollectionsTableProxy::setSortCaseSensitivity(Qt::CaseSensitivity cs)
{
  m_collator.setCaseSensitivity(cs);
  QSortFilterProxyModel::setSortCaseSensitivity(cs);
}

void 	GenericDataCollectionsTableProxy::setIgnorePunctuation(bool on)
{
  m_collator.setIgnorePunctuation(on);
}

void 	GenericDataCollectionsTableProxy::setLocale(const QLocale & locale)
{
  m_collator.setLocale(locale);
}

void 	GenericDataCollectionsTableProxy::setNumericMode(bool on)
{
  m_collator.setNumericMode(on);
}

bool GenericDataCollectionsTableProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
  QVariant leftData = sourceModel()->data(left);
  QVariant rightData = sourceModel()->data(right);

  if (leftData.type() == QVariant::String)
  {
    QString leftString = leftData.toString();
    QString rightString = rightData.toString();
    //return specialStringLessThan(leftString, rightString);
    return m_collator.compare(leftString, rightString) < 0;
  }
  else
  {
    return QSortFilterProxyModel::lessThan(left, right);
  }
}


bool GenericDataCollectionsTableProxy::specialStringLessThan(const QString &left, const QString &right) const
{
    // The default compare is at least 5 times faster than mine!
    //if (left.compare(right, sortCaseSensitivity) == 0)
    //{
    //    return false;
    //}

    QString::ConstIterator iLeft = left.constBegin();
    QString::ConstIterator iRight = right.constBegin();

    QString tempLeft;
    QString tempRight;

    // Skip leading text that is similar. In my case, this is many cases.
    while (iLeft != left.constEnd() && iRight != right.constEnd() && *iLeft == *iRight && !iLeft->isDigit())
    {
        ++iLeft;
        ++iRight;
    }

    while (iLeft != left.constEnd() && iRight != right.constEnd())
    {
        if (iLeft->isDigit() && iRight->isDigit())
        {
            tempLeft = *iLeft;
            tempRight = *iRight;
            ++iLeft;
            ++iRight;
            while (iLeft != left.constEnd() && iLeft->isDigit()) {
                tempLeft.append(*iLeft);
                ++iLeft;
            }
            while (iRight != right.constEnd() && iRight->isDigit()) {
                tempRight.append(*iRight);
                ++iRight;
            }
            // Compare the strings by making them the same length by prepending zeros.
            if (tempLeft.size() < tempRight.size())
            {
                tempLeft.prepend(QString(tempRight.size() - tempLeft.size(), '0'));
            }
            else if (tempLeft.size() > tempRight.size())
            {
                tempRight.prepend(QString(tempLeft.size() - tempRight.size(), '0'));
            }
            int rc = isSortLocaleAware() ? tempLeft.localeAwareCompare(tempRight) : tempLeft.compare(tempRight, sortCaseSensitivity());
            if (rc != 0)
            {
                return (rc < 0);
            }
        }
        else
        {
            tempLeft = *iLeft;
            tempRight = *iRight;
            ++iLeft;
            ++iRight;
            while (iLeft != left.constEnd() && iRight != right.constEnd() && (!iLeft->isDigit() || !iRight->isDigit()))
            {
                tempLeft.append(*iLeft);
                ++iLeft;
                tempRight.append(*iRight);
                ++iRight;
            }
            int rc = isSortLocaleAware() ? tempLeft.localeAwareCompare(tempRight) : tempLeft.compare(tempRight, sortCaseSensitivity());
            if (rc != 0) {
                return (rc < 0);
            }
        }
    }
    if (iRight != right.constEnd()) {
        // The left ended sooner
        return true;
    }

    return false;
}
