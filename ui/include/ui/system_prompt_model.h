#pragma once

#include <ui/model_interface.h>
#include <ui/named_object_model.h>

#include <model/chat_types.h>

class SystemPromptModel : public NamedObjectModelWithData<SystemPromptData>, public ModelInterface
{
    using Base = NamedObjectModelWithData<SystemPromptData>;

public:
    using Base::Base;

    TileChildData tileData() const override { return { tr("System prompts") }; }

    void readSettings() override { Base::readSettings(cId); }
    void storeSettings() const override { Base::storeSettings(cId); }

protected:
    QString newObjectName() const override { return tr("new system prompt"); }

private:
    static constexpr const char *cId = "systemPrompts";
};
