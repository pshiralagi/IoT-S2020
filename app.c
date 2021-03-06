/***************************************************************************//**
 * @file
 * @brief Application code
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 * Modified by Pavan Shiralagi
 *
 *
 ******************************************************************************/

/* C Standard Library headers */
#include <stdio.h>
#include "src/main.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "mesh_generic_model_capi_types.h"
#include "mesh_lighting_model_capi_types.h"
#include "mesh_lib.h"


/* GPIO peripheral library */
#include <em_gpio.h>

/* Own header */
#include "app.h"


/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

/*******************************************************************************
 * Timer handles defines.
 ******************************************************************************/
#define TIMER_ID_RESTART    78
#define TIMER_ID_FACTORY_RESET  77

#define TIMER_ID_PROVISIONING   66
/// Flag for indicating DFU Reset must be performed
static uint8_t boot_to_dfu = 0;
/// Address of the Primary Element of the Node
static uint16_t _my_address = 0;
/// Number of active Bluetooth connections
static uint8_t num_connections = 0;
/// Handle of the last opened LE connection
static uint8_t conn_handle = 0xFF;
//Count of connected LPNs
static uint8_t lpnCount = 0;


//Storage types for peristent data
float high_temp = 0;
uint8_t authorized_personnel = 0;
static uint8_t buttonPressed = 0;


/// Flag for indicating that initialization was performed
static uint8_t init_done = 0;
struct gecko_msg_mesh_generic_server_client_request_evt_t *req = NULL;
/***************************************************************************//**
 * This function is called to initiate factory reset. Factory reset may be
 * initiated by keeping one of the WSTK pushbuttons pressed during reboot.
 * Factory reset is also performed if it is requested by the provisioner
 * (event gecko_evt_mesh_node_reset_id).
 ******************************************************************************/
static void initiate_factory_reset(void)
{
  LOG_INFO("****FACTORY RESET");
  displayPrintf(DISPLAY_ROW_TEMPERATURE, "***FACTORY RESET***");

  /* if connection is open then close it before rebooting */
  if (conn_handle != 0xFF) {
    BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_close(conn_handle));
  }

  /* perform a factory reset by erasing PS storage. This removes all the keys and other settings
     that have been configured for this node */
  BTSTACK_CHECK_RESPONSE(gecko_cmd_flash_ps_erase_all());
  // reboot after a small delay
  BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_FACTORY_RESET, 1));
}

/***************************************************************************//**
 * Set device name in the GATT database. A unique name is generated using
 * the two last bytes from the Bluetooth address of this device.
 *
 * @param[in] pAddr  Pointer to Bluetooth address.
 ******************************************************************************/
static void set_device_name(bd_addr *pAddr)
{
  char name[20];
  uint16_t res;
  // create unique device name using the last two bytes of the Bluetooth address
  sprintf(name, "5823Sub %02x:%02x", pAddr->addr[1], pAddr->addr[0]);

  LOG_INFO("Device name: '%s", name);

  // write device name to the GATT database
  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(name), (uint8_t *)name)->result;
  if (res) {
    LOG_INFO("gecko_cmd_gatt_server_write_attribute_value() failed, code %x", res);
  }
}



/*******************************************************************************
 * Initialise used bgapi classes.
 ******************************************************************************/
void gecko_bgapi_classes_init(void)
{
  gecko_bgapi_class_dfu_init();
  gecko_bgapi_class_system_init();
  gecko_bgapi_class_le_gap_init();
  gecko_bgapi_class_le_connection_init();
  //gecko_bgapi_class_gatt_init();
  gecko_bgapi_class_gatt_server_init();
  gecko_bgapi_class_hardware_init();
  gecko_bgapi_class_flash_init();
  gecko_bgapi_class_test_init();
  //gecko_bgapi_class_sm_init();
  gecko_bgapi_class_mesh_node_init();
  //gecko_bgapi_class_mesh_prov_init();
  gecko_bgapi_class_mesh_proxy_init();
  gecko_bgapi_class_mesh_proxy_server_init();
  //gecko_bgapi_class_mesh_proxy_client_init();
  //gecko_bgapi_class_mesh_generic_client_init();
  gecko_bgapi_class_mesh_generic_server_init();
//  gecko_bgapi_class_mesh_vendor_model_init();
  //gecko_bgapi_class_mesh_health_client_init();
  //gecko_bgapi_class_mesh_health_server_init();
  //gecko_bgapi_class_mesh_test_init();
  //gecko_bgapi_class_mesh_lpn_init();
  gecko_bgapi_class_mesh_friend_init();
  gecko_bgapi_class_mesh_lc_server_init();
  gecko_bgapi_class_mesh_lc_setup_server_init();
  gecko_bgapi_class_mesh_scene_server_init();
  gecko_bgapi_class_mesh_scene_setup_server_init();
}


