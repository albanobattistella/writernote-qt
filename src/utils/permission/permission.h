#pragma once

class permission
{

public:
    enum e_permission {
        exist,
        readOnly,
        writeOnly
    };

    static bool open(const char *file, e_permission);
};

