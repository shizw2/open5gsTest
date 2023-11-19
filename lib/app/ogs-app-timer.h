#ifndef OGS_APP_TIMER_H
#define OGS_APP_TIMER_H
#ifdef __cplusplus
extern "C" {
#endif
void ogs_timer_yaml_config_check(void *data);
void ogs_timer_license_check(void *data);
int yaml_check_init(void);
int yaml_check_close(void);
int yaml_check_restart(void);

int license_check_init(void);
int license_check_close(void);
int license_check_restart(void);

#ifdef __cplusplus
}
#endif
#endif