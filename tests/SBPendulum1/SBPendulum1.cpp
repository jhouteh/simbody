/* Copyright (c) 2005-6 Stanford University and Michael Sherman.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**@file
 * A one-body pendulum, to test proper frame alignment and basic
 * functioning of Simbody.
 */

/* Sketch:
 *
 *     |           \           | g
 *     *--          *--        v
 *    / G          / Jb
 *
 *
 *   |           |
 *   *==---------*==---------W
 *  / J         / B         weight
 *   <--- L/2 ---|--- L/2 --->
 *
 *
 * The pendulum is a massless rod with origin frame
 * B, joint attachment frame J, and a point mass W.
 * The rod length is L, with the joint and mass
 * located in opposite directions along the B
 * frame X axis.
 *
 * There is a frame Jb on Ground which will connect
 * to J via a torsion joint around their mutual z axis.
 * Gravity is in the -y direction of the Ground frame.
 * Note that Jb may not be aligned with G, and J may
 * differ from B so the reference configuration may 
 * involve twisting the pendulum around somewhat.
 */

#include "SimTKcommon.h"
#include "Simmatrix.h"
#include "Simbody.h"

#include <string>
#include <iostream>
using std::cout;
using std::endl;

using namespace SimTK;


void stateTest() {
  try {
    State s;
    s.advanceToStage(Stage::Built);

    Vector v3(3), v2(2);
    long q1 = s.allocateQ(v3);
    long q2 = s.allocateQ(v2);

    printf("q1,2=%d,%d\n", q1, q2);
    cout << s;

    long dv = s.allocateDiscreteVariable(Stage::Dynamics, new Value<int>(5));


    s.advanceToStage(Stage::Modeled);
        long dv2 = s.allocateDiscreteVariable(Stage::Configured, new Value<int>(5));

    Value<int>::downcast(s.updDiscreteVariable(dv)) = 71;
    cout << s.getDiscreteVariable(dv) << endl;

  }
  catch(const std::exception& e) {
      printf("*** STATE TEST EXCEPTION\n%s\n***\n", e.what());
  }

}

