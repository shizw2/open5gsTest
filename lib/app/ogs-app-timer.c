#include "ogs-app-timer.h"
#include "ogs-app.h"
#include "ogs-proto.h"
#include "license.h"


//这个方法可以挪到lib库中，但目前没合适的lib库存放
void ogs_timer_yaml_config_check(void *data)
{
    int rv;
    ogs_event_t *e = NULL;

    e = ogs_event_new(OGS_EVENT_APP_CHECK_TIMER);
    ogs_assert(e);
    e->timer_id = OGS_TIMER_YAML_CONFIG_CHECK;

    rv = ogs_queue_push(ogs_app()->queue, e);
    if (rv != OGS_OK) {
        ogs_error("ogs_queue_push() failed:%d", (int)rv);
        ogs_event_free(e);
    }
}

void ogs_timer_license_check(void *data)
{
    int rv;
    ogs_event_t *e = NULL;

    e = ogs_event_new(OGS_EVENT_APP_CHECK_TIMER);
    ogs_assert(e);
    e->timer_id = OGS_TIMER_LICENSE_CHECK;

    rv = ogs_queue_push(ogs_app()->queue, e);
    if (rv != OGS_OK) {
        ogs_error("ogs_queue_push() failed:%d", (int)rv);
        ogs_event_free(e);
    }
}

static ogs_timer_t     *t_yaml_check;
int ogs_yaml_check_init(void)
{  
    //set timer
    t_yaml_check = ogs_timer_add(ogs_app()->timer_mgr,ogs_timer_yaml_config_check,0);
    ogs_timer_start(t_yaml_check,ogs_time_from_sec(YAML_CONFIG_CHECK_INTERVAL));

    ogs_info("yaml_check_init sucess.");
    return OGS_OK;
}

int ogs_yaml_check_close(void)
{  
    if (t_yaml_check != NULL){
        ogs_timer_delete(t_yaml_check);
        t_yaml_check = NULL;
    }
    return OGS_OK;
}


int ogs_yaml_check_restart(void)
{  
    //重设定时器
    ogs_timer_start(t_yaml_check,ogs_time_from_sec(YAML_CONFIG_CHECK_INTERVAL));
    return OGS_OK;
}

static ogs_timer_t     *t_license_check;
int license_check_init(void)
{  
    char errorMsg[100];
    size_t errorMsgSize = sizeof(errorMsg);
    bool result = dsCheckLicense(errorMsg, errorMsgSize);
    if (!result) {
        ogs_info("系统已运行:%lu秒, 有效时长:%lu秒, 截止时间:%s,在线用户数:%d\r\n", getLicenseRunTime(),
                        getLicenseDurationTime(),
                        timestampToString(getLicenseExpireTime()),
                        getLicenseUeNum());
        ogs_fatal("License错误: %s\n", errorMsg);
        return OGS_ERROR;
    } 

    //set timer
    t_license_check = ogs_timer_add(ogs_app()->timer_mgr,ogs_timer_license_check,0);
    ogs_timer_start(t_license_check,ogs_time_from_sec(LICENSE_CHECK_INTERVAL));

    ogs_info("license_check_init sucess.");
    return OGS_OK;
}

int license_check_close(void)
{  
    if (t_license_check != NULL){
        ogs_timer_delete(t_license_check);
        t_license_check = NULL;
    }
    return OGS_OK;
}


int license_check_restart(void)
{  
    //重设定时器
    ogs_timer_start(t_license_check,ogs_time_from_sec(LICENSE_CHECK_INTERVAL));
    return OGS_OK;
}