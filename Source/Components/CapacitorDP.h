/** Capacitor
 *
 * @file
 * @author Markus Mirz <mmirz@eonerc.rwth-aachen.de>
 * @copyright 2017, Institute for Automation of Complex Power Systems, EONERC
 * @license GNU General Public License (version 3)
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

#include "BaseComponent.h"


namespace DPsim {

	/// Capacitor model:
	/// The capacitor is represented by a DC equivalent circuit which corresponds to one iteration of the trapezoidal integration method.
	/// The equivalent DC circuit is a resistance in paralel with a current source. The resistance is constant for a defined time step and system
	///frequency and the current source changes for each iteration.
	class Capacitor : public BaseComponent {
	protected:
		/// Capacitance [F]
		Real capacitance;

		/// Real part of the voltage across the capacitor [V]
		Real deltavr;

		/// Imaginary part of the voltage across the capacitor [V]
		Real deltavi;

		/// Real and imaginary part of the current trough the capacitor [A]
		Real currr;
		Real curri;

		///Real and imaginary part of the DC equivalent current source [A]
		Real cureqr;
		Real cureqi;

		Real mGcr;
		Real mGci;


	public:
		Capacitor() { };

		/// define capacitor name, conected nodes and capacitance
		Capacitor(String name, Int src, Int dest, Real capacitance);

		/// initializes variables detalvr, deltavi, currr, curri, cureqr and curreqi
		void init(Real om, Real dt);

		/// Stamps DC equivalent resistance to the conductance matrix
		void applySystemMatrixStamp(SystemModel& system);

		/// Stamps DC equivalent current source to the current vector
		void applyRightSideVectorStamp(SystemModel& system) { }

		/// calculates the value of the current source for one time step and apply it to the current vector
		void step(SystemModel& system, Real time);

		/// Recalculates variables detalvr, deltavi, currr and curri based on the simulation results of one time step
		void postStep(SystemModel& system);

		/// Return current from the previous step
		Complex getCurrent(SystemModel& system);
	};
}