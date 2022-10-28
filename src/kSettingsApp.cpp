#include "kSettingsApp.h"
#include "kaimen.h"

int network_menu() {
    K::text("Airplane mode");

    K::set_layout(K::Layout::TWO_COLUMNS);
    if (K::button("ON", K::Color::TRANSPARENT, TFT_WHITE)) {
        mp.airplaneMode = 0;
    }

    if (K::button("OFF", K::Color::TRANSPARENT, TFT_WHITE)) {
        mp.airplaneMode = 1;
    }

    K::set_layout(K::Layout::ONE_COLUMN);
    return 0;
}

int settings_app() {
    if (K::button("Network", K::Color::RED, TFT_BLACK)) {
        K::open(network_menu);
    }

    if (K::button("Display", K::Color::GREEN, TFT_BLACK)) {
        // K::open(display_menu);
    }
}
