#ifndef __IYEXPORT_HEADER__
#define __IYEXPORT_HEADER__

#if defined(_WIN32)
	#ifdef IYEXPORTS
		#ifdef __cplusplus
			#define IYAPI extern "C" __declspec(dllexport)
		#else
			#define IYAPI __declspec(dllexport)
		#endif
	#else
		#ifdef __cplusplus
			#define IYAPI extern "C" __declspec(dllimport)
		#else
			#define IYAPI __declspec(dllimport)			
		#endif
	#endif
#else
	#ifndef IYAPI
		#ifdef __cplusplus
			#define IYAPI extern "C" __attribute__((visibility("default")))
		#else
			#define IYAPI __attribute__((visibility("default")))
		#endif
	#endif
#endif

#endif