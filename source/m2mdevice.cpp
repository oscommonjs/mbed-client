/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include <cstdio>
#include "lwm2m-client/m2mdevice.h"
#include "lwm2m-client/m2mconstants.h"
#include "lwm2m-client/m2mobject.h"
#include "lwm2m-client/m2mobjectinstance.h"
#include "lwm2m-client/m2mresource.h"

M2MDevice* M2MDevice::_instance = NULL;

M2MDevice* M2MDevice::get_instance()
{
    if(_instance == NULL) {
        _instance = new M2MDevice();
    }
    return _instance;
}

void M2MDevice::delete_instance()
{
    if(_instance) {
        delete _instance;
        _instance = NULL;
    }
}

M2MDevice::M2MDevice()
: M2MObject(M2M_DEVICE_ID)
{    
    _device_instance = M2MObject::create_object_instance();

    if(_device_instance) {
        M2MResource* res = _device_instance->create_dynamic_resource(DEVICE_REBOOT,OMA_RESOURCE_TYPE,false);
        if(res) {
            res->set_operation(M2MBase::POST_ALLOWED);
        }

        res = _device_instance->create_dynamic_resource(DEVICE_ERROR_CODE,OMA_RESOURCE_TYPE,false,true);
        if(res) {
            res->set_operation(M2MBase::GET_PUT_ALLOWED);
            res->set_value((const uint8_t*)ERROR_CODE_VALUE.c_str(),
                           (uint32_t)ERROR_CODE_VALUE.length());
        }
        res = _device_instance->create_dynamic_resource(DEVICE_SUPPORTED_BINDING_MODE,OMA_RESOURCE_TYPE,false);
        if(res) {
            res->set_operation(M2MBase::GET_PUT_ALLOWED);
            res->set_value((const uint8_t*)BINDING_MODE_UDP.c_str(),
                           (uint32_t)BINDING_MODE_UDP.length());
        }
    }
}

M2MDevice::~M2MDevice()
{
}

M2MResource* M2MDevice::create_resource(DeviceResource resource, const String &value)
{
    M2MResource* res = NULL;
    String device_id = "";
    if(!is_resource_present(resource)) {
        switch(resource) {
            case Manufacturer:
               device_id = DEVICE_MANUFACTURER;
               break;
            case DeviceType:
                device_id = DEVICE_DEVICE_TYPE;
                break;
            case ModelNumber:
                device_id = DEVICE_MODEL_NUMBER;
                break;
            case SerialNumber:
                device_id = DEVICE_SERIAL_NUMBER;
                break;
            case HardwareVersion:
                device_id = DEVICE_HARDWARE_VERSION;
                break;
            case FirmwareVersion:
                device_id = DEVICE_FIRMWARE_VERSION;
                break;
            case SoftwareVersion:
                device_id = DEVICE_SOFTWARE_VERSION;
                break;
            case UTCOffset:
                device_id = DEVICE_UTC_OFFSET;
                break;
            case Timezone:
                device_id = DEVICE_TIMEZONE;
                break;
            default:
                break;
        }
    }    
    if(!device_id.empty()) {
        if(_device_instance) {
            res = _device_instance->create_dynamic_resource(device_id,OMA_RESOURCE_TYPE,false);

            if(res ) {
                if((device_id == DEVICE_UTC_OFFSET) ||
                   (device_id == DEVICE_TIMEZONE)) {
                    res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
                } else {
                    res->set_operation(M2MBase::GET_PUT_ALLOWED);
                }
                res->set_value((const uint8_t*)value.c_str(),
                               (uint32_t)value.length());
            }
        }
    }
    return res;
}

M2MResource* M2MDevice::create_resource(DeviceResource resource, uint32_t value)
{
    M2MResource* res = NULL;
    String device_id = "";
    if(!is_resource_present(resource)) {
        switch(resource) {
        case BatteryLevel:
            device_id = DEVICE_BATTERY_LEVEL;
            break;
        case BatteryStatus:
            device_id = DEVICE_BATTERY_STATUS;
            break;
        case MemoryFree:
            device_id = DEVICE_MEMORY_FREE;
            break;
        case MemoryTotal:
            device_id = DEVICE_MEMORY_TOTAL;
            break;
        case CurrentTime: {
            if(_device_instance) {
                res = _device_instance->create_dynamic_resource(DEVICE_CURRENT_TIME,OMA_RESOURCE_TYPE,false);

                if(res) {
                    char buffer[20];
                    int size = sprintf(buffer,"%ld",(long int)value);
                    res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
                    res->set_value((const uint8_t*)buffer,
                                   (const uint32_t)size);
                }
            }
        }
            break;
        default:
            break;
        }
    }
    // For these resources multiple instance can exist
    if(AvailablePowerSources == resource) {
        device_id = DEVICE_AVAILABLE_POWER_SOURCES;
    } else if(PowerSourceVoltage == resource) {
        device_id = DEVICE_POWER_SOURCE_VOLTAGE;
    } else if(PowerSourceCurrent == resource) {
        device_id = DEVICE_POWER_SOURCE_CURRENT;
    } else if(ErrorCode == resource) {
        device_id = DEVICE_ERROR_CODE;
    }

    if(!device_id.empty()) {
        if(_device_instance) {
            res = _device_instance->create_dynamic_resource(device_id,OMA_RESOURCE_TYPE,false);

            if(res) {
                char buffer[20];
                int size = sprintf(buffer,"%ld",(long int)value);
                res->set_operation(M2MBase::GET_PUT_ALLOWED);
                res->set_value((const uint8_t*)buffer,
                               (const uint32_t)size);
            }
        }
    }
    return res;
}

