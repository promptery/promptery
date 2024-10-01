#pragma once

class ModelInterface
{
public:
    virtual ~ModelInterface() = default;

    virtual void readSettings()        = 0;
    virtual void storeSettings() const = 0;

};
