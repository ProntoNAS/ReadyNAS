/*
 * i2c_nct6775 - Driver for SMBUS module of Nuvitin NCT677x Super-I/O chips
 *
 * Copyright (c) 2016 Hiro Sugawara <hiro.sugawara@netgear.com>
 *
 * Distribution for free use under GNU General Public Licemse.
 */

#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/i2c.h>

#define NCT6775_REG(p, off)	((p)->smba + I2C_NCT6775_##off)
#define I2C_NCT6775_DATA	0
#define I2C_NCT6775_WRSIZE	1
#define I2C_NCT6775_CMD		2
#define I2C_NCT6775_IDX		3
#define I2C_NCT6775_CTL		4
#define I2C_NCT6775_ADDR	5
#define I2C_NCT6775_FREQ	6
#define I2C_NCT6775_ERR_STATUS	6
#define I2C_NCT6775_CTL3	0xe

#define NCT6775_CTL3_F_EMPT	(1 << 0)
#define NCT6775_CTL3_F_FULL	(1 << 1)
#define NCT6775_CTL3_CRC_CHK	(1 << 3)

#define NCT6775_READ_BYTE	0x00
#define NCT6775_READ_WORD	0x01
#define NCT6775_READ_BLOCK	0x02
#define NCT6775_WRITE_BYTE	0x08
#define NCT6775_WRITE_WORD	0x09
#define NCT6775_WRITE_BLOCK	0x0a

#define NCT6775_CTL_BYTE_EN	0x02
#define NCT6775_CTL_CRC8_EN	0x20
#define NCT6775_CTL_S_RST	0x40
#define NCT6775_CTL_MMODE_S	0x80

#define NCT6775_CLK_365K	0x0
#define NCT6775_CLK_261K	0x1
#define NCT6775_CLK_200K	0x2
#define NCT6775_CLK_162K	0x3
#define NCT6775_CLK_136K	0x4
#define NCT6775_CLK_117K	0x5
#define NCT6775_CLK_103K	0x6
#define NCT6775_CLK_92K		0x7
#define NCT6775_CLK_83K		0x8
#define NCT6775_CLK_76K		0x9
#define NCT6775_CLK_71K		0xa
#define NCT6775_CLK_65K		0xb
#define NCT6775_CLK_61K		0xc
#define NCT6775_CLK_57K		0xd
#define NCT6775_CLK_53K		0xe
#define NCT6775_CLK_47K		0xf

#define NCT6775_STATUS_ADNACK	(1<<5)
#define NCT6775_STATUS_TIMEOUT	(1<<4)
#define NCT6775_STATUS_BERR	(1<<2)
#define NCT6775_STATUS_NACK	(1<<1)

static struct i2c_nct6775_priv {
	u16 smba;
	struct i2c_adapter	adapter;
} *i2c_nct6775_priv;

static inline int wait_for_room(struct i2c_nct6775_priv *priv)
{
	int i;
	u8 ctl3;

	for (i = 0; i < 1000; i++) {
		ctl3 = inb(NCT6775_REG(priv, CTL3));
		if (!(ctl3 & NCT6775_CTL3_F_FULL))
			break;
		usleep_range(100, 200);
	}
	return (ctl3 & NCT6775_CTL3_F_EMPT) ? -ETIMEDOUT : 0;
}

static inline int fifo_empty(struct i2c_nct6775_priv *priv)
{
	return inb(NCT6775_REG(priv, CTL3)) & NCT6775_CTL3_F_EMPT;
}

static inline int flush_fifo(struct i2c_nct6775_priv *priv)
{
	int i;

	for (i = 0; i < 32 && !fifo_empty(priv); i++)
		inb(NCT6775_REG(priv, DATA));
	return fifo_empty(priv) ? 0 : -ETIMEDOUT;
}

