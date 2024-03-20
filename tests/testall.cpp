
#include "testall.h"
#include "imageutility.h"
#include "stampdb.h"

void TestAll::testImageUtility() {
    ImageUtility iu;
    iu.setBaseDirectory("/andrew0/home/andy/Devsrc/Qt/Projects/Stamps/Images/book");
    QStringList list;
    list = iu.findBookImages("USA", "", "100");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "", "11a");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "", "11");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "", "3278b");
    QVERIFY(list.size() == 0);
    list = iu.findBookImages("USA", "", "4972");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "", "4972a");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "rw", "rw1");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "RW", "RW1");
    QVERIFY(list.size() > 0);
}

