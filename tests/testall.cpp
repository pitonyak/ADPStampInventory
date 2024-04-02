
#include "testall.h"
#include "imageutility.h"
//#include "stampdb.h"

void TestAll::testImageUtility() {
    ImageUtility iu;
    iu.setBaseDirectory("/andrew0/home/andy/Devsrc/Qt/Projects/Stamps/Images/book");



    QString catalogNumber("RW7a");
    QString category;
    QString num;
    QString trailer;

    QVERIFY(iu.splitCatalogNumber(catalogNumber, category, num, trailer));
    QVERIFY(category == "RW");
    QVERIFY(num == "7");
    QVERIFY(trailer == "a");

    catalogNumber = "1CVP2";
    QVERIFY(iu.splitCatalogNumber(catalogNumber, category, num, trailer));
    QVERIFY(category == "1CVP");
    QVERIFY(num == "2");
    QVERIFY(trailer == "");

    catalogNumber = "2CVP4Cd";
    QVERIFY(iu.splitCatalogNumber(catalogNumber, category, num, trailer));
    QVERIFY(category == "2CVP");
    QVERIFY(num == "4");
    QVERIFY(trailer == "Cd");

    catalogNumber = "2004a";
    QVERIFY(iu.splitCatalogNumber(catalogNumber, category, num, trailer));
    QVERIFY(category == "");
    QVERIFY(num == "2004");
    QVERIFY(trailer == "a");

    catalogNumber = "2004";
    QVERIFY(iu.splitCatalogNumber(catalogNumber, category, num, trailer));
    QVERIFY(category == "");
    QVERIFY(num == "2004");
    QVERIFY(trailer == "");

    QVERIFY(! iu.splitCatalogNumber("", category, num, trailer));
    QVERIFY(! iu.splitCatalogNumber("xXa", category, num, trailer));
    QVERIFY(! iu.splitCatalogNumber("!", category, num, trailer));


/**
    QStringList list;
    list = iu.findBookImages("USA", "100");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "11a");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "11");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "3278b");
    QVERIFY(list.size() == 0);
    list = iu.findBookImages("USA", "4972");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "4972a");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "rw1");
    QVERIFY(list.size() > 0);
    list = iu.findBookImages("USA", "RW1");
    QVERIFY(list.size() > 0);
**/
}