/*******************************************************************************
 * Handling of stack events. Both Bluetooth LE and Bluetooth mesh events
 * are handled here.
 * @param[in] evt_id  Incoming event ID.
 * @param[in] evt     Pointer to incoming event.
 ******************************************************************************/

void handle_ecen5823_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
	  uint16_t result;
	  char buf[30];

	  if (NULL == evt)
	  {
	    return;
	  }

	  switch (evt_id)
	  {
	    case gecko_evt_system_boot_id:
	      // check pushbutton state at startup. If either PB0 or PB1 is held down then do factory reset
	      if ((GPIO_PinInGet(PB0_Port,PB0_Pin) == 0 ) || (GPIO_PinInGet(PB1_Port,PB1_Pin) == 0 ))
	      {
	        initiate_factory_reset();
	      }
	      else
	      {
	        struct gecko_msg_system_get_bt_address_rsp_t *pAddr = gecko_cmd_system_get_bt_address();

	        set_device_name(&pAddr->address);

	        // Initialize Mesh stack in Node operation mode, it will generate initialized event
	        result = gecko_cmd_mesh_node_init()->result;

	        if (result)
	        {
	          sprintf(buf, "init failed (0x%x)", result);
	          displayPrintf(DISPLAY_ROW_TEMPERATURE, buf);
	        }
	      }
	      break;

	    case gecko_evt_hardware_soft_timer_id:
	      switch (evt->data.evt_hardware_soft_timer.handle)
	      {
	        case TIMER_ID_DISPLAY_UPDATES:
				displayUpdate();
				break;
	        case TIMER_ID_FACTORY_RESET:
	          // reset the device to finish factory reset
	          gecko_cmd_system_reset(0);
	          break;

	        case TIMER_ID_RESTART:
	          // restart timer expires, reset the device
	          gecko_cmd_system_reset(0);
	          break;

	        case TIMER_ID_PROVISIONING:
	          // toggle LED to indicate the provisioning state
	          if (!init_done)
	          {
	            toggleLed();
	          }
	          break;

	        default:

	          break;
	      }
	      break;

	    case gecko_evt_mesh_node_initialized_id:
	      LOG_INFO("Node Initialized");

	      // Initialize generic server models
	      result = gecko_cmd_mesh_generic_server_init()->result;
	      if (result)
	      {
	        LOG_INFO("mesh_generic_server_init failed, code 0x%x", result);
	      }

	      psDataLoad(BUTTON_COUNT, &buttonPressed, sizeof(buttonPressed));
	      LOG_INFO("******ALERTS CLEARED******** %d ***********", buttonPressed);
	      psDataLoad(MAX_TEMP, &high_temp, sizeof(high_temp));
	      LOG_INFO("******HIGHEST TEMPERATURE RECORDED******** %f ***********", high_temp);
	      psDataLoad(AUTHORIZED_PERSONNEL, &authorized_personnel, sizeof(authorized_personnel));
	      if(authorized_personnel)
	      {
	    	  LOG_INFO("Authorized personnel present in room");
	    	  displayPrintf(DISPLAY_ROW_AUTHORITY, "Authority Present");
	      }
	      else
	      {
	    	  LOG_INFO("Authorized personnel not present in room");
	    	  displayPrintf(DISPLAY_ROW_AUTHORITY, "Authority Not Present");
	      }
	      struct gecko_msg_mesh_node_initialized_evt_t *pData = (struct gecko_msg_mesh_node_initialized_evt_t *)&(evt->data);

	      if (pData->provisioned)
	      {
	        LOG_INFO("node is provisioned. address:%x, ivi:%ld", pData->address, pData->ivi);

	        _my_address = pData->address;
	        enable_button_interrupts();
	        friendInit();
	        displayPrintf(DISPLAY_ROW_TEMPERATURE, "Provisioned");
	        init_all_models();
	      }
	      else
	      {
	        LOG_INFO("node is unprovisioned");
	        displayPrintf(DISPLAY_ROW_TEMPERATURE, "Un-provisioned");

	        LOG_INFO("starting unprovisioned beaconing...");
	        BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_node_start_unprov_beaconing(0x3));   // enable ADV and GATT provisioning bearer

	      }

	      break;

	    case gecko_evt_mesh_node_provisioning_started_id:
	      LOG_INFO("Started provisioning");
	      displayPrintf(DISPLAY_ROW_TEMPERATURE, "Provisioning");
	      // start timer for blinking LEDs to indicate which node is being provisioned
	      BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(32768 / 4, TIMER_ID_PROVISIONING, 0));
	      break;

	    case gecko_evt_mesh_node_provisioned_id:
	      LOG_INFO("node provisioned, got address=%x", evt->data.evt_mesh_node_provisioned.address);
	      enable_button_interrupts();
	      friendInit();
	      // stop LED blinking when provisioning complete
	      BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(0, TIMER_ID_PROVISIONING, 0));
	      clearAlert();
	      init_all_models();
	      displayPrintf(DISPLAY_ROW_TEMPERATURE, "Provisioned");
	      break;

	    case gecko_evt_mesh_node_provisioning_failed_id:
	      LOG_INFO("provisioning failed, code %x", evt->data.evt_mesh_node_provisioning_failed.result);
	      displayPrintf(DISPLAY_ROW_TEMPERATURE, "Provisioning failed");
	      /* start a one-shot timer that will trigger soft reset after small delay */
	      BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_RESTART, 1));
	      break;

	    case gecko_evt_mesh_node_key_added_id:
	      LOG_INFO("got new %s key with index %x", evt->data.evt_mesh_node_key_added.type == 0 ? "network" : "application",
	             evt->data.evt_mesh_node_key_added.index);
	      break;

	    case gecko_evt_mesh_node_model_config_changed_id:
	      LOG_INFO("model config changed");
	      break;

	    case gecko_evt_mesh_generic_server_client_request_id:
	      mesh_lib_generic_server_event_handler(evt);
	      break;

	      case gecko_evt_system_external_signal_id:
	      {
//	    	  	  struct mesh_generic_state req;
					if ((evt->data.evt_system_external_signal.extsignals) == 0x01)
					{
//						LOG_INFO("In external signal 0x01");
						state(); //Calling state machine implementation
						Hum_Buffer(); //Loading humidity buffer with appropriate values
					}
					else if (((evt->data.evt_system_external_signal.extsignals) >= 0x02) && ((evt->data.evt_system_external_signal.extsignals) <= 0x06))
					{
//						LOG_INFO("In external signal 0x02-0x06");
						state(); //Calling state machine implementation
					}
					if ((evt->data.evt_system_external_signal.extsignals) == 0x40)
					{
						clearAlert();
//						  req.kind = mesh_generic_state_on_off;
						  if(GPIO_PinInGet(PB0_Port, PB0_Pin) == 0)
						  {
//							  req.on_off.on = MESH_GENERIC_ON_OFF_STATE_ON;
							  displayPrintf(DISPLAY_ROW_ALERT_CARETAKER,"Alert Cleared");
							  displayPrintf(DISPLAY_ROW_ALERT_PATIENT,"Alert Cleared");
							  buttonPressed++;
							  psDataSave(BUTTON_COUNT, &buttonPressed, sizeof(buttonPressed));
							  LOG_INFO("Alert cleared");
						  }
/*Code to Publish data (Not supported by SDK)*/
/*						  mesh_lib_generic_server_update(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
																   0,
																   NULL,
																   &req,
																   0);
						  uint16_t resp = mesh_lib_generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID, 0, mesh_generic_state_on_off);
						  LOG_INFO("\n\r**************parameter data = %d**********************\n\r", req.on_off.on);

						  if (resp)
						  {
							  LOG_INFO("gecko_cmd_mesh_generic_client_publish failed, code 0x%x\r\n", resp);
						  }	*/

					}
					if ((evt->data.evt_system_external_signal.extsignals) == 0x50)
					{
						if (authorized_personnel)
						{
							clearAlert();
						}
						else
						{
							redAlert();
							displayPrintf(DISPLAY_ROW_ALERT_CARETAKER, "Unauthorized person");
						}
						LOG_INFO("******************HUMAN DETECTED*********************");
					}
	      }
					break;

	    case gecko_evt_mesh_generic_server_state_changed_id:
	      mesh_lib_generic_server_event_handler(evt);
	      break;

	    case gecko_evt_mesh_generic_server_state_recall_id:
	      LOG_INFO("evt gecko_evt_mesh_generic_server_state_recall_id");
	      mesh_lib_generic_server_event_handler(evt);
	      break;


	    case gecko_evt_mesh_node_reset_id:
	      LOG_INFO("evt gecko_evt_mesh_node_reset_id");
	      initiate_factory_reset();
	      break;

	    case gecko_evt_mesh_friend_friendship_established_id:
	      printf("evt gecko_evt_mesh_friend_friendship_established, lpn_address=%x\r\n", evt->data.evt_mesh_friend_friendship_established.lpn_address);
	      lpnCount++;
	      LOG_INFO("Number of LPNs in mesh - %d",lpnCount);
	      displayPrintf(DISPLAY_ROW_FRIEND, "FRIEND -- %d LPNs", lpnCount);
			/*	Initialize timer	*/
		    LETIMER_Enable(LETIMER0, true);
		    pirInit();
	      break;

	    case gecko_evt_mesh_friend_friendship_terminated_id:
	      printf("evt gecko_evt_mesh_friend_friendship_terminated, reason=%x\r\n", evt->data.evt_mesh_friend_friendship_terminated.reason);
	      lpnCount--;
	      LOG_INFO("Number of LPNs in mesh - %d",lpnCount);
	      displayPrintf(DISPLAY_ROW_FRIEND,"FRIEND -- %d LPNs", lpnCount);
	      break;

	    case gecko_evt_le_gap_adv_timeout_id:
	      // adv timeout events silently discarded
	      break;

	    case gecko_evt_le_connection_opened_id:
	      LOG_INFO("evt:gecko_evt_le_connection_opened_i");
	      num_connections++;
	      conn_handle = evt->data.evt_le_connection_opened.connection;
	      gecko_cmd_system_get_bt_address();
	      displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");

	      break;

	    case gecko_evt_le_connection_parameters_id:
	      LOG_INFO("evt:gecko_evt_le_connection_parameters_id");
	      break;

	    case gecko_evt_le_connection_closed_id:
	      /* Check if need to boot to dfu mode */
	      if (boot_to_dfu) {
	        /* Enter to DFU OTA mode */
	        gecko_cmd_system_reset(2);
	      }

	      LOG_INFO("evt:conn closed, reason 0x%x", evt->data.evt_le_connection_closed.reason);
	      conn_handle = 0xFF;
	      if (num_connections > 0) {
	        if (--num_connections == 0) {
	        	displayPrintf(DISPLAY_ROW_CONNECTION, "");
	        }
	      }
	      break;
	    case gecko_evt_gatt_server_user_write_request_id:
	      if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
	        /* Set flag to enter to OTA mode */
	        boot_to_dfu = 1;
	        /* Send response to Write Request */
	        BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_user_write_response(
	          evt->data.evt_gatt_server_user_write_request.connection,
	          gattdb_ota_control,
	          bg_err_success));

	        /* Close connection to enter to DFU OTA mode */
	        BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection));
	      }
	      break;


	    default:
	      //LOG_INFO("unhandled evt: %8.8x class %2.2x method %2.2x\r\n", evt_id, (evt_id >> 16) & 0xFF, (evt_id >> 24) & 0xFF);
	      break;
	  }
}

