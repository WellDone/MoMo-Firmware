#define _bus_slave_reset_address 0x4a3
#define _copy_mib_to_boot_address 0x12b
#define _bus_slave_validateparams_address 0x40e
#define _i2c_getstate_address 0x48
#define _exec_verify_address 0x490
#define _bus_slave_setreturn_address 0x4c4
#define _enter_bootloader_address 0x341
#define _bus_master_rpc_sync_address 0x306
#define _call_handler_address 0x211
#define _flash_erase_application_address 0x106
#define _i2c_core_receivechecksum_address 0x5e
#define _i2c_setstate_address 0x40
#define i2c_setup_buffer_address 0x4c
#define read_app_row_address 0xe9
#define i1_i2c_lasterror_address 0x4e2
#define _exec_prepare_reflash_address 0x488
#define _i2c_slave_receive_message_address 0x4d5
#define _get_half_row_address 0x393
#define _get_firmware_id_address 0x125
#define _service_isr_address 0x4
#define _i2c_receive_address 0x51
#define _flash_erase_row_address 0x102
#define _load_boot_address_address 0x113
#define _i2c_slave_interrupt_address 0x24d
#define _app_init_vector_address 0x4f0
#define prepare_row_address_address 0xdd
#define _validate_param_spec_address 0x23e
#define _bus_master_lastaddress_address 0x4dd
#define _bus_slave_seterror_address 0x245
#define _set_firmware_id_address 0x4af
#define _bus_master_finish_address 0x3be
#define _wdt_settimeout_address 0x4ba
#define _plist_param_length_address 0x234
#define _main_address 0x37a
#define _bus_master_readstatus_address 0x42b
#define exec_cmd_map_address 0x474
#define _bus_slave_searchcommand_address 0x283
#define unlock_and_write_address 0xd5
#define i1_wdt_settimeout_address 0x4d1
#define _bus_master_handleerror_address 0x3f0
#define _i2c_finish_transmission_address 0x4bf
#define _wdt_popenabled_address 0x425
#define _bus_slave_startcommand_address 0x3d0
#define _i2c_master_receivedata_address 0x469
#define _get_boot_source_address 0x11f
#define _plist_int_count_address 0x231
#define _app_interrupt_vector_address 0x4f1
#define _restore_status_address 0x324
#define _bus_master_sendrpc_address 0x35e
#define _get_magic_address 0x22e
#define __initialization_address 0x2e
#define _i2c_slave_active_address 0x4d9
#define _app_task_vector_address 0x4f2
#define mib12_api_address 0x4ee
#define _get_feature_address 0x21b
#define _i2c_slave_setidle_address 0x4a9
#define _flash_write_row_address 0x139
#define _check_app_fault_address 0x4b5
#define _i2c_master_interrupt_address 0xaa
#define _exec_reset_address 0x4e4
#define _prepare_reflash_address 0x3e0
#define _register_module_address 0x2b4
#define _i2c_enable_address 0x86
#define _verify_application_address 0xf6
#define _i2c_calc_check_address 0x59
#define _find_handler_address 0x2e0
#define _i2c_transmit_address 0x55
#define _i2c_core_transfer_address 0x93
#define _i2c_choose_direction_address 0x39
#define powerup_address 0x28
#define _i2c_master_receivechecksum_address 0x47e
#define _get_command_address 0x21c
#define _bus_is_idle_address 0x3ff
#define _i2c_master_send_message_address 0x4cd
#define _i2c_slave_send_message_address 0x45d
#define _initialize_address 0x3ac
#define _i2c_start_transmission_address 0x438
#define _wdt_pushenabled_address 0x41d
#define _get_param_spec_address 0x21d
#define _bus_slave_callback_address 0x1ba
#define _i2c_set_master_mode_address 0x6a
#define _bus_master_callback_address 0x150
#define exec_spec_map_address 0x478
#define _get_num_features_address 0x230
#define _exec_call_cmd_address 0x47d
#define _bus_init_address 0x445
#define _bus_slave_callcommand_address 0x451
#define _i2c_lasterror_address 0x4e0
#define _get_mib_block_address 0x227
#define _exec_get_spec_address 0x47c
#define _i2c_master_receive_message_address 0x4c9
#define c_call_bus_slave_reset() asm("call 0x4a3")
#define c_call_copy_mib_to_boot() asm("call 0x12b")
#define c_call_bus_slave_validateparams() asm("call 0x40e")
#define c_call_i2c_getstate() asm("call 0x48")
#define c_call_exec_verify() asm("call 0x490")
#define c_call_bus_slave_setreturn() asm("call 0x4c4")
#define c_call_enter_bootloader() asm("call 0x341")
#define c_call_bus_master_rpc_sync() asm("call 0x306")
#define c_call_call_handler() asm("call 0x211")
#define c_call_flash_erase_application() asm("call 0x106")
#define c_call_i2c_core_receivechecksum() asm("call 0x5e")
#define c_call_i2c_setstate() asm("call 0x40")
#define c_calli2c_setup_buffer() asm("call 0x4c")
#define c_callread_app_row() asm("call 0xe9")
#define c_calli1_i2c_lasterror() asm("call 0x4e2")
#define c_call_exec_prepare_reflash() asm("call 0x488")
#define c_call_i2c_slave_receive_message() asm("call 0x4d5")
#define c_call_get_half_row() asm("call 0x393")
#define c_call_get_firmware_id() asm("call 0x125")
#define c_call_service_isr() asm("call 0x4")
#define c_call_i2c_receive() asm("call 0x51")
#define c_call_flash_erase_row() asm("call 0x102")
#define c_call_load_boot_address() asm("call 0x113")
#define c_call_i2c_slave_interrupt() asm("call 0x24d")
#define c_call_app_init_vector() asm("call 0x4f0")
#define c_callprepare_row_address() asm("call 0xdd")
#define c_call_validate_param_spec() asm("call 0x23e")
#define c_call_bus_master_lastaddress() asm("call 0x4dd")
#define c_call_bus_slave_seterror() asm("call 0x245")
#define c_call_set_firmware_id() asm("call 0x4af")
#define c_call_bus_master_finish() asm("call 0x3be")
#define c_call_wdt_settimeout() asm("call 0x4ba")
#define c_call_plist_param_length() asm("call 0x234")
#define c_call_main() asm("call 0x37a")
#define c_call_bus_master_readstatus() asm("call 0x42b")
#define c_callexec_cmd_map() asm("call 0x474")
#define c_call_bus_slave_searchcommand() asm("call 0x283")
#define c_callunlock_and_write() asm("call 0xd5")
#define c_calli1_wdt_settimeout() asm("call 0x4d1")
#define c_call_bus_master_handleerror() asm("call 0x3f0")
#define c_call_i2c_finish_transmission() asm("call 0x4bf")
#define c_call_wdt_popenabled() asm("call 0x425")
#define c_call_bus_slave_startcommand() asm("call 0x3d0")
#define c_call_i2c_master_receivedata() asm("call 0x469")
#define c_call_get_boot_source() asm("call 0x11f")
#define c_call_plist_int_count() asm("call 0x231")
#define c_call_app_interrupt_vector() asm("call 0x4f1")
#define c_call_restore_status() asm("call 0x324")
#define c_call_bus_master_sendrpc() asm("call 0x35e")
#define c_call_get_magic() asm("call 0x22e")
#define c_call__initialization() asm("call 0x2e")
#define c_call_i2c_slave_active() asm("call 0x4d9")
#define c_call_app_task_vector() asm("call 0x4f2")
#define c_callmib12_api() asm("call 0x4ee")
#define c_call_get_feature() asm("call 0x21b")
#define c_call_i2c_slave_setidle() asm("call 0x4a9")
#define c_call_flash_write_row() asm("call 0x139")
#define c_call_check_app_fault() asm("call 0x4b5")
#define c_call_i2c_master_interrupt() asm("call 0xaa")
#define c_call_exec_reset() asm("call 0x4e4")
#define c_call_prepare_reflash() asm("call 0x3e0")
#define c_call_register_module() asm("call 0x2b4")
#define c_call_i2c_enable() asm("call 0x86")
#define c_call_verify_application() asm("call 0xf6")
#define c_call_i2c_calc_check() asm("call 0x59")
#define c_call_find_handler() asm("call 0x2e0")
#define c_call_i2c_transmit() asm("call 0x55")
#define c_call_i2c_core_transfer() asm("call 0x93")
#define c_call_i2c_choose_direction() asm("call 0x39")
#define c_callpowerup() asm("call 0x28")
#define c_call_i2c_master_receivechecksum() asm("call 0x47e")
#define c_call_get_command() asm("call 0x21c")
#define c_call_bus_is_idle() asm("call 0x3ff")
#define c_call_i2c_master_send_message() asm("call 0x4cd")
#define c_call_i2c_slave_send_message() asm("call 0x45d")
#define c_call_initialize() asm("call 0x3ac")
#define c_call_i2c_start_transmission() asm("call 0x438")
#define c_call_wdt_pushenabled() asm("call 0x41d")
#define c_call_get_param_spec() asm("call 0x21d")
#define c_call_bus_slave_callback() asm("call 0x1ba")
#define c_call_i2c_set_master_mode() asm("call 0x6a")
#define c_call_bus_master_callback() asm("call 0x150")
#define c_callexec_spec_map() asm("call 0x478")
#define c_call_get_num_features() asm("call 0x230")
#define c_call_exec_call_cmd() asm("call 0x47d")
#define c_call_bus_init() asm("call 0x445")
#define c_call_bus_slave_callcommand() asm("call 0x451")
#define c_call_i2c_lasterror() asm("call 0x4e0")
#define c_call_get_mib_block() asm("call 0x227")
#define c_call_exec_get_spec() asm("call 0x47c")
#define c_call_i2c_master_receive_message() asm("call 0x4c9")
#define asm_call_bus_slave_reset() call 0x4a3
#define asm_call_copy_mib_to_boot() call 0x12b
#define asm_call_bus_slave_validateparams() call 0x40e
#define asm_call_i2c_getstate() call 0x48
#define asm_call_exec_verify() call 0x490
#define asm_call_bus_slave_setreturn() call 0x4c4
#define asm_call_enter_bootloader() call 0x341
#define asm_call_bus_master_rpc_sync() call 0x306
#define asm_call_call_handler() call 0x211
#define asm_call_flash_erase_application() call 0x106
#define asm_call_i2c_core_receivechecksum() call 0x5e
#define asm_call_i2c_setstate() call 0x40
#define asm_calli2c_setup_buffer() call 0x4c
#define asm_callread_app_row() call 0xe9
#define asm_calli1_i2c_lasterror() call 0x4e2
#define asm_call_exec_prepare_reflash() call 0x488
#define asm_call_i2c_slave_receive_message() call 0x4d5
#define asm_call_get_half_row() call 0x393
#define asm_call_get_firmware_id() call 0x125
#define asm_call_service_isr() call 0x4
#define asm_call_i2c_receive() call 0x51
#define asm_call_flash_erase_row() call 0x102
#define asm_call_load_boot_address() call 0x113
#define asm_call_i2c_slave_interrupt() call 0x24d
#define asm_call_app_init_vector() call 0x4f0
#define asm_callprepare_row_address() call 0xdd
#define asm_call_validate_param_spec() call 0x23e
#define asm_call_bus_master_lastaddress() call 0x4dd
#define asm_call_bus_slave_seterror() call 0x245
#define asm_call_set_firmware_id() call 0x4af
#define asm_call_bus_master_finish() call 0x3be
#define asm_call_wdt_settimeout() call 0x4ba
#define asm_call_plist_param_length() call 0x234
#define asm_call_main() call 0x37a
#define asm_call_bus_master_readstatus() call 0x42b
#define asm_callexec_cmd_map() call 0x474
#define asm_call_bus_slave_searchcommand() call 0x283
#define asm_callunlock_and_write() call 0xd5
#define asm_calli1_wdt_settimeout() call 0x4d1
#define asm_call_bus_master_handleerror() call 0x3f0
#define asm_call_i2c_finish_transmission() call 0x4bf
#define asm_call_wdt_popenabled() call 0x425
#define asm_call_bus_slave_startcommand() call 0x3d0
#define asm_call_i2c_master_receivedata() call 0x469
#define asm_call_get_boot_source() call 0x11f
#define asm_call_plist_int_count() call 0x231
#define asm_call_app_interrupt_vector() call 0x4f1
#define asm_call_restore_status() call 0x324
#define asm_call_bus_master_sendrpc() call 0x35e
#define asm_call_get_magic() call 0x22e
#define asm_call__initialization() call 0x2e
#define asm_call_i2c_slave_active() call 0x4d9
#define asm_call_app_task_vector() call 0x4f2
#define asm_callmib12_api() call 0x4ee
#define asm_call_get_feature() call 0x21b
#define asm_call_i2c_slave_setidle() call 0x4a9
#define asm_call_flash_write_row() call 0x139
#define asm_call_check_app_fault() call 0x4b5
#define asm_call_i2c_master_interrupt() call 0xaa
#define asm_call_exec_reset() call 0x4e4
#define asm_call_prepare_reflash() call 0x3e0
#define asm_call_register_module() call 0x2b4
#define asm_call_i2c_enable() call 0x86
#define asm_call_verify_application() call 0xf6
#define asm_call_i2c_calc_check() call 0x59
#define asm_call_find_handler() call 0x2e0
#define asm_call_i2c_transmit() call 0x55
#define asm_call_i2c_core_transfer() call 0x93
#define asm_call_i2c_choose_direction() call 0x39
#define asm_callpowerup() call 0x28
#define asm_call_i2c_master_receivechecksum() call 0x47e
#define asm_call_get_command() call 0x21c
#define asm_call_bus_is_idle() call 0x3ff
#define asm_call_i2c_master_send_message() call 0x4cd
#define asm_call_i2c_slave_send_message() call 0x45d
#define asm_call_initialize() call 0x3ac
#define asm_call_i2c_start_transmission() call 0x438
#define asm_call_wdt_pushenabled() call 0x41d
#define asm_call_get_param_spec() call 0x21d
#define asm_call_bus_slave_callback() call 0x1ba
#define asm_call_i2c_set_master_mode() call 0x6a
#define asm_call_bus_master_callback() call 0x150
#define asm_callexec_spec_map() call 0x478
#define asm_call_get_num_features() call 0x230
#define asm_call_exec_call_cmd() call 0x47d
#define asm_call_bus_init() call 0x445
#define asm_call_bus_slave_callcommand() call 0x451
#define asm_call_i2c_lasterror() call 0x4e0
#define asm_call_get_mib_block() call 0x227
#define asm_call_exec_get_spec() call 0x47c
#define asm_call_i2c_master_receive_message() call 0x4c9
