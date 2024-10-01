#pragma once

#include <QIcon>
#include <QString>

class TileChildInterface;

class QWidget;

struct PageData {
    QString name;
    QIcon icon;
    QString tooltip;
};

class PageInterface
{
public:
    virtual ~PageInterface() = default;

    virtual PageData pageData() const = 0;

    virtual TileChildInterface *mainView() = 0;
    virtual QWidget *sideView()            = 0;

    virtual void readSettings() {}
    /**
     * @brief prepareStoreSettings should be used to write all data to the respective models
     */
    virtual void prepareStoreSettings() {}
    virtual void storeSettings() const {}
    virtual void saveState() const { /* Save UI, splitters etc. */ }
};
