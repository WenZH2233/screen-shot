#include "base.h"
#include "native_theme_bridge.h"

#include <iomanip>
#include <sstream>
#include <string>

namespace core {
    bool refreshSystemThemeFromRust() {
        native_theme_system_theme theme{};
        if (!native_theme_query_system_theme(&theme)) {
            systemAccentColor = Color(0, 0, 225);
            systemBackgroundColor = Color(255, 255, 255);
            systemTextColor = Color(0, 0, 0);

            bool isDark = false;
            if (native_theme_query_is_dark(&isDark)) {
                isDarkMode = isDark;
            }
            return false;
        }

        isDarkMode = theme.is_dark;
        systemAccentColor = theme.has_accent_color ? Color(theme.accent_color.r, theme.accent_color.g, theme.accent_color.b) : Color(0, 0, 225);
        systemBackgroundColor = theme.has_background_color ? Color(theme.background_color.r, theme.background_color.g, theme.background_color.b) : Color(255, 255, 255);
        systemTextColor = theme.has_text_color ? Color(theme.text_color.r, theme.text_color.g, theme.text_color.b) : Color(0, 0, 0);
        return true;
    }
}
