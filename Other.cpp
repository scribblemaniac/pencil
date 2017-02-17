#include <hayai.hpp>

#include <QIcon>
#include <QPixmap>

BENCHMARK(QIcon, Init, 10, 50)
{
    QIcon i;
}

BENCHMARK(QIcon, InitNull, 10, 50)
{
    QIcon i{QString()};
}

BENCHMARK(QPainter, Init, 10, 50)
{
    QPixmap pixmap;
}

BENCHMARK_P(QPainter, InitSize, 10, 50, (QSize size))
{
    QPixmap pixmap(size);
}

BENCHMARK_P_INSTANCE(QPainter, InitSize, (QSize(16, 16)));
BENCHMARK_P_INSTANCE(QPainter, InitSize, (QSize(24, 24)));
BENCHMARK_P_INSTANCE(QPainter, InitSize, (QSize(32, 32)));
BENCHMARK_P_INSTANCE(QPainter, InitSize, (QSize(64, 64)));
