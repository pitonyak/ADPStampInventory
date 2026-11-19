
#include "imageutility.h"
#include "constants.h"
#include "globals.h"

#include <QDebug>
#include <QPainter>
#include <QRect>
#include <QRegularExpression>
#include <QSettings>

QRegularExpression ImageUtility::m_catNumberRxNoCat("^(\\d+)(.*)$",
    QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);

QRegularExpression ImageUtility::m_catNumberRxWithCat("^([^0-9]*)(\\d+)(.*)$",
    QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);

ImageUtility::ImageUtility(QObject *parent)
    : QObject(parent), m_noFileFoundImage(nullptr) {
  // Build the default image when no file is found.
  m_noFileFoundImage = new QPixmap(400, 400);
  m_noFileFoundImage->fill(Qt::white);
  QPainter painter(m_noFileFoundImage);
  painter.setPen(Qt::black);
  painter.drawText(m_noFileFoundImage->rect(), Qt::AlignCenter,
                   "No Image Found");

  QScopedPointer<QSettings> pSettings(getQSettings());
  m_BaseDirectory =
      pSettings->value(Constants::Settings_CatalogImagePath).toString();
  m_MyBaseDirectory =
      pSettings->value(Constants::Settings_UserImagePath).toString();
}

ImageUtility::~ImageUtility() {
  delete m_noFileFoundImage;
  m_noFileFoundImage = nullptr;
}

QString ImageUtility::getStampPath(const QString &country,
                                   const QString &category,
                                   const QString &catNumber,
                                   const bool isGeneric) const {
  QString cat_path = getCategoryPath(country, category, isGeneric);

  //
  // The numeric portion is preceded by the category, case does not matter.
  // This regular expression can capture trailing letters but we do not bother.
  // Can probably speed up this regular expression by prestoring based on
  // category and not bothering with the letter.
  //
  QRegularExpression catNumberRx(
      "^" + category + "(\\d+)(.*)$",
      QRegularExpression::CaseInsensitiveOption |
          QRegularExpression::DotMatchesEverythingOption);

  QRegularExpressionMatchIterator i = catNumberRx.globalMatch(catNumber);
  QString leading_num_str;

  if (!i.hasNext()) {
    qCritical() << "Category (" << category
                << ") with catelog number does not have the correct format: "
                << catNumber;
    return "return_list";
  }

  QRegularExpressionMatch match = i.next();
  leading_num_str = match.captured(1);
  // QString letter = match.captured(2);
  //  Numbers are in sets grouped by 1000
  int thousands = leading_num_str.toInt() / 1000 * 1000;
  if (thousands == 0) {
    return cat_path + QDir::separator() + "0000";
  }
  return cat_path + QDir::separator() + QString::number(thousands);
}

bool ImageUtility::splitCatalogNumber(const QString &catalogNumber,
                                      QString &num, QString &trailer) {
  QRegularExpressionMatchIterator i =
      m_catNumberRxNoCat.globalMatch(catalogNumber);

  if (!i.hasNext()) {
    qCritical() << "Catalog Number (" << catalogNumber
                << ") does not have the correct format";
    num = "";
    trailer = "";
    return false;
  }

  QRegularExpressionMatch match = i.next();
  num = match.captured(1);
  trailer = match.captured(2);
  return true;
}

bool ImageUtility::splitCatalogNumber(const QString &catalogNumber,
                                      QString &category, QString &num,
                                      QString &trailer) {
  if (catalogNumber.startsWith("1CVPO", Qt::CaseInsensitive)) {
     category = "1CVPO";
     return splitCatalogNumber(catalogNumber.mid(5), num, trailer);
  }
  if (catalogNumber.startsWith("1CVP", Qt::CaseInsensitive)) {
    category = "1CVP";
    return splitCatalogNumber(catalogNumber.mid(4), num, trailer);
  }
  if (catalogNumber.startsWith("2CVP", Qt::CaseInsensitive)) {
    category = "2CVP";
    return splitCatalogNumber(catalogNumber.mid(4), num, trailer);
  }
  QRegularExpressionMatchIterator i =
      m_catNumberRxWithCat.globalMatch(catalogNumber);
  QString leading_num_str;

  if (!i.hasNext()) {
    qCritical() << "Category (" << catalogNumber
                << ") does not have the correct format";
    category = "";
    num = "";
    trailer = "";
    return false;
  }

  QRegularExpressionMatch match = i.next();
  category = match.captured(1);
  num = match.captured(2);
  trailer = match.captured(3);
  return true;
}

