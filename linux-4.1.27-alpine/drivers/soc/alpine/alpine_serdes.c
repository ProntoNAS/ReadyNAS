/*
 * linux/drivers/soc/alpine/alpine_serdes.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/init.h>

#include <soc/alpine/al_hal_serdes.h>
#include <soc/alpine/alpine_serdes.h>

static void __iomem *serdes_base;

#define SERDES_NUM_GROUPS	4
#define SERDES_GROUP_SIZE	0x400

static int __init al_serdes_resource_init(void)
{
	struct device_node *np;

	/* Find the serdes node and make sure it is not disabled */
	np = of_find_compatible_node(NULL, NULL, "annapurna-labs,al-serdes");

	if (np && of_device_is_available(np)) {
		serdes_base = of_iomap(np, 0);
		BUG_ON(!serdes_base);
	} else {
		pr_err("%s: init serdes regs base failed!\n", __func__);
		serdes_base = NULL;
	}
	return 0;
}
arch_initcall(al_serdes_resource_init);

void __iomem *alpine_serdes_resource_get(u32 group)
{
	void __iomem *base = NULL;

	if (group >= SERDES_NUM_GROUPS)
		return NULL;

	if (serdes_base)
		base = serdes_base + group * SERDES_GROUP_SIZE;

	return base;
}
EXPORT_SYMBOL(alpine_serdes_resource_get);

static struct alpine_serdes_eth_group_mode {
	struct mutex			lock;
	enum alpine_serdes_eth_mode	mode;
	bool				mode_set;
} alpine_serdes_eth_group_mode[SERDES_NUM_GROUPS] = {
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[0].lock),
		.mode_set = false,
	},
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[1].lock),
		.mode_set = false,
	},
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[2].lock),
		.mode_set = false,
	},
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[3].lock),
		.mode_set = false,
	}};

int alpine_serdes_eth_mode_set(
	u32				group,
	enum alpine_serdes_eth_mode	mode)
{
	struct alpine_serdes_eth_group_mode *group_mode =
		&alpine_serdes_eth_group_mode[group];

	if (!serdes_base)
		return -EINVAL;

	if (group >= SERDES_NUM_GROUPS)
		return -EINVAL;

	mutex_lock(&group_mode->lock);

	if (!group_mode->mode_set || (group_mode->mode != mode)) {
		struct al_serdes_obj obj;

		al_serdes_handle_init(serdes_base, &obj);

		if (mode == ALPINE_SERDES_ETH_MODE_SGMII)
			al_serdes_mode_set_sgmii(&obj, group);
		else
			al_serdes_mode_set_kr(&obj, group);

		group_mode->mode = mode;
		group_mode->mode_set = true;
	}

	mutex_unlock(&group_mode->lock);

	return 0;
}
EXPORT_SYMBOL(alpine_serdes_eth_mode_set);

void alpine_serdes_eth_group_lock(u32 group)
{
	struct alpine_serdes_eth_group_mode *group_mode =
		&alpine_serdes_eth_group_mode[group];

	mutex_lock(&group_mode->lock);
}
EXPORT_SYMBOL(alpine_serdes_eth_group_lock);

void alpine_serdes_eth_group_unlock(u32 group)
{
	struct alpine_serdes_eth_group_mode *group_mode =
		&alpine_serdes_eth_group_mode[group];

	mutex_unlock(&group_mode->lock);
}
EXPORT_SYMBOL(alpine_serdes_eth_group_unlock);

