/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   Host Stack                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_host_stack.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_host_stack_device_configuration_reset           PORTABLE C      */ 
/*                                                           6.0          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function resets the configuration of the device to zero.       */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    configuration                          Pointer to configuration     */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    Completion Status                                                   */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_host_stack_configuration_instance_delete                        */
/*                                           Delete configuration instance*/ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application                                                         */ 
/*    USBX Components                                                     */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/
UINT  _ux_host_stack_device_configuration_reset(UX_DEVICE *device)
{

UX_TRANSFER             *transfer_request;
UX_ENDPOINT             *control_endpoint;
UX_CONFIGURATION        *current_configuration;
UINT                    status;
    
    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_HOST_STACK_DEVICE_CONFIGURATION_SELECT, device, 0, 0, 0, UX_TRACE_HOST_STACK_EVENTS, 0, 0)

    /* A configuration is selected. Retrieve the pointer to the control endpoint 
       and its transfer request.  */
    control_endpoint =  &device -> ux_device_control_endpoint;
    transfer_request =  &control_endpoint -> ux_endpoint_transfer_request;

    /* Check for the state of the device . If the device is already configured, 
       we need to cancel the existing configuration before resetting it.   */
    if (device -> ux_device_state == UX_DEVICE_CONFIGURED)
    {

        /* The device is configured. Get the first configuration pointer.  */
        current_configuration =  device -> ux_device_first_configuration;

        /* Traverse the configuration list until we find the right one. */
        while (current_configuration -> ux_configuration_descriptor.bConfigurationValue !=
                device -> ux_device_current_configuration)
        {

            current_configuration =  current_configuration -> ux_configuration_next_configuration;
        }

        /* Deselect this instance */
        _ux_host_stack_configuration_instance_delete(current_configuration);
    }

    /* Set state of device to ATTACHED.  */
    device -> ux_device_state = UX_DEVICE_ATTACHED;

    /* Create a transfer_request for the SET_CONFIGURATION request. No data for this request.  */
    transfer_request -> ux_transfer_request_requested_length =  0;
    transfer_request -> ux_transfer_request_function =          UX_SET_CONFIGURATION;
    transfer_request -> ux_transfer_request_type =              UX_REQUEST_OUT | UX_REQUEST_TYPE_STANDARD | UX_REQUEST_TARGET_DEVICE;
    transfer_request -> ux_transfer_request_value =             0;
    transfer_request -> ux_transfer_request_index =             0;

    /* Send request to HCD layer.  */
    status =  _ux_host_stack_transfer_request(transfer_request);

    /* Return status.  */
    return(status);
}
