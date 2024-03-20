#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <QDialog>
#include <QItemSelection>

#include "tablesortfieldtablemodel.h"
#include "genericdatacollection.h"
#include "typemapper.h"

class QLineEdit;

//**************************************************************************
/*! \class ConfigureDialog
 *
 * \brief Edit the base paths for the database.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2024
 ***************************************************************************/
class ConfigureDialog : public QDialog
{
  Q_OBJECT
public:
  //**************************************************************************
  //! Default Constructor
  /*!
   * \param [in] parent This is a QObject, so you can set a parent that will control the lifetime of this object.
   ***************************************************************************/
  explicit ConfigureDialog(QWidget *parent = nullptr);

  /*! \brief Destructor saves dialog geometry. */
  virtual ~ConfigureDialog();

  void setDBPath(const QString& path) { setLineEdit(m_DBPath, path); }
  QString getDBPath() const { return getLineEdit(m_DBPath); }

  void setCatalogImagePath(const QString& path) { setLineEdit(m_CatalogImagePath, path); }
  QString getCatalogImagePath() const { return getLineEdit(m_CatalogImagePath); }

  void setUserImagePath(const QString& path) { setLineEdit(m_UserImagePath, path); }
  QString getUserImagePath() const { return getLineEdit(m_UserImagePath); }

  void setLineEdit(QLineEdit *line_edit, const QString& txt);
  QString getLineEdit(QLineEdit *line_edit) const;

signals:

public slots:
  void done(int r);                // Save settings
  void selectDbFile();
  void selectCatalogImagesDir();
  void selectUserImagesDir();

private:
  /*! \brief Set list columns, delegates, and initial values. */
  void buildDialog();

  void selectDir(QLineEdit* edit, const QString& header_txt);
  void selectFile(QLineEdit* edit, const QString& header_txt);

  QLineEdit* m_DBPath;
  QLineEdit* m_CatalogImagePath;
  QLineEdit* m_UserImagePath;
};

#endif // CONFIGUREDIALOG_H
