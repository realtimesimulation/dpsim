/* Copyright 2017-2020 Institute for Automation of Complex Power Systems,
 *                     EONERC, RWTH Aachen University
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *********************************************************************************/

#include <cps/SP/SP_Ph3_AvVoltageSourceInverterDQ.h>

using namespace CPS;


SP::Ph3::AvVoltageSourceInverterDQ::AvVoltageSourceInverterDQ(String uid, String name, Logger::Level logLevel)
	:SimPowerComp<Complex>(uid, name, logLevel) {
	setVirtualNodeNumber(3);
	setTerminalNumber(1);
	mIntfVoltage = MatrixComp::Zero(3, 1);
	mIntfCurrent = MatrixComp::Zero(3, 1);

	addAttribute<Real>("p", &mP, Flags::read | Flags::write);
	addAttribute<Real>("q", &mQ, Flags::read | Flags::write);
	addAttribute<Real>("P_ref", &mPref, Flags::read | Flags::write);
	addAttribute<Real>("Q_ref", &mQref, Flags::read | Flags::write);
	addAttribute<Real>("theta", &mThetaPLL, Flags::read | Flags::write);
	addAttribute<Real>("phipll", &mPhiPLL, Flags::read | Flags::write);
	addAttribute<Real>("phid", &mPhi_d, Flags::read | Flags::write);
	addAttribute<Real>("phiq", &mPhi_q, Flags::read | Flags::write);
	addAttribute<Real>("gammad", &mGamma_d, Flags::read | Flags::write);
	addAttribute<Real>("gammaq", &mGamma_q, Flags::read | Flags::write);

	//additional loggers
	addAttribute<Matrix>("Vcabc", &mVcabc, Flags::read | Flags::write);
	addAttribute<Matrix>("Vsdq", &mVsdq, Flags::read | Flags::write);
	addAttribute<Matrix>("ifabc", &mIfabc, Flags::read | Flags::write);
	addAttribute<Matrix>("igabc", &mIgabc, Flags::read | Flags::write);
	addAttribute<Matrix>("igdq", &mIgdq, Flags::read | Flags::write);
	addAttribute<Matrix>("ifdq", &mIfdq, Flags::read | Flags::write);
	addAttribute<Real>("omega", &mOmegaInst, Flags::read | Flags::write);
	addAttribute<Real>("freq", &mFreqInst, Flags::read | Flags::write);

}

void SP::Ph3::AvVoltageSourceInverterDQ::setParameters(Real sysOmega, Complex sysVoltNom, Real Pref, Real Qref, Real Kp_pll, Real Ki_pll,
	Real Kp_powerCtrl, Real Ki_powerCtrl, Real Kp_currCtrl, Real Ki_currCtrl, Real Lf, Real Cf,
	Real Rf, Real Rc) {
	mPref = Pref;
	mQref = Qref;

	mKpPLL = Kp_pll;
	mKiPLL = Ki_pll;

	mKiPowerCtrld = Ki_powerCtrl;
	mKiPowerCtrlq = Ki_powerCtrl;

	mKpPowerCtrld = Kp_powerCtrl;
	mKpPowerCtrlq = Kp_powerCtrl;

	mKiCurrCtrld = Ki_currCtrl;
	mKiCurrCtrlq = Ki_currCtrl;

	mKpCurrCtrld = Kp_currCtrl;
	mKpCurrCtrlq = Kp_currCtrl;

	mVoltNom = sysVoltNom;

	mOmegaN = sysOmega;
	mOmegaCutoff = sysOmega;

	mLf = Lf;
	mCf = Cf;
	mRf = Rf;
	mRc = Rc;
	parametersSet = true;
}

void SP::Ph3::AvVoltageSourceInverterDQ::addMonitoredNodes(std::shared_ptr<Resistor> resistor, std::shared_ptr<Capacitor> cap, std::shared_ptr<Inductor> inductor) {

	mResistorF = resistor;
	mCapacitorF = cap;
	mInductorF = inductor;
}

