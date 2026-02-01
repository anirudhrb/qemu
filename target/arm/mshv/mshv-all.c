/*
 * QEMU MSHV support
 *
 * Copyright Microsoft, Corp. 2026
 *
 * Authors: Aastha Rawat          <aastharawat@linux.microsoft.com>
 *          Anirudh Rayabharam    <anirudh@anirudhrb.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/accel.h"
#include "qemu/error-report.h"

#include "internals.h"

#include "hw/core/boards.h"
#include "hw/hyperv/hvhdk.h"
#include "hw/hyperv/hvhdk_mini.h"
#include "hw/hyperv/hvgdk.h"
#include "hw/hyperv/hvgdk_mini.h"

#include "system/mshv.h"
#include "system/mshv_int.h"

#include "mshv_helpers.h"

uint32_t mshv_arm_get_ipa_bit_size(int mshv_fd)
{
    int ret;
    struct hv_input_get_partition_property in = {0};
    struct hv_output_get_partition_property out = {0};
    struct mshv_root_hvcall args = {0};

    in.partition_id = HV_PARTITION_ID_SELF;
    in.property_code = HV_PARTITION_PROPERTY_PHYSICAL_ADDRESS_WIDTH;

    args.code = HVCALL_GET_PARTITION_PROPERTY;
    args.in_sz = sizeof(in);
    args.in_ptr = (uint64_t)&in;
    args.out_sz = sizeof(out);
    args.out_ptr = (uint64_t)&out;

    ret = mshv_hvcall(mshv_fd, &args);

    if (ret < 0) {
        error_report("Failed to get IPA size");
        return -1;
    }

    return out.property_value;
}

int set_partition_prop(int vm_fd, uint32_t prop_code,
                            uint64_t prop_value)
{
    int ret;
    struct hv_input_set_partition_property in = {0};
    in.property_code = prop_code;
    in.property_value = prop_value;

    struct mshv_root_hvcall args = {0};
    args.code = HVCALL_SET_PARTITION_PROPERTY;
    args.in_sz = sizeof(in);
    args.in_ptr = (uint64_t)&in;

    ret = mshv_hvcall(vm_fd, &args);
    if (ret < 0) {
        error_report("Failed to set partition property code %u", prop_code);
        return -1;
    }

    return 0;
}

int mshv_arch_accel_init(AccelState *as, MachineState *ms, int mshv_fd)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    int pa_range;
    uint32_t ipa_size;

    if (mc->get_physical_address_range) {
        ipa_size = mshv_arm_get_ipa_bit_size(mshv_fd);
        pa_range = mc->get_physical_address_range(ms, ipa_size, ipa_size);
        if (pa_range < 0) {
            return -EINVAL;
        }
    }

    return 0;
}