/*
 * Copyright 2026 Rafael Melo Reis.
 * See the repository LICENSE.txt.
 *
 * Fixed metadata for freestanding RMR modules. Source comments do not survive
 * compilation; live architectural notes are emitted explicitly into
 * .rmr.notes and descriptors into .rmr.modules.
 */
#ifndef RMR_MODULE_H
#define RMR_MODULE_H

#include "rmr_silicon.h"

#define RMR_MODULE_ABI_VERSION 0x00010000u
#define RMR_MODULE_MAGIC 0x524d524du /* RMRM */

#define RMR_MODULE_ID_SHA256 0x53323536u /* S256 */
#define RMR_MODULE_ID_UOP16  0x55313630u /* U160 */

#define RMR_MODULE_F_FREESTANDING      (1u << 0)
#define RMR_MODULE_F_NO_HEAP           (1u << 1)
#define RMR_MODULE_F_CALLER_STORAGE    (1u << 2)
#define RMR_MODULE_F_FIXED_STEPS       (1u << 3)
#define RMR_MODULE_F_SELECTIVE_PATCH   (1u << 4)
#define RMR_MODULE_F_BRANCHLESS_ALU    (1u << 5)
#define RMR_MODULE_F_ZERO_EXTERNAL     (1u << 6)
#define RMR_MODULE_F_BOUNDED_CONTROL   (1u << 7)

struct rmr_module_descriptor {
    rmr_u32 magic;
    rmr_u32 abi_version;
    rmr_u32 module_id;
    rmr_u32 flags;
    rmr_u32 state_bytes;
    rmr_u32 scratch_bytes;
    rmr_u32 max_steps;
    rmr_u32 semantic_fold;
};

rmr_u32 rmr_module_descriptor_fold(
    const struct rmr_module_descriptor *descriptor);

#endif
