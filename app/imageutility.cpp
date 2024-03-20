
#include "imageutility.h"
#include <QDebug>
#include <QRegularExpression>


ImageUtility::ImageUtility(QObject *parent) : QObject(parent)
{
    // TODO: Add more such as initializing paths perhaps.
}

QString ImageUtility::getStampPath(const QString& country, const QString& category, const QString& catNumber) const
{
    QString cat_path = getCategoryPath(country, category);

    //
    // The numeric portion is preceded by the category, case does not matter.
    // This regular expression can capture trailing letters but we do not bother.
    // Can probably speed up this regular expression by prestoring based on category and
    // not bothering with the letter.
    //
    QRegularExpression catNumberRx("^" + category+ "(\\d+)(.*)$", QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator i = catNumberRx.globalMatch(catNumber);
    QString leading_num_str;
    //QString letter;

    if (!i.hasNext()) {
        qCritical() << "Category (" << category << ") with catelog number does not have the correct format: " << catNumber;
        return "return_list";
    }

    QRegularExpressionMatch match = i.next();
    leading_num_str = match.captured(1);
    //letter = match.captured(2);
    // Numbers are in sets grouped by 1000
    int thousands = leading_num_str.toInt() / 1000 * 1000;
    if (thousands == 0) {
        return cat_path + QDir::separator() + "0000";
    }
    return cat_path + QDir::separator() + QString::number(thousands);
}

QStringList ImageUtility::findBookImages(const QString& country, const QString& category, const QString& catNumber) const
{
    // Path looks something like:
    // base/country/category/<number>/catNumber<optional_stuff>
    // Country code is always uppercase!
    QStringList return_list;

    QString stamp_path = getStampPath(country, category, catNumber);
    QDir stamp_dir(stamp_path);
    if (!stamp_dir.exists()) {
        qCritical() << "Path to image directory does not exist: " << stamp_path;
        return return_list;
    }

    QStringList const files = stamp_dir.entryList(QStringList() << "*.png", QDir::Files);
    //
    // Must begin with the cat number (case-insensitive).
    // May have a trailing '_' followed by random text.
    // must end with .png.
    // This allows 11A to find 11a.png.
    // when searching for 11, can find things such as '11_rose_red.png'.
    //
    QRegularExpression catNumberFileRx("^" + catNumber + "(_.+|)\\.png$", QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator i;
    for (QString const& x : files) {
        i = catNumberFileRx.globalMatch(x);
        if (i.hasNext()) {
            qDebug() << "Found " << x << " matching " << catNumber;
            return_list << stamp_path + QDir::separator() + x;
        }
    }

    return return_list;
}

void ImageUtility::setBaseDirectory(const QDir& x)
{
    if (!x.exists()) {
        qCritical() << "Base image directory does not exist: " << x.path();
    }
    m_BaseDirectory = x;
}

bool ImageUtility::hasCountry(const QString& country) const
{
    if (!m_BaseDirectory.exists()) {
        qCritical() << "Base image directory does not exist: " << m_BaseDirectory.path();
        return false;
    }
    if (!QDir(getCountryPath(country)).exists()) {
        qCritical() << "Base image country path does not exist: " << getCountryPath(country);
        return false;
    }
    return true;
}

bool ImageUtility::hasCategory(const QString& country, const QString& category) const
{
    if (!hasCountry(country)) {
        return false;
    }
    if (!QDir(getCategoryPath(country, category)).exists()) {
        qCritical() << "Base image category path does not exist: " << getCategoryPath(country, category);
        return false;
    }
    return true;
}

