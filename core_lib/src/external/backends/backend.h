#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>

class DebugDetails;

class Backend : public QObject
{
public:
    explicit Backend(QObject *parent = nullptr) : QObject(parent) {}

    /** Checks if the backend is read to be used.
      *
      * This can be used to make enable/disable features provided by the backend if
      * there is some precondition for the backend that may or may not be met.
      * For example, if the backend requires an external binary, isAvailable should
      * check if that binary is accessible and operates as expected.
      *
      * @return true if the backend is available in any capacity, false otherwise.
      */
    virtual bool isAvailable() const = 0;

    /** Retrieves information about the backend (version number, path, current status, etc).
      *
      * This will be used for displaying information during crashes and in the "About Me" dialog.
      * At minimum it should start with a line identifying the backend, and then it should provide
      * any useful information about any external resources being used, such as their version number
      * and path.
      *
      * @return DebugDetails about this backend.
      */
    virtual DebugDetails getDebugInfo() const = 0;
};

#endif // BACKEND_H
