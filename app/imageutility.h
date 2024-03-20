#ifndef IMAGEUTILITY_H
#define IMAGEUTILITY_H

#include <QObject>
#include <QString>

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

signals:

public slots:

private:

    /*! \brief Where to start when trying to find an image. */
    QString m_BaseDirectory;

};


#endif // IMAGEUTILITY_H
