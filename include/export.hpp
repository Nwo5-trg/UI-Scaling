#pragma once

#ifdef GEODE_IS_WINDOWS
	#ifdef NWO5_UI_SCALING_EXPORTING
		#define	UI_SCALING_DLL __declspec(dllexport)
	#else
		#define	UI_SCALING_DLL __declspec(dllimport)
	#endif
#else
	#define	UI_SCALING_DLL __attribute__((visibility("default")))
#endif