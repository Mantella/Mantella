/*******************************************************************************
 * Copyright (c) 2013 Sebastian Niemann <niemann@sra.uni-hannover.de> and contributors.
 *
 * Licensed under the MIT License (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://opensource.org/licenses/MIT
 *******************************************************************************/
#pragma once

#include <armadillo>
using arma::Mat;

/**
 * Base class for robot mechanisms.
 * 
 * @author Sebastian Niemann <niemann@sra.uni-hannover.de>
 */

namespace optimisation {
  namespace problem {
    namespace robotic {
      namespace mechanism {
        class Robot {
          public:
            Robot() {};

            /**
             * Returns true if the given configuration can be realised by this robot.
             *
             * @param parameter The robot specific parameter
             * @param position The position of the end effector. <b>Note:</b> Must be relative to the origin of the plate.
             * @param rollAngle The roll angle of the end effector
             * @param pitchAngle The pitch angle of the end effector
             * @param yawAngle The yaw angle of the end effector
             *
             * @return Whether the configuration is valid
             */
            virtual bool isValidConfiguration(const Mat<double>& parameter, const Row<
                double>& position, double rollAngle, double pitchAngle, double yawAngle) = 0;

            /**
             * Returns the (complete) kinematic Jacobian of the robot in respect to a trajectory point and a robot specific
             * parameter.
             * <p>
             * <b>Note:</b> The size of each parameter might be varying between different robots.
             *
             * @param parameter The robot specific parameter
             * @param position The position of the end effector. <b>Note:</b> Must be relative to the origin of the plate.
             * @param rollAngle The roll angle of the end effector
             * @param pitchAngle The pitch angle of the end effector
             * @param yawAngle The yaw angle of the end effector
             *
             * @return The Jacobian
             */
            virtual Mat<double> getJacobian(const Mat<double>& parameter, const Row<
                double>& position, double rollAngle, double pitchAngle, double yawAngle) = 0;

            virtual ~Robot() {
            }
            ;
        };
      }
    }
  }
}
