#ifndef __controller_mib_feature_h
#define __controller_mib_feature_h

typedef struct {
	unsigned int type;
	char 		 name[8];
	unsigned int version;
	unsigned int feature_count;
	unsigned int features[8];
} momo_module_descriptor;

void cleanup_unresponsive_modules();

#endif