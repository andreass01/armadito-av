#include "jsonhandler.h"
#include <stdio.h>
#include "libuhuru-config.h"
#include <libuhuru/core.h>
#include <json.h>
#include "conf.h"
#include <Windows.h>




json_object * conf_change_entry() {

	json_object * jfiles = NULL;
	return jfiles;
}

enum uhuru_json_status conf_response_cb(struct uhuru *uhuru, struct json_request *req, struct json_response *resp, void **request_data) {

	enum uhuru_json_status status = JSON_OK;		
	json_object * jsection = NULL;
	json_object * jkey = NULL;
	json_object * jvalue = NULL;
	char * section = NULL;
	char * key = NULL;	
	char * val_str = NULL;
	int val_int = 0,prev = 0; 
	int len = 0;
	struct uhuru_conf * conf = NULL;

	// TODO
	printf("[+] Debug :: conf_response_cb...\n");

	// Get conf struct
	conf = uhuru_get_conf(uhuru);
	if (conf == NULL) {
		uhuru_log(UHURU_LOG_LIB, UHURU_LOG_LEVEL_ERROR, "[-] Error :: conf_response_cb :: invalid configuration !\n");
		return JSON_UNEXPECTED_ERR;		
	}
	
	// Get the section key and value.
	json_object_object_get_ex(req->params, "section", &jsection);
	if (!json_object_is_type(jsection, json_type_string)) {		
		uhuru_log(UHURU_LOG_LIB, UHURU_LOG_LEVEL_ERROR, "[-] Error :: conf_response_cb :: bad object type [section] :: \n");
		return JSON_INVALID_REQUEST;
	}
	section = json_object_get_string(jsection);

	json_object_object_get_ex(req->params, "key", &jkey);
	if (!json_object_is_type(jkey, json_type_string)) {
		uhuru_log(UHURU_LOG_LIB,UHURU_LOG_LEVEL_ERROR,"[-] Error :: conf_response_cb :: bad object type [key] :: \n");
		return JSON_INVALID_REQUEST;
	}
	key = json_object_get_string(jkey);

	json_object_object_get_ex(req->params, "value", &jvalue);	


	if (json_object_is_type(jvalue, json_type_string)) {

		val_str = json_object_to_json_string(jvalue);
		if (uhuru_conf_set_string(conf, section, key, val_str) != 0) {
			uhuru_log(UHURU_LOG_LIB, UHURU_LOG_LEVEL_ERROR, "[-] Error :: conf_response_cb :: setting key=%s :: value=%s in section [%s] failed!\n",key,val_str,section);
			status = JSON_UNEXPECTED_ERR;
			goto cleanup;
		}
		printf("[+] Debug :: conf_response_cb :: key=%s :: value=%s in section [%s]\n",key,val_str,section);

	}
	else if (json_object_is_type(jvalue, json_type_int)) {

		prev = uhuru_conf_get_uint(conf,section,key);
		printf("[+] Debug :: conf_response_cb :: key=%s :: oldvalue=%d in section [%s]\n",key,prev,section);

		val_int = json_object_get_int(jvalue);
		if (uhuru_conf_set_uint(conf, section, key, val_int) != 0) {
			uhuru_log(UHURU_LOG_LIB, UHURU_LOG_LEVEL_ERROR, "[-] Error :: conf_response_cb :: setting key=%s :: value=%d in section [%s] failed!\n",key,val_int,section);
			status = JSON_UNEXPECTED_ERR;
			goto cleanup;
		}
		printf("[+] Debug :: conf_response_cb :: key=%s :: value=%d in section [%s]\n",key,val_int,section);

	}
	else if (json_object_is_type(jvalue, json_type_array)) {
		// TODO...
	}
	else {
		uhuru_log(UHURU_LOG_LIB, UHURU_LOG_LEVEL_ERROR, "[-] Error :: conf_response_cb :: setting key=%s in section [%s] failed :: Bad value type.\n",key,section);
		status = JSON_UNEXPECTED_ERR;
		goto cleanup;
	}
	
	printf("[+] Debug :: conf_response_cb :: configuration modified successfully!\n");


	cleanup:
		;

	return status;

}
