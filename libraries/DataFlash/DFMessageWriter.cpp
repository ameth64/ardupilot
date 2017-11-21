#include "AP_Common/AP_FWVersion.h"
#include "DFMessageWriter.h"

#define FORCE_VERSION_H_INCLUDE
#include "ap_version.h"
#undef FORCE_VERSION_H_INCLUDE

extern const AP_HAL::HAL& hal;

/* LogStartup - these are simple state machines which allow us to
 * trickle out messages to the log files
 */

void DFMessageWriter::reset()
{
    _finished = false;
}

void DFMessageWriter_DFLogStart::reset()
{
    DFMessageWriter::reset();

    _fmt_done = false;
    _writesysinfo.reset();
    _writeentiremission.reset();

    stage = ls_blockwriter_stage_init;
    next_format_to_send = 0;
    ap = AP_Param::first(&token, &type);
}

void DFMessageWriter_DFLogStart::process()
{
    switch(stage) {
    case ls_blockwriter_stage_init:
        stage = ls_blockwriter_stage_formats;
        FALLTHROUGH;

    case ls_blockwriter_stage_formats:
        // write log formats so the log is self-describing
        while (next_format_to_send < _dataflash_backend->num_types()) {
            if (!_dataflash_backend->Log_Write_Format(_dataflash_backend->structure(next_format_to_send))) {
                return; // call me again!
            }
            next_format_to_send++;
        }
        _fmt_done = true;
        stage = ls_blockwriter_stage_parms;
        FALLTHROUGH;

    case ls_blockwriter_stage_parms:
        while (ap) {
            if (!_dataflash_backend->Log_Write_Parameter(ap, token, type)) {
                return;
            }
            ap = AP_Param::next_scalar(&token, &type);
        }

        stage = ls_blockwriter_stage_sysinfo;
        FALLTHROUGH;

    case ls_blockwriter_stage_sysinfo:
        _writesysinfo.process();
        if (!_writesysinfo.finished()) {
            return;
        }
        stage = ls_blockwriter_stage_write_entire_mission;
        FALLTHROUGH;

    case ls_blockwriter_stage_write_entire_mission:
        _writeentiremission.process();
        if (!_writeentiremission.finished()) {
            return;
        }
        stage = ls_blockwriter_stage_vehicle_messages;
        FALLTHROUGH;

    case ls_blockwriter_stage_vehicle_messages:
        // we guarantee 200 bytes of space for the vehicle startup
        // messages.  This allows them to be simple functions rather
        // than e.g. DFMessageWriter-based state machines
        if (_dataflash_backend->vehicle_message_writer()) {
            if (_dataflash_backend->bufferspace_available() < 200) {
                return;
            }
            (_dataflash_backend->vehicle_message_writer())();
        }
        stage = ls_blockwriter_stage_done;
        FALLTHROUGH;

    case ls_blockwriter_stage_done:
        break;
    }

    _finished = true;
}

void DFMessageWriter_WriteSysInfo::reset()
{
    DFMessageWriter::reset();
    stage = ws_blockwriter_stage_init;
}

void DFMessageWriter_DFLogStart::set_mission(const AP_Mission *mission)
{
    _writeentiremission.set_mission(mission);
}


void DFMessageWriter_WriteSysInfo::process() {
    switch(stage) {

    case ws_blockwriter_stage_init:
        stage = ws_blockwriter_stage_firmware_string;
        FALLTHROUGH;

    case ws_blockwriter_stage_firmware_string:
        if (! _dataflash_backend->Log_Write_Message(_firmware_string)) {
            return; // call me again
        }
        stage = ws_blockwriter_stage_git_versions;
        FALLTHROUGH;

    case ws_blockwriter_stage_git_versions:
#if defined(PX4_GIT_VERSION) && defined(NUTTX_GIT_VERSION)
        if (! _dataflash_backend->Log_Write_Message("PX4: " PX4_GIT_VERSION " NuttX: " NUTTX_GIT_VERSION)) {
            return; // call me again
        }
#endif
        stage = ws_blockwriter_stage_system_id;
        FALLTHROUGH;

    case ws_blockwriter_stage_system_id:
        char sysid[40];
        if (hal.util->get_system_id(sysid)) {
            if (! _dataflash_backend->Log_Write_Message(sysid)) {
                return; // call me again
            }
        }
    }

    _finished = true;  // all done!
}

void DFMessageWriter_WriteEntireMission::process() {
    switch(stage) {

    case em_blockwriter_stage_init:
        if (_mission == nullptr) {
            stage = em_blockwriter_stage_done;
            break;
        } else {
            stage = em_blockwriter_stage_write_new_mission_message;
        }
        FALLTHROUGH;

    case em_blockwriter_stage_write_new_mission_message:
        if (! _dataflash_backend->Log_Write_Message("New mission")) {
            return; // call me again
        }
        stage = em_blockwriter_stage_write_mission_items;
        FALLTHROUGH;

    case em_blockwriter_stage_write_mission_items:
        AP_Mission::Mission_Command cmd;
        while (_mission_number_to_send < _mission->num_commands()) {
            // upon failure to write the mission we will re-read from
            // storage; this could be improved.
            if (_mission->read_cmd_from_storage(_mission_number_to_send,cmd)) {
                if (!_dataflash_backend->Log_Write_Mission_Cmd(*_mission, cmd)) {
                    return; // call me again
                }
            }
            _mission_number_to_send++;
        }
        stage = em_blockwriter_stage_done;
        FALLTHROUGH;

    case em_blockwriter_stage_done:
        break;
    }

    _finished = true;
}

void DFMessageWriter_WriteEntireMission::reset()
{
    DFMessageWriter::reset();
    stage = em_blockwriter_stage_init;
    _mission_number_to_send = 0;
}


void DFMessageWriter_WriteEntireMission::set_mission(const AP_Mission *mission)
{
    _mission = mission;
}
