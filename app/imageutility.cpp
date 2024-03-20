
#include "imageutility.h"
#include <QDebug>
#include <QRegularExpression>


ImageUtility::ImageUtility(QObject *parent) : QObject(parent)
{
    // TODO: Add more such as initializing paths perhaps.
}

QStringList ImageUtility::findBookImages(const QString& country, const QString& category, const QString& catNumber) const
{
    // Path looks something like:
    // base/country/category/<number>/catNumber<optional_stuff>
    // Country code is always uppercase!
    QStringList return_list;

    if (!m_BaseDirectory.exists()) {
        qCritical() << "Base image directory does not exist: " << m_BaseDirectory.path();
        return return_list;
    }
    QString cat_to_use = category.toLower();
    if (cat_to_use.length() == 0) {
        cat_to_use = "number";
    }
    QDir cat_path(m_BaseDirectory.path() + QDir::separator() + country.toUpper() + QDir::separator() + cat_to_use);
    if (!cat_path.exists()) {
        qCritical() << "Directory path to stamp catalog image does not exist: " << cat_path.path();
        return return_list;
    }
    // catNumber should start with all numbers so extract the numeric portion.
    //QRegularExpression rx("^(\\d+)(.*?)_{0,1}(.*)$");
    QRegularExpression catNumberRx("^(\\d+)(.*)$");

    QRegularExpressionMatchIterator i = catNumberRx.globalMatch(catNumber);
    QString leading_num_str;
    QString letter;

    if (!i.hasNext()) {
        qCritical() << "Catelog number does not have the correct format: " << catNumber;
        return return_list;
    }
    QRegularExpressionMatch match = i.next();
    leading_num_str = match.captured(1);
    letter = match.captured(2);
    // Numbers are in sets grouped by 1000
    int thousands = leading_num_str.toInt() / 1000 * 1000;
    if (thousands == 0) {
        cat_path = QDir(cat_path.path() + QDir::separator() + "0000");
    } else {
        cat_path = QDir(cat_path.path() + QDir::separator() + QString::number(thousands));
    }

    if (!cat_path.exists()) {
        qCritical() << "Directory path to stamp catalog image does not exist: " << cat_path.path();
        return return_list;
    }

    QStringList const files = cat_path.entryList(QStringList() << "*.png", QDir::Files);
    //
    // Must begin with the cat number (case-insensitive).
    // May have a trailing '_' followed by random text.
    // must end with .png.
    // This allows 11A to find 11a.png.
    // when searching for 11, can find things such as '11_rose_red.png'.
    //
    QRegularExpression catNumberFileRx("^" + catNumber + "(_.+|)\\.png$", QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
    for (QString const& x : files) {
        i = catNumberFileRx.globalMatch(x);
        if (i.hasNext()) {
            qDebug() << "Found " << x << " matching " << catNumber;
            return_list << cat_path.path() + QDir::separator() + x;
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
