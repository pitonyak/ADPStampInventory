#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include "catalogentry.h"
#include "stampimage.h"

#include <QDialog>
#include <QPixmap>

class QLabel;
class QPushButton;
class ImageUtility;
class QComboBox;
class StampDB;

//**************************************************************************
/*! \class ImageDialog
 *  \brief Display images based on Catalog Number
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2026
 *
 **************************************************************************/

class ImageDialog : public QDialog {
  Q_OBJECT
public:
  /*! \brief Constructor
   *
   *  \param [in, out] StampDB Pointer to the database object (used to get the
   * stamp Catalog).
   *  \param [in, out] parent Object owner.
   */
  explicit ImageDialog(StampDB *db, QWidget *parent = nullptr);

  /*! \brief Constructor that loads a specific image.
   *
   *  \param [in, out] StampDB Pointer to the database object (used to get the
   * stamp Catalog).
   *  \param [in] country Three character country abreviation, must mach what is
   * in the DB.
   *  \param [in] catNumber Full catalog number.
   *  \param [in, out] parent Object owner.
   */
  explicit ImageDialog(StampDB *db, const QString &country,
                       const QString &catNumber, QWidget *parent = nullptr);

  /*! \brief Set the image, the combo box, stamp description, and dialog title.
   *
   *  \param [in] country Three character country abreviation, must mach what is
   * in the DB.
   *  \param [in] catNumber Full catalog number.
   */
  void setBookImages(const QString &country, const QString &catNumber);

signals:

private slots:
  void prevImagePressed();
  void nextImagePressed();
  void prevCatPressed();
  void nextCatPressed();
  void onIndexChanged(int index); // When the ComboBox index changes (user chose
                                  // a different stamp from the drop down).

private:
  void buildDialog();
  void enableButtons();

  /*! \brief Each stamp may have multiple images. This selects a specific image.
   */
  void setImage(int i);

  /*! \brief Set the "stamp" based on an index into the Catalog entries. This
   * will reset the image and entire dialog.*/
  void setSelectedCatalogId(int selectedCatalogId);

  /*! \brief Database reference to get the catalog entries. */
  StampDB *m_db;

  /*! \brief Index into both the catalog and the combo box */
  int m_SelectedCatalogId;

  /*! \brief This label provides summary information about the stamp. */
  QLabel *m_SummaryText;

  /*! \brief ComboBox used to select a different catalog (Scott) number. */
  QComboBox *m_CatalogComboBox;

  /*! \brief Image is displayed in this. */
  QLabel *m_ImageLabel;
  QPushButton *m_PrevImage;
  QPushButton *m_NextImage;
  QPushButton *m_PrevCat;
  QPushButton *m_NextCat;

  /*! \brief Index into the array of images available to display */
  int m_CurrentImageIndex;

  /*! \brief Stock images are first and this tells me how many there are. */
  int m_NumStockImages;

  /*! \brief Personal images are next, this tells me how many there are. */
  int m_NumPersonalImages;

  /*! \brief This contains utilities for splitting a catalog number into its
   * component parts. */
  ImageUtility *m_ImageUtility;

  /*! \brief Current catalog number */
  QString m_CurrentScott;

  /*! \brief List of images for the current stamp. */
  QList<StampImage> m_StampImageList;

  /*! \brief ALL entries from the catalog table. */
  QList<QSharedPointer<CatalogEntry>> m_CatalogEntries;
};

#endif // IMAGEDIALOG_H
