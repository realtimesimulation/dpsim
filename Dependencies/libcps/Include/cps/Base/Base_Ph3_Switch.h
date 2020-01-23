/**
 * @file
 * @author Markus Mirz <mmirz@eonerc.rwth-aachen.de>
 * @copyright 2017-2018, Institute for Automation of Complex Power Systems, EONERC
 *
 * CPowerSystems
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************/

#pragma once

#include <cps/Definitions.h>

namespace CPS {
namespace Base {
namespace Ph3 {
	/// Dynamic Phasor Three-Phase Switch
	class Switch {
	protected:
		/// Resistance if switch is open [ohm]
		Matrix mOpenResistance;
		/// Resistance if switch is closed [ohm]
		Matrix mClosedResistance;
		/// Defines if Switch is open or closed
		Bool mSwitchClosed;
	public:
		///
		void setParameters(Matrix openResistance, Matrix closedResistance, Bool closed = false) {
			mOpenResistance = openResistance;
			mClosedResistance = closedResistance;
			mSwitchClosed = closed;
		}
		void closeSwitch() { mSwitchClosed = true; }
		void openSwitch() { mSwitchClosed = false; }
	};
}
}
}