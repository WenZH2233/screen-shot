#include "base.h"

#include "log.h"

bool g_Quited = false;


namespace core {
    void init() {
        Log.init();
    }
}