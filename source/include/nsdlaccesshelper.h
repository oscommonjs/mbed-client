/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef NSDL_ACCESS_HELPER_H
#define NSDL_ACCESS_HELPER_H

#include "m2mnsdlinterface.h"

extern M2MNsdlInterface  *__nsdl_interface;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t __nsdl_c_callback(struct nsdl_s * nsdl_handle,
                          sn_coap_hdr_s *received_coap_ptr,
                          sn_nsdl_addr_s *address,
                          sn_nsdl_capab_e nsdl_capab);
void *__nsdl_c_memory_alloc(uint16_t size);
void __nsdl_c_memory_free(void *ptr);
uint8_t __nsdl_c_send_to_server(struct nsdl_s * nsdl_handle,
                                sn_nsdl_capab_e protocol,
                                uint8_t *data_ptr,
                                uint16_t data_len,
                                sn_nsdl_addr_s *address_ptr);
uint8_t __nsdl_c_received_from_server(struct nsdl_s * nsdl_handle,
                                      sn_coap_hdr_s *coap_header,
                                      sn_nsdl_addr_s *address_ptr);
void __nsdl_c_bootstrap_done(sn_nsdl_oma_server_info_t *server_info_ptr);
void *__socket_malloc( void * context, size_t size);
void __socket_free(void * context, void * ptr);

#ifdef __cplusplus
}
#endif

#endif // NSDL_ACCESS_HELPER_H
