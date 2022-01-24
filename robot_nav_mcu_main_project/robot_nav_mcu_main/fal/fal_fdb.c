#include "fal_log.h"
#include "cmsis_os.h"
#include "shell.h"
#include "rtc.h"
#include "sfud.h"
#include "flashdb.h"
#include "wwdg.h"
#include <time.h>

#include "elog.h"

extern struct fdb_tsdb tsdb;

/* KVDB object */
struct fdb_kvdb kvdb = {0};
/* TSDB object */
struct fdb_tsdb tsdb = {0};
/* counts for simulated timestamp */
static fdb_time_t counts = 0;

osMutexId_t mutex_fdb = NULL;

const osMutexAttr_t mutex_fdb_attr = {
    "mutex_fdb",                            // human readable mutex name
    osMutexRecursive | osMutexPrioInherit,  // attr_bits
    NULL,                                   // memory for control block
    0U                                      // size for control block
};

static fdb_time_t get_time(void);
static void       lock(fdb_db_t db);
static void       unlock(fdb_db_t db);

int fal_fdb_init(void) {
    if (sfud_init() != SFUD_SUCCESS) {
        log_e("SFUD init fail");
    } else {
        log_i("SFUD init ok");
    }

    mutex_fdb = osMutexNew(&mutex_fdb_attr);

    if (mutex_fdb == NULL) {
        log_e("mutex create failed");
    }

    struct fdb_default_kv default_kv;

    default_kv.kvs = NULL;
    default_kv.num = 0;

    /* set the lock and unlock function if you want */
    fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, lock);
    fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, unlock);
    /* Key-Value database initialization
     *
     *       &kvdb: database object
     *       "env": database name
     * "fdb_kvdb": The flash partition name base on FAL. Please make sure it's
     * in FAL partition table. Please change to YOUR partition name.
     * &default_kv: The default KV nodes. It will auto add to KVDB when first
     * initialize successfully. NULL: The user data if you need, now is empty.
     */
    if (fdb_kvdb_init(&kvdb, "env", "fdb_kvdb", &default_kv, NULL) != FDB_NO_ERR) {
        log_e("fdb_kvdb init fail");
    } else {
        log_i("fdb_kvdb init ok");
    }

    /* set the lock and unlock function if you want */
    fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_LOCK, lock);
    fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_UNLOCK, unlock);
    /* Time series database initialization
     *
     *       &tsdb: database object
     *       "log": database name
     * "fdb_tsdb": The flash partition name base on FAL. Please make sure it's
     * in FAL partition table. Please change to YOUR partition name. get_time:
     * The get current timestamp function. 128: maximum length of each log NULL:
     * The user data if you need, now is empty.
     */

    if (fdb_tsdb_init(&tsdb, "log", "fdb_tsdb", get_time, 1024, NULL) != FDB_NO_ERR) {
        log_e("fdb_tsdb init fail");
    } else {
        log_i("fdb_tsdb init ok");
    }
    /* read last saved time for simulated timestamp */
    fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_GET_LAST_TIME, &counts);

    return 0;
}

static void lock(fdb_db_t db) {
    // __disable_irq();
    // HAL_WWDG_Refresh(&hwwdg);
    // HAL_IWDG_Refresh(&hiwdg);
    if (mutex_fdb != NULL)
        osMutexAcquire(mutex_fdb,
                       osWaitForever);  ///< 在中断中调用，会直接返回 err, 不影响使用
}

static void unlock(fdb_db_t db) {
    // __enable_irq();
    // HAL_WWDG_Refresh(&hwwdg);
    // HAL_IWDG_Refresh(&hiwdg);
    if (mutex_fdb != NULL)
        osMutexRelease(mutex_fdb);  ///< 在中断中调用，会直接返回 err, 不影响使用
}

static fdb_time_t get_time(void) {
    /* Using the counts instead of timestamp.
     * Please change this function to return RTC time.
     */

    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    struct tm tm_counts;

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    tm_counts.tm_year = date.Year + 2000;
    tm_counts.tm_mon  = date.Month;
    tm_counts.tm_mday = date.Date;
    tm_counts.tm_hour = time.Hours;
    tm_counts.tm_min  = time.Minutes;
    tm_counts.tm_sec  = time.Seconds;

    counts = mktime(&tm_counts);

    return counts;  ///< 北京时间时区：东八区
}
