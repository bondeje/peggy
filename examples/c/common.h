#ifndef CCOMMON_H
#define CCOMMON_H

#if __STDC_VERSION__ < 201112L
#ifndef _Alignof
	#define _Alignof(type) offsetof(struct CAT(type, _ALIGNMENT), b)
#endif
	#define BUILD_ALIGNMENT_STRUCT(type) \
	struct CAT(type, _ALIGNMENT) {\
		char a;\
		type b;\
	};
#else
	#define BUILD_ALIGNMENT_STRUCT(type)
#endif

#endif

