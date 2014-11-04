#ifndef SCROLLMESSAGEBOX_H
#define SCROLLMESSAGEBOX_H

#include <QDialog>
#include <QMessageBox>
#include <QDialogButtonBox>

//**************************************************************************
/*! \class ScrollMessageBox
 * \brief Scrollable message box.
 *
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2013-2014
 ***************************************************************************/

class ScrollMessageBox : public QDialog
{
    Q_OBJECT
public:
    //**************************************************************************
    /*! \brief Default constructor with the specified parent.
     *
     *  All objects owned by the parent are destroyed in the destructor.
     *  \param [in,out] parent The object's owner.
     ***************************************************************************/
    explicit ScrollMessageBox(QWidget * parent = nullptr);

    //**************************************************************************
    /*! \brief Full constructor.
     *
     *  All objects owned by the parent are destroyed in the destructor.
     *
     *  \param [in] icon Enum that specifies the pixelmap shown on the dialog to visually indicate message type.
     *  \param [in] title Text to show in the title bar.
     *  \param [in] text Text to show in the scroll area.
     *  \param [in] buttons Buttons to include. Defaults to none.
     *  \param [in,out] parent The object's owner.
     ***************************************************************************/
    explicit ScrollMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text,
                QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::NoButton, QWidget *parent = nullptr);

    //**************************************************************************
    /*! \brief Set the default button on the contained button box.
     *
     *  \param [in] button Enum indicating the button to use as default.
     ***************************************************************************/
    void setDefaultButton(QDialogButtonBox::StandardButton button);

    //**************************************************************************
    /*! \brief Display a dialog with a Critical icon and an OK button.
     *
     *  \param [in,out] parent The object's owner.
     *  \param [in] title Text to show in the title bar.
     *  \param [in] text Text to show in the scroll area.
     *  \param [in] buttons Buttons to include. Defaults to an OK button.
     *  \param [in] defaultButton Which button is the default button. Defaults to no button as default.
     *
     * \return An enum indicating the button that was pressed.
     ***************************************************************************/
    static QDialogButtonBox::StandardButton critical(QWidget* parent, QString const& title, QString const& text, QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok, QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton);

    //**************************************************************************
    /*! \brief Display a dialog with an information icon and  an OK button.
     *
     *  \param [in,out] parent The object's owner.
     *  \param [in] title Text to show in the title bar.
     *  \param [in] text Text to show in the scroll area.
     *  \param [in] buttons Buttons to include. Defaults to an OK button.
     *  \param [in] defaultButton Which button is the default button. Defaults to no button is the default button.
     *
     * \return An enum indicating the button that was pressed.
     ***************************************************************************/
    static QDialogButtonBox::StandardButton information(QWidget* parent, QString const& title, QString const& text, QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok , QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton);

    //**************************************************************************
    /*! \brief Display a dialog with a question icon and a Yes and No button.
     *
     *  \param [in,out] parent The object's owner.
     *  \param [in] title Text to show in the title bar.
     *  \param [in] text Text to show in the scroll area.
     *  \param [in] buttons Buttons to include. Defaults to a Yes and a No button.
     *  \param [in] defaultButton Which button is the default button. Defaults to no button is the default button.
     *
     * \return An enum indicating the button that was pressed.
     ***************************************************************************/
    static QDialogButtonBox::StandardButton question(QWidget* parent, QString const& title, QString const& text, QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Yes | QDialogButtonBox::No, QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton);

    //**************************************************************************
    /*! \brief Display a dialog with a warning icon and  an OK button.
     *
     *  \param [in,out] parent The object's owner.
     *  \param [in] title Text to show in the title bar.
     *  \param [in] text Text to show in the scroll area.
     *  \param [in] buttons Buttons to include. Defaults to an OK button.
     *  \param [in] defaultButton Which button is the default button. Defaults to no button is the default button.
     *
     * \return An enum indicating the button that was pressed.
     ***************************************************************************/
    static QDialogButtonBox::StandardButton warning(QWidget* parent, QString const& title, QString const& text, QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok, QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::NoButton);

    //**************************************************************************
    /*! \brief Called when the dialog is shown.
     *
     *  Update the size then call the base class showEvent.
     *
     * There are two kinds of show events:
     * show events caused by the window system (spontaneous), and internal show events.
     * Spontaneous show events are sent just after the window system shows the window;
     * they are also sent when a top-level window is redisplayed after being iconified.
     * Internal show events are delivered just before the widget becomes visible.
     *
     *  \param [in,out] event Specific event type that is accepted or ignored by the base class.
     ***************************************************************************/
    void showEvent ( QShowEvent * event );

private:
    //**************************************************************************
    /*! \brief Get the pixel map associated to the icon type.
     *
     *  \param [in] icon Enum indicating the icon type.
     *
     * \return Pixel map assocaited to the icon type. If the type is not known, a blank icon is returned.
     ***************************************************************************/
    QPixmap standardIcon(QMessageBox::Icon icon);

    //**************************************************************************
    /*! \brief Set the default button
     *
     *  \param [in] button Button to make default.
     ***************************************************************************/
    void setDefaultButton(QPushButton *button);

    //**************************************************************************
    /*! \brief Guess the screen size based on the contained text.
     *
     *  Dialog width may be the entire screen width for smaller displays (less than 1024)
     * but is limited to a percent width for monitors with greater resolution.
     ***************************************************************************/
    void updateSize();

    /*! \brief Contains the scroll text */
    QLabel *label;

    /*! \brief Contains the buttons */
    QDialogButtonBox *buttonBox;

private Q_SLOTS:
    //**************************************************************************
    /*! \brief Close the dialog and set the return code based on the button.
     *
     * So, pressing any button causes the dialog to close.
     *
     *  \param [in] button Button that was pressed. This is converted into a button enum set for the return code.
     ***************************************************************************/
    void handle_buttonClicked(QAbstractButton *button);
};

#endif // SCROLLMESSAGEBOX_H
