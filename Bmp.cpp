#include <hayai.hpp>

#include <QIcon>
#include <QPixmap>

const QStringList simpleLoc = {":/resources/simple16x16.bmp", ":/resources/simple24x24.bmp", ":/resources/simple32x32.bmp", ":/resources/simple64x64.bmp"};

class BmpFixture1 : public hayai::Fixture {
public:
    virtual void SetUp() {

    }

    virtual void TearDown() {
        delete icon;
    }

    QIcon *icon;
};

BENCHMARK_P_F(BmpFixture1, QIconInit, 10, 1000, (int i))
{
    icon = new QIcon(simpleLoc[i]);
}

BENCHMARK_P_INSTANCE(BmpFixture1, QIconInit, (0));
BENCHMARK_P_INSTANCE(BmpFixture1, QIconInit, (1));
BENCHMARK_P_INSTANCE(BmpFixture1, QIconInit, (2));
BENCHMARK_P_INSTANCE(BmpFixture1, QIconInit, (3));

class BmpFixture2 : public hayai::Fixture {
public:
    virtual void SetUp() {
        icon = new QIcon();
    }

    virtual void TearDown() {
        delete icon;
    }

    QIcon *icon;
};

BENCHMARK_P_F(BmpFixture2, QIconAddFile, 10, 1000, (QString loc))
{
    icon->addFile(loc);
}

BENCHMARK_P_INSTANCE(BmpFixture2, QIconAddFile, (simpleLoc[0]));
BENCHMARK_P_INSTANCE(BmpFixture2, QIconAddFile, (simpleLoc[1]));
BENCHMARK_P_INSTANCE(BmpFixture2, QIconAddFile, (simpleLoc[2]));
BENCHMARK_P_INSTANCE(BmpFixture2, QIconAddFile, (simpleLoc[3]));

class BmpFixture3 : public hayai::Fixture {
public:
    virtual void SetUp() {

    }

    virtual void TearDown() {
        delete pixmap;
    }

    QPixmap *pixmap;
};

BENCHMARK_P_F(BmpFixture3, QPixmapInit, 10, 1000, (QString loc))
{
    pixmap = new QPixmap(loc);
}

BENCHMARK_P_INSTANCE(BmpFixture3, QPixmapInit, (simpleLoc[0]));
BENCHMARK_P_INSTANCE(BmpFixture3, QPixmapInit, (simpleLoc[1]));
BENCHMARK_P_INSTANCE(BmpFixture3, QPixmapInit, (simpleLoc[2]));
BENCHMARK_P_INSTANCE(BmpFixture3, QPixmapInit, (simpleLoc[3]));

class BmpFixture4 : public hayai::Fixture {
public:
    virtual void SetUp() {

    }

    virtual void TearDown() {
        delete img;
    }

    QImage *img;
};

BENCHMARK_P_F(BmpFixture4, QImageInit, 10, 1000, (QString loc))
{
    img = new QImage(loc);
}

BENCHMARK_P_INSTANCE(BmpFixture4, QImageInit, (simpleLoc[0]));
BENCHMARK_P_INSTANCE(BmpFixture4, QImageInit, (simpleLoc[1]));
BENCHMARK_P_INSTANCE(BmpFixture4, QImageInit, (simpleLoc[2]));
BENCHMARK_P_INSTANCE(BmpFixture4, QImageInit, (simpleLoc[3]));
