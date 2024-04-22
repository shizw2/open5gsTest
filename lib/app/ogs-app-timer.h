#ifndef OGS_APP_TIMER_H
#define OGS_APP_TIMER_H
#ifdef __cplusplus
extern "C" {
#endif
#define YAML_CONFIG_CHECK_INTERVAL 5
#define LICENSE_CHECK_INTERVAL     (30*60)

void ogs_timer_yaml_config_check(void *data);
void ogs_timer_license_check(void *data);
int ogs_yaml_check_init(void);
int ogs_yaml_check_close(void);
int ogs_yaml_check_restart(void);
int yaml_check_proc(void); //各NF自己实现

int license_check_init(void);
int license_check_close(void);
int license_check_restart(void);

#ifdef __cplusplus
}
#endif
#endif