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

uint32_t mshv_arm_get_ipa_bit_size(int mshv_fd);
int set_partition_prop(int vm_fd, uint32_t prop_code,
                            uint64_t prop_value);