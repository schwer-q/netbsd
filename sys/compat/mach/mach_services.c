/* $NetBSD: mach_services.c,v 1.4 2003/11/15 22:55:35 manu Exp $ */

/*
 * Mach services table.
 *
 * DO NOT EDIT -- this file is automatically generated.
 * created from  $NetBSD: mach_services.c,v 1.4 2003/11/15 22:55:35 manu Exp $
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: mach_services.c,v 1.4 2003/11/15 22:55:35 manu Exp $");

#include <sys/types.h>
#include <sys/param.h>
#include <compat/mach/mach_types.h>
#include <compat/mach/mach_message.h>
#include <compat/mach/mach_bootstrap.h>
#include <compat/mach/mach_iokit.h>
#include <compat/mach/mach_clock.h>
#include <compat/mach/mach_host.h>
#include <compat/mach/mach_port.h>
#include <compat/mach/mach_task.h>
#include <compat/mach/mach_thread.h>
#include <compat/mach/mach_semaphore.h>
#include <compat/mach/mach_vm.h>
#include <compat/mach/mach_services.h>

struct mach_service mach_services_table[] = {
	{64, NULL, "obsolete notify_first", 0, 0},
	{65, NULL, "unimpl. notify_port_deleted", 0, 0},
	{66, NULL, "obsolete notify_msg_accepted", 0, 0},
	{67, NULL, "obsolete notify_ownership_rights", 0, 0},
	{68, NULL, "obsolete notify_receive_rights", 0, 0},
	{69, NULL, "unimpl. notify_port_destroyed", 0, 0},
	{70, NULL, "unimpl. notify_port_no_senders", 0, 0},
	{71, NULL, "unimpl. notify_port_send_once", 0, 0},
	{72, NULL, "unimpl. notify_port_dead_name", 0, 0},
	{200, mach_host_info, "host_info", sizeof(mach_host_info_request_t), sizeof(mach_host_info_reply_t)},
	{201, NULL, "unimpl. host_kernel_version", 0, 0},
	{202, mach_host_page_size, "host_page_size", sizeof(mach_host_page_size_request_t), sizeof(mach_host_page_size_reply_t)},
	{203, NULL, "unimpl. memory_object_memory_entry", 0, 0},
	{204, NULL, "unimpl. host_processor_info", 0, 0},
	{205, mach_host_get_io_master, "host_get_io_master", sizeof(mach_host_get_io_master_request_t), sizeof(mach_host_get_io_master_reply_t)},
	{206, mach_host_get_clock_service, "host_get_clock_service", sizeof(mach_host_get_clock_service_request_t), sizeof(mach_host_get_clock_service_reply_t)},
	{207, NULL, "unimpl. kmod_get_info", 0, 0},
	{208, NULL, "unimpl. host_zone_info", 0, 0},
	{209, NULL, "unimpl. host_virtual_physical_table_info", 0, 0},
	{210, NULL, "unimpl. host_ipc_hash_info", 0, 0},
	{211, NULL, "unimpl. enable_bluebox", 0, 0},
	{212, NULL, "unimpl. disable_bluebox", 0, 0},
	{213, NULL, "unimpl. processor_set_default", 0, 0},
	{214, NULL, "unimpl. processor_set_create", 0, 0},
	{215, NULL, "unimpl. memory_object_memory_entry_64", 0, 0},
	{216, NULL, "unimpl. host_statistics", 0, 0},
	{400, NULL, "unimpl. host_get_boot_info", 0, 0},
	{401, NULL, "unimpl. host_reboot", 0, 0},
	{402, NULL, "unimpl. host_priv_statistics", 0, 0},
	{403, NULL, "unimpl. host_default_memory_manager", 0, 0},
	{404, mach_bootstrap_look_up, "bootstrap_look_up", sizeof(mach_bootstrap_look_up_request_t), sizeof(mach_bootstrap_look_up_reply_t)},
	{405, NULL, "unimpl. thread_wire", 0, 0},
	{406, NULL, "unimpl. vm_allocate_cpm", 0, 0},
	{407, NULL, "unimpl. host_processors", 0, 0},
	{408, NULL, "unimpl. host_get_clock_control", 0, 0},
	{409, NULL, "unimpl. kmod_create", 0, 0},
	{410, NULL, "unimpl. kmod_destroy", 0, 0},
	{411, NULL, "unimpl. kmod_control", 0, 0},
	{412, NULL, "unimpl. host_get_special_port", 0, 0},
	{413, NULL, "unimpl. host_set_special_port", 0, 0},
	{414, NULL, "unimpl. host_set_exception_ports", 0, 0},
	{415, NULL, "unimpl. host_get_exception_ports", 0, 0},
	{416, NULL, "unimpl. host_swap_exception_ports", 0, 0},
	{417, NULL, "unimpl. host_load_symbol_table", 0, 0},
	{418, NULL, "unimpl. task_swappable", 0, 0},
	{419, NULL, "unimpl. host_processor_sets", 0, 0},
	{420, NULL, "unimpl. host_processor_set_priv", 0, 0},
	{421, NULL, "unimpl. set_dp_control_port", 0, 0},
	{422, NULL, "unimpl. get_dp_control_port", 0, 0},
	{423, NULL, "unimpl. host_set_UNDServer", 0, 0},
	{424, NULL, "unimpl. host_get_UNDServer", 0, 0},
	{600, NULL, "unimpl. host_security_create_task_token", 0, 0},
	{601, NULL, "unimpl. host_security_set_task_token", 0, 0},
	{1000, mach_clock_get_time, "clock_get_time", sizeof(mach_clock_get_time_request_t), sizeof(mach_clock_get_time_reply_t)},
	{1001, NULL, "unimpl. clock_get_attributes", 0, 0},
	{1002, NULL, "unimpl. clock_alarm", 0, 0},
	{1200, NULL, "unimpl. clock_set_time", 0, 0},
	{1201, NULL, "unimpl. clock_set_attributes", 0, 0},
	{2000, NULL, "unimpl. memory_object_get_attributes", 0, 0},
	{2001, NULL, "unimpl. memory_object_change_attributes", 0, 0},
	{2002, NULL, "unimpl. memory_object_synchronize_completed", 0, 0},
	{2003, NULL, "unimpl. memory_object_lock_request", 0, 0},
	{2004, NULL, "unimpl. memory_object_destroy", 0, 0},
	{2005, NULL, "unimpl. memory_object_upl_request", 0, 0},
	{2006, NULL, "unimpl. memory_object_super_upl_request", 0, 0},
	{2007, NULL, "unimpl. memory_object_page_op", 0, 0},
	{2008, NULL, "unimpl. memory_object_recover_named", 0, 0},
	{2009, NULL, "unimpl. memory_object_release_name", 0, 0},
	{2050, NULL, "unimpl. upl_abort", 0, 0},
	{2051, NULL, "unimpl. upl_abort_range", 0, 0},
	{2052, NULL, "unimpl. upl_commit", 0, 0},
	{2053, NULL, "unimpl. upl_commit_range", 0, 0},
	{2200, NULL, "unimpl. memory_object_init", 0, 0},
	{2201, NULL, "unimpl. memory_object_terminate", 0, 0},
	{2202, NULL, "unimpl. memory_object_data_request", 0, 0},
	{2203, NULL, "unimpl. memory_object_data_return", 0, 0},
	{2204, NULL, "unimpl. memory_object_data_initialize", 0, 0},
	{2205, NULL, "unimpl. memory_object_data_unlock", 0, 0},
	{2206, NULL, "unimpl. memory_object_synchronize", 0, 0},
	{2207, NULL, "unimpl. memory_object_unmap", 0, 0},
	{2250, NULL, "unimpl. memory_object_create", 0, 0},
	{2275, NULL, "unimpl. default_pager_object_create", 0, 0},
	{2276, NULL, "unimpl. default_pager_info", 0, 0},
	{2277, NULL, "unimpl. default_pager_objects", 0, 0},
	{2278, NULL, "unimpl. default_pager_object_pages", 0, 0},
	{2279, NULL, "unimpl. default_pager_backing_store_create", 0, 0},
	{2280, NULL, "unimpl. default_pager_backing_store_delete", 0, 0},
	{2281, NULL, "unimpl. default_pager_add_segment", 0, 0},
	{2282, NULL, "unimpl. default_pager_backing_store_info", 0, 0},
	{2283, NULL, "unimpl. default_pager_add_file", 0, 0},
	{2284, NULL, "unimpl. default_pager_triggers", 0, 0},
	{2295, NULL, "unimpl. default_pager_space_alert", 0, 0},
	{2401, NULL, "unimpl. exception_raise", 0, 0},
	{2402, NULL, "unimpl. exception_raise_state", 0, 0},
	{2403, NULL, "unimpl. exception_raise_state_identity", 0, 0},
	{2450, NULL, "unimpl. samples", 0, 0},
	{2451, NULL, "unimpl. notices", 0, 0},
	{2800, mach_io_object_get_class, "io_object_get_class", sizeof(mach_io_object_get_class_request_t), sizeof(mach_io_object_get_class_reply_t)},
	{2801, mach_io_object_conforms_to, "io_object_conforms_to", sizeof(mach_io_object_conforms_to_request_t), sizeof(mach_io_object_conforms_to_reply_t)},
	{2802, mach_io_iterator_next, "io_iterator_next", sizeof(mach_io_iterator_next_request_t), sizeof(mach_io_iterator_next_reply_t)},
	{2803, mach_io_iterator_reset, "io_iterator_reset", sizeof(mach_io_iterator_reset_request_t), sizeof(mach_io_iterator_reset_reply_t)},
	{2804, mach_io_service_get_matching_services, "io_service_get_matching_services", sizeof(mach_io_service_get_matching_services_request_t), sizeof(mach_io_service_get_matching_services_reply_t)},
	{2805, mach_io_registry_entry_get_property, "io_registry_entry_get_property", sizeof(mach_io_registry_entry_get_property_request_t), sizeof(mach_io_registry_entry_get_property_reply_t)},
	{2806, NULL, "unimpl. io_registry_create_iterator", 0, 0},
	{2807, NULL, "unimpl. io_registry_iterator_enter_entry", 0, 0},
	{2808, NULL, "unimpl. io_registry_iterator_exit_entry", 0, 0},
	{2809, mach_io_registry_entry_from_path, "io_registry_entry_from_path", sizeof(mach_io_registry_entry_from_path_request_t), sizeof(mach_io_registry_entry_from_path_reply_t)},
	{2810, NULL, "unimpl. io_registry_entry_get_name", 0, 0},
	{2811, mach_io_registry_entry_get_properties, "io_registry_entry_get_properties", sizeof(mach_io_registry_entry_get_properties_request_t), sizeof(mach_io_registry_entry_get_properties_reply_t)},
	{2812, NULL, "unimpl. io_registry_entry_get_property_bytes", 0, 0},
	{2813, mach_io_registry_entry_get_child_iterator, "io_registry_entry_get_child_iterator", sizeof(mach_io_registry_entry_get_child_iterator_request_t), sizeof(mach_io_registry_entry_get_child_iterator_reply_t)},
	{2814, mach_io_registry_entry_get_parent_iterator, "io_registry_entry_get_parent_iterator", sizeof(mach_io_registry_entry_get_parent_iterator_request_t), sizeof(mach_io_registry_entry_get_parent_iterator_reply_t)},
	{2815, mach_io_service_open, "io_service_open", sizeof(mach_io_service_open_request_t), sizeof(mach_io_service_open_reply_t)},
	{2816, mach_io_service_close, "io_service_close", sizeof(mach_io_service_close_request_t), sizeof(mach_io_service_close_reply_t)},
	{2817, mach_io_connect_get_service, "io_connect_get_service", sizeof(mach_io_connect_get_service_request_t), sizeof(mach_io_connect_get_service_reply_t)},
	{2818, mach_io_connect_set_notification_port, "io_connect_set_notification_port", sizeof(mach_io_connect_set_notification_port_request_t), sizeof(mach_io_connect_set_notification_port_reply_t)},
	{2819, mach_io_connect_map_memory, "io_connect_map_memory", sizeof(mach_io_connect_map_memory_request_t), sizeof(mach_io_connect_map_memory_reply_t)},
	{2820, mach_io_connect_add_client, "io_connect_add_client", sizeof(mach_io_connect_add_client_request_t), sizeof(mach_io_connect_add_client_reply_t)},
	{2821, mach_io_connect_set_properties, "io_connect_set_properties", sizeof(mach_io_connect_set_properties_request_t), sizeof(mach_io_connect_set_properties_reply_t)},
	{2822, mach_io_connect_method_scalari_scalaro, "io_connect_method_scalari_scalaro", sizeof(mach_io_connect_method_scalari_scalaro_request_t), sizeof(mach_io_connect_method_scalari_scalaro_reply_t)},
	{2823, mach_io_connect_method_scalari_structo, "io_connect_method_scalari_structo", sizeof(mach_io_connect_method_scalari_structo_request_t), sizeof(mach_io_connect_method_scalari_structo_reply_t)},
	{2824, mach_io_connect_method_scalari_structi, "io_connect_method_scalari_structi", sizeof(mach_io_connect_method_scalari_structi_request_t), sizeof(mach_io_connect_method_scalari_structi_reply_t)},
	{2825, mach_io_connect_method_structi_structo, "io_connect_method_structi_structo", sizeof(mach_io_connect_method_structi_structo_request_t), sizeof(mach_io_connect_method_structi_structo_reply_t)},
	{2826, mach_io_registry_entry_get_path, "io_registry_entry_get_path", sizeof(mach_io_registry_entry_get_path_request_t), sizeof(mach_io_registry_entry_get_path_reply_t)},
	{2827, mach_io_registry_get_root_entry, "io_registry_get_root_entry", sizeof(mach_io_registry_get_root_entry_request_t), sizeof(mach_io_registry_get_root_entry_reply_t)},
	{2828, NULL, "unimpl. io_registry_entry_set_properties", 0, 0},
	{2829, NULL, "unimpl. io_registry_entry_in_plane", 0, 0},
	{2830, NULL, "unimpl. io_object_get_retain_count", 0, 0},
	{2831, NULL, "unimpl. io_service_get_busy_state", 0, 0},
	{2832, NULL, "unimpl. io_service_wait_quiet", 0, 0},
	{2833, mach_io_registry_entry_create_iterator, "io_registry_entry_create_iterator", sizeof(mach_io_registry_entry_create_iterator_request_t), sizeof(mach_io_registry_entry_create_iterator_reply_t)},
	{2834, NULL, "unimpl. io_iterator_is_valid", 0, 0},
	{2835, NULL, "unimpl. io_make_matching", 0, 0},
	{2836, NULL, "unimpl. io_catalog_send_data", 0, 0},
	{2837, NULL, "unimpl. io_catalog_terminate", 0, 0},
	{2838, NULL, "unimpl. io_catalog_get_data", 0, 0},
	{2839, NULL, "unimpl. io_catalog_get_gen_count", 0, 0},
	{2840, NULL, "unimpl. io_catalog_module_loaded", 0, 0},
	{2841, NULL, "unimpl. io_catalog_reset", 0, 0},
	{2842, NULL, "unimpl. io_service_request_probe", 0, 0},
	{2843, mach_io_registry_entry_get_name_in_plane, "io_registry_entry_get_name_in_plane", sizeof(mach_io_registry_entry_get_name_in_plane_request_t), sizeof(mach_io_registry_entry_get_name_in_plane_reply_t)},
	{2844, NULL, "unimpl. io_service_match_property_table", 0, 0},
	{2845, NULL, "unimpl. io_async_method_scalari_scalaro", 0, 0},
	{2846, NULL, "unimpl. io_async_method_scalari_structo", 0, 0},
	{2847, NULL, "unimpl. io_async_method_scalari_structi", 0, 0},
	{2848, NULL, "unimpl. io_async_method_structi_structo", 0, 0},
	{2849, NULL, "unimpl. io_service_add_notification", 0, 0},
	{2850, mach_io_service_add_interest_notification, "io_service_add_interest_notification", sizeof(mach_io_service_add_interest_notification_request_t), sizeof(mach_io_service_add_interest_notification_reply_t)},
	{2851, NULL, "unimpl. io_service_acknowledge_notification", 0, 0},
	{2852, NULL, "unimpl. io_connect_get_notification_semaphore", 0, 0},
	{2853, NULL, "unimpl. io_connect_unmap_memory", 0, 0},
	{2854, mach_io_registry_entry_get_location_in_plane, "io_registry_entry_get_location_in_plane", sizeof(mach_io_registry_entry_get_location_in_plane_request_t), sizeof(mach_io_registry_entry_get_location_in_plane_reply_t)},
	{2855, NULL, "unimpl. io_registry_entry_get_property_recursively", 0, 0},
	{3000, NULL, "unimpl. processor_start", 0, 0},
	{3001, NULL, "unimpl. processor_exit", 0, 0},
	{3002, NULL, "unimpl. processor_info", 0, 0},
	{3003, NULL, "unimpl. processor_control", 0, 0},
	{3004, NULL, "unimpl. processor_assign", 0, 0},
	{3005, NULL, "unimpl. processor_get_assignment", 0, 0},
	{3200, NULL, "unimpl. port_names", 0, 0},
	{3201, mach_port_type, "port_type", sizeof(mach_port_type_request_t), sizeof(mach_port_type_reply_t)},
	{3202, NULL, "unimpl. port_rename", 0, 0},
	{3203, NULL, "unimpl. port_allocate_name", 0, 0},
	{3204, mach_port_allocate, "port_allocate", sizeof(mach_port_allocate_request_t), sizeof(mach_port_allocate_reply_t)},
	{3205, mach_port_destroy, "port_destroy", sizeof(mach_port_destroy_request_t), sizeof(mach_port_destroy_reply_t)},
	{3206, mach_port_deallocate, "port_deallocate", sizeof(mach_port_deallocate_request_t), sizeof(mach_port_deallocate_reply_t)},
	{3207, NULL, "unimpl. port_get_refs", 0, 0},
	{3208, NULL, "unimpl. port_mod_refs", 0, 0},
	{3210, NULL, "unimpl. port_set_mscount", 0, 0},
	{3211, NULL, "unimpl. port_get_set_status", 0, 0},
	{3212, mach_port_move_member, "port_move_member", sizeof(mach_port_move_member_request_t), sizeof(mach_port_move_member_reply_t)},
	{3213, mach_port_request_notification, "port_request_notification", sizeof(mach_port_request_notification_request_t), sizeof(mach_port_request_notification_reply_t)},
	{3214, mach_port_insert_right, "port_insert_right", sizeof(mach_port_insert_right_request_t), sizeof(mach_port_insert_right_reply_t)},
	{3215, NULL, "unimpl. port_extract_right", 0, 0},
	{3216, NULL, "unimpl. port_set_seqno", 0, 0},
	{3217, mach_port_get_attributes, "port_get_attributes", sizeof(mach_port_get_attributes_request_t), sizeof(mach_port_get_attributes_reply_t)},
	{3218, mach_port_set_attributes, "port_set_attributes", sizeof(mach_port_set_attributes_request_t), sizeof(mach_port_set_attributes_reply_t)},
	{3219, NULL, "unimpl. port_allocate_qos", 0, 0},
	{3220, NULL, "unimpl. port_allocate_full", 0, 0},
	{3221, NULL, "unimpl. task_set_port_space", 0, 0},
	{3222, NULL, "unimpl. port_get_srights", 0, 0},
	{3223, NULL, "unimpl. port_space_info", 0, 0},
	{3224, NULL, "unimpl. port_dnrequest_info", 0, 0},
	{3225, NULL, "unimpl. port_kernel_object", 0, 0},
	{3226, mach_port_insert_member, "port_insert_member", sizeof(mach_port_insert_member_request_t), sizeof(mach_port_insert_member_reply_t)},
	{3227, NULL, "unimpl. port_extract_member", 0, 0},
	{3400, NULL, "unimpl. task_create", 0, 0},
	{3401, NULL, "unimpl. task_terminate", 0, 0},
	{3402, mach_task_threads, "task_threads", sizeof(mach_task_threads_request_t), sizeof(mach_task_threads_reply_t)},
	{3403, NULL, "unimpl. ports_register", 0, 0},
	{3404, mach_ports_lookup, "ports_lookup", sizeof(mach_ports_lookup_request_t), sizeof(mach_ports_lookup_reply_t)},
	{3405, mach_task_info, "task_info", sizeof(mach_task_info_request_t), sizeof(mach_task_info_reply_t)},
	{3406, NULL, "unimpl. task_set_info", 0, 0},
	{3407, mach_task_suspend, "task_suspend", sizeof(mach_task_suspend_request_t), sizeof(mach_task_suspend_reply_t)},
	{3408, mach_task_resume, "task_resume", sizeof(mach_task_resume_request_t), sizeof(mach_task_resume_reply_t)},
	{3409, mach_task_get_special_port, "task_get_special_port", sizeof(mach_task_get_special_port_request_t), sizeof(mach_task_get_special_port_reply_t)},
	{3410, mach_task_set_special_port, "task_set_special_port", sizeof(mach_task_set_special_port_request_t), sizeof(mach_task_set_special_port_reply_t)},
	{3411, NULL, "unimpl. thread_create", 0, 0},
	{3412, mach_thread_create_running, "thread_create_running", sizeof(mach_thread_create_running_request_t), sizeof(mach_thread_create_running_reply_t)},
	{3413, mach_task_set_exception_ports, "task_set_exception_ports", sizeof(mach_task_set_exception_ports_request_t), sizeof(mach_task_set_exception_ports_reply_t)},
	{3414, mach_task_get_exception_ports, "task_get_exception_ports", sizeof(mach_task_get_exception_ports_request_t), sizeof(mach_task_get_exception_ports_reply_t)},
	{3415, NULL, "unimpl. task_swap_exception_ports", 0, 0},
	{3416, NULL, "unimpl. lock_set_create", 0, 0},
	{3417, NULL, "unimpl. lock_set_destroy", 0, 0},
	{3418, mach_semaphore_create, "semaphore_create", sizeof(mach_semaphore_create_request_t), sizeof(mach_semaphore_create_reply_t)},
	{3419, mach_semaphore_destroy, "semaphore_destroy", sizeof(mach_semaphore_destroy_request_t), sizeof(mach_semaphore_destroy_reply_t)},
	{3420, NULL, "unimpl. task_policy_set", 0, 0},
	{3421, NULL, "unimpl. task_policy_get", 0, 0},
	{3422, NULL, "unimpl. task_sample", 0, 0},
	{3423, NULL, "unimpl. task_policy", 0, 0},
	{3424, NULL, "unimpl. task_set_emulation", 0, 0},
	{3425, NULL, "unimpl. task_get_emulation_vector", 0, 0},
	{3426, NULL, "unimpl. task_set_emulation_vector", 0, 0},
	{3427, NULL, "unimpl. task_set_ras_pc", 0, 0},
	{3428, NULL, "unimpl. kernel_task_create", 0, 0},
	{3429, NULL, "unimpl. task_assign", 0, 0},
	{3430, NULL, "unimpl. task_assign_default", 0, 0},
	{3431, NULL, "unimpl. task_get_assignment", 0, 0},
	{3432, NULL, "unimpl. task_set_policy", 0, 0},
	{3600, NULL, "unimpl. thread_terminate", 0, 0},
	{3601, NULL, "unimpl. act_get_state", 0, 0},
	{3602, NULL, "unimpl. act_set_state", 0, 0},
	{3603, mach_thread_get_state, "thread_get_state", sizeof(mach_thread_get_state_request_t), sizeof(mach_thread_get_state_reply_t)},
	{3604, mach_thread_set_state, "thread_set_state", sizeof(mach_thread_set_state_request_t), sizeof(mach_thread_set_state_reply_t)},
	{3605, NULL, "unimpl. thread_suspend", 0, 0},
	{3606, NULL, "unimpl. thread_resume", 0, 0},
	{3607, NULL, "unimpl. thread_abort", 0, 0},
	{3608, NULL, "unimpl. thread_abort_safely", 0, 0},
	{3609, NULL, "unimpl. thread_depress_abort", 0, 0},
	{3610, NULL, "unimpl. thread_get_special_port", 0, 0},
	{3611, NULL, "unimpl. thread_set_special_port", 0, 0},
	{3612, mach_thread_info, "thread_info", sizeof(mach_thread_info_request_t), sizeof(mach_thread_info_reply_t)},
	{3613, NULL, "unimpl. thread_set_exception_ports", 0, 0},
	{3614, NULL, "unimpl. thread_get_exception_ports", 0, 0},
	{3615, NULL, "unimpl. thread_swap_exception_ports", 0, 0},
	{3616, mach_thread_policy, "thread_policy", sizeof(mach_thread_policy_request_t), sizeof(mach_thread_policy_reply_t)},
	{3617, NULL, "unimpl. thread_policy_set", 0, 0},
	{3618, NULL, "unimpl. thread_policy_get", 0, 0},
	{3619, NULL, "unimpl. thread_sample", 0, 0},
	{3620, NULL, "unimpl. etap_trace_thread", 0, 0},
	{3621, NULL, "unimpl. thread_assign", 0, 0},
	{3622, NULL, "unimpl. thread_assign_default", 0, 0},
	{3623, NULL, "unimpl. thread_get_assignment", 0, 0},
	{3624, NULL, "unimpl. thread_set_policy", 0, 0},
	{3800, mach_vm_region, "vm_region", sizeof(mach_vm_region_request_t), sizeof(mach_vm_region_reply_t)},
	{3801, mach_vm_allocate, "vm_allocate", sizeof(mach_vm_allocate_request_t), sizeof(mach_vm_allocate_reply_t)},
	{3802, mach_vm_deallocate, "vm_deallocate", sizeof(mach_vm_deallocate_request_t), sizeof(mach_vm_deallocate_reply_t)},
	{3803, mach_vm_protect, "vm_protect", sizeof(mach_vm_protect_request_t), sizeof(mach_vm_protect_reply_t)},
	{3804, mach_vm_inherit, "vm_inherit", sizeof(mach_vm_inherit_request_t), sizeof(mach_vm_inherit_reply_t)},
	{3805, NULL, "unimpl. vm_read", 0, 0},
	{3806, NULL, "unimpl. vm_read_list", 0, 0},
	{3807, NULL, "unimpl. vm_write", 0, 0},
	{3808, mach_vm_copy, "vm_copy", sizeof(mach_vm_copy_request_t), sizeof(mach_vm_copy_reply_t)},
	{3809, NULL, "unimpl. vm_read_overwrite", 0, 0},
	{3810, mach_vm_msync, "vm_msync", sizeof(mach_vm_msync_request_t), sizeof(mach_vm_msync_reply_t)},
	{3811, NULL, "unimpl. vm_behavior_set", 0, 0},
	{3812, mach_vm_map, "vm_map", sizeof(mach_vm_map_request_t), sizeof(mach_vm_map_reply_t)},
	{3813, NULL, "unimpl. vm_machine_attribute", 0, 0},
	{3814, NULL, "unimpl. vm_remap", 0, 0},
	{3815, NULL, "unimpl. task_wire", 0, 0},
	{3816, NULL, "unimpl. make_memory_entry", 0, 0},
	{3817, NULL, "unimpl. vm_map_page_query", 0, 0},
	{3818, NULL, "unimpl. vm_region_info", 0, 0},
	{3819, NULL, "unimpl. vm_mapped_pages_info", 0, 0},
	{3820, NULL, "unimpl. vm_region_object_create", 0, 0},
	{3821, NULL, "unimpl. vm_region_recurse", 0, 0},
	{3822, NULL, "unimpl. vm_region_recurse_64", 0, 0},
	{3823, NULL, "unimpl. vm_region_info_64", 0, 0},
	{3824, mach_vm_region_64, "vm_region_64", sizeof(mach_vm_region_64_request_t), sizeof(mach_vm_region_64_reply_t)},
	{3825, mach_make_memory_entry_64, "make_memory_entry_64", sizeof(mach_make_memory_entry_64_request_t), sizeof(mach_make_memory_entry_64_reply_t)},
	{3826, NULL, "unimpl. vm_map_64", 0, 0},
	{3827, NULL, "unimpl. vm_map_get_upl", 0, 0},
	{3828, NULL, "unimpl. vm_upl_map", 0, 0},
	{3829, NULL, "unimpl. vm_upl_unmap", 0, 0},
	{4000, NULL, "unimpl. processor_set_statistics", 0, 0},
	{4001, NULL, "unimpl. processor_set_destroy", 0, 0},
	{4002, NULL, "unimpl. processor_set_max_priority", 0, 0},
	{4003, NULL, "unimpl. processor_set_policy_enable", 0, 0},
	{4004, NULL, "unimpl. processor_set_policy_disable", 0, 0},
	{4005, NULL, "unimpl. processor_set_tasks", 0, 0},
	{4006, NULL, "unimpl. processor_set_threads", 0, 0},
	{4007, NULL, "unimpl. processor_set_policy_control", 0, 0},
	{4008, NULL, "unimpl. processor_set_stack_usage", 0, 0},
	{4009, NULL, "unimpl. processor_set_info", 0, 0},
	{5000, NULL, "unimpl. ledger_create", 0, 0},
	{5001, NULL, "unimpl. ledger_terminate", 0, 0},
	{5002, NULL, "unimpl. ledger_transfer", 0, 0},
	{5003, NULL, "unimpl. ledger_read", 0, 0},
	{6000, NULL, "unimpl. und_execute_rpc", 0, 0},
	{6001, NULL, "unimpl. und_display_notice_from_bundle_rpc", 0, 0},
	{6002, NULL, "unimpl. und_display_alert_from_bundle_rpc", 0, 0},
	{6003, NULL, "unimpl. und_display_custom_from_bundle_rpc", 0, 0},
	{6004, NULL, "unimpl. und_display_custom_from_dictionary_rpc", 0, 0},
	{6005, NULL, "unimpl. und_cancel_notification_rpc", 0, 0},
	{6006, NULL, "unimpl. und_display_notice_simple_rpc", 0, 0},
	{6007, NULL, "unimpl. und_display_alert_simple_rpc", 0, 0},
	{6200, NULL, "unimpl. und_alert_completed_with_result_rpc", 0, 0},
	{6201, NULL, "unimpl. und_notification_created_rpc(", 0, 0},
	{555001, NULL, "unimpl. task_set_child_node", 0, 0},
	{555002, NULL, "unimpl. norma_node_self", 0, 0},
	{555005, NULL, "unimpl. norma_task_clone", 0, 0},
	{555006, NULL, "unimpl. norma_task_create", 0, 0},
	{555007, NULL, "unimpl. norma_get_special_port", 0, 0},
	{555008, NULL, "unimpl. norma_set_special_port", 0, 0},
	{555009, NULL, "unimpl. norma_task_teleport", 0, 0},
	{555012, NULL, "unimpl. norma_port_location_hint", 0, 0},
	{617000, NULL, "unimpl. lock_acquire", 0, 0},
	{617001, NULL, "unimpl. lock_release", 0, 0},
	{617002, NULL, "unimpl. lock_try", 0, 0},
	{617003, NULL, "unimpl. lock_make_stable", 0, 0},
	{617004, NULL, "unimpl. lock_handoff", 0, 0},
	{617005, NULL, "unimpl. lock_handoff_accept", 0, 0},
	{617005, NULL, "unimpl. lock_set_create", 0, 0},
	{617006, NULL, "unimpl. lock_set_destroy", 0, 0},
	{617007, NULL, "unimpl. lock_acquire", 0, 0},
	{617008, NULL, "unimpl. lock_release", 0, 0},
	{617009, NULL, "unimpl. lock_try", 0, 0},
	{617010, NULL, "unimpl. lock_make_stable", 0, 0},
	{617011, NULL, "unimpl. lock_handoff", 0, 0},
	{617012, NULL, "unimpl. lock_handoff_accept", 0, 0},
	{617200, NULL, "unimpl. semaphore_signal", 0, 0},
	{617201, NULL, "unimpl. semaphore_signal_all", 0, 0},
	{617202, NULL, "unimpl. semaphore_wait", 0, 0},
	{617203, NULL, "unimpl. semaphore_signal_thread", 0, 0},
	{617204, NULL, "unimpl. semaphore_timedwait", 0, 0},
	{617205, NULL, "unimpl. semaphore_wait_signal", 0, 0},
	{617206, NULL, "unimpl. semaphore_timedwait_signal", 0, 0},
	{0, NULL, NULL, 0, 0}
};
