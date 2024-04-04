/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 or V3 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in
	  the documentation and/or other materials provided with the
	  distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

/**
 * @addtogroup group_muio_mux
 *
 *  @{
 * @file   al_hal_muio_mux.c
 *
 * @brief  MUIO mux HAL driver
 *
 */

#include "al_hal_muio_mux.h"
#include "al_hal_muio_mux_map.h"
#include "al_hal_pbs_regs.h"

#define AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS	2

#define AL_MUIO_MUX_NUM_MUIO_PER_REG		\
	(32 / (AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS))

#define AL_MUIO_MUX_NUM_MUIO_FUNC_MASK		3

/**
 * Extended function for allocating an interface - with and without forcing
 */
static int al_muio_mux_iface_alloc_ex_s(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg,
	int			force);

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_init(
	void __iomem		*regs_base,
	char			*name,
	struct al_muio_mux_obj	*obj)
{
	int status = 0;

	int pin;

	al_dbg("%s(%p, %s)\n", __func__, regs_base, name);

	al_assert(regs_base);
	al_assert(name);
	al_assert(obj);

	obj->regs_base = regs_base;

	obj->name = name;

	for (pin = 0; pin < AL_MUIO_MUX_NUM_MUIO; pin++)
		obj->pins_iface[pin] = AL_MUIO_MUX_IF_NONE;

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_terminate(
	struct al_muio_mux_obj	*obj)
{
	int status = 0;

	al_dbg("%s()\n", __func__);

	al_assert(obj);