void SP::Ph3::AvVoltageSourceInverterDQ::updateMonitoredValues(const Matrix& leftVector, Real time) {
	MatrixComp VcSP = MatrixComp::Zero(3, 1);
	VcSP(0, 0) = Math::complexFromVectorElement(leftVector, mCapacitorF->matrixNodeIndex(0, 0));
	VcSP(1, 0) = Math::complexFromVectorElement(leftVector, mCapacitorF->matrixNodeIndex(0, 1));
	VcSP(2, 0) = Math::complexFromVectorElement(leftVector, mCapacitorF->matrixNodeIndex(0, 2));

	mVcabc <<
		VcSP(0, 0).real() * cos(mOmegaN * time) - VcSP(0, 0).imag() * sin(mOmegaN * time),
		VcSP(1, 0).real()* cos(mOmegaN * time) - VcSP(1, 0).imag() * sin(mOmegaN * time),
		VcSP(2, 0).real()* cos(mOmegaN * time) - VcSP(2, 0).imag() * sin(mOmegaN * time);

	MatrixComp IfSP = MatrixComp::Zero(3, 1);
	MatrixComp IgSP = MatrixComp::Zero(3, 1);

	IfSP = mResistorF->attribute<MatrixComp>("i_intf")->get();
	IgSP = -(mResistorF->attribute<MatrixComp>("i_intf")->get()
		- mCapacitorF->attribute<MatrixComp>("i_intf")->get());

	mIfabc <<
		IfSP(0, 0).real() * cos(mOmegaN * time) - IfSP(0, 0).imag() * sin(mOmegaN * time),
		IfSP(1, 0).real()* cos(mOmegaN * time) - IfSP(1, 0).imag() * sin(mOmegaN * time),
		IfSP(2, 0).real()* cos(mOmegaN * time) - IfSP(2, 0).imag() * sin(mOmegaN * time);

	mIgabc <<
		IgSP(0, 0).real() * cos(mOmegaN * time) - IgSP(0, 0).imag() * sin(mOmegaN * time),
		IgSP(1, 0).real()* cos(mOmegaN * time) - IgSP(1, 0).imag() * sin(mOmegaN * time),
		IgSP(2, 0).real()* cos(mOmegaN * time) - IgSP(2, 0).imag() * sin(mOmegaN * time);
}


