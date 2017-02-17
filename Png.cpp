#include <hayai.hpp>

#include <QIcon>
#include <QPixmap>
#include <QPainter>

const QStringList simpleLoc = {":/resources/simple16x16.png", ":/resources/simple24x24.png", ":/resources/simple32x32.png", ":/resources/simple64x64.png"};

class PngFixture1 : public hayai::Fixture {
public:
    virtual void SetUp() {

    }

    virtual void TearDown() {
        delete icon;
    }

    QIcon *icon;
};

BENCHMARK_P_F(PngFixture1, QIconInit, 10, 50, (int i))
{
    icon = new QIcon(simpleLoc[i]);
}

BENCHMARK_P_INSTANCE(PngFixture1, QIconInit, (0));
BENCHMARK_P_INSTANCE(PngFixture1, QIconInit, (1));
BENCHMARK_P_INSTANCE(PngFixture1, QIconInit, (2));
BENCHMARK_P_INSTANCE(PngFixture1, QIconInit, (3));

class PngFixture2 : public hayai::Fixture {
public:
    virtual void SetUp() {
        icon = new QIcon();
    }

    virtual void TearDown() {
        delete icon;
    }

    QIcon *icon;
};

BENCHMARK_P_F(PngFixture2, QIconAddFile, 10, 50, (QString loc))
{
    icon->addFile(loc);
}

BENCHMARK_P_INSTANCE(PngFixture2, QIconAddFile, (simpleLoc[0]));
BENCHMARK_P_INSTANCE(PngFixture2, QIconAddFile, (simpleLoc[1]));
BENCHMARK_P_INSTANCE(PngFixture2, QIconAddFile, (simpleLoc[2]));
BENCHMARK_P_INSTANCE(PngFixture2, QIconAddFile, (simpleLoc[3]));

class PngFixture3 : public hayai::Fixture {
public:
    virtual void SetUp() {
        for(int i = 0; i < simpleLoc.length(); i++) {
            simpleIcon[i] = new QIcon(simpleLoc[i]);
        }
    }

    virtual void TearDown() {
        for(int i = 0; i < simpleLoc.length(); i++) {
            delete simpleIcon[i];
        }
    }

    QIcon *simpleIcon[4];
};

BENCHMARK_P_F(PngFixture3, QIconPixmap, 5, 50, (int i, QSize size))
{
    simpleIcon[i]->pixmap(size);
}

BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (0, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (0, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (0, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (0, QSize(64, 64)));

BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (1, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (1, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (1, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (1, QSize(64, 64)));

BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (2, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (2, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (2, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (2, QSize(64, 64)));

BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (3, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (3, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (3, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture3, QIconPixmap, (3, QSize(64, 64)));

class PngFixture4 : public hayai::Fixture {
public:
    virtual void SetUp() {

    }

    virtual void TearDown() {
        delete pixmap;
    }

    QPixmap *pixmap;
};

BENCHMARK_P_F(PngFixture4, QPixmapInit, 10, 50, (QString loc))
{
    pixmap = new QPixmap(loc);
}

BENCHMARK_P_INSTANCE(PngFixture4, QPixmapInit, (simpleLoc[0]));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapInit, (simpleLoc[1]));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapInit, (simpleLoc[2]));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapInit, (simpleLoc[3]));

BENCHMARK_P_F(PngFixture4, QPixmapFast, 5, 50, (int i, QSize size))
{
    pixmap = new QPixmap(simpleLoc[i]);
    pixmap->scaled(size, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}

BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (0, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (0, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (0, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (0, QSize(64, 64)));

BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (1, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (1, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (1, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (1, QSize(64, 64)));

BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (2, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (2, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (2, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (2, QSize(64, 64)));

BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (3, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (3, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (3, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapFast, (3, QSize(64, 64)));

BENCHMARK_P_F(PngFixture4, QPixmapSmooth, 5, 50, (int i, QSize size))
{
    pixmap = new QPixmap(simpleLoc[i]);
    pixmap->scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (0, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (0, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (0, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (0, QSize(64, 64)));

BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (1, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (1, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (1, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (1, QSize(64, 64)));

BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (2, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (2, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (2, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (2, QSize(64, 64)));

BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (3, QSize(16, 16)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (3, QSize(24, 24)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (3, QSize(32, 32)));
BENCHMARK_P_INSTANCE(PngFixture4, QPixmapSmooth, (3, QSize(64, 64)));

class PngFixture5 : public hayai::Fixture {
public:
    virtual void SetUp() {
        painter = new QPainter();
        for(int i = 0; i < simpleLoc.length(); i++) {
            simplePixmap[i] = new QPixmap(simpleLoc[i]);
        }
    }

    virtual void TearDown() {
        delete painter;
        for(int i = 0; i < simpleLoc.length(); i++) {
            delete simplePixmap[i];
        }
    }

    QPainter *painter;
    QPixmap *simplePixmap[4];
};

BENCHMARK_P_F(PngFixture5, QPainterDrawPixmap, 5, 50, (int i, QRectF size))
{
    painter->drawPixmap(size, *simplePixmap[i], simplePixmap[i]->rect());
}

BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (0, QRectF(0, 0, 16, 16)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (0, QRectF(0, 0, 24, 24)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (0, QRectF(0, 0, 32, 32)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (0, QRectF(0, 0, 64, 64)));

BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (1, QRectF(0, 0, 16, 16)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (1, QRectF(0, 0, 24, 24)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (1, QRectF(0, 0, 32, 32)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (1, QRectF(0, 0, 64, 64)));

BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (2, QRectF(0, 0, 16, 16)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (2, QRectF(0, 0, 24, 24)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (2, QRectF(0, 0, 32, 32)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (2, QRectF(0, 0, 64, 64)));

BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (3, QRectF(0, 0, 16, 16)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (3, QRectF(0, 0, 24, 24)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (3, QRectF(0, 0, 32, 32)));
BENCHMARK_P_INSTANCE(PngFixture5, QPainterDrawPixmap, (3, QRectF(0, 0, 64, 64)));

class PngFixture6 : public hayai::Fixture {
public:
    virtual void SetUp() {
        painter = new QPainter();
        for(int i = 0; i < simpleLoc.length(); i++) {
            simpleImage[i] = new QImage(simpleLoc[i]);
        }
    }

    virtual void TearDown() {
        delete painter;
        for(int i = 0; i < simpleLoc.length(); i++) {
            delete simpleImage[i];
        }
    }

    QPainter *painter;
    QImage *simpleImage[4];
};

BENCHMARK_P_F(PngFixture6, QPainterDrawImage, 5, 50, (int i, QRectF size))
{
    painter->drawImage(size, *simpleImage[i]);
}

BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (0, QRectF(0, 0, 16, 16)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (0, QRectF(0, 0, 24, 24)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (0, QRectF(0, 0, 32, 32)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (0, QRectF(0, 0, 64, 64)));

BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (1, QRectF(0, 0, 16, 16)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (1, QRectF(0, 0, 24, 24)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (1, QRectF(0, 0, 32, 32)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (1, QRectF(0, 0, 64, 64)));

BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (2, QRectF(0, 0, 16, 16)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (2, QRectF(0, 0, 24, 24)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (2, QRectF(0, 0, 32, 32)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (2, QRectF(0, 0, 64, 64)));

BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (3, QRectF(0, 0, 16, 16)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (3, QRectF(0, 0, 24, 24)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (3, QRectF(0, 0, 32, 32)));
BENCHMARK_P_INSTANCE(PngFixture6, QPainterDrawImage, (3, QRectF(0, 0, 64, 64)));

class PngFixture7 : public hayai::Fixture {
public:
    virtual void SetUp() {

    }

    virtual void TearDown() {
        delete img;
    }

    QImage *img;
};

BENCHMARK_P_F(PngFixture7, QImageInit, 10, 50, (QString loc))
{
    img = new QImage(loc);
}

BENCHMARK_P_INSTANCE(PngFixture7, QImageInit, (simpleLoc[0]));
BENCHMARK_P_INSTANCE(PngFixture7, QImageInit, (simpleLoc[1]));
BENCHMARK_P_INSTANCE(PngFixture7, QImageInit, (simpleLoc[2]));
BENCHMARK_P_INSTANCE(PngFixture7, QImageInit, (simpleLoc[3]));