	return status;
}

/******************************************************************************/
/******************************************************************************/
static int al_muio_mux_iface_alloc_ex_s(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg,
	int			force)
{
	int status = 0;
	int i;
	int gpio_pin = arg;

	const struct al_muio_mux_iface_pins gpio_pins = {
		AL_MUIO_MUX_IF_GPIO,
		AL_MUIO_MUX_FUNC_GPIO,
		1,
		&gpio_pin };

	const struct al_muio_mux_iface_pins *iface_pins = NULL;
	uint32_t mux_sel_reg0_val;
	uint32_t mux_sel_reg1_val;
	uint32_t mux_sel_reg2_val;

	if (iface != AL_MUIO_MUX_IF_GPIO) {
		for (i = 0; i < AL_MUIO_MUX_NUM_IFACES; i++)
			if (_al_muio_mux_iface_pins[i].iface == iface) {
				iface_pins = &_al_muio_mux_iface_pins[i];
				break;
			}
	} else {
		iface_pins = &gpio_pins;
	}

	if (!iface_pins) {
		al_err(
			"%s: invalid iface (%d, %d)!\n",
			__func__,
			iface,
			arg);

		status = -EINVAL;
		goto done;
	}

	if (!force) {
		for (i = 0; i < iface_pins->num_pins; i++) {
			int pin = iface_pins->pins[i];
			enum al_muio_mux_if pin_if = obj->pins_iface[pin];

			if (pin_if != AL_MUIO_MUX_IF_NONE) {
				al_err(
					"%s(%d): pin %d taken by iface %d!\n",
					__func__,
					iface,
					pin,
					obj->pins_iface[pin]);
				status = -EBUSY;
				goto done;
			}
		}
	}

	mux_sel_reg0_val = al_reg_read32(&obj->regs_base->unit.pbs_mux_sel_0);
	mux_sel_reg1_val = al_reg_read32(&obj->regs_base->unit.pbs_mux_sel_1);
	mux_sel_reg2_val = al_reg_read32(&obj->regs_base->unit.pbs_mux_sel_2);

	for (i = 0; i < iface_pins->num_pins; i++) {
		int pin = iface_pins->pins[i];

		al_dbg(
			"%s: pin %d, func %d\n",
			__func__,
			pin,
			iface_pins->mux_func);

		obj->pins_iface[pin] = iface;

		if (pin < AL_MUIO_MUX_NUM_MUIO_PER_REG) {
			al_dbg(
				"%s: mux_sel0 &= ~%08x\n",
				__func__,
				(AL_MUIO_MUX_NUM_MUIO_FUNC_MASK <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin)));

			mux_sel_reg0_val &= ~(AL_MUIO_MUX_NUM_MUIO_FUNC_MASK <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));

			mux_sel_reg0_val |= (iface_pins->mux_func <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));
		} else if (pin < (2 * AL_MUIO_MUX_NUM_MUIO_PER_REG)) {
			pin -= AL_MUIO_MUX_NUM_MUIO_PER_REG;

			al_dbg(
				"%s: mux_sel1 &= ~%08x\n",
				__func__,
				(AL_MUIO_MUX_NUM_MUIO_FUNC_MASK <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin)));

			mux_sel_reg1_val &= ~(AL_MUIO_MUX_NUM_MUIO_FUNC_MASK <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));

			mux_sel_reg1_val |= (iface_pins->mux_func <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));
		} else {
			pin -= 2 * AL_MUIO_MUX_NUM_MUIO_PER_REG;

			al_dbg(
				"%s: mux_sel2 &= ~%08x\n",
				__func__,
				(AL_MUIO_MUX_NUM_MUIO_FUNC_MASK <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin)));

			mux_sel_reg2_val &= ~(AL_MUIO_MUX_NUM_MUIO_FUNC_MASK <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));

			mux_sel_reg2_val |= (iface_pins->mux_func <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));
		}
	}

	al_reg_write32(&obj->regs_base->unit.pbs_mux_sel_0, mux_sel_reg0_val);
	al_reg_write32(&obj->regs_base->unit.pbs_mux_sel_1, mux_sel_reg1_val);
	al_reg_write32(&obj->regs_base->unit.pbs_mux_sel_2, mux_sel_reg2_val);

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_iface_alloc(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg)
{
	al_dbg(
		"%s(%d, %d)\n",
		__func__,
		iface,
		arg);

	al_assert(obj);

	return al_muio_mux_iface_alloc_ex_s(obj, iface, arg, 0);
}

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_iface_alloc_force(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg)
{
	al_dbg(
		"%s(%d, %d)\n",
		__func__,
		iface,
		arg);

	al_assert(obj);

	return al_muio_mux_iface_alloc_ex_s(obj, iface, arg, 1);
}

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_iface_alloc_multi(
	struct al_muio_mux_obj			*obj,
	const struct al_muio_mux_if_and_arg	*ifaces_and_args,
	int					ifaces_cnt)
{
	int status = 0;
	int i;

	al_dbg(
		"%s(%p, %d)\n",
		__func__,
		ifaces_and_args,
		ifaces_cnt);

	al_assert(obj);
	al_assert(ifaces_and_args);

	for (i = 0; i < ifaces_cnt; i++) {
		status = al_muio_mux_iface_alloc_ex_s(
			obj,
			ifaces_and_args[i].iface,
			ifaces_and_args[i].arg,
			0);

		if (status != 0) {
			al_err(
				"%s: al_muio_mux_iface_alloc(%d, %d) failed!\n",
				__func__,
				ifaces_and_args[i].iface,
				ifaces_and_args[i].arg);
			i--;
			goto fail_free_allocated;
		}
	}

	goto done;

fail_free_allocated:

	for (; i >= 0; i--)
		al_muio_mux_iface_free(
			obj,
			ifaces_and_args[i].iface,
			ifaces_and_args[i].arg);

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_iface_free(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg)
{
	int status = 0;
	int i;
	int gpio_pin = arg;
	const struct al_muio_mux_iface_pins gpio_pins = {
		AL_MUIO_MUX_IF_GPIO,
		AL_MUIO_MUX_FUNC_GPIO,
		1,
		&gpio_pin };
	const struct al_muio_mux_iface_pins *iface_pins = NULL;
	uint32_t mux_sel_reg0_val;
	uint32_t mux_sel_reg1_val;
	uint32_t mux_sel_reg2_val;

	al_dbg(
		"%s(%d, %d)\n",
		__func__,
		iface,
		arg);

	al_assert(obj);

	if (iface != AL_MUIO_MUX_IF_GPIO) {
		for (i = 0; i < AL_MUIO_MUX_NUM_IFACES; i++)
			if (_al_muio_mux_iface_pins[i].iface == iface) {
				iface_pins = &_al_muio_mux_iface_pins[i];
				break;
			}
	} else {
		iface_pins = &gpio_pins;
	}

	if (!iface_pins) {
		al_err("%s(%d): invalid iface!\n", __func__, iface);
		status = -EINVAL;
		goto done;
	}

	for (i = 0; i < iface_pins->num_pins; i++) {
		int pin = iface_pins->pins[i];
		enum al_muio_mux_if pin_if = obj->pins_iface[pin];

		if (pin_if != iface) {
			al_err(
				"%s(%d): not fully allocated!",
				__func__,
				iface);

			status = -EIO;
			goto done;
		}
	}

	mux_sel_reg0_val = al_reg_read32(&obj->regs_base->unit.pbs_mux_sel_0);
	mux_sel_reg1_val = al_reg_read32(&obj->regs_base->unit.pbs_mux_sel_1);
	mux_sel_reg2_val = al_reg_read32(&obj->regs_base->unit.pbs_mux_sel_2);

	for (i = 0; i < iface_pins->num_pins; i++) {
		int pin = iface_pins->pins[i];

		obj->pins_iface[pin] = AL_MUIO_MUX_IF_NONE;

		if (pin < AL_MUIO_MUX_NUM_MUIO_PER_REG) {
			mux_sel_reg0_val &= ~(AL_MUIO_MUX_NUM_MUIO_FUNC_MASK <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));

			mux_sel_reg0_val |= (AL_MUIO_MUX_FUNC_GPIO <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));
		} else if (pin < (2 * AL_MUIO_MUX_NUM_MUIO_PER_REG)) {
			pin -= AL_MUIO_MUX_NUM_MUIO_PER_REG;

			mux_sel_reg1_val &= ~(AL_MUIO_MUX_NUM_MUIO_FUNC_MASK <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));

			mux_sel_reg1_val |= (AL_MUIO_MUX_FUNC_GPIO <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));
		} else {
			pin -= 2 * AL_MUIO_MUX_NUM_MUIO_PER_REG;

			mux_sel_reg2_val &= ~(AL_MUIO_MUX_NUM_MUIO_FUNC_MASK <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));

			mux_sel_reg2_val |= (AL_MUIO_MUX_FUNC_GPIO <<
				(AL_MUIO_MUX_NUM_MUIO_FUNC_NUM_BITS * pin));
		}
	}

	al_reg_write32(&obj->regs_base->unit.pbs_mux_sel_0, mux_sel_reg0_val);
	al_reg_write32(&obj->regs_base->unit.pbs_mux_sel_1, mux_sel_reg1_val);
	al_reg_write32(&obj->regs_base->unit.pbs_mux_sel_2, mux_sel_reg2_val);

done:

	return status;
}

/** @} end of MUIO mux group */