void SP::Ph3::AvVoltageSourceInverterDQ::initializeStates(Real omega, Real timeStep,
	Attribute<Matrix>::Ptr leftVector) {
	mTimeStep = timeStep;
	mOmegaN = omega;
	mOmegaCutoff = omega;

	// initialize mInftCurrent and mIgabc
	mIntfCurrent = mResistorF->attribute<MatrixComp>("i_intf")->get();

	MatrixComp IgSP = -(mResistorF->attribute<MatrixComp>("i_intf")->get()
		- mCapacitorF->attribute<MatrixComp>("i_intf")->get());
	mIgabc <<
		IgSP(0, 0).real(),// * cos(0) - IgSP(0, 0).imag()*sin(0),
		IgSP(1, 0).real(),//* cos(0) - IgSP(1, 0).imag()*sin(0),
		IgSP(2, 0).real();// *cos(0) -IgSP(2, 0).imag() * sin(0);

	Matrix backShiftIntfCurrent = Matrix::Zero(3, 1);
	backShiftIntfCurrent <<
		mIntfCurrent(0, 0).real(),// * cos(0) - mIntfCurrent(0, 0).imag()*sin(0),
		mIntfCurrent(1, 0).real(),// * cos(0) - mIntfCurrent(1, 0).imag()*sin(0),
		mIntfCurrent(2, 0).real();// *cos(0) -mIntfCurrent(2, 0).imag() * sin(0);

	// create matrices for state space representation
	mA = Matrix::Zero(8, 8);
	mB = Matrix::Zero(8, 9);
	// mC = Tabc * mC_right
	mC = Matrix::Zero(2, 8);
	// mD = Tabc * mD_right
	mD = Matrix::Zero(2, 9);
	mStates = Matrix::Zero(8, 1);
	mU = Matrix::Zero(9, 1);
	Matrix initVgabc = Matrix::Zero(3, 1);

	// abc to dq
	mThetaPLL = 0;
	Matrix Tabc_dq = getParkTransformMatrix(mThetaPLL);
	Matrix Td = Tabc_dq.row(0);
	Matrix Tq = Tabc_dq.row(1);

	// dq to abc
	Matrix Tdq_abc = getInverseParkTransformMatrix(mThetaPLL);
	Matrix Tabc1 = Tdq_abc.col(0);
	Matrix Tabc2 = Tdq_abc.col(1);

	mA <<
		0, mKiPLL, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, -mOmegaCutoff, 0, 0, 0, 0, 0,
		0, 0, 0, -mOmegaCutoff, 0, 0, 0, 0,
		0, 0, -1, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, -mKpPowerCtrld, 0, mKiPowerCtrld, 0, 0, 0,
		0, 0, 0, mKpPowerCtrlq, 0, mKiPowerCtrlq, 0, 0;

	mB <<
		1, 0, 0, mKpPLL * Tq, Matrix::Zero(1, 3),
		0, 0, 0, Tq, Matrix::Zero(1, 3),
		0, 0, 0, 3. / 2. * mOmegaCutoff * (Td * mIgabc * Td + Tq * mIgabc * Tq), Matrix::Zero(1, 3),
		0, 0, 0, 3. / 2. * mOmegaCutoff * (Td * mIgabc * Tq - Tq * mIgabc * Td), Matrix::Zero(1, 3),
		0, 1, 0, Matrix::Zero(1, 3), Matrix::Zero(1, 3),
		0, 0, -1, Matrix::Zero(1, 3), Matrix::Zero(1, 3),
		0, mKpPowerCtrld, 0, Matrix::Zero(1, 3), -Td,
		0, 0, -mKpPowerCtrlq, Matrix::Zero(1, 3), -Tq;

	mC <<
		0, 0, -mKpPowerCtrld * mKpCurrCtrld, 0, mKpCurrCtrld* mKiPowerCtrld, 0, mKiCurrCtrld, 0,
		0, 0, 0, mKpPowerCtrlq* mKpCurrCtrlq, 0, mKpCurrCtrlq* mKiPowerCtrlq, 0, mKiCurrCtrlq;

	mD <<
		0, mKpCurrCtrld * mKpPowerCtrld, 0, Matrix::Zero(1, 3), -mKpCurrCtrld * Td,
		0, 0, -mKpCurrCtrlq * mKpPowerCtrlq, Matrix::Zero(1, 3), -mKpCurrCtrlq * Tq;

	mPhiPLL = 0;
	//mP = (3. / 2.  * (Td * mIgabc * Td * mVcabc + Tq * mIgabc * Tq * mVcabc)).coeff(0, 0);
	//mQ = (3. / 2.  * (Td * mIgabc * Tq * mVcabc - Tq * mIgabc * Td * mVcabc)).coeff(0, 0);
	mP = 0;
	mQ = 0;
	mPhi_d = 0;
	mPhi_q = 0;
	mGamma_d = 0;
	mGamma_q = 0;

	// ###### for additional logging start ######
	/*
	mVcdq = Tabc_dq * mVcabc;
	mIgdq = Tabc_dq * mIgabc;
	mIfdq = Tabc_dq * mIfabc;*/
	// ###### for additional logging end #######

	// initialize output
	mStates <<
		mThetaPLL, mPhiPLL, mP, mQ, mPhi_d, mPhi_q, mGamma_d, mGamma_q;
	mU <<
		mOmegaN, mPref, mQref, mVcabc, -backShiftIntfCurrent;

	mVsdq = mC * mStates + mD * mU;
}

