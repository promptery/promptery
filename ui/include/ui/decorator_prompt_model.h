#pragma once

#include <ui/model_interface.h>
#include <ui/named_object_model.h>

#include <model/chat_types.h>

class DecoratorPromptModel : public NamedObjectModelWithData<DecoratorPromptData>,
                             public ModelInterface
{
    using Base = NamedObjectModelWithData<DecoratorPromptData>;

public:
    using Base::Base;

    TileChildData tileData() const override { return { tr("Prompt decorators") }; }

    void readSettings() override { Base::readSettings(cId); }
    void storeSettings() const override { Base::storeSettings(cId); }

protected:
    QString newObjectName() const override { return tr("new decorator prompt"); }

private:
    static constexpr const char *cId = "decoratorPrompts";
};
