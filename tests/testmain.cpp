//**************************************************************************
/*! \class
 * \brief Provides a way to run unit tests
 *
 * Frankly, the testing capability in QT is not very good.
 * Last I checked, the official method is to create a single runtime for each
 * class that you want to test, but that is crazy.
 * There is a solution listed here:
 *
 * https://stackoverflow.com/questions/37987301/how-to-run-multiple-qtest-classes
 *
 * For my purposes I will simply create a single test class, which is a horrid way to
 * do this but it will work for now.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2023-2024
 ***************************************************************************/

#include <QTest>

#include "testall.h"

QTEST_MAIN(TestAll)