void SP::Ph3::AvVoltageSourceInverterDQ::step(Real time) {
	Matrix newStates = Matrix::Zero(8, 1);
	Matrix newU = Matrix::Zero(9, 1);
	Matrix backShiftIntfCurrent = Matrix::Zero(3, 1);
	backShiftIntfCurrent <<
		mIntfCurrent(0, 0).real() * cos(mOmegaN * time) - mIntfCurrent(0, 0).imag() * sin(mOmegaN * time),
		mIntfCurrent(1, 0).real()* cos(mOmegaN * time) - mIntfCurrent(1, 0).imag() * sin(mOmegaN * time),
		mIntfCurrent(2, 0).real()* cos(mOmegaN * time) - mIntfCurrent(2, 0).imag() * sin(mOmegaN * time);
	newU <<
		mOmegaN, mPref, mQref, mVcabc, -backShiftIntfCurrent;
	newStates = Math::StateSpaceTrapezoidal(mStates, mA, mB, mTimeStep, newU, mU);

	// update states
	mThetaPLL = newStates(0, 0);

	// update measurements ( for additional loggers)
	mOmegaInst = (time > 0) ? mThetaPLL / time : 0;
	mFreqInst = mOmegaInst / 2 / M_PI;

	mPhiPLL = newStates(1, 0);
	mP = newStates(2, 0);
	mQ = newStates(3, 0);
	mPhi_d = newStates(4, 0);
	mPhi_q = newStates(5, 0);
	mGamma_d = newStates(6, 0);
	mGamma_q = newStates(7, 0);

	mStates = newStates;
	mU = newU;

	// update B,C,D matrices with new theta_pll
	updateLinearizedModel();

	// new output
	mVsdq = mC * mStates + mD * mU;
}

void SP::Ph3::AvVoltageSourceInverterDQ::updateLinearizedModel() {
	Matrix Tabc_dq = getParkTransformMatrix(mThetaPLL);
	Matrix Tdq_abc = getInverseParkTransformMatrix(mThetaPLL);
	Matrix Td = Tabc_dq.row(0);
	Matrix Tq = Tabc_dq.row(1);
	Matrix Tabc1 = Tdq_abc.col(0);
	Matrix Tabc2 = Tdq_abc.col(1);

	// mA does not need updates
	// update B, C, D
	mB <<
		1, 0, 0, mKpPLL * Tq, Matrix::Zero(1, 3),
		0, 0, 0, Tq, Matrix::Zero(1, 3),
		0, 0, 0, 3. / 2. * mOmegaCutoff * (Td * mIgabc * Td + Tq * mIgabc * Tq), Matrix::Zero(1, 3),
		0, 0, 0, 3. / 2. * mOmegaCutoff * (Td * mIgabc * Tq - Tq * mIgabc * Td), Matrix::Zero(1, 3),
		0, 1, 0, Matrix::Zero(1, 3), Matrix::Zero(1, 3),
		0, 0, -1, Matrix::Zero(1, 3), Matrix::Zero(1, 3),
		0, mKpPowerCtrld, 0, Matrix::Zero(1, 3), -Td,
		0, 0, -mKpPowerCtrlq, Matrix::Zero(1, 3), -Tq;

	mD <<
		0, mKpCurrCtrld * mKpPowerCtrld, 0, Matrix::Zero(1, 3), -mKpCurrCtrld * Td,
		0, 0, -mKpCurrCtrlq * mKpPowerCtrlq, Matrix::Zero(1, 3), -mKpCurrCtrlq * Tq;

	// ###### for additional logging start ######
	/*
	mVcdq = Tabc_dq * mVcabc;
	mIgdq = Tabc_dq * mIgabc;
	mIfdq = Tabc_dq * mIfabc;*/
	// ###### for additional logging end #######
}



Matrix SP::Ph3::AvVoltageSourceInverterDQ::parkTransform(Real theta, Real fa, Real fb, Real fc) {
	Matrix dqvector(2, 1);
	Real d, q;

	d = 2. / 3. * sin(theta) * fa + 2. / 3. * sin(theta - 2. * M_PI / 3.) * fb + 2. / 3. * sin(theta + 2. * M_PI / 3.) * fc;
	q = 2. / 3. * cos(theta) * fa + 2. / 3. * cos(theta - 2. * M_PI / 3.) * fb + 2. / 3. * cos(theta + 2. * M_PI / 3.) * fc;

	dqvector << d,
		q;

	return dqvector;
}

