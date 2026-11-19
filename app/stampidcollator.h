#ifndef STAMPIDCOLLATOR_H
#define STAMPIDCOLLATOR_H

#include <QCollator>

class StampIdCollator
{
public:
    StampIdCollator();

  /*! \brief Recognizes integers inside of strings so that they are sorted
   * properly.
   *
   * A number in a string sorts incorrectly. It is even worse if the strings are
   * similar to: RW2 and RW10. This sort routine recognizes numbers and then
   * sorts them as integers. This will still fail on floating point numbers. If
   * we have "1.23" and "1.101", it will compare 1 to 1 and then 23 to 101. This
   * will incorrectly claim that 1.23 < 1.101. This is not a use case for me, so
   * I don't care.
   *
   *  \param [in] left
   *  \param [in] right
   *  \return Equivalent meta type if known, or QMetaType::Void otherwise.
   */
    bool stampIdLessThan(const QString &left, const QString &right) const;
    bool operator()(const QString &left, const QString &right) const;

private:
    QCollator m_collator;
};

#endif // STAMPIDCOLLATOR_H
