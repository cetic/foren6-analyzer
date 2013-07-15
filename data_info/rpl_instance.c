#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "rpl_instance.h"
#include "dodag.h"

typedef struct di_rpl_instance {
	di_rpl_instance_key_t key;

	hash_container_ptr dodags;			//Via DIO, DAO
	di_rpl_mop_e mode_of_operation;	//Via DIO

	void *user_data;
} di_rpl_instance_t;

size_t rpl_instance_sizeof() {
	return sizeof(di_rpl_instance_t);
}

void rpl_instance_init(void* data, void *key, size_t key_size) {
	di_rpl_instance_t *instance = (di_rpl_instance_t*) data;

	assert(key_size == sizeof(di_rpl_instance_ref_t));

	instance->dodags = hash_create(sizeof(di_dodag_ref_t), NULL);
	instance->key.ref = *(di_rpl_instance_ref_t*) key;
	instance->key.version = 0;
}

di_rpl_instance_t* rpl_instance_dup(di_rpl_instance_t* rpl_instance) {
	di_rpl_instance_t *new_instance;

	new_instance = malloc(sizeof(di_rpl_instance_t));
	memcpy(new_instance, rpl_instance, sizeof(di_rpl_instance_t));
	new_instance->dodags = hash_dup(rpl_instance->dodags);

	return new_instance;
}

void rpl_instance_key_init(di_rpl_instance_key_t *key, uint8_t rpl_instance, uint32_t version) {
	memset(key, 0, sizeof(di_rpl_instance_key_t));

	key->ref.rpl_instance = rpl_instance;
	key->version = version;
}

void rpl_instance_ref_init(di_rpl_instance_ref_t *ref, uint8_t rpl_instance) {
	memset(ref, 0, sizeof(di_rpl_instance_ref_t));

	ref->rpl_instance = rpl_instance;
}

void rpl_instance_set_key(di_rpl_instance_t* rpl_instance, const di_rpl_instance_key_t* key) {
	rpl_instance->key = *key;
}

void rpl_instance_set_mop(di_rpl_instance_t* rpl_instance, di_rpl_mop_e mop) {
	rpl_instance->mode_of_operation = mop;
}

void rpl_instance_set_user_data(di_rpl_instance_t* rpl_instance, void *user_data) {
	rpl_instance->user_data = user_data;
}

void rpl_instance_add_dodag(di_rpl_instance_t* rpl_instance, di_dodag_t *dodag) {
	hash_add(rpl_instance->dodags, hash_key_make(dodag_get_key(dodag)->ref), &dodag_get_key(dodag)->ref, NULL, HAM_OverwriteIfExists, NULL);
	dodag_set_rpl_instance(dodag, &rpl_instance->key.ref);
}

void rpl_instance_del_dodag(di_rpl_instance_t* rpl_instance, di_dodag_t *dodag) {
	static const di_rpl_instance_ref_t null_rpl_instance = {-1};

	hash_delete(rpl_instance->dodags, hash_key_make(dodag_get_key(dodag)->ref));
	dodag_set_rpl_instance(dodag, &null_rpl_instance);
}


const di_rpl_instance_key_t* rpl_instance_get_key(const di_rpl_instance_t* rpl_instance) {
	return &rpl_instance->key;
}

di_rpl_mop_e rpl_instance_get_mop(const di_rpl_instance_t* rpl_instance) {
	return rpl_instance->mode_of_operation;
}

void *rpl_instance_get_user_data(const di_rpl_instance_t* rpl_instance) {
	return rpl_instance->user_data;
}
