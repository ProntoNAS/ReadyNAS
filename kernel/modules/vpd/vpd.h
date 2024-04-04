struct vpd_data_struct {
	char vpd_hwm_vol_label[10][9];
	unsigned char vpd_hwm_vol_min0[10],vpd_hwm_vol_max0[10];
	unsigned char vpd_hwm_vol_min1[10],vpd_hwm_vol_max1[10];
	
	char vpd_hwm_fan_label[5][8];
	unsigned char vpd_hwm_fan_req[5], vpd_hwm_fan_self[5], vpd_hwm_fan_alt[5];
	unsigned short vpd_hwm_fan_min[5], vpd_hwm_fan_typ[5];

	unsigned char vpd_cpu_target, vpd_cpu_tolerance, vpd_ver;

	unsigned char vpd_mac0[6],vpd_mac1[6];
	char vpd_vendor[32],vpd_model[32];
	char vpd_serial[16],vpd_version[24];
	unsigned char vpd_oc,vpd_sata_chn_max;
	unsigned long vpd_features;	/* 0 */
	unsigned long vpd_features1;	/* 1 */
	unsigned char vpd_boardid;
	unsigned char vpd_fan_rpm_min,vpd_fan_pwm_min,vpd_fan_pwm_cft;
	unsigned short vpd_board_rev,vpd_system_rev;
	unsigned char  vpd_ecc_flag;
	unsigned short vpd_ecc_ignore;
	unsigned long vpd_ecc_count;
	unsigned short vpd_ecc_correct;
	unsigned char vpd_spd03t30[28];
	unsigned char vpd_spd41t43[3];
	unsigned long vpd_ssh_timestamp;
	unsigned char vpd_maxspeed,vpd_maxtemp,vpd_maxrpm;
	unsigned char vpd_spindly;
	unsigned char vpd_ledlum0_1,vpd_ledlum2_3,vpd_ledlum4_5;
	unsigned char vpd_max_temp_1,vpd_min_temp_1;
	unsigned char vpd_max_temp_2,vpd_min_temp_2;
	unsigned char vpd_max_temp_3,vpd_min_temp_3;
};

struct basic_vpd_data_struct {
	char vendor[32];
	char model[32];
	char serial[16];
	char sku[16];
	char uuid[37];
};

