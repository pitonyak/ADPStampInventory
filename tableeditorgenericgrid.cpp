#include "tableeditorgenericgrid.h"

#include "constants.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QMap>
#include <QTextEdit>
#include <QRadioButton>
#include <QGridLayout>
#include <QButtonGroup>
#include <QLineEdit>
#include <QTableWidget>
#include <QSettings>
#include "csvreader.h"
#include "csvline.h"


TableEditorGenericGrid::TableEditorGenericGrid(QWidget *parent) :
  QDialog(parent)
{
}
