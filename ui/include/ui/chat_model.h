#pragma once

#include <ui/model_interface.h>
#include <ui/named_object_model.h>

#include <model/types.h>

class ChatModel : public NamedObjectModelWithData<ChatData>, public ModelInterface
{
    using Base = NamedObjectModelWithData<ChatData>;

public:
    using Base::Base;

    TileChildData tileData() const override { return { tr("Chats") }; }

    void readSettings() override { Base::readSettings("chats"); }
    void storeSettings() const override { Base::storeSettings("chats"); }

protected:
    QString newObjectName() const override { return tr("new chat"); }
};
