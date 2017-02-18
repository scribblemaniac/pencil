#include <hayai.hpp>

#include <QIcon>
#include <QSvgRenderer>
#include <QPixmap>
#include <QPainter>

const QString simpleLoc = ":/resources/simple.svg";
const QString fancyLoc = ":/resources/fancy.svg";

class SvgFixture1 : public hayai::Fixture {
public:
    virtual void SetUp() {

    }

    virtual void TearDown() {
        delete i;
    }

    QIcon *i;
};

BENCHMARK_F(SvgFixture1, QIconSimpleInit, 10, 1000)
{
    i = new QIcon(simpleLoc);
}

BENCHMARK_F(SvgFixture1, QIconFancyInit, 10, 1000)
{
    i = new QIcon(fancyLoc);
}

class SvgFixture2 : public hayai::Fixture {
public:
    virtual void SetUp() {
        icon = new QIcon();
    }

    virtual void TearDown() {
        delete icon;
    }

    QIcon *icon;
};

BENCHMARK_P_F(SvgFixture2, QIconAddFile, 10, 1000, (QString loc))
{
    icon->addFile(loc);
}

BENCHMARK_P_INSTANCE(SvgFixture2, QIconAddFile, (simpleLoc));
BENCHMARK_P_INSTANCE(SvgFixture2, QIconAddFile, (fancyLoc));

class SvgFixture3 : public hayai::Fixture {
public:
    virtual void SetUp() {
        simpleIcon = new QIcon(simpleLoc);
    }

    virtual void TearDown() {
        delete simpleIcon;
    }

    QIcon *simpleIcon;
};

BENCHMARK_P_F(SvgFixture3, QIconSimplePixmap, 10, 1000, (QSize size))
{
    simpleIcon->pixmap(size);
}

BENCHMARK_P_INSTANCE(SvgFixture3, QIconSimplePixmap, (QSize(16, 16)));
BENCHMARK_P_INSTANCE(SvgFixture3, QIconSimplePixmap, (QSize(24, 24)));
BENCHMARK_P_INSTANCE(SvgFixture3, QIconSimplePixmap, (QSize(32, 32)));
BENCHMARK_P_INSTANCE(SvgFixture3, QIconSimplePixmap, (QSize(64, 64)));

class SvgFixture4 : public hayai::Fixture {
public:
    virtual void SetUp() {
        fancyIcon = new QIcon(fancyLoc);
    }

    virtual void TearDown() {
        delete fancyIcon;
    }

    QIcon *fancyIcon;
};

BENCHMARK_P_F(SvgFixture4, QIconFancyPixmap, 10, 1000, (QSize size))
{
    fancyIcon->pixmap(size);
}

BENCHMARK_P_INSTANCE(SvgFixture4, QIconFancyPixmap, (QSize(16, 16)));
BENCHMARK_P_INSTANCE(SvgFixture4, QIconFancyPixmap, (QSize(24, 24)));
BENCHMARK_P_INSTANCE(SvgFixture4, QIconFancyPixmap, (QSize(32, 32)));
BENCHMARK_P_INSTANCE(SvgFixture4, QIconFancyPixmap, (QSize(64, 64)));

class SvgFixture5 : public hayai::Fixture {
public:
    virtual void SetUp() {
        renderer = new QSvgRenderer();
    }

    virtual void TearDown() {
        delete renderer;
    }

    QSvgRenderer *renderer;
};

BENCHMARK_P_F(SvgFixture5, QSvgRendererLoad, 10, 1000, (QString loc))
{
    renderer->load(loc);
}

BENCHMARK_P_INSTANCE(SvgFixture5, QSvgRendererLoad, (simpleLoc));
BENCHMARK_P_INSTANCE(SvgFixture5, QSvgRendererLoad, (fancyLoc));

class SvgFixture6 : public hayai::Fixture {
public:
    virtual void SetUp() {
        renderer = new QSvgRenderer(simpleLoc);
        pixmap = new QPixmap(64, 64);
        painter = new QPainter(pixmap);
    }

    virtual void TearDown() {
        delete renderer;
        delete painter;
        delete pixmap;
    }

    QSvgRenderer *renderer;
    QPixmap *pixmap;
    QPainter *painter;
};

BENCHMARK_P_F(SvgFixture6, QSvgRendererSimpleRender, 10, 1000, (QRectF bounds)) {
    renderer->render(painter, bounds);
}

BENCHMARK_P_INSTANCE(SvgFixture6, QSvgRendererSimpleRender, (QRectF(0, 0, 16, 16)));
BENCHMARK_P_INSTANCE(SvgFixture6, QSvgRendererSimpleRender, (QRectF(0, 0, 24, 24)));
BENCHMARK_P_INSTANCE(SvgFixture6, QSvgRendererSimpleRender, (QRectF(0, 0, 32, 32)));
BENCHMARK_P_INSTANCE(SvgFixture6, QSvgRendererSimpleRender, (QRectF(0, 0, 64, 64)));

class SvgFixture7 : public hayai::Fixture {
public:
    virtual void SetUp() {
        renderer = new QSvgRenderer(fancyLoc);
        pixmap = new QPixmap(64, 64);
        painter = new QPainter(pixmap);
    }

    virtual void TearDown() {
        delete renderer;
        delete painter;
        delete pixmap;
    }

    QSvgRenderer *renderer;
    QPixmap *pixmap;
    QPainter *painter;
};

BENCHMARK_P_F(SvgFixture7, QSvgRendererFancyRender, 10, 1000, (QRectF bounds)) {
    renderer->render(painter, bounds);
}

BENCHMARK_P_INSTANCE(SvgFixture7, QSvgRendererFancyRender, (QRectF(0, 0, 16, 16)));
BENCHMARK_P_INSTANCE(SvgFixture7, QSvgRendererFancyRender, (QRectF(0, 0, 24, 24)));
BENCHMARK_P_INSTANCE(SvgFixture7, QSvgRendererFancyRender, (QRectF(0, 0, 32, 32)));
BENCHMARK_P_INSTANCE(SvgFixture7, QSvgRendererFancyRender, (QRectF(0, 0, 64, 64)));
