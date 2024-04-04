/**
 * Redundant PSU - lm_sensors compatible PMBus support for ReadyNAS X86
 *
 * Copyright 2016 Hiro Sugawara <hiro.sugawara@netgear.com>
 */

#include <linux/module.h>
#include <linux/acpi.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/i2c.h>
#include <linux/kthread.h>

#include "../../hwmon/pmbus/pmbus.h"
#include "readynas-io.h"

int num_pmbus_clients = 2;

extern void __i2c_nct6775_reset(struct i2c_adapter *);
extern void __pmbus_set_id_table(const struct i2c_device_id *id_table);
extern const u8 *__pmbus_set_supported_regs(const u8 *regs);

/* DELTA PSU supports only one page. Trying to set to page other than #0,
 * like other invalid parameter values, results in Unsupported Value and
 * PEC (incorrect) errors in STATUS_CML, which cannot be cleared by
 * CLEAR_FAULTS until the system is restarted from poweroff.
 * Letting PMBus driver know there's only one page works around this.
 */
static const struct i2c_device_id pmbus_delt_id_table[] = {
	{"pmbus", 1},
	{}
};

/* DELTA PSU sets Unsupported Command error in STATUS_CML, which cannot be
 * cleared by CLEAR_FAULTS until the system is restarted from poweroff.
 * Filtering commands with a known good command list works around this.
 */
static const u8 pmbus_delt_supported_regs[] = {
	PMBUS_VOUT_MODE,
	PMBUS_FAN_CONFIG_12,

	PMBUS_IOUT_OC_FAULT_LIMIT,
	PMBUS_IOUT_OC_WARN_LIMIT,
	PMBUS_OT_WARN_LIMIT,

	PMBUS_IIN_OC_FAULT_LIMIT,
	PMBUS_IIN_OC_WARN_LIMIT,
	PMBUS_POUT_OP_WARN_LIMIT,

	PMBUS_READ_VIN,
	PMBUS_READ_IIN,
	PMBUS_READ_VOUT,
	PMBUS_READ_IOUT,
	PMBUS_READ_TEMPERATURE_1,
	PMBUS_READ_TEMPERATURE_2,
	PMBUS_READ_FAN_SPEED_1,
	PMBUS_READ_POUT,
	PMBUS_READ_PIN,
	0
};

static void pmbus_delt_init(void)
{
	__pmbus_set_id_table(pmbus_delt_id_table);
}

/* ZIPPY PSU for RR4360 replies bogus values for unsupported registers.
 */
static const u8 pmbus_zippy_supported_regs[] = {
	PMBUS_VOUT_MODE,
	PMBUS_FAN_CONFIG_12,

	PMBUS_READ_VIN,
	PMBUS_READ_VOUT,
	PMBUS_READ_IOUT,
	PMBUS_READ_TEMPERATURE_1,
	PMBUS_READ_POUT,
	PMBUS_READ_PIN,
	0
};

static u8 pmbus_min_supported_regs[] = {
	PMBUS_VOUT_MODE,
	PMBUS_FAN_CONFIG_12,

	PMBUS_READ_VIN,
	PMBUS_READ_VOUT,
	PMBUS_READ_IOUT,
	PMBUS_READ_TEMPERATURE_1,
	PMBUS_READ_POUT,
	PMBUS_READ_PIN,
	0
};

/* Supported PSU type search list.
 * More specific (i.e. w/ mfr_model, mfr_revision) entries must be listed
 * earlier.
 */
static struct pmbus_psu {
	const char *mfr_id, *mfr_model, *mfr_revision;
	unsigned short flags;	/* To be copied to client and adapter. */
	void (*init)(void);
	const u8 *supported_regs;
} pmbus_psu[] = {
	{
		.mfr_id	= "ZIPPY",
		.supported_regs	= pmbus_zippy_supported_regs,
	}, {
		.mfr_id	= "DELT",
		.init	= pmbus_delt_init,
		.flags	= I2C_CLIENT_PEC_PMBUS_WRITE_ONLY,
		.supported_regs	= pmbus_delt_supported_regs,
	}, {
		.mfr_id	= "ACBE",
		.flags	= I2C_CLIENT_PEC_PMBUS_WRITE_ONLY,
	}, {	.mfr_id = "",
		.supported_regs	= pmbus_min_supported_regs,
	}, {}
};

