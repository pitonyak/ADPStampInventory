#include "configuredialog.h"
#include "constants.h"
#include "linkbackfilterdelegate.h"
#include "checkboxonlydelegate.h"
#include "globals.h"

#include <QSettings>
#include <QLineEdit>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QScopedPointer>

ConfigureDialog::ConfigureDialog(QWidget *parent) :
  QDialog(parent), m_DBPath(nullptr), m_CatalogImagePath(nullptr), m_UserImagePath(nullptr)
{
  buildDialog();
}

ConfigureDialog::~ConfigureDialog()
{
  // Settings saved in the done() method
  // usually I save them here!
}

void ConfigureDialog::setLineEdit(QLineEdit *line_edit, const QString& txt)
{
    if (line_edit != nullptr)
    {
      line_edit->setText(txt);
    }
}

QString ConfigureDialog::getLineEdit(QLineEdit *line_edit) const
{
    return (line_edit != nullptr) ? line_edit->text() : "";
}


void ConfigureDialog::selectDir(QLineEdit* edit, const QString& header_txt)
{
    if (edit == nullptr) {
        return;
    }
    QString dir = QFileDialog::getExistingDirectory(this, header_txt, edit->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.length() > 0)
    {
        edit->setText(dir);
    }
}

void ConfigureDialog::selectFile(QLineEdit* edit, const QString& header_txt)
{
    if (edit == nullptr) {
        return;
    }

    QString fileExtension;
    QString s = QFileDialog::getOpenFileName(this, header_txt, edit->text(), tr("SQLite(*.sqlite);;All(*.*)"), &fileExtension, QFileDialog::DontUseNativeDialog);
    if (s.length() > 0)
    {
        edit->setText(s);
    }
}

void ConfigureDialog::selectDbFile() { selectFile(m_DBPath, "Select DB File"); }
void ConfigureDialog::selectCatalogImagesDir() { selectDir(m_CatalogImagePath, "Select Catalog Images Base Directory"); }
void ConfigureDialog::selectUserImagesDir() { selectDir(m_UserImagePath, "Select User Images Base Directory"); }

void ConfigureDialog::done(int r)
{
    QScopedPointer<QSettings> pSettings(getQSettings());
    pSettings->setValue(Constants::Settings_ConfigureDlgGeometry, saveGeometry());

    // Only save values if did not cancel
    if(QDialog::Accepted == r) {
        pSettings->setValue(Constants::Settings_DBPath, getDBPath());
        pSettings->setValue(Constants::Settings_CatalogImagePath, getCatalogImagePath());
        pSettings->setValue(Constants::Settings_UserImagePath, getUserImagePath());
    }
    QDialog::done(r);
    return;
}

void ConfigureDialog::buildDialog()
{
  setWindowTitle(tr("Configure"));
  QPushButton* button;
  QVBoxLayout *vLayout;
  QHBoxLayout *hLayout;
  QFormLayout *fLayout = new QFormLayout;

  m_DBPath = new QLineEdit();
  m_CatalogImagePath = new QLineEdit();
  m_UserImagePath = new QLineEdit();


  hLayout = new QHBoxLayout();

  vLayout = new QVBoxLayout();
  vLayout->addWidget(new QLabel(tr("DB Path: ")));
  hLayout->addLayout(vLayout);

  vLayout = new QVBoxLayout();
  vLayout->addWidget(m_DBPath);
  hLayout->addLayout(vLayout);

  button = new QPushButton(tr("Select"));
  connect(button, SIGNAL(clicked()), this, SLOT(selectDbFile()));
  hLayout->addWidget(button);

  fLayout->addRow(hLayout);

  // Next
  hLayout = new QHBoxLayout();

  vLayout = new QVBoxLayout();
  vLayout->addWidget(new QLabel(tr("Catalog Images: ")));
  hLayout->addLayout(vLayout);

  vLayout = new QVBoxLayout();
  vLayout->addWidget(m_CatalogImagePath);
  hLayout->addLayout(vLayout);

  button = new QPushButton(tr("Select"));
  connect(button, SIGNAL(clicked()), this, SLOT(selectCatalogImagesDir()));
  hLayout->addWidget(button);
  fLayout->addRow(hLayout);


  // Next
  hLayout = new QHBoxLayout();

  vLayout = new QVBoxLayout();
  vLayout->addWidget(new QLabel(tr("User Images: ")));
  hLayout->addLayout(vLayout);

  vLayout = new QVBoxLayout();
  vLayout->addWidget(m_UserImagePath);
  hLayout->addLayout(vLayout);

  button = new QPushButton(tr("Select"));
  connect(button, SIGNAL(clicked()), this, SLOT(selectUserImagesDir()));
  hLayout->addWidget(button);
  fLayout->addRow(hLayout);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  fLayout->addRow(buttonBox);

  setLayout(fLayout);

  QScopedPointer<QSettings> pSettings(getQSettings());
  restoreGeometry(pSettings->value(Constants::Settings_ConfigureDlgGeometry).toByteArray());
  setDBPath(pSettings->value(Constants::Settings_DBPath).toString());
  setCatalogImagePath(pSettings->value(Constants::Settings_CatalogImagePath).toString());
  setUserImagePath(pSettings->value(Constants::Settings_UserImagePath).toString());
}
