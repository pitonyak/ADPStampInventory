#include "scrollmessagebox.h"

#include <QScrollArea>
#include <QLabel>
#include <QStyle>
#include <QGridLayout>
#include <QList>
#include <QPushButton>
#include <QAbstractButton>
#include <QApplication>
#include <QDesktopWidget>
#include <QStringList>


ScrollMessageBox::ScrollMessageBox(QWidget * parent) : QDialog(parent)
{
}

ScrollMessageBox::ScrollMessageBox(QMessageBox::Icon icon,
                                   const QString &title,
                                   const QString &text,
                                   QDialogButtonBox::StandardButtons buttons,
                                   QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint)

{
  QLabel *iconLabel;
  QScrollArea *scroll;

  label = new QLabel();
  label->setTextInteractionFlags(Qt::TextInteractionFlags(style()->styleHint(QStyle::SH_MessageBox_TextInteractionFlags, 0, this)));
  label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
  label->setOpenExternalLinks(true);
  label->setContentsMargins(2, 0, 0, 0);
  label->setIndent(9);

  scroll = new QScrollArea(this);
  scroll->setGeometry(QRect(10, 20, 560, 430));
  scroll->setWidget(label);
  scroll->setWidgetResizable(true);

  iconLabel = new QLabel;
  iconLabel->setPixmap(standardIcon((QMessageBox::Icon)icon));
  iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  buttonBox = new QDialogButtonBox(buttons);
  buttonBox->setCenterButtons(style()->styleHint(QStyle::SH_MessageBox_CenterButtons, 0, this));
  QObject::connect(buttonBox, SIGNAL(clicked(QAbstractButton*)),
    this, SLOT(handle_buttonClicked(QAbstractButton*)));

  QGridLayout *grid = new QGridLayout;

  grid->addWidget(iconLabel, 0, 0, 2, 1, Qt::AlignTop);
  grid->addWidget(scroll, 0, 1, 1, 1);
  grid->addWidget(buttonBox, 1, 0, 1, 2);
  grid->setSizeConstraint(QLayout::SetNoConstraint);
  setLayout(grid);

  if (!title.isEmpty() || !text.isEmpty())
  {
    setWindowTitle(title);
    label->setText(text);
  }
  setModal(true);
}

QPixmap ScrollMessageBox::standardIcon(QMessageBox::Icon icon)
{
  QStyle *style = this->style();
  int iconSize = style->pixelMetric(QStyle::PM_MessageBoxIconSize, 0, this);
  QIcon tmpIcon;
  switch (icon)
  {
    case QMessageBox::Information:
      tmpIcon = style->standardIcon(QStyle::SP_MessageBoxInformation, 0, this);
      break;
    case QMessageBox::Warning:
      tmpIcon = style->standardIcon(QStyle::SP_MessageBoxWarning, 0, this);
      break;
    case QMessageBox::Critical:
      tmpIcon = style->standardIcon(QStyle::SP_MessageBoxCritical, 0, this);
      break;
    case QMessageBox::Question:
      tmpIcon = style->standardIcon(QStyle::SP_MessageBoxQuestion, 0, this);
    default:
      break;
  }
  if (!tmpIcon.isNull())
  {
    return tmpIcon.pixmap(iconSize, iconSize);
  }
  return QPixmap();
}

void ScrollMessageBox::handle_buttonClicked(QAbstractButton *button)
{
  int ret = buttonBox->standardButton(button);
  done(ret);
}

void ScrollMessageBox::setDefaultButton(QPushButton *button)
{
  if (!buttonBox->buttons().contains(button))
    return;
  button->setDefault(true);
  button->setFocus();
}

void ScrollMessageBox::setDefaultButton(QDialogButtonBox::StandardButton button)
{
  setDefaultButton(buttonBox->button(button));
}

void ScrollMessageBox::showEvent(QShowEvent *e)
{
  updateSize();
  QDialog::showEvent(e);
}

void ScrollMessageBox::updateSize()
{
  if (!isVisible())
    return;

  QSize screenSize = QApplication::desktop()->availableGeometry(QCursor::pos()).size();

  // Limit screen size to 85% width except for screens less than 1024.
  // Since on a smaller screen I may really want the entire width; especially for even smaller screens.
  int hardLimit = screenSize.width() - screenSize.width() / 15;
  if (screenSize.width() <= 1024)
  {
    hardLimit = screenSize.width();
  }

  layout()->activate();
  int width = layout()->totalMinimumSize().width();

  {
    QFontMetrics fm(QApplication::font("QWorkspaceTitleBar"));
    int windowTitleWidth = qMin(fm.width(windowTitle()) + 50, hardLimit);
    if (windowTitleWidth > width)
    {
      width = windowTitleWidth;
    }
  }

  hardLimit = screenSize.height() - screenSize.height() / 15;

  int height;
  {
    QFontMetrics fm(label->font());
    int expectedHeight = fm.height() * (label->text().count("\n") + 6);
    height = qMin(expectedHeight, hardLimit);
    height = qMax(height, hardLimit / 10);


    QStringList sl = label->text().split("\n");
    for (int k=0; k<sl.size(); ++k)
    {
        width = qMax(width, fm.width(sl[k])+100);
    }
    width = qMin(width, hardLimit);
    width = qMax(width, hardLimit / 10);

  }

  resize(width, height);
}


QDialogButtonBox::StandardButton ScrollMessageBox::critical(QWidget* parent, QString const& title, QString const& text,
  QDialogButtonBox::StandardButtons buttons, QDialogButtonBox::StandardButton defaultButton)
{
  ScrollMessageBox box(QMessageBox::Critical, title, text, buttons, parent);
  box.setDefaultButton(defaultButton);
  return static_cast<QDialogButtonBox::StandardButton>(box.exec());
}

QDialogButtonBox::StandardButton ScrollMessageBox::information(QWidget* parent, QString const& title, QString const& text,
  QDialogButtonBox::StandardButtons buttons, QDialogButtonBox::StandardButton defaultButton)
{
  ScrollMessageBox box(QMessageBox::Information, title, text, buttons, parent);
  box.setDefaultButton(defaultButton);
  return static_cast<QDialogButtonBox::StandardButton>(box.exec());
}

QDialogButtonBox::StandardButton ScrollMessageBox::question(QWidget* parent, QString const& title, QString const& text,
  QDialogButtonBox::StandardButtons buttons, QDialogButtonBox::StandardButton defaultButton)
{
  ScrollMessageBox box(QMessageBox::Question, title, text, buttons, parent);
  box.setDefaultButton(defaultButton);
  return static_cast<QDialogButtonBox::StandardButton>(box.exec());
}

QDialogButtonBox::StandardButton ScrollMessageBox::warning(QWidget* parent, QString const& title, QString const& text,
  QDialogButtonBox::StandardButtons buttons, QDialogButtonBox::StandardButton defaultButton)
{
  ScrollMessageBox box(QMessageBox::Warning, title, text, buttons, parent);
  box.setDefaultButton(defaultButton);
  return static_cast<QDialogButtonBox::StandardButton>(box.exec());
}