int main() {
    //stateTest();

    int major,minor,build;
    char out[100];
    const char* keylist[] = { "version", "library", "type", "debug", "authors", "copyright", "svn_revision", 0 };

    //SimTK_version_SimTKlapack(&major,&minor,&build);
    //std::printf("SimTKlapack library version: %d.%d.%d\n", major, minor, build);


    SimTK_version_SimTKcommon(&major,&minor,&build);
    std::printf("==> SimTKcommon library version: %d.%d.%d\n", major, minor, build);
    std::printf("    SimTK_about_SimTKcommon():\n");
    for (const char** p = keylist; *p; ++p) {
        SimTK_about_SimTKcommon(*p, 100, out);
        std::printf("      about(%s)='%s'\n", *p, out);
    }
    SimTK_version_simmatrix(&major,&minor,&build);
    std::printf("==> simmatrix library version: %d.%d.%d\n", major, minor, build);
    std::printf("    SimTK_about_simmatrix():\n");
    for (const char** p = keylist; *p; ++p) {
        SimTK_about_simmatrix(*p, 100, out);
        std::printf("      about(%s)='%s'\n", *p, out);
    }
    SimTK_version_simbody(&major,&minor,&build);
    std::printf("==> simbody library version: %d.%d.%d\n", major, minor, build);
    std::printf("    SimTK_about_simbody():\n");
    for (const char** p = keylist; *p; ++p) {
        SimTK_about_simbody(*p, 100, out);
        std::printf("      about(%s)='%s'\n", *p, out);
    }


try {
    SimbodySubsystem pend;

    Real L = 5.; 
    Real m = 3.;
    Real g = 9.8;
    Transform groundFrame;
    Transform baseFrame;

    //int baseBody =
     //   pend.addRigidBody(0, groundFrame, 
     //                     JointSpecification(JointSpecification::Pin, false),
      //                    Transform(), MassProperties(0.,Vec3(0.),InertiaMat(0.)));
    Transform jointFrame(Vec3(-L/2,0,0));
    MassProperties mprops(m, Vec3(L/2,0,0), InertiaMat(Vec3(L/2,0,0), m)+InertiaMat(1e-6,1e-6,1e-6));
    cout << "mprops about body frame: " << mprops.getMass() << ", " 
        << mprops.getCOM() << ", " << mprops.getInertia() << endl;

    Vec3 gravity(0.,-g,0.);
    cout << "period should be " << 2*std::acos(-1.)*std::sqrt(L/g) << " seconds." << endl;
    int theBody = 
      pend.addRigidBody(mprops, jointFrame,
                        0, Transform(), 
                        //JointSpecification(JointSpecification::Cartesian, false)
                        //JointSpecification(JointSpecification::Sliding, false)
                        //JointSpecification(JointSpecification::Pin, false)
                        //JointSpecification(JointSpecification::Ball, false)
                        JointSpecification(JointSpecification::Free, false)
                        );

/*
    int secondBody = 
      pend.addRigidBody(mprops, jointFrame,
                        theBody, Transform(Vec3(L/2,0,0)), 
                        //JointSpecification(JointSpecification::Cartesian, false),
                        //JointSpecification(JointSpecification::Sliding, false),
                        JointSpecification(JointSpecification::Pin, false),
                        //JointSpecification(JointSpecification::Ball, false),
                        //JointSpecification(JointSpecification::Free, false));
*/
    //int theConstraint =
    //    pend.addConstantDistanceConstraint(0, Vec3((L/2)*std::sqrt(2.)+1,1,0),
    //                                       theBody, Vec3(0,0,0),
    //                                       L/2+std::sqrt(2.));
 
    int ballConstraint =
        pend.addCoincidentStationsConstraint(0, Transform().T(),
                                             theBody, jointFrame.T()); 
/*
    Transform harderOne;
    harderOne.updR().setToBodyFixed123(Vec3(.1,.2,.3));
    harderOne.updT() = jointFrame.T()+Vec3(.1,.2,.3);
    int weldConstraint =
        pend.addWeldConstraint(0, Transform(),
                              theBody, harderOne);    
*/

    //pend.endConstruction();

    State s;
    pend.realize(s, Stage::Built);

    // set Modeling stuff (s)
    pend.setUseEulerAngles(s, false); // this is the default
    pend.setUseEulerAngles(s, true);
    pend.realize(s, Stage::Modeled);

    //pend.setJointQ(s,1,0,0);
   // pend.setJointQ(s,1,3,-1.1);
   // pend.setJointQ(s,1,4,-2.2);
   // pend.setJointQ(s,1,5,-3.3);

    pend.realize(s, Stage::Configured);

    Transform bodyConfig = pend.getBodyConfiguration(s, theBody);
    cout << "body frame: " << bodyConfig;

    pend.enforceConfigurationConstraints(s);
    pend.realize(s, Stage::Configured);

    cout << "after assembly body frame: " << pend.getBodyConfiguration(s,theBody); 

    Vector_<SpatialVec> dEdR(pend.getNBodies());
    dEdR[0] = 0;
    for (int i=1; i < pend.getNBodies()-1; ++i)
        dEdR[i] = SpatialVec(Vec3(0), Vec3(0.,2.,0.));
    Vector dEdQ;
    pend.calcInternalGradientFromSpatial(s, dEdR, dEdQ);
    cout << "dEdR=" << dEdR << endl;
    cout << "dEdQ=" << dEdQ << endl;

    pend.setJointU(s, 1, 0, 10.);

    pend.clearAppliedForces(s);
    pend.applyGravity(s, gravity);
    pend.applyJointForce(s, 1, 0, 147);

    pend.realize(s, Stage::Moving);
    SpatialVec bodyVel = pend.getBodyVelocity(s, theBody);
    cout << "body vel: " << bodyVel << endl;

    cout << "wXwXr=" << bodyVel[0] % (bodyVel[0] % Vec3(2.5,0,0)) << endl;


    cout << "after applying gravity, body forces=" << pend.getAppliedBodyForces(s) << endl;
    cout << "   joint forces=" << pend.getAppliedJointForces(s) << endl;

    pend.realize(s, Stage::Dynamics);
    Vector equivT;
    pend.calcTreeEquivalentJointForces(s, pend.getAppliedBodyForces(s), equivT);
    cout << "body forces -> equiv joint forces=" << equivT << endl;

    pend.realize(s, Stage::Reacting);

    SpatialVec bodyAcc = pend.getBodyAcceleration(s, theBody);
    cout << "body acc: " << bodyAcc << endl;

    //pend.updQ(s) = Vector(4, &Vec4(1.,0.,0.,0.)[0]);
    //pend.updQ(s)[0] = -1.5; // almost hanging straight down
    pend.setJointU(s, 1, 0,   0.);
    //pend.setJointU(s, 1, 1,   0.);
    //pend.setJointU(s, 1, 2, -10.);
   // pend.setJointU(s, 1, 2,   0.);

    //pend.updQ(s)[2] = -.1;
    //pend.setJointQ(s, 1, 2, -0.999*std::acos(-1.)/2);

    const Real h = 0.0001;
    const Real tstart = 0.;
    const Real tmax = 10;

    for (int step=0; ; ++step) { 
        const Real t = tstart + step*h;
        if (t > tmax) break;

        pend.enforceConfigurationConstraints(s);
        pend.realize(s,Stage::Configured);

        pend.enforceMotionConstraints(s);
        pend.realize(s,Stage::Moving);
        const Vector qdot = pend.getQDot(s);

        pend.clearAppliedForces(s);
        pend.applyGravity(s,gravity);

        Transform  x = pend.getBodyConfiguration(s,theBody);
        SpatialVec v = pend.getBodyVelocity(s,theBody);

        //Vec3 err = x.T()-Vec3(2.5,0.,0.);
        //Real d = err.norm();
        //Real k = m*gravity.norm(); // stiffness, should balance at 1
        // Real c = 10.; // damping
        //Vec3 fk = -k*err;
        //Real fc = -c*pend.getU(s)[2];
        //pend.applyPointForce(s,theBody,Vec3(0,0,0),fk);
        //pend.applyJointForce(s,theBody,2,fc);

        if (!(step % 100)) {
            cout << t << " " 
                 << pend.getQ(s) << " " << pend.getU(s) 
                 << endl;
            cout << "body config=" << x;
            cout << "body velocity=" << v << endl;
            //cout << "err=" << err << " |err|=" << d << endl;
            //cout << "spring force=" << fk << endl;
            //cout << "damping joint forces=" << fc << endl;
        }


        pend.realize(s, Stage::Reacting);

        const Vector udot = pend.getUDot(s);
        Vector udot2;
        pend.calcTreeUDot(s, 
            pend.getAppliedJointForces(s),
            pend.getAppliedBodyForces(s),
            udot2);
        if (!(step % 100)) {
            cout << "udot = " << udot << endl;
            cout << "udot2= " << udot2 << endl;
        }

        //cout << "qdot=" << qdot << "  udot=" << udot << endl;
        pend.updQ(s) += h*qdot;
        pend.updU(s) += h*udot;
    }

}
catch(const Exception::Base& e) {
    std::cout << e.getMessage() << std::endl;
}

    return 0;
}