static inline int wait_for_data(struct i2c_nct6775_priv *priv, u16 flags)
{
	int i;
	u8 ctl3;

	for (i = 0; i < 1000; i++) {
		ctl3 = inb(NCT6775_REG(priv, CTL3));
		if (!(ctl3 & NCT6775_CTL3_F_EMPT))
			break;
		usleep_range(100, 200);
	}
	if ((ctl3 & NCT6775_CTL3_F_EMPT))
		return -ETIMEDOUT;
	if ((flags & I2C_CLIENT_PEC) && !(ctl3 & NCT6775_CTL3_CRC_CHK))
		return -EIO;
	return 0;
}

static s32 i2c_nct6775_xfer(struct i2c_adapter *adap, u16 addr,
			unsigned short flags, char read_write, u8 command,
			int size, union i2c_smbus_data *data)
{
	u8 op, ctl = NCT6775_CTL_MMODE_S;
	int err = 0;
	bool send_command = true;
	size_t len = 1;
	struct i2c_nct6775_priv *priv = i2c_get_adapdata(adap);

	if (flags & I2C_CLIENT_PEC)
		ctl |= NCT6775_CTL_CRC8_EN;

	switch (size) {
	case I2C_SMBUS_BYTE:
		ctl |= NCT6775_CTL_BYTE_EN;
		send_command = (read_write == I2C_SMBUS_WRITE);
		len = 0;
		/* Fall through */
	case I2C_SMBUS_BYTE_DATA:
		op = (read_write == I2C_SMBUS_WRITE)	? NCT6775_WRITE_BYTE
							: NCT6775_READ_BYTE;
		break;
	case I2C_SMBUS_WORD_DATA:
		len = 2;
		op = (read_write == I2C_SMBUS_WRITE)	? NCT6775_WRITE_WORD
							: NCT6775_READ_WORD;
		break;
	case I2C_SMBUS_BLOCK_DATA:
		len = data->block[0];
		op = (read_write == I2C_SMBUS_WRITE)	? NCT6775_WRITE_BLOCK
							: NCT6775_READ_BLOCK;
		break;
	default:
		pr_err("%s: unexpected transfer type: %d\n", __func__, size);
	case I2C_SMBUS_QUICK:	/* Return error even if claimed "supported" */
		return -EOPNOTSUPP;
	}

	if ((err = flush_fifo(priv))) {
		pr_err("%s: FIFO busy.\n",__func__);
		return err;
	}

	outb((addr << 1) | ((read_write == I2C_SMBUS_WRITE) ? 0 : 0x01),
						NCT6775_REG(priv, ADDR));
	outb(op, NCT6775_REG(priv, CMD));

	if (read_write == I2C_SMBUS_WRITE) {
		unsigned int i = 1;

		if (len > 0)
			outb(len, NCT6775_REG(priv, WRSIZE));

		if (send_command)
			outb(command, NCT6775_REG(priv, IDX));

		switch (size) {
		case I2C_SMBUS_BYTE_DATA:
			outb(data->byte, NCT6775_REG(priv, DATA));
			/* Fall through. */
		case I2C_SMBUS_BYTE:
			break;
		case I2C_SMBUS_WORD_DATA:
			outw(data->word, NCT6775_REG(priv, DATA));
			break;
		case I2C_SMBUS_BLOCK_DATA:
			outb(data->block[0], NCT6775_REG(priv, DATA));
			while (i < len) {
				if ((err = wait_for_room(priv)))
					return err;
				outb(data->block[i++], NCT6775_REG(priv, DATA));
			}
			break;
		}
		outb(ctl, NCT6775_REG(priv, CTL));
		flush_fifo(priv);
	} else {
		unsigned int i = 0;

		if (send_command)
			outb(command, NCT6775_REG(priv, IDX));
		outb(ctl, NCT6775_REG(priv, CTL));
		if ((err = wait_for_data(priv, flags))) {
			pr_err("%s: data timed out addr:%x cmd:%x\n",
				__func__, addr, command);
			return err;
		}
		data->word = 0;
		switch (size) {
		case I2C_SMBUS_BYTE:
		case I2C_SMBUS_BYTE_DATA:
			data->byte = inb(NCT6775_REG(priv, DATA));
			break;
		case I2C_SMBUS_WORD_DATA:
			data->word = inb(NCT6775_REG(priv, DATA));
			if ((err = wait_for_data(priv, flags))) {
				pr_err("%s: word data timed out "
				"addr:%x cmd:%x\n", __func__, addr, command);
				return err;
			}
			data->word |= inb(NCT6775_REG(priv, DATA)) << 8;
			break;
		case I2C_SMBUS_BLOCK_DATA:
			len = inb(NCT6775_REG(priv, DATA));
			if (len >= 32) {
				err = -EIO;
				break;
			}
			data->block[i++] = len;
			while (i < len && !(err = wait_for_data(priv, flags)))
				data->block[i++] = inb(NCT6775_REG(priv, DATA));
			if (err)
				pr_err("%s: block data timed out "
				"addr:%x cmd:%x\n", __func__, addr, command);
			break;
		}
	}

	usleep_range(1000, 2000);
	return err;
}