// fdq = Tdq * fabc
Matrix SP::Ph3::AvVoltageSourceInverterDQ::getParkTransformMatrix(Real theta) {
	/// with d-axis starts 90deg behind to phase a
	//	Matrix Tdq = Matrix::Zero(2, 3);
//	Tdq <<
//		2. / 3. * sin(theta), 2. / 3. * sin(theta - 2. * M_PI / 3.), 2. / 3. * sin(theta + 2. * M_PI / 3.),
//		2. / 3. * cos(theta), 2. / 3. * cos(theta - 2. * M_PI / 3.), 2. / 3. * cos(theta + 2. * M_PI / 3.);
//
//	return Tdq;

/// with d-axis starts aligned with phase a
	Matrix Tdq = Matrix::Zero(2, 3);
	Tdq <<
		2. / 3. * cos(theta), 2. / 3. * cos(theta - 2. * M_PI / 3.), 2. / 3. * cos(theta + 2. * M_PI / 3.),
		-2. / 3. * sin(theta), -2. / 3. * sin(theta - 2. * M_PI / 3.), -2. / 3. * sin(theta + 2. * M_PI / 3.);

	return Tdq;
}

// fabc = Tabc * fdq
Matrix SP::Ph3::AvVoltageSourceInverterDQ::inverseParkTransform(Real theta, Real fd, Real fq, Real zero) {
	Matrix abcVector(3, 1);

	// inverse Park transform
	Real a, b, c;

	a = sin(theta) * fd + cos(theta) * fq + 1. * zero;
	b = sin(theta - 2. * M_PI / 3.) * fd + cos(theta - 2. * M_PI / 3.) * fq + 1. * zero;
	c = sin(theta + 2. * M_PI / 3.) * fd + cos(theta + 2. * M_PI / 3.) * fq + 1. * zero;

	abcVector << a,
		b,
		c;

	return abcVector;
}


Matrix SP::Ph3::AvVoltageSourceInverterDQ::getInverseParkTransformMatrix(Real theta) {
	/// with d-axis starts 90deg behind to phase a
	//	Matrix Tabc = Matrix::Zero(3, 2);
	//	Tabc <<
	//		sin(theta), cos(theta),
	//		sin(theta - 2. * M_PI / 3.), cos(theta - 2. * M_PI / 3.),
	//		sin(theta + 2. * M_PI / 3.), cos(theta + 2. * M_PI / 3.);
	//
	//	return Tabc;

	/// with d-axis starts aligned with phase a
	Matrix Tabc = Matrix::Zero(3, 2);
	Tabc <<
		cos(theta), -sin(theta),
		cos(theta - 2. * M_PI / 3.), -sin(theta - 2. * M_PI / 3.),
		cos(theta + 2. * M_PI / 3.), -sin(theta + 2. * M_PI / 3.);

	return Tabc;
}

Complex SP::Ph3::AvVoltageSourceInverterDQ::syncFrame2to1(Complex f2, Real theta1, Real theta2) {
	Real delta = theta2 - theta1;
	Real f1_real = f2.real() * cos(delta) + f2.imag() * sin(delta);
	Real f1_imag = -f2.real() * sin(delta) + f2.imag() * cos(delta);
	return Complex(f1_real, f1_imag);
}



void SP::Ph3::AvVoltageSourceInverterDQ::initializeFromPowerflow(Real frequency) {

	checkForUnconnectedTerminals();

	// Static calculation based on load flow
	mIntfVoltage(0, 0) = initialSingleVoltage(0);
	mIntfVoltage(1, 0) = Complex(Math::abs(initialSingleVoltage(0)) * cos(Math::phase(initialSingleVoltage(0)) - 2. / 3. * M_PI),
		Math::abs(initialSingleVoltage(0)) * sin(Math::phase(initialSingleVoltage(0)) - 2. / 3. * M_PI));
	mIntfVoltage(2, 0) = Complex(Math::abs(initialSingleVoltage(0)) * cos(Math::phase(initialSingleVoltage(0)) + 2. / 3. * M_PI),
		Math::abs(initialSingleVoltage(0)) * sin(Math::phase(initialSingleVoltage(0)) + 2. / 3. * M_PI));

	// here getting the initial voltage from the node rather than from component
	// in case the component is initialized after it
	Real vca = Math::abs(mCapacitorF->terminal(0)->node()->initialSingleVoltage());
	Real vca_phase = Math::phase(mCapacitorF->terminal(0)->node()->initialSingleVoltage());
	mVcabc = Matrix::Zero(3, 1);
	mVcabc <<
		vca * cos(vca_phase), vca* cos(vca_phase - 2. / 3. * M_PI), vca* cos(vca_phase + 2. / 3. * M_PI);

	// Create sub voltage source for emf
	mSubCtrledVoltageSource = SP::Ph3::ControlledVoltageSource::make(mName + "_src", mLogLevel);
	mSubCtrledVoltageSource->connect({ SimNode::GND, terminal(0)->node() });
	mSubCtrledVoltageSource->setVirtualNodeAt(mVirtualNodes[0], 0);
	mSubCtrledVoltageSource->setVirtualNodeAt(mVirtualNodes[1], 1);
	mSubCtrledVoltageSource->setVirtualNodeAt(mVirtualNodes[2], 2);
	mSubCtrledVoltageSource->setParameters(mIntfVoltage);
	mSubCtrledVoltageSource->initializeFromPowerflow(frequency);
}

