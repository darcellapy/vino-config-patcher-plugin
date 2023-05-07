#ifndef VCP_CONFIG_H
#define VCP_CONFIG_H

class Config {
public:
    static void Init();

    // wups config items
    static bool connect_to_latte;

    // private stuff
    static bool need_relaunch;
};

#endif //VCP_CONFIG_H
