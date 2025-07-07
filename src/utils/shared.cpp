#include "shared.hpp"

String Shared::tsToString(time_t ts) {
    struct tm tminfo;
    char timestr[32];
    // Usamos gmtime_r para evitar problemas si usaras hilos en el futuro
    gmtime_r(&ts, &tminfo); 
    strftime(timestr, sizeof(timestr), "%Y-%m-%d-%H:%M:%S", &tminfo);
    return String(timestr);
}