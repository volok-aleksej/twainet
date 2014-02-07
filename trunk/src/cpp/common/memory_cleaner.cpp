#include <typeinfo>
#include <cstdlib>

#ifdef WIN32
#	include <windows.h>
#endif/*WIN32*/

#ifdef SUPPORT_SSL
#	include <openssl/ssl.h>
#	include <openssl/err.h>
#	include <openssl/conf.h>
#	include <openssl/engine.h>
#endif/*SUPPORT_SSL*/

void clear_type_info_cache()
{
	__type_info_node* & node = __type_info_root_node.next;
	while(node)
	{
		if (node->memPtr)
		{
			delete node->memPtr;
		}
		__type_info_node* tempNode = node;
		node = node->next;
		delete tempNode;
	}
}

#ifdef SUPPORT_SSL
void clear_ssl()
{
	ERR_remove_state(0);
	ENGINE_cleanup();
	CONF_modules_unload(1);
	ERR_free_strings();
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
}

#endif/*SUPPORT_SSL*/