/*
 * @brief	Initialize server register handles
 */
void init_all_models()
{
	LOG_INFO("***********Initializing all models************");
	errorcode_t mesh_reg_response = mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID, 0, onoff_request,NULL, NULL);
	if(mesh_reg_response != 0)
	{
		LOG_ERROR("Handler register failed with %d response",mesh_reg_response);
	}
	mesh_reg_response = mesh_lib_generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID, 0, level_request, 0, NULL);
	if(mesh_reg_response != 0)
	{
		LOG_ERROR("Handler register failed with %d response",mesh_reg_response);
	}
}

/*
 * @brief	Store data in persistent memory
 *
 */
void psDataSave(uint16_t key, void *value, uint8_t size)
{
	struct gecko_msg_flash_ps_save_rsp_t *resp;

	resp = gecko_cmd_flash_ps_save(key, size, value);

	if(resp->result != 0)
	{
		LOG_ERROR("Error saving data\r\n");
	}

}

/*
 * @brief	Load data from persistent memory
 */
void psDataLoad(uint16_t key, void *value, uint8_t size)
{
	struct gecko_msg_flash_ps_load_rsp_t *resp;

	resp = gecko_cmd_flash_ps_load(key);

	if(resp->result == 0)
	{
		memcpy(value, resp->value.data, resp->value.len);
	}
	else
	{
		LOG_ERROR("Error loading data\r\n");
	}
}

/*
 * @brief	Initialize friend node
 */
void friendInit(void)
{
	static uint16_t result = 0;
    mesh_lib_init(malloc, free, 11);
    result = gecko_cmd_mesh_friend_init()->result;
    if (result)
    {
      printf("*******************Friend init failed 0x%x\r\n", result);
    }
}

/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */
