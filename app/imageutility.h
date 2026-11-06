#ifndef IMAGEUTILITY_H
#define IMAGEUTILITY_H

#include "stampimage.h"

#include <QDir>
#include <QFile>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QRegularExpression>
#include <QString>

//**************************************************************************
/*! \class ImageUtility
 *  \brief Basic functions for finding images
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2023-2026
 *
 **************************************************************************/

class ImageUtility : public QObject {
  Q_OBJECT

public:
  enum class Image_Flags : unsigned char {
    None = 0,
    Stock = 1 << 0,
    Mine = 1 << 1,
    Both = 3,
  };

  /*! \brief Constructor
   *
   *  \param [in, out] parent Object owner.
   */
  explicit ImageUtility(QObject *parent = 0);

  /*! \brief Destructor to delete the m_noFileFoundImage object. */
  ~ImageUtility();

  /*! \brief Get list of paths to images associated with a specific stamp (Scott
   * Number)
   *
   *  \param [in] country is probably the three letter country code
   *  \param [in] catNumber is the stamp number (probably Scott number)
   *  \param [in] flags Indicate if we want images for Stock, Owned stamps, or
   * both.
   *  \return List of paths to images for the specified stamp.
   */
  QStringList getBookImagePaths(const QString &country, const QString &catNumber,
                             ImageUtility::Image_Flags flags) const;

  /*! \brief Get list of images associated with a specific stamp (Scott Number)
   *
   *  QPixmap has implicit data sharing (shallow copying/copy-on-write), so
   *  storing them in containers is highly efficient and lightweight so images
   *  are returned and stored directly
   *
   *  \param [in] country is probably the three letter country code
   *  \param [in] catNumber is the stamp number (probably Scott number)
   *  \param [in] allowEmpty if false the default empty image is returned if no
   * images are present.
   *  \param [in] flags Indicate if we want images for Stock, Owned stamps, or
   * both.
   *  \return List of images for the specified stamp.
   */
  QList<QPixmap> getBookImages(const QString &country, const QString &catNumber,
                               ImageUtility::Image_Flags flags,
                               bool allowEmpty = false) const;

  /*! \brief Get list of images associated the provided paths.
   *
   *  Paths are assumed to exist. A missing path causes the image to be skipped.
   *
   *  QPixmap has implicit data sharing (shallow copying/copy-on-write), so
   *  storing them in containers is highly efficient and lightweight so images
   *  are returned and stored directly
   *
   *  \param [in] paths is a list of paths to images.
   *  \param [in] allowEmpty if false the default empty image is returned if no
   * images are present.
   *  \return List of images for the specified stamp.
   */
  QList<QPixmap> getBookImages(const QStringList &paths,
                               bool allowEmpty = false) const;

  QList<StampImage> getStampImages(const QString &country, const QString &catNumber,
                             ImageUtility::Image_Flags flags,
                                   bool allowEmpty = false) const;

  const QPixmap &getEmptyImage() const { return *m_noFileFoundImage; }

  bool splitCatalogNumber(const QString &catalogNumber, QString &num,
                          QString &trailer) const;
  bool splitCatalogNumber(const QString &catalogNumber, QString &category,
                          QString &num, QString &trailer) const;

  void setBaseDirectory(const QString &x) { setBaseDirectory(QDir(x)); }
  void setBaseDirectory(const QDir &x);

  QDir getBaseDirectory() { return m_BaseDirectory; }
  const QDir &getBaseDirectory() const { return m_BaseDirectory; }

  void setMyBaseDirectory(const QString &x) { setMyBaseDirectory(QDir(x)); }
  void setMyBaseDirectory(const QDir &x);

  QDir getMyBaseDirectory() { return m_MyBaseDirectory; }
  const QDir &getMyBaseDirectory() const { return m_MyBaseDirectory; }

  bool hasCountry(const QString &country) const;
  bool hasCategory(const QString &country, const QString &category) const;

  /*! \brief Path to where images are stored for a specific country
   *
   *  \param [in] country is probably the three letter country code
   *  \param [in] isGeneric if true return path to generic stamp images,
   * otherwise return my own images.
   *  \return base path plus the country in uppercase.
   */
  QString getCountryPath(const QString &country, const bool isGeneric) const;