static struct pmbus_adapter_priv {
	const char *compatible;
	const char **adapter_names;
	struct i2c_adapter *adapter;
	void (*adapter_reset)(struct i2c_adapter *);
	int num_psus;	/* Default: 2 */
	struct pmbus_psu *psu;
} pmbus_adapter_priv[] = {
	{
		.compatible	= "rnx220",
		.adapter_names	= rn_i2c_adapters_nct677x,
		.adapter_reset	= __i2c_nct6775_reset,
	}, {
		.compatible	= "rrx312",
		.adapter_names	= rn_i2c_adapters_ipch,
	}, {
		.compatible	= "rr4360",
		.adapter_names	= rn_i2c_adapters_ipch,
		.num_psus	= 4,
	}, {}
};

/* NCT SMBus is multiplexed with GPIO. Determine if we want to use SMBus. */
int rn_use_i2c_nct6775(void)
{
	return readynas_io_compatible("rnx220");
}

static char pmbus_mfr_id[32], pmbus_mfr_model[32], pmbus_mfr_revision[32];

static struct pmbus_client_priv {
	int index;
	struct i2c_adapter *adapter;
	struct i2c_board_info *i2c_info;
	struct i2c_client *client;
	struct proc_dir_entry	*clear_proc,
				*voltage_proc;
} *pmbus_client_priv;

static int pmbus_adapter_reset_show(struct seq_file *s, void *v)
{
        seq_printf(s, "write any to reset PMBus adapter\n");
        return 0;
}

static ssize_t pmbus_adapter_reset_write(struct file *file,
					const char __user *buffer,
					size_t count, loff_t *off)
{
	struct pmbus_adapter_priv *apriv = PDE_DATA(file_inode(file));

	apriv->adapter_reset(apriv->adapter);
        return count;
}

static int pmbus_adapter_reset_open(struct inode *inode, struct file *file)
{
	return single_open(file, pmbus_adapter_reset_show, PDE_DATA(inode));
}

static const struct file_operations pmbus_adapter_reset_fop = {
	.open		= pmbus_adapter_reset_open,
	.read		= seq_read,
	.write		= pmbus_adapter_reset_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int pmbus_clear_show(struct seq_file *s, void *v)
{
        seq_printf(s, "write any to clear PMBus faults\n");
        return 0;
}

static ssize_t pmbus_clear_write(struct file *file,
					const char __user *buffer,
					size_t count, loff_t *off)
{
	struct pmbus_client_priv *cpriv = PDE_DATA(file_inode(file));

	i2c_smbus_xfer(cpriv->adapter, cpriv->i2c_info->addr, 0,
			I2C_SMBUS_WRITE, PMBUS_CLEAR_FAULTS,
			I2C_SMBUS_BYTE, NULL);
        return count;
}

static int pmbus_clear_open(struct inode *inode, struct file *file)
{
	return single_open(file, pmbus_clear_show, PDE_DATA(inode));
}

static const struct file_operations pmbus_clear_fop = {
	.open		= pmbus_clear_open,
	.read		= seq_read,
	.write		= pmbus_clear_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int pmbus_voltage_show(struct seq_file *s, void *v)
{
	struct pmbus_client_priv *cpriv = s->private;
	union i2c_smbus_data i2c_data;
	int err = i2c_smbus_xfer(cpriv->adapter,
				cpriv->i2c_info->addr, 0,
				I2C_SMBUS_READ, 0x88,
				I2C_SMBUS_WORD_DATA, &i2c_data);
	if (err)
		return err;
	seq_printf(s, "%u\n", i2c_data.word);
        return 0;
}

static int pmbus_voltage_open(struct inode *inode, struct file *file)
{
	return single_open(file, pmbus_voltage_show, PDE_DATA(inode));
}

static const struct file_operations pmbus_voltage_fop = {
	.open		= pmbus_voltage_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int pmbus_mfr_show(struct seq_file *s, void *v)
{
	seq_printf(s, "%s\n%s\n%s\n", pmbus_mfr_id,
				pmbus_mfr_model,
				pmbus_mfr_revision);
        return 0;
}

static int pmbus_mfr_open(struct inode *inode, struct file *file)
{
	return single_open(file, pmbus_mfr_show, PDE_DATA(inode));
}

static const struct file_operations pmbus_mfr_fop = {
	.open		= pmbus_mfr_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static struct proc_dir_entry
	*pmbus_adapter_reset_proc,
	*pmbus_mfr_proc;

static void pmbus_adapter_reset_procfs_init(struct pmbus_adapter_priv *apriv)
{
	pmbus_adapter_reset_proc =
			readynas_proc_create("pmbus_reset", 0644,
					&pmbus_adapter_reset_fop, apriv);
}

static void pmbus_client_procfs_init(struct pmbus_client_priv *cpriv)
{
	char name[20];

	sprintf(name, "psu%d_clear", cpriv->index + 1);
	cpriv->clear_proc = readynas_proc_create(name, 0644,
					&pmbus_clear_fop, cpriv);

	sprintf(name, "psu%d_vin", cpriv->index + 1);
	cpriv->voltage_proc = readynas_proc_create(name, 0644,
					&pmbus_voltage_fop, cpriv);
}

static void __pmbus_mfr(struct pmbus_adapter_priv *apriv, u8 idx,
			struct i2c_board_info *info, char *buf)
{
	union i2c_smbus_data i2c_data;
	int i, err = -ENODEV;

	for (i = 0; i < num_pmbus_clients; i++)
		if (!(err = i2c_smbus_xfer(apriv->adapter, info[i].addr, 0,
			I2C_SMBUS_READ, idx,
			I2C_SMBUS_BLOCK_DATA, &i2c_data))) {
			unsigned int len = i2c_data.block[0];

			if (0 < len && len < I2C_SMBUS_BLOCK_MAX) {
				memcpy(buf, &i2c_data.block[1], len - 1);
				buf[len - 1] = '\0';
				break;
			}
		}
}

static void pmbus_mfr_procfs_init(struct pmbus_adapter_priv *apriv,
					struct i2c_board_info *info)
{
	__pmbus_mfr(apriv, PMBUS_MFR_ID, info, pmbus_mfr_id);
	__pmbus_mfr(apriv, PMBUS_MFR_MODEL, info, pmbus_mfr_model);
	__pmbus_mfr(apriv, PMBUS_MFR_REVISION, info, pmbus_mfr_revision);

	pmbus_mfr_proc = readynas_proc_create("pmbus_mfr", 0644,
						&pmbus_mfr_fop, apriv);
}

static void pmbus_procfs_exit(void)
{
	int i;

	for (i = 0; i < num_pmbus_clients; i++) {
		if (pmbus_client_priv[i].voltage_proc) {
			proc_remove(pmbus_client_priv[i].voltage_proc);
			pmbus_client_priv[i].voltage_proc = NULL;
		}
		if (pmbus_client_priv[i].clear_proc) {
			proc_remove(pmbus_client_priv[i].clear_proc);
			pmbus_client_priv[i].clear_proc = NULL;
		}
	}

	if (pmbus_mfr_proc) {
		proc_remove(pmbus_mfr_proc);
		pmbus_mfr_proc = NULL;
	}
	if (pmbus_adapter_reset_proc) {
		proc_remove(pmbus_adapter_reset_proc);
		pmbus_adapter_reset_proc = NULL;
	}
}

extern int pmbus_num_attributes(struct i2c_client *);
static int pmbus_client_probe(struct pmbus_client_priv *cpriv)
{
	union i2c_smbus_data i2c_data;
	int err;

	/* Send CLEAR_FAULTS command. */
	if ((err = i2c_smbus_xfer(cpriv->adapter,
			cpriv->i2c_info->addr,
			(cpriv->i2c_info->flags &
					I2C_CLIENT_PEC_PMBUS_WRITE_ONLY) ?
				I2C_CLIENT_PEC : 0,
			I2C_SMBUS_WRITE, PMBUS_CLEAR_FAULTS,
			I2C_SMBUS_BYTE, NULL)))
		return err;
	msleep(10);
	/* Dummy read-data for preventing receive-byte. */
	if ((err = i2c_smbus_xfer(cpriv->adapter,
			cpriv->i2c_info->addr, 0,
			I2C_SMBUS_READ, PMBUS_STATUS_BYTE,
			I2C_SMBUS_BYTE_DATA, &i2c_data)))
		return err;

	cpriv->client = i2c_new_device(cpriv->adapter, cpriv->i2c_info);

	/**
	 * i2c_new_device() eventually calls bus_probe_device(),
	 * which does not return status.
	 * Check the  etected number of PMBus attributes for
	 * determining the PMBus device registration failure.
	 */
	if (cpriv->client && !pmbus_num_attributes(cpriv->client)) {
		i2c_unregister_device(cpriv->client);
		cpriv->client = NULL;
		err = -EIO;
	}
	return err;
}

/* A background thread for probing initially undetected PSUs.
 * This thread runs only if any PSU is missing at initialization time
 * and will exit when all PSUs are detected.
 */
static struct task_struct *pmbus_probe_kthread;
static int pmbus_client_probe_loop(void *data)
{
	while (!kthread_should_stop()) {
		int i, missing = 0;

		msleep_interruptible(30 * HZ);

		for (i = 0; i < num_pmbus_clients; i++) {
			struct pmbus_client_priv *cpriv = &pmbus_client_priv[i];

			if (!cpriv->client && pmbus_client_probe(cpriv))
				missing++;
		}
		if (!missing)
			break;
	}
	return 0;
}

static int rn_pmbus_probe(struct platform_device *pdev)
{
	struct pmbus_adapter_priv *apriv = dev_get_platdata(&pdev->dev);
	struct i2c_adapter *adapter = apriv->adapter;
	int i, err;
	static struct i2c_board_info psu_i2c_info[] = {
		{ I2C_BOARD_INFO("pmbus", 0x58), },
		{ I2C_BOARD_INFO("pmbus", 0x59), },
		{ I2C_BOARD_INFO("pmbus", 0x5a), },
		{ I2C_BOARD_INFO("pmbus", 0x5b), },
		{}
	};

	pr_info("%s: probing PMBus devices on %s.\n",
		__func__, adapter->name);

	apriv->psu = NULL;
	if (apriv->adapter_reset) {
		pmbus_adapter_reset_procfs_init(apriv);
		apriv->adapter_reset(adapter);
	}
	pmbus_mfr_procfs_init(apriv, psu_i2c_info);
	pr_info("%s: PSU model: %s %s %s\n", __func__,
			pmbus_mfr_id, pmbus_mfr_model, pmbus_mfr_revision);

	/* Look for best-match PSU profile */
	for (i = 0; pmbus_psu[i].mfr_id; i++) {
		if (strcasecmp(pmbus_psu[i].mfr_id, pmbus_mfr_id))
			continue;
		if (!pmbus_psu[i].mfr_model) {
			apriv->psu = &pmbus_psu[i];
			break;
		} else if (strcasecmp(pmbus_psu[i].mfr_model, pmbus_mfr_model))
			continue;
		else if (!pmbus_psu[i].mfr_revision) {
			apriv->psu = &pmbus_psu[i];
			break;
		} else if (!strcasecmp(pmbus_psu[i].mfr_revision,
						pmbus_mfr_revision)) {
			apriv->psu = &pmbus_psu[i];
			break;
		}
	}

	if (!apriv->psu) {
		pr_warn("%s: PSU not found in support list"
			" - using minimal config\n", __func__);
		apriv->psu = &pmbus_psu[i - 1];
	}
	if (apriv->psu->init)
		apriv->psu->init();
	if (apriv->psu->supported_regs)
		__pmbus_set_supported_regs(apriv->psu->supported_regs);

	for (i = 0; i < num_pmbus_clients; i++) {
		struct pmbus_client_priv *cpriv = &pmbus_client_priv[i];

		if (apriv->psu)
			psu_i2c_info[i].flags = apriv->psu->flags;
		cpriv->index = i;
		cpriv->adapter = adapter;
		cpriv->i2c_info = &psu_i2c_info[i];

		pmbus_client_procfs_init(cpriv);

		if (pmbus_client_probe(cpriv))
			pr_err("%s: PMBus failed to detect PSU at 0x%x.\n",
				__func__, psu_i2c_info[i].addr);
	}

	/* Successful if any address is responsive. */
	err = -EIO;
	for (i = 0; i < num_pmbus_clients; i++)
		if (pmbus_client_priv[i].client)
			err = 0;
		else if (!pmbus_probe_kthread) {
			pmbus_probe_kthread =
				kthread_run(pmbus_client_probe_loop, 0,
						"pmbus_probe");
			if (IS_ERR(pmbus_probe_kthread)) {
				pr_err("%s: Failed to start PMBus client "
					"prober thread.\n", __func__);
				err = PTR_ERR(pmbus_probe_kthread);
			}
		}
	if (err == -EIO)
		pr_err("%s: No PMBus devices found.\n", __func__);
	return err;
}

static int rn_pmbus_remove(struct platform_device *pdev)
{
	int i;

	for (i = 0; i < num_pmbus_clients; i++) {
		if (pmbus_client_priv[i].client)
			i2c_unregister_device(pmbus_client_priv[i].client);
		pmbus_client_priv[i].client = NULL;
	}
	return 0;
}


struct platform_driver rn_pmbus_driver = {
	.probe	= rn_pmbus_probe,
	.remove	= rn_pmbus_remove,
	.driver = { .name	= "rn-pmbus", },
};

static struct platform_device *pmbus_pdev;

static int rn_pmbus_init(void)
{
	struct i2c_adapter *adapter;
	int err, i;
	struct pmbus_adapter_priv *apriv =
		__compatible_find(pmbus_adapter_priv, compatible, 0, "");

	if (!apriv->adapter_names || !*apriv->adapter_names)
		return 0;

	pr_info("%s: initializing PMBus for %s...\n",
			__func__, *apriv->adapter_names);

	adapter = readynas_i2c_adapter_find(apriv->adapter_names);
	if (IS_ERR_OR_NULL(adapter)) {
		pr_err("%s: failed to find PMBus adapter %s\n",
			__func__, *apriv->adapter_names);
		return adapter ? PTR_ERR(adapter) : -ENODEV;
	}

	apriv->adapter = adapter;
	if (apriv->num_psus)
		num_pmbus_clients = apriv->num_psus;

	if (!(pmbus_client_priv =
		kzalloc(sizeof(*pmbus_client_priv) * num_pmbus_clients,
			GFP_KERNEL))) {
		pr_err( "%s: failed to allocate cpriv memory.\n", __func__);
		return -ENOMEM;
	}

	if ((err = platform_driver_register(&rn_pmbus_driver))) {
		pr_err( "%s: failed to register ReadyNAS PMBus driver.\n",
			__func__);
		return err;
	}

	pmbus_pdev = platform_device_register_data(NULL,
					rn_pmbus_driver.driver.name, -1,
					apriv, sizeof *apriv);
	if (IS_ERR_OR_NULL(pmbus_pdev)) {
		err = pmbus_pdev ? PTR_ERR(pmbus_pdev) : -EIO;

		platform_driver_unregister(&rn_pmbus_driver);
		pmbus_pdev = NULL;
		return err;
	}

	for (i = 0; i < num_pmbus_clients; i++)
		if (pmbus_client_priv[i].client)
			return 0;
	platform_driver_unregister(&rn_pmbus_driver);
	pmbus_pdev = NULL;
	return -ENODEV;
}

static void rn_pmbus_exit(void)
{
	if (pmbus_pdev) {
		platform_device_unregister(pmbus_pdev);
		platform_driver_unregister(&rn_pmbus_driver);
		pmbus_pdev = NULL;
	}
	if (pmbus_client_priv)
		pmbus_procfs_exit();
	kfree(pmbus_client_priv);
	pmbus_client_priv = NULL;
}

/* This module must be loaded after GPIO and I2C. */
device_initcall_sync(rn_pmbus_init);
module_exit(rn_pmbus_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("NETGEAR PMBus driver");
MODULE_AUTHOR("hiro.sugawara@netgear.com");
