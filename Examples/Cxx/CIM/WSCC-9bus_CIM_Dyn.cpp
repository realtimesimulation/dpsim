/* Copyright 2017-2020 Institute for Automation of Complex Power Systems,
 *                     EONERC, RWTH Aachen University
 * DPsim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************/

#include <iostream>
#include <list>

#include <DPsim.h>

using namespace DPsim;
using namespace CPS::DP;

int main(int argc, char *argv[]) {

	// Find CIM files
	std::list<fs::path> filenames;
	if (argc <= 1) {
		filenames = Utils::findFiles({
			"WSCC-09_RX_DI.xml",
			"WSCC-09_RX_EQ.xml",
			"WSCC-09_RX_SV.xml",
			"WSCC-09_RX_TP.xml"
		}, "Examples/CIM/grid-data/WSCC-09/WSCC-09_RX_Dyn", "CIMPATH");
	}
	else {
		filenames = std::list<fs::path>(argv + 1, argv + argc);
	}

	String simName = "WSCC-9bus_dyn";
	Logger::setLogDir("logs/"+simName);

	CPS::CIM::Reader reader(simName, Logger::Level::debug, Logger::Level::debug);
	SystemTopology sys = reader.loadCIM(60, filenames);

	// Logging
	auto logger = DataLogger::make(simName);
	logger->addAttribute("v1", sys.node<SimNode>("BUS1")->attribute("v"));
	logger->addAttribute("v2", sys.node<SimNode>("BUS2")->attribute("v"));
	logger->addAttribute("v3", sys.node<SimNode>("BUS3")->attribute("v"));
	logger->addAttribute("v4", sys.node<SimNode>("BUS4")->attribute("v"));
	logger->addAttribute("v5", sys.node<SimNode>("BUS5")->attribute("v"));
	logger->addAttribute("v6", sys.node<SimNode>("BUS6")->attribute("v"));
	logger->addAttribute("v7", sys.node<SimNode>("BUS7")->attribute("v"));
	logger->addAttribute("v8", sys.node<SimNode>("BUS8")->attribute("v"));
	logger->addAttribute("v9", sys.node<SimNode>("BUS9")->attribute("v"));
	logger->addAttribute("wr_1", sys.component<Ph1::SynchronGeneratorTrStab>("GEN1")->attribute("w_r"));
	logger->addAttribute("wr_2", sys.component<Ph1::SynchronGeneratorTrStab>("GEN2")->attribute("w_r"));
	logger->addAttribute("wr_3", sys.component<Ph1::SynchronGeneratorTrStab>("GEN3")->attribute("w_r"));

	Simulation sim(simName, Logger::Level::info);
	sim.setSystem(sys);
	sim.setDomain(Domain::DP);
	sim.setTimeStep(0.0001);
	sim.setFinalTime(2);
	sim.doSteadyStateInit(true);
	sim.doHarmonicParallelization(false);
	sim.addLogger(logger);
	sim.run();

	//std::ofstream ofTopo("topology_graph.svg");
	//sys.topologyGraph().render(ofTopo);
	//std::ofstream ofDep("task_dependencies.svg");
	//sim.dependencyGraph().render(ofDep);

	return 0;
}
