/* outdoor-weather-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config_rfm210.h: Configuration for RFM210
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef RFM210_CONFIG_H
#define RFM210_CONFIG_H

#define RFM210_RECV_PIN             P2_9
#define RFM210_NENABLE_PIN          P0_5

#define RFM210_RECV_ETL_CHANNEL     3
#define RFM210_RECV_OGU_CHANNEL     0

#define RFM210_RECV_IRQ_N           3
#define RFM210_RECV_IRQ_PRIO        1

#define RFM210_SERVICE_REQUEST_RECV 2

#define RFM210_IRQ_RECV             11
#define RFM210_IRQ_RECV_PRIORITY    0

#endif
