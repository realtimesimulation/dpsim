/** Linear Resistor
 *
 * @file
 * @author Markus Mirz <mmirz@eonerc.rwth-aachen.de>
 * @copyright 2017, Institute for Automation of Complex Power Systems, EONERC
 *
 * DPsim
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

#include <iostream>
#include "Base.h"

namespace DPsim {
namespace Components {
namespace DP {

	class Resistor : public Components::Base {

	protected:
		///Resistance [ohm]
		Real mResistance;

		///Conductance [S]
		Real mConductance;

		///Real Part of the voltage at node 1 [V]
		Real mVoltageAtNode1Re;

		///Imaginary Part of the voltage at node 1 [V]
		Real mVoltageAtNode1Im;

		///Real Part of the voltage at node 2 [V]
		Real mVoltageAtNode2Re;

		///Imaginary Part of the voltage at node 2 [V]
		Real mVoltageAtNode2Im;

	public:
		Resistor(String name, Int src, Int dest, Real resistance, LogLevel loglevel = LogLevel::NONE, Bool decrementNodes = true);

		void init(SystemModel& system) { }
		void applySystemMatrixStamp(SystemModel& system);
		void applyRightSideVectorStamp(SystemModel& system) { }
		void step(SystemModel& system, Real time) { }
		void postStep(SystemModel& system) { }
		Complex getCurrent(SystemModel& model);
	};
}
}
}