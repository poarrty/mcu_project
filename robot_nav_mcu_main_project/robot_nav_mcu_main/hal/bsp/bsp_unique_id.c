#include "stdio.h"
#include "bsp_unique_id.h"

void SocIDGet(uint32_t *id) {
    id[0] = *(uint32_t *) (SOC_TYPE_ADDR + 0);
    id[1] = *(uint32_t *) (SOC_TYPE_ADDR + 4);
    id[2] = *(uint32_t *) (SOC_TYPE_ADDR + 8);
}
