#include "config.h"

namespace core
{
	void Config::setDefaultConfig()
	{
		setifno(HOTKRY_SHOT_SCREEN, ConfigItem("Ctrl+Shift+S"));
	}
}