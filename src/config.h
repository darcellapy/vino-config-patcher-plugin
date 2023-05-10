#ifndef _CONFIG_H
#define _CONFIG_H

class Config {
public:
    static void Init();

    // wups config items
    static bool connect_to_latte;
    static bool replace_download_management;
};

#endif