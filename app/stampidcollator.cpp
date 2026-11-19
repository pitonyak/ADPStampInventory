#include "stampidcollator.h"
#include "imageutility.h"

StampIdCollator::StampIdCollator() {
    m_collator.setNumericMode(true);
    m_collator.setCaseSensitivity(Qt::CaseInsensitive);
}

bool StampIdCollator::operator()(const QString &left, const QString &right) const {
    return stampIdLessThan(left, right);
}

bool StampIdCollator::stampIdLessThan(const QString &left, const QString &right) const {

    // Assume in the form
    // "country_A3/scott/type_name"
    // First check for the A3 country code.
    if (left == right) {
        return false;
    }
    if (left.size() < 5 || right.size() < 5 || left.at(3) != '/' || right.at(3) != '/' ) {
        return m_collator.compare(left, right) < 0;
    }
    const auto leftParts = left.split(u'/');
    const auto rightParts = right.split(u'/');
    if (leftParts.size() == 3 && rightParts.size() == 3) {
        if (leftParts[0] == rightParts[0]) {
            QString leftCategory;
            QString leftNum;
            QString leftTrailer;
            QString rightCategory;
            QString rightNum;
            QString rightTrailer;
            if (ImageUtility::splitCatalogNumber(leftParts[1], leftCategory, leftNum, leftTrailer) && ImageUtility::splitCatalogNumber(rightParts[1], rightCategory, rightNum, rightTrailer)) {
                // was able to split both as though they are catalog numbers!
                // Compare the leading portion.
                int cmp = leftCategory.compare(rightCategory);
                if (cmp == 0) {
                    // Leading portion is the same!
                    int leftInt = leftNum.toInt();
                    int rightInt = rightNum.toInt();
                    if (leftInt == rightInt) {
                        cmp = leftTrailer.compare(rightTrailer);
                        if (cmp == 0) {
                            return leftParts[2] < rightParts[2];
                        }
                        return cmp < 0;
                    }
                    return leftInt < rightInt;
                }
                return cmp < 0;
            }
            // Falls through to the collator because it failed to split these.
        } else {
            return leftParts[0] < rightParts[0];
        }
    }
    return m_collator.compare(left, right) < 0;
}