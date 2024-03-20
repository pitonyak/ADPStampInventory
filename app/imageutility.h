#ifndef IMAGEUTILITY_H
#define IMAGEUTILITY_H

#include <QObject>
#include <QString>
#include <QList>
#include <QFile>
#include <QDir>

//**************************************************************************
/*! \class ImageUtility
 *  \brief Basic functions for finding images
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2023-2024
 *
 **************************************************************************/


class ImageUtility : public QObject
{
  Q_OBJECT

public:

  /*! \brief Constructor
   *
   *  \param [in, out] parent Object owner.
   *  \return
   */
    explicit ImageUtility(QObject *parent = 0);

    QStringList findBookImages(const QString& country, const QString& category, const QString& catNumber) const;


    void setBaseDirectory(const QString& x) { setBaseDirectory(QDir(x)); }
    void setBaseDirectory(const QDir& x);

    QDir getBaseDirectory() { return m_BaseDirectory; }
    const QDir& getBaseDirectory() const { return m_BaseDirectory; }
signals:

public slots:

private:

    /*! \brief Where to start when trying to find an image. */
    QDir m_BaseDirectory;

};

#endif // IMAGEUTILITY_H
