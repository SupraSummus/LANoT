#pragma once

struct cap_t * mem_add (
        struct cap_t * caps, struct cap_t const* caps_past_end,
        void const* ptr, void const* ptr_past_end,
        unsigned int perms
);