M2MResource* M2MDevice::create_resource(DeviceResource resource)
{
    M2MResource* res = NULL;
    if(!is_resource_present(resource)) {
        String device_Id;
        if(FactoryReset == resource) {
            device_Id = DEVICE_FACTORY_RESET;
        } else if(ResetErrorCode == resource) {
            device_Id = DEVICE_RESET_ERROR_CODE;
        }
        if(_device_instance && !device_Id.empty()) {
            res = _device_instance->create_dynamic_resource(device_Id,OMA_RESOURCE_TYPE,false);
            if(res) {
                res->set_operation(M2MBase::POST_ALLOWED);
            }
        }
    }
    return res;
}

bool M2MDevice::delete_resource(DeviceResource resource,
                                uint16_t instance_id)
{
    bool success = false;
    if(M2MDevice::Reboot != resource             &&
       M2MDevice::ErrorCode != resource          &&
       M2MDevice::SupportedBindingMode != resource) {
        if(_device_instance) {
            success = _device_instance->remove_resource(resource_name(resource),instance_id);
        }
    }
    return success;
}

bool M2MDevice::set_resource_value(DeviceResource resource,
                                   const String &value,
                                   uint16_t instance_id)
{
    bool success = false;
    M2MResource* res = get_resource(resource,instance_id);
    if(res) {
        if(M2MDevice::Manufacturer == resource          ||
           M2MDevice::ModelNumber == resource           ||
           M2MDevice::DeviceType == resource            ||
           M2MDevice::SerialNumber == resource          ||
           M2MDevice::HardwareVersion == resource       ||
           M2MDevice::FirmwareVersion == resource       ||
           M2MDevice::SoftwareVersion == resource       ||
           M2MDevice::UTCOffset == resource             ||
           M2MDevice::Timezone == resource) {

            success = res->set_value((const uint8_t*)value.c_str(),(uint32_t)value.length());
        }
    }
    return success;
}

bool M2MDevice::set_resource_value(DeviceResource resource,
                                       uint32_t value,
                                       uint16_t instance_id)
{
    bool success = false;
    M2MResource* res = get_resource(resource,instance_id);
    if(res) {
        if(M2MDevice::BatteryLevel == resource          ||
           M2MDevice::BatteryStatus == resource         ||
           M2MDevice::MemoryFree == resource            ||
           M2MDevice::MemoryTotal == resource           ||
           M2MDevice::ErrorCode == resource             ||
           M2MDevice::CurrentTime == resource           ||
           M2MDevice::AvailablePowerSources == resource ||
           M2MDevice::PowerSourceVoltage == resource    ||
           M2MDevice::PowerSourceCurrent == resource) {
            // If it is any of the above resource
            // set the value of the resource.
            char buffer[20];
            int size = sprintf(buffer,"%ld",(long int)value);
            success = res->set_value((const uint8_t*)buffer,
                                     (const uint32_t)size,
                                     true);
        }
    }
    return success;
}

String M2MDevice::resource_value_string(DeviceResource resource,
                                        uint16_t instance_id) const
{
    String value = "";
    M2MResource* res = get_resource(resource,instance_id);
    if(res) {
        if(M2MDevice::Manufacturer == resource          ||
           M2MDevice::ModelNumber == resource           ||
           M2MDevice::DeviceType == resource            ||
           M2MDevice::SerialNumber == resource          ||
           M2MDevice::HardwareVersion == resource       ||
           M2MDevice::FirmwareVersion == resource       ||
           M2MDevice::SoftwareVersion == resource       ||
           M2MDevice::UTCOffset == resource             ||
           M2MDevice::Timezone == resource) {

            uint8_t* buffer = NULL;
            uint32_t length = 0;
            res->get_value(buffer,length);

            char *char_buffer = (char*)malloc(length+1);
            if(char_buffer) {
                memset(char_buffer,0,length+1);
                memcpy(char_buffer,(char*)buffer,length);

                String s_name(char_buffer);
                value = s_name;
                if(buffer) {
                    free(buffer);
                }
                if(char_buffer) {
                    free(char_buffer);
                }
            }
        }
    }
    return value;
}

