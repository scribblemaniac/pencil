#include "theming.h"

#include <QDir>
#include <QPalette>
#include <QSettings>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QStyle>

#include "pencilerror.h"

/**
 * Get a list of all valid keys that can be passed to Theming::getStyle().
 *
 * The exact list will vary depending on the platform, loaded Qt plugins,
 * and the Qt runtime configuration.
 *
 * @return A list of string keys.
 * @see getStyle()
 */
QStringList Theming::availableStyles()
{
    return QStyleFactory::keys();
}

/**
 * Get a list of all valid keys that can be passed to Theming::getPalette().
 *
 * This list is generated by finding all palette configuration files in the
 * standard search path and removing any that cannot be validated.
 *
 * @return A list of string keys.
 * @see getPalette()
 */
QStringList Theming::availablePalettes()
{
    QStringList palettes;

    // Built-in palettes
    for (const QString& palette : QDir(":/theme_palettes").entryList({"*.conf"}, QDir::Files))
    {
        palettes.append(palette.chopped(5));
    }

    // User palettes
    for (const QString& palette : userPaletteDir().entryList({"*.conf"}, QDir::Files))
    {
        palettes.append(palette.chopped(5));
    }

    return palettes;
}

/**
 * Fetch a style with a given key.
 *
 * @param key A case-insensitive style identifier. Current defined by the Qt plugin providing the style.
 * @return A QStyle instance corresponding to the given key or a null pointer if there is none.
 * @see availableStyles()
 */
QStyle* Theming::getStyle(const QString& key)
{
    QStyle* style = QStyleFactory::create(key);

    return style;
}

/**
 * Fetch a palette with a given key.
 *
 * @param key A palette identifier.
 * @return A QPalette instance corresponding to the given key. If no palette with the key exists,
 *         or the conf file for the palette is invalid, a null pointer will be returned.
 * @see availablePalettes()
 */
QPalette* Theming::getPalette(const QString& key)
{
    QPalette* builtinPalette = loadPaletteConf(QString(":/theme_palettes/%1.conf").arg(key));
    if (builtinPalette != nullptr) return builtinPalette;

    return loadPaletteConf(userPaletteDir().filePath(QString("%1.conf").arg(key)));
}

/**
 * Saves a palette to the user's palette directory.
 *
 * @param filepath The path to a .conf palette file to load.
 * @return A Status indicating if the palette was added successfully.
 */
Status Theming::addPalette(const QString &filePath)
{
    if (!filePath.endsWith(".conf")) return Status::INVALID_ARGUMENT;
    QFileInfo fileInfo(filePath);
    if (fileInfo.baseName().isEmpty()) return Status::INVALID_ARGUMENT;
    if (!fileInfo.isFile()) return Status::FILE_NOT_FOUND;
    if (!fileInfo.isReadable()) return Status::ERROR_FILE_CANNOT_OPEN;

    QFile inFile(filePath);
    if (inFile.copy(userPaletteDir().filePath(fileInfo.fileName())))
    {
        return Status::OK;
    }
    return Status::FAIL;
}

/**
 * Removes a palette from the user's palette directory.
 *
 * The .conf palette file will be deleted by this function and cannot be undone.
 *
 * @param key The palette id of the palette to remove.
 * @return A Status indicated if the palette was removed successfully.
 */
Status Theming::removePalette(const QString& key)
{
    QFile paletteFile(userPaletteDir().filePath(QString("%1.conf").arg(key)));
    if (paletteFile.remove())
    {
        return Status::OK;
    }
    return Status::FAIL;
}

/**
 * Create a QPalette instance from a .conf palette file.
 *
 * The file format must be in the INI format. It should have a ColorScheme group with
 * active_colors, disabled_colors, and inactive_colors keys. Each should have at least 20 string list values
 * corresponding to colors (typically in a hexadecimal RGBA format, ex #ff424245).
 *
 * @param filename The path to the .conf file to load.
 * @return A QPalette instance with the color set from the .conf or nullptr if the file cannot be loaded or is not valid.
 */
QPalette* Theming::loadPaletteConf(const QString& filename)
{
    QPalette palette;
    QSettings conf(filename, QSettings::IniFormat);
    conf.beginGroup("ColorScheme");
    QList<std::pair<QPalette::ColorGroup, QString>> colorGroups = {
        { QPalette::Active, "active_colors" },
        { QPalette::Disabled, "disabled_colors" },
        { QPalette::Inactive, "inactive_colors" }
    };
    for (const auto& colorGroup : colorGroups)
    {
        QStringList colors = conf.value(colorGroup.second).toStringList();
        // 20 is QPalette::NColorRoles prior to Qt 5.12, and is the minimum number of colors required for this format.
        if (colors.count() < 20) return nullptr;

        for (int i = 0; i < qMin(colors.count(), static_cast<int>(QPalette::NColorRoles)); i++)
        {
            palette.setColor(colorGroup.first, QPalette::ColorRole(i), colors[i]);
        }
    }

    return new QPalette(palette);
}

const QDir Theming::userPaletteDir()
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    dir.cd("theme_palettes");

    return dir;
}