QList<StampImage> ImageUtility::getStampImages(const QString &country, const QString &catNumber,
                                 ImageUtility::Image_Flags flags,
                                 bool allowEmpty) const
{
    QStringList paths = getBookImagePaths(country, catNumber, flags);
    QList<QPixmap> images = getBookImages(getBookImagePaths(country, catNumber, flags), false);
    if (paths.isEmpty() && allowEmpty) {
        images.append(*m_noFileFoundImage);
        paths.append("empty.png");
    }
    QList<StampImage> returnList;
    for (int i=0; i<paths.size(); ++i)
    {
        returnList.append(StampImage(paths.at(i), images.at(i)));
    }
    return returnList;
}



QStringList
ImageUtility::getBookImagePaths(const QString &country, const QString &catNumber,
                             ImageUtility::Image_Flags flags) const {
  // Path looks something like:
  // base/country/category/<number>/catNumber<optional_stuff>
  // Country code is always uppercase!
  QStringList return_list;

  QString category;
  QString num;
  QString trailer;
  splitCatalogNumber(catNumber, category, num, trailer);

  //
  // Must begin with the cat number (case-insensitive).
  // May have a trailing '_' followed by random text.
  // must end with .png.
  // This allows 11A to find 11a.png.
  // when searching for 11, can find things such as '11_rose_red.png'.
  //
  QRegularExpression catNumberFileRx(
      "^" + catNumber + "(_.+|)\\.png$",
      QRegularExpression::CaseInsensitiveOption |
          QRegularExpression::DotMatchesEverythingOption);
  QRegularExpressionMatchIterator i;

  if ((flags & ImageUtility::Image_Flags::Stock) ==
      ImageUtility::Image_Flags::Stock) {
    QString catalog_stamp_path =
        getStampPath(country, category, catNumber, true);
    QDir catalog_stamp_dir(catalog_stamp_path);
    if (!catalog_stamp_dir.exists()) {
      qCritical() << "Path to the catalog image directory does not exist: "
                  << catalog_stamp_path;
      return return_list;
    }

    QStringList const files =
        catalog_stamp_dir.entryList(QStringList() << "*.png", QDir::Files);
    for (QString const &x : files) {
      i = catNumberFileRx.globalMatch(x);
      if (i.hasNext()) {
        qDebug() << "Found " << x << " matching " << catNumber;
        return_list << catalog_stamp_path + QDir::separator() + x;
      }
    }
  }

  if ((flags & ImageUtility::Image_Flags::Mine) ==
      ImageUtility::Image_Flags::Mine) {
    // repeat for personal images
    QString my_stamp_path = getStampPath(country, category, catNumber, false);
    QDir my_stamp_dir(my_stamp_path);
    if (!my_stamp_dir.exists()) {
      return return_list;
    }
    QStringList const myFiles =
        my_stamp_dir.entryList(QStringList() << "*.png", QDir::Files);
    for (QString const &x : myFiles) {
      i = catNumberFileRx.globalMatch(x);
      if (i.hasNext()) {
        qDebug() << "Found " << x << " matching " << catNumber;
        return_list << my_stamp_path + QDir::separator() + x;
      }
    }
  }

  return return_list;
}

QList<QPixmap> ImageUtility::getBookImages(const QStringList &paths,
                                           bool allowEmpty) const {
  QList<QPixmap> images;
  for (const QString &path : paths) {
    QFileInfo fileInfo(path);
    // This first check is probably redundant.
    if (fileInfo.exists() && fileInfo.isFile()) {
      QPixmap pix(path);
      if (!pix.isNull()) {
        images.append(pix);
      } else {
        qDebug() << "ImageUtility::getBookImages : Path exists, but image "
                    "failed to load : "
                 << path;
      }
    } else {
      qDebug() << "ImageUtility::getBookImages : Ignoring path : " << path;
    }
  }
  if (images.isEmpty() && !allowEmpty) {
    images.append(*m_noFileFoundImage);
  }
  return images;
}

void ImageUtility::setBaseDirectory(const QDir &x) {
  if (!x.exists()) {
    qCritical() << "In setBaseDirectory, path does not exist: " << x.path();
  }
  m_BaseDirectory = x;
}

bool ImageUtility::hasCountry(const QString &country) const {
  if (!m_BaseDirectory.exists()) {
    qCritical() << "Base image directory does not exist: "
                << m_BaseDirectory.path();
    return false;
  }
  if (!QDir(getCountryPath(country, true)).exists()) {
    qCritical() << "Base image country path does not exist: "
                << getCountryPath(country, true);
    return false;
  }
  return true;
}

bool ImageUtility::hasCategory(const QString &country,
                               const QString &category) const {
  if (!hasCountry(country)) {
    return false;
  }
  if (!QDir(getCategoryPath(country, category, true)).exists()) {
    qCritical() << "Base image category path does not exist: "
                << getCategoryPath(country, category, true);
    return false;
  }
  return true;
}
