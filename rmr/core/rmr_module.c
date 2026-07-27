/*
 * Copyright 2026 Rafael Melo Reis.
 * See the repository LICENSE.txt.
 */
#include "../include/rmr_module.h"

rmr_u32
rmr_module_descriptor_fold(const struct rmr_module_descriptor *descriptor)
{
    rmr_u32 fold;

    if (descriptor == (const struct rmr_module_descriptor *)0) {
        return 0u;
    }

    fold = descriptor->magic ^ descriptor->abi_version ^
        descriptor->module_id;
    fold = rmr_mix32_ref(fold ^ descriptor->flags);
    fold = rmr_mix32_ref(fold ^ descriptor->state_bytes);
    fold = rmr_mix32_ref(fold ^ descriptor->scratch_bytes);
    fold = rmr_mix32_ref(fold ^ descriptor->max_steps);
    return fold;
}
