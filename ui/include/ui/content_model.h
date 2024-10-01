#pragma once

#include <ui/model_interface.h>
#include <ui/named_object_model.h>

#include <QJsonValue>


struct ContentData {
    QString content() const { return text; }

    QJsonValue toJson() const { return text; }
    ContentData(const QJsonValue &json)
        : text(json.toString())
    {
    }

    QString text;
};

class ContentModel : public NamedObjectModelWithData<ContentData>, public ModelInterface
{
    using Base = NamedObjectModelWithData<ContentData>;

public:
    using Base::Base;

    TileChildData tileData() const override { return { tr("Content pages") }; }

    void readSettings() override { Base::readSettings(cId); }
    void storeSettings() const override { Base::storeSettings(cId); }

private:
    static constexpr const char *cId = "content";
};
