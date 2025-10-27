#include "SonikSTringConvert.hpp"

namespace SonikConvStaticLocale
{
	//グローバルでコールすることで初期化をする。
	LocaleManagerSingleton& _global_ = LocaleManagerSingleton::instance();

};// end namespace SonikConvStaticLocale