/**
 * Backdoor accesses for RNx220 PMBus
 */
void __i2c_nct6775_reset(struct i2c_adapter *adapter)
{
	struct i2c_nct6775_priv *priv = i2c_get_adapdata(adapter);

	i2c_lock_adapter(adapter);
	outb(NCT6775_CTL_S_RST, NCT6775_REG(priv, CTL));
	msleep(300);
	i2c_unlock_adapter(adapter);
}

static u32 i2c_nct6775_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_SMBUS_BYTE |
		I2C_FUNC_SMBUS_BYTE_DATA |
		I2C_FUNC_SMBUS_WORD_DATA |
		I2C_FUNC_SMBUS_BLOCK_DATA |
		I2C_FUNC_SMBUS_PEC |
		I2C_FUNC_SMBUS_QUICK;
}

static const struct i2c_algorithm i2c_nct6775_algorithm = {
	.smbus_xfer	= i2c_nct6775_xfer,
	.functionality	= i2c_nct6775_func,
};

static const char *chips[] = {"NCT6775", "NCT6776", "NCT6779", NULL};
int __init __i2c_nct6775_init(u16 ba, const char *name)
{
	int err, i;

	for (i = 0; chips[i]; i++)
		if (strstr(name, chips[i]) == name)
			break;
	if (!chips[i]) {
		pr_err("%s: LPC chip has no I2C master support.\n", __func__);
		return 0;
	}

	pr_info("%s: initializing %s SMBus\n", __func__, name);
	if (!(i2c_nct6775_priv = kzalloc(sizeof *i2c_nct6775_priv, GFP_KERNEL)))
		return -ENOMEM;

	i2c_set_adapdata(&i2c_nct6775_priv->adapter, i2c_nct6775_priv);
	i2c_nct6775_priv->adapter.owner	= THIS_MODULE;
	i2c_nct6775_priv->adapter.class	= I2C_CLASS_HWMON | I2C_CLASS_SPD;
	i2c_nct6775_priv->adapter.algo	= &i2c_nct6775_algorithm;
	i2c_nct6775_priv->adapter.timeout	= HZ/5;
	i2c_nct6775_priv->adapter.retries	= 3;
	i2c_nct6775_priv->smba = ba;

	outb(NCT6775_CLK_92K, NCT6775_REG(i2c_nct6775_priv, FREQ));

        snprintf(i2c_nct6775_priv->adapter.name,
		sizeof i2c_nct6775_priv->adapter.name,
		"SMBus %s adapter at %04x", name, i2c_nct6775_priv->smba);

	err = i2c_add_adapter(&i2c_nct6775_priv->adapter);
	if (err) {
		kfree(i2c_nct6775_priv);
		i2c_nct6775_priv = NULL;
		return err;
	}

	return err;
}

void __exit __i2c_nct6775_exit(void)
{
	i2c_del_adapter(&i2c_nct6775_priv->adapter);
	kfree(i2c_nct6775_priv);
	i2c_nct6775_priv = NULL;
}
