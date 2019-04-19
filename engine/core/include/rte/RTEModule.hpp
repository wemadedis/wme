#pragma once

class RTEModule
{
protected:
    static RTEModule* _instance;

public:
    static RTEModule* &get()
    {
        return _instance;
    }

    RTEModule()
    {
        _instance = this;
    }
};