uint32_t M2MDevice::resource_value_int(DeviceResource resource,
                                      uint16_t instance_id) const
{
    int64_t value = -1;
    M2MResource* res = get_resource(resource,instance_id);
    if(res) {
        if(M2MDevice::BatteryLevel == resource          ||
           M2MDevice::BatteryStatus == resource         ||
           M2MDevice::MemoryFree == resource            ||
           M2MDevice::MemoryTotal == resource           ||
           M2MDevice::ErrorCode == resource             ||
           M2MDevice::CurrentTime == resource           ||
           M2MDevice::AvailablePowerSources == resource ||
           M2MDevice::PowerSourceVoltage == resource    ||
           M2MDevice::PowerSourceCurrent == resource) {
            // Get the value and convert it into integer
            uint8_t* buffer = NULL;
            uint32_t length = 0;
            res->get_value(buffer,length);
            if(buffer) {
                value = atoi((const char*)buffer);
                free(buffer);
            }
        }
    }
    return value;
}

bool M2MDevice::is_resource_present(DeviceResource resource) const
{
    bool success = false;
    M2MResource* res = get_resource(resource);
    if(res) {
        success = true;
    }
    return success;
}

uint16_t M2MDevice::per_resource_count(DeviceResource res) const
{
    uint16_t count = 0;
    if(_device_instance) {
        count = _device_instance->resource_count(resource_name(res));
    }
    return count;
}

uint16_t M2MDevice::total_resource_count() const
{
    uint16_t count = 0;
    if(_device_instance) {
        count = _device_instance->resources().size();
    }
    return count;
}

M2MResource* M2MDevice::get_resource(DeviceResource dev_res,
                                     uint16_t instance_id) const
{
    M2MResource* res = NULL;
    if(_device_instance) {
        res = _device_instance->resource(resource_name(dev_res),instance_id);
    }
    return res;
}

String M2MDevice::resource_name(DeviceResource resource) const
{
    String res_name = "";
    switch(resource) {
        case Manufacturer:
            res_name = DEVICE_MANUFACTURER;
            break;
        case DeviceType:
            res_name = DEVICE_DEVICE_TYPE;
            break;
        case ModelNumber:
            res_name = DEVICE_MODEL_NUMBER;
            break;
        case SerialNumber:
            res_name = DEVICE_SERIAL_NUMBER;
            break;
        case HardwareVersion:
            res_name = DEVICE_HARDWARE_VERSION;
            break;
        case FirmwareVersion:
            res_name = DEVICE_FIRMWARE_VERSION;
            break;
        case SoftwareVersion:
            res_name = DEVICE_SOFTWARE_VERSION;
            break;
        case Reboot:
            res_name = DEVICE_REBOOT;
            break;
        case FactoryReset:
            res_name = DEVICE_FACTORY_RESET;
            break;
        case AvailablePowerSources:
            res_name = DEVICE_AVAILABLE_POWER_SOURCES;
            break;
        case PowerSourceVoltage:
            res_name = DEVICE_POWER_SOURCE_VOLTAGE;
            break;
        case PowerSourceCurrent:
            res_name = DEVICE_POWER_SOURCE_CURRENT;
            break;
        case BatteryLevel:
            res_name = DEVICE_BATTERY_LEVEL;
            break;
        case BatteryStatus:
            res_name = DEVICE_BATTERY_STATUS;
            break;
        case MemoryFree:
            res_name = DEVICE_MEMORY_FREE;
            break;
        case MemoryTotal:
            res_name = DEVICE_MEMORY_TOTAL;
            break;
        case ErrorCode:
            res_name = DEVICE_ERROR_CODE;
            break;
        case ResetErrorCode:
            res_name = DEVICE_RESET_ERROR_CODE;
            break;
        case CurrentTime:
            res_name = DEVICE_CURRENT_TIME;
            break;
        case UTCOffset:
            res_name = DEVICE_UTC_OFFSET;
            break;
        case Timezone:
            res_name = DEVICE_TIMEZONE;
            break;
        case SupportedBindingMode:
            res_name = DEVICE_SUPPORTED_BINDING_MODE;
            break;
    }
    return res_name;
}