void SP::Ph3::AvVoltageSourceInverterDQ::mnaInitialize(Real omega, Real timeStep, Attribute<Matrix>::Ptr leftVector) {
	mTimeStep = timeStep;
	initializeStates(omega, timeStep, leftVector);
	mSubCtrledVoltageSource->mnaInitialize(omega, timeStep, leftVector);
	mRightVector = Matrix::Zero(leftVector->get().rows(), 1);

	for (auto task : mSubCtrledVoltageSource->mnaTasks()) {
		mMnaTasks.push_back(task);
	}
	mMnaTasks.push_back(std::make_shared<MnaPreStep>(*this));
	mMnaTasks.push_back(std::make_shared<AddBStep>(*this));
	mMnaTasks.push_back(std::make_shared<MnaPostStep>(*this, leftVector));
}


void SP::Ph3::AvVoltageSourceInverterDQ::mnaApplySystemMatrixStamp(Matrix& systemMatrix) {

	mSubCtrledVoltageSource->mnaApplySystemMatrixStamp(systemMatrix);

}

void SP::Ph3::AvVoltageSourceInverterDQ::mnaApplyRightSideVectorStamp(Matrix& rightVector) {

	mSubCtrledVoltageSource->mnaApplyRightSideVectorStamp(rightVector);
}

void SP::Ph3::AvVoltageSourceInverterDQ::dqToSP(Real time) {
	/*Complex vsDqOmegaS = syncFrame2to1(Complex(mVsdq(0, 0), mVsdq(1, 0)), mOmegaN * time, mThetaPLL);
	mIntfVoltage(0, 0) = vsDqOmegaS;*/
	mIntfVoltage(0, 0) = Complex(mVsdq(0, 0), mVsdq(1, 0));
	mIntfVoltage(1, 0) = mIntfVoltage(0, 0) * Complex(cos(-2 * M_PI / 3), sin(-2 * M_PI / 3));
	mIntfVoltage(2, 0) = mIntfVoltage(0, 0) * Complex(cos(2 * M_PI / 3), sin(2 * M_PI / 3));

}

void SP::Ph3::AvVoltageSourceInverterDQ::MnaPreStep::execute(Real time, Int timeStepCount) {
	// shift EMT voltage into SP
	mAvVoltageSourceInverterDQ.dqToSP(time);
	// update voltage of subVoltage source
	mAvVoltageSourceInverterDQ.mSubCtrledVoltageSource->setParameters(mAvVoltageSourceInverterDQ.mIntfVoltage);
}

void SP::Ph3::AvVoltageSourceInverterDQ::MnaPostStep::execute(Real time, Int timeStepCount) {
	mAvVoltageSourceInverterDQ.mnaUpdateCurrent(*mLeftVector);
	// update Vcabc, Igabc
	mAvVoltageSourceInverterDQ.updateMonitoredValues(*mLeftVector, time);
	mAvVoltageSourceInverterDQ.step(time);
}

void SP::Ph3::AvVoltageSourceInverterDQ::AddBStep::execute(Real time, Int timeStepCount) {
	mAvVoltageSourceInverterDQ.mRightVector = mAvVoltageSourceInverterDQ.mSubCtrledVoltageSource->attribute<Matrix>("right_vector")->get();
}

void SP::Ph3::AvVoltageSourceInverterDQ::mnaUpdateCurrent(const Matrix& leftvector) {

	mIntfCurrent = mSubCtrledVoltageSource->attribute<MatrixComp>("i_intf")->get();

}