  /*! \brief Path to where images are stored for a specific country and category
   *
   *  If category is empty, "number" is used instead.
   *
   *  \param [in] country is probably the three letter country code
   *  \param [in] category is the stamp category such as C for airmail or RW for
   * federal duck stamps.
   *  \param [in] isGeneric if true return path to generic stamp images,
   * otherwise return my own images.
   *  \return country path + category in lower case.
   */
  QString getCategoryPath(const QString &country, const QString &category,
                          const bool isGeneric) const;

  /*! \brief Path to where images are stored for a specific stamp
   *
   *  Start with the base path. Add the country in upper case.
   *  Add the category in lower case.
   *  Extract the numeric portion so for C23a this is 23.
   *  The numeric portion is broken into a four digit number counting by 1000
   * so, 0000, 1000, 2000, 3000, ... So, c23a is stored in "base/USA/c/0000"
   *
   *  \param [in] country is probably the three letter country code
   *  \param [in] category is the stamp category such as C for airmail or RW for
   * federal duck stamps.
   *  \param [in] catNumber is the catalog number.
   *  \param [in] isGeneric if true return path to generic stamp images,
   * otherwise return my own images.
   *
   *  \return country path + category in lower case.
   */
  QString getStampPath(const QString &country, const QString &category,
                       const QString &catNumber, const bool isGeneric) const;
signals:

public slots:

private:
  /*! \brief The default "No Image" bit map.*/
  QPixmap *m_noFileFoundImage;

  /*! \brief Matches a catalog number that starts with a number, so, something
   * like 1000 or 1000a, will not match RW42a */
  QRegularExpression m_catNumberRxNoCat;

  /*! \brief Matches a catalog number that MAY start with a letter such as
   * RW42a. Will also match 1000 and 1000a */
  QRegularExpression m_catNumberRxWithCat;

  /*! \brief Where to start when trying to find an image. */
  QDir m_BaseDirectory;
  /*! \brief Where to start when trying to find an image for my personal stamps.
   */
  QDir m_MyBaseDirectory;
};

inline QString ImageUtility::getCountryPath(const QString &country,
                                            const bool isGeneric) const {
  return (isGeneric ? m_BaseDirectory.path() : m_MyBaseDirectory.path()) +
         QDir::separator() + country.toUpper();
}

inline QString ImageUtility::getCategoryPath(const QString &country,
                                             const QString &category,
                                             const bool isGeneric) const {
  return getCountryPath(country, isGeneric) + QDir::separator() +
         (category.isEmpty() ? "number" : category.toLower());
}

inline QList<QPixmap>
ImageUtility::getBookImages(const QString &country, const QString &catNumber,
                            ImageUtility::Image_Flags flags,
                            bool allowEmpty) const {
  return getBookImages(getBookImagePaths(country, catNumber, flags), allowEmpty);
}

// Overload bitwise operators for ImageUtility::Image_Flags
inline constexpr ImageUtility::Image_Flags
operator|(ImageUtility::Image_Flags lhs, ImageUtility::Image_Flags rhs) {
  using Underlying = std::underlying_type_t<ImageUtility::Image_Flags>;
  return static_cast<ImageUtility::Image_Flags>(static_cast<Underlying>(lhs) |
                                                static_cast<Underlying>(rhs));
}

inline constexpr ImageUtility::Image_Flags
operator&(ImageUtility::Image_Flags lhs, ImageUtility::Image_Flags rhs) {
  using Underlying = std::underlying_type_t<ImageUtility::Image_Flags>;
  return static_cast<ImageUtility::Image_Flags>(static_cast<Underlying>(lhs) &
                                                static_cast<Underlying>(rhs));
}

inline constexpr ImageUtility::Image_Flags
operator~(ImageUtility::Image_Flags flag) {
  using Underlying = std::underlying_type_t<ImageUtility::Image_Flags>;
  return static_cast<ImageUtility::Image_Flags>(~static_cast<Underlying>(flag));
}

#endif // IMAGEUTILITY_H
