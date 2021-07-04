#ifndef EXPORTMOVIEDESC_H
#define EXPORTMOVIEDESC_H

#include <QSize>

struct ExportMovieDesc
{
    QString strFileName;
    int     startFrame = 0;
    int     endFrame = 0;
    int     fps = 12;
    QSize   exportSize{0, 0};
    QString strCameraName;
    bool    loop = false;
    bool    alpha = false;
};

#endif // EXPORTMOVIEDESC_